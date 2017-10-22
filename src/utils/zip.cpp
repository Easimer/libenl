/*
   Copyright 2017 Daniel Meszaros <easimer@gmail.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <enl/zip.h>
#include <enl/checksum.h>
#include <cstring>
#include <memory>

#include <iostream>

enlzip_writer::enlzip_writer(const char* szFilename)
{
	m_file = std::ofstream(szFilename, std::ios::out | std::ios::binary);
}

enlzip_writer::~enlzip_writer()
{
	close();
}

ssize_t enlzip_writer::write(const char* szFilename, const char* pData, size_t nLength)
{
    return write(szFilename, pData, nLength, 0, 0, 644);
}

ssize_t enlzip_writer::write(const char* szFilename, const char* pData, size_t nLength, uint32_t iOwnerUser, uint32_t iOwnerGroup, uint32_t nPermissions)
{
	size_t iPos = m_file.tellp();
	size_t iNewPos = ((iPos + 511) & -512);
	m_file.seekp(iNewPos);
	m_file.write(pData, nLength);
	uint32_t nHash = m_hash.get_hash(szFilename);
	m_reqs.push_back({nHash, iNewPos, nLength, iOwnerUser, iOwnerGroup, nPermissions});
	std::cout << "File with hash " << nHash << " written " << strlen(szFilename) << std::endl;
	return nLength;
}

void enlzip_writer::close()
{
	size_t nOTPLen = 0;
	size_t nHTableLen = 0;
	// flush write requests

	// seek next sector
	m_file.seekp(((size_t)m_file.tellp() + 511) & -512);

	// write hash table
	for(auto& req : m_reqs)
	{
		ezip_hentry hentry;
		hentry.nPathHash = req.nHash;
		hentry.nDataOffset = req.nOffset;
		hentry.nDataSize = req.nSize;
		hentry.iFlags = 0;
		hentry.iOwnerUser = req.iOwnerUser;
		hentry.iOwnerGroup = req.iOwnerGroup;
		hentry.nPermissions = req.nPermissions;

		m_file.write((const char*)&hentry, sizeof(ezip_hentry));
		nHTableLen++;
	}

	std::cout << nHTableLen << " hash table entries written" << std::endl;

	// write otp table
	nOTPLen = m_hash.get_otp_size();
	std::cout << "OTP OFF: " << std::dec << m_file.tellp() << std::endl;
	m_file.write((const char*)m_hash.get_otp(), nOTPLen);

	std::cout << "OTP written (" << nOTPLen << " bytes)" << std::endl;

	ezip_header hdr;
	memset(&hdr, 0, sizeof(hdr));
	hdr.iID = IDENLZIP;
	hdr.nHTableLen = nHTableLen;
	hdr.nOTPLen = 0x100;
	hdr.nVersion = ENLZIP_VERSION;
	hdr.nOTPChecksum = m_hash.get_checksum();

	m_file.write((const char*)&hdr, sizeof(hdr));

	std::cout << "Header written" << std::endl;

	m_file.close();
}

enlzip_reader::enlzip_reader(const char* szFilename)
{
	std::cout << "READER" << std::endl;
	ezip_header header;
	size_t iEndOfFile;
	m_file = std::ifstream(szFilename, std::ios::in | std::ios::binary);

	if(!m_file)
		throw std::runtime_error("cannot open ezip " + std::string(szFilename));

	m_file.seekg(0, std::ios::end);
	iEndOfFile = m_file.tellg();
	m_iEndOfFile = iEndOfFile;
	std::cout << "Length of archive is " << m_iEndOfFile << std::endl;
	
	m_file.seekg(iEndOfFile - sizeof(ezip_header));

	m_file.read((char*)&header, sizeof(ezip_header));

	std::cout << "Opened " << szFilename << std::endl;

	if(header.iID != IDENLZIP)
	{
		throw std::runtime_error("bad signature");
	}
	if(header.nVersion != ENLZIP_VERSION)
	{
		throw std::runtime_error("bad version");
	}
	std::cout << "HTable length: " << header.nHTableLen << std::endl;
	std::cout << "OTP length: " << header.nOTPLen << std::endl;

	size_t off = iEndOfFile - sizeof(ezip_header) - header.nOTPLen;
	
	std::cout << "OTP OFF: " << std::dec << off << std::endl;
	
	m_file.seekg(off);

	char otp_buf[header.nOTPLen];
	memset(otp_buf, 0, header.nOTPLen);
	m_file.read(otp_buf, header.nOTPLen);
	std::cout << "Loading OTP" << std::endl;
	m_hash = hash(header.nOTPLen, (uint8_t*)otp_buf);
	
	std::cout << "OTP checksum" << std::endl <<
			"Header checksum: " << header.nOTPChecksum << std::endl <<
			"Calculated checksum: " << m_hash.get_checksum() << std::endl;
	
	if(m_hash.get_checksum() != header.nOTPChecksum)
		throw std::runtime_error("bad OTP checksum");

	m_nOTPLen = header.nOTPLen;
	m_nHTableLen = header.nHTableLen;

	// Offset to HTable from end of file
	m_iHTableOff = iEndOfFile - sizeof(ezip_header) - m_nOTPLen - m_nHTableLen * sizeof(ezip_hentry);
	
	m_file.seekg(m_iHTableOff);
	
	for(size_t i = 0; i < m_nHTableLen; i++)
	{
		ezip_hentry hentry;
		m_file.read((char*)&hentry, sizeof(hentry));
		enlzip_file file;
		file.nHash = hentry.nPathHash;
		file.nPermissions = hentry.nPermissions;
		file.iOwnerUser = hentry.iOwnerUser;
		file.iOwnerGroup = hentry.iOwnerGroup;
		file.nOffset = hentry.nDataOffset;
		file.nSize = hentry.nDataSize;
		
		m_files.push_back(file);
	}
}
uint32_t enlzip_reader::open(const char* szPath)
{
	uint32_t nHash = m_hash.get_hash(szPath);
	std::cout << "File with hash " << nHash << " is opening " << strlen(szPath) << std::endl;
	/*m_file.seekg(m_iHTableOff);
	for(size_t i = 0; i < m_nHTableLen; i++)
	{
		ezip_hentry hentry;
		m_file.read((char*)&hentry, sizeof(hentry));
		if(hentry.nPathHash == nHash)
		{
			return i;
		}
	}*/
	for(size_t i = 0; i < m_files.size(); i++)
	{
		if(m_files[i].nHash == nHash)
			return i;
	}

	return ~0;
}
size_t enlzip_reader::size(uint32_t pFile)
{
	/*if(pFile > m_nHTableLen)
		return 0;
	m_file.seekg(m_iHTableOff + pFile * sizeof(ezip_hentry), std::ios::end);
	ezip_hentry hentry;
	m_file.read((char*)&hentry, sizeof(hentry));
	return hentry.nDataSize;*/
	if(pFile >= m_files.size())
		return 0;
	return m_files[pFile].nSize;
}
ssize_t enlzip_reader::read(uint32_t pFile, uint8_t* pDst)
{
	size_t iDataOff, nDataSize;
	if(pFile > m_nHTableLen)
		return 0;
	/*m_file.seekg(m_iHTableOff + pFile * sizeof(ezip_hentry), std::ios::end);
	ezip_hentry hentry;
	m_file.read((char*)&hentry, sizeof(hentry));
	iDataOff = hentry.nDataOffset;
	nDataSize = hentry.nDataSize;*/
	iDataOff = m_files[pFile].nOffset;
	nDataSize = m_files[pFile].nSize;

	m_file.seekg(iDataOff);
	m_file.read((char*)pDst, nDataSize);
	return nDataSize;
}

uint32_t enlzip_reader::owner_user(uint32_t pFile)
{
	if(pFile >= m_files.size())
		return 0;
	return m_files[pFile].iOwnerUser;
	/*m_file.seekg(m_iHTableOff + pFile * sizeof(ezip_hentry), std::ios::end);
	ezip_hentry hentry;
	m_file.read((char*)&hentry, sizeof(hentry));
	return hentry.iOwnerUser;*/
}

uint32_t enlzip_reader::owner_group(uint32_t pFile)
{
	if(pFile >= m_files.size())
		return 0;
	return m_files[pFile].iOwnerGroup;
	/*m_file.seekg(m_iHTableOff + pFile * sizeof(ezip_hentry), std::ios::end);
	ezip_hentry hentry;
	m_file.read((char*)&hentry, sizeof(hentry));
	return hentry.iOwnerGroup;*/
}

uint32_t enlzip_reader::permissions(uint32_t pFile)
{
	if(pFile >= m_files.size())
		return 0;
	return m_files[pFile].nPermissions;
	/*m_file.seekg(m_iHTableOff + pFile * sizeof(ezip_hentry), std::ios::end);
	ezip_hentry hentry;
	m_file.read((char*)&hentry, sizeof(hentry));
	return hentry.nPermissions;*/
}

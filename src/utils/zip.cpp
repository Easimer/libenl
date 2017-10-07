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
#include <cstring>

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
	size_t iPos = m_file.tellp();
	size_t iNewPos = ((iPos + 511) & -512);
	std::cout << "Current cur: " << iPos << " new pos: " << iNewPos << std::endl;
	m_file.seekp(iNewPos);
	m_file.write(pData, nLength);
	m_reqs.push_back({m_hash.get_hash(szFilename), iNewPos, nLength});
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

		m_file.write((const char*)&hentry, sizeof(ezip_hentry));
		nHTableLen++;
	}

	std::cout << nHTableLen << " hash table entries written" << std::endl;

	// write otp table
	nOTPLen = m_hash.get_otp_size();
	m_file.write((const char*)m_hash.get_otp(), nOTPLen);

	std::cout << "OTP written (" << nOTPLen << " bytes)" << std::endl;

	ezip_header hdr;
	memset(&hdr, 0, sizeof(hdr));
	hdr.iID = IDENLZIP;
	hdr.nHTableLen = nHTableLen;
	hdr.nOTPLen = nOTPLen;

	m_file.write((const char*)&hdr, sizeof(hdr));

	std::cout << "Header written" << std::endl;

	m_file.close();
}

enlzip_reader::enlzip_reader(const char* szFilename)
{
	ezip_header header;
	m_file = std::ifstream(szFilename, std::ios::in | std::ios::binary);

	if(!m_file)
		throw std::runtime_error("cannot open ezip " + std::string(szFilename));

	m_file.seekg(-sizeof(ezip_header), std::ios::end);

	m_file.read((char*)&header, sizeof(ezip_header));

	std::cout << "Opened " << szFilename << std::endl;

	if(header.iID != IDENLZIP)
	{
		throw std::runtime_error("bad signature");
	}
	std::cout << "HTable length: " << header.nHTableLen << std::endl;
	std::cout << "OTP length: " << header.nOTPLen << std::endl;

	m_file.seekg(-sizeof(ezip_header) - header.nOTPLen, std::ios::end);

	char otp_buf[header.nOTPLen];
	m_file.read(otp_buf, header.nOTPLen);
	m_hash = hash(header.nOTPLen, (uint8_t*)otp_buf);

	m_nOTPLen = header.nOTPLen;
	m_nHTableLen = header.nHTableLen;

	// Offset to HTable from end of file
	m_iHTableOff = -sizeof(ezip_header) - m_nOTPLen - m_nHTableLen * sizeof(ezip_hentry);
}
uint32_t enlzip_reader::open(const char* szPath)
{
	uint32_t nHash = m_hash.get_hash(szPath);
	m_file.seekg(m_iHTableOff, std::ios::end);
	for(size_t i = 0; i < m_nHTableLen; i++)
	{
		ezip_hentry hentry;
		m_file.read((char*)&hentry, sizeof(hentry));
		if(hentry.nPathHash == nHash)
		{
			return i;
		}
	}

	return ~0;
}
size_t enlzip_reader::size(uint32_t pFile)
{
	if(pFile > m_nHTableLen)
		return 0;
	m_file.seekg(m_iHTableOff + pFile * sizeof(ezip_hentry), std::ios::end);
	ezip_hentry hentry;
	m_file.read((char*)&hentry, sizeof(hentry));
	return hentry.nDataSize;
}
ssize_t enlzip_reader::read(uint32_t pFile, uint8_t* pDst)
{
	size_t iDataOff, nDataSize;
	if(pFile > m_nHTableLen)
		return 0;
	m_file.seekg(m_iHTableOff + pFile * sizeof(ezip_hentry), std::ios::end);
	ezip_hentry hentry;
	m_file.read((char*)&hentry, sizeof(hentry));
	iDataOff = hentry.nDataOffset;
	nDataSize = hentry.nDataSize;

	m_file.seekg(iDataOff);
	m_file.read((char*)pDst, nDataSize);
	return nDataSize;
}


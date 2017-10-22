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

#include <enl/hash_otp.h>
#include <enl/checksum.h>
#include <iostream>
#include <cstring>

#include <csignal>

hash::hash(size_t nOtpLen)
{
	/*if(nOtpLen < 0x100)
		nOtpLen = 0x100;
	nOtpLen &= ~3; // pad to 4*/
	nOtpLen = 0x100;
	for(size_t i = 0; i < 256; i++)
	{
		m_pOTP[i] = rand();
	}
	m_nOTPLen = nOtpLen;
	uint16_t nChecksum = checksum_ip4(m_pOTP, nOtpLen);
	std::cout << "Generated OTP table with checksum " << nChecksum << std::endl;
	m_nChecksum = nChecksum;
}

hash::hash(size_t nOtpLen, uint8_t* pOTP)
{
	/*if(nOtpLen < 0x100)
		nOtpLen = 0x100;
	nOtpLen &= ~3; // pad to 4*/
	nOtpLen = 0x100;
	std::cout << "Copying " << nOtpLen << " bytes from " << pOTP << " to " << m_pOTP << std::endl;
	memcpy(m_pOTP, pOTP, nOtpLen);
	m_nOTPLen = nOtpLen;
	uint16_t nChecksum = checksum_ip4(pOTP, nOtpLen);
	std::cout << "Loaded OTP table with checksum " << nChecksum << std::endl;
	m_nChecksum = nChecksum;
}

uint32_t hash::get_hash(uint8_t *pData, size_t nLength)
{
	uint32_t nHash = 0;
	size_t iOTPIndex = 0;
	for(size_t i = 0; i < nLength; i++)
	{
		iOTPIndex = pData[i];
		uint8_t nData = iOTPIndex ^ m_pOTP[iOTPIndex];
		std::cout << std::hex << (unsigned)nData << ' ' << std::endl;
		nHash += nData;
		nHash += nHash << 10;
		nHash ^= nHash >> 6;
	}
	nHash += nHash << 3;
	nHash ^= nHash >> 11;
	nHash += nHash << 15;
	return nHash;
}

uint32_t hash::get_hash(const char* szString)
{
	std::cout << "Hashing string \"" << szString << '\"' << std::endl;
	return get_hash((uint8_t*)szString, strlen(szString));
}


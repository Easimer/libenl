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

#pragma once

#include <cstdlib>
#include <cstdint>

class hash {
public:
	hash(size_t nOtpLen = 4096);
	hash(size_t nOtpLen, uint8_t* pOTP);
	~hash();

	uint32_t get_hash(uint8_t* pData, size_t nLength);
	uint32_t get_hash(const char* szString);

	uint8_t* get_otp() { return m_pOTP; }
	size_t get_otp_size() { return m_nOTPLen; }

private:
	uint8_t* m_pOTP;
	size_t m_nOTPLen;
};


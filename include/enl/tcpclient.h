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

#include <string>
#include <enl/platform.h>

#if defined(PLAT_WINDOWS)
typedef unsigned socket_t;
void NETWORK_INIT();
void NETWORK_SHUTDOWN();
#elif defined(PLAT_LINUX)
typedef int socket_t;
#define INVALID_SOCKET (-1)
#define NETWORK_INIT()
#define NETWORK_SHUTDOWN()
#endif

class CTCPClient
{
public:
	ENL_EXPORT CTCPClient(std::string& addr, unsigned short iPort);
	ENL_EXPORT ~CTCPClient();

	ENL_EXPORT CTCPClient(const CTCPClient& other) = delete;
	ENL_EXPORT CTCPClient& operator=(const CTCPClient& other) = delete;

	ENL_EXPORT size_t Read(unsigned char* pBuf, size_t nBufSiz);
	ENL_EXPORT size_t Write(const unsigned char* pBuf, size_t nBufSiz);

	ENL_EXPORT size_t available() const;

protected:
	socket_t OpenSocket(std::string& addr, unsigned short iPort);

private:
	socket_t m_iSocket;
	std::string m_addr;
	unsigned short m_iPort;
};

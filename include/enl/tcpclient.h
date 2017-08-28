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

protected:
	socket_t OpenSocket(std::string& addr, unsigned short iPort);

private:
	socket_t m_iSocket;
	std::string m_addr;
	unsigned short m_iPort;
};

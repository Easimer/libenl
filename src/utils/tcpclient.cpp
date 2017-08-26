#include <enl/platform.h>
#if defined(PLAT_LINUX)
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#elif defined(PLAT_WINDOWS)
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#endif
#include <string>
#include <enl/tcpclient.h>

#if defined(PLAT_WINDOWS)
void NETWORK_INIT()
{
	WSADATA wsadata;
	unsigned iRes = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iRes != 0)
	{
		std::cerr << "TCPClient: WSAStartup failed with error " << iRes << std::endl;
		return;
	}
}

void NETWORK_SHUTDOWN()
{
	WSACleanup();
}
#endif

CTCPClient::CTCPClient(std::string & addr, unsigned short port)
{
	m_iSocket = OpenSocket(addr, port);
}

CTCPClient::~CTCPClient()
{
	if (m_iSocket != INVALID_SOCKET)
	{
#if defined(PLAT_LINUX)
		close(m_iSocket);
#elif defined(PLAT_WINDOWS)
		closesocket(m_iSocket);
#endif
	}
	m_iSocket = INVALID_SOCKET;
}

size_t CTCPClient::Read(unsigned char * pBuf, size_t nBufSiz)
{
	if (m_iSocket == INVALID_SOCKET)
		return 0;
#if defined(PLAT_WINDOWS) || defined(PLAT_LINUX)
	return recv(m_iSocket, (char*)pBuf, nBufSiz, 0);
#else
	return 0;
#endif
}

size_t CTCPClient::Write(const unsigned char * pBuf, size_t nBufSiz)
{
	if (m_iSocket == INVALID_SOCKET)
		return 0;
#if defined(PLAT_WINDOWS) || defined(PLAT_LINUX)
	return send(m_iSocket, (const char*)pBuf, nBufSiz, 0);
#else
	return 0;
#endif
}

socket_t CTCPClient::OpenSocket(std::string & addr, unsigned short iPort)
{
#if defined(PLAT_WINDOWS)
	struct addrinfo *result = NULL;
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;
	unsigned iResult;
	socket_t iRet;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(addr.c_str(), std::to_string(iPort).c_str(), &hints, &result);

	if (iResult != 0)
	{
		std::cerr << "TCPClient: getaddrinfo failed" << std::endl;
		return INVALID_SOCKET;
	}

	for (ptr = result; ptr; ptr = ptr->ai_next)
	{
		iRet = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (iRet == INVALID_SOCKET)
		{
			std::cerr << "TCPClient: cannot create socket: " << iRet << std::endl;
			return INVALID_SOCKET;
		}

		iResult = connect(iRet, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			std::cerr << "TCPClient: cannot connect: " << WSAGetLastError() << std::endl;
			closesocket(iRet);
			iRet = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result);

	if (iRet == INVALID_SOCKET)
	{
		std::cerr << "TCPClient: cannot create socket: " << iRet << std::endl;
		return INVALID_SOCKET;
	}

	return iRet;

#elif defined(PLAT_LINUX)
	socket_t iRet;
	struct sockaddr_in server;
	iRet = socket(AF_INET, SOCK_STREAM, 0);
	if (iRet == INVALID_SOCKET)
		return INVALID_SOCKET;
	if (inet_addr(addr.c_str()) == -1)
	{
		struct hostent *he;
		struct in_addr **addr_list;
		if ((he = gethostbyname(addr.c_str())) == NULL)
		{
			herror("TCPClient: Cannot get host by name");
			close(iRet);
			iRet = INVALID_SOCKET;
			return INVALID_SOCKET;
		}
		addr_list = (struct in_addr **) he->h_addr_list;
		for (int i = 0; addr_list[i] != NULL; i++)
		{
			//strcpy(ip , inet_ntoa(*addr_list[i]) );
			server.sin_addr = *addr_list[i];
			break;
		}
	}
	else
		server.sin_addr.s_addr = inet_addr(addr.c_str());
	server.sin_family = AF_INET;
	server.sin_port = htons(iPort);
	if (connect(iRet, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("TCPClient: Connect failed");
		close(iRet);
		iRet = INVALID_SOCKET;
	}
	return iRet;
#else
	return 0;
#endif
}

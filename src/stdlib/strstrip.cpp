#include <enl/strstrip.h>
#include <cstdlib>

char* strnlstrip(char* szTarget, size_t nSize)
{
	size_t nCounter = 0;
	for (size_t i = 0; i < nSize; i++)
	{
		switch (szTarget[i])
		{
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			szTarget[i] = '\0';
			nCounter++;
			break;
		default:
			return szTarget + i;
		}
	}
	return szTarget + nSize - 1;
}
size_t strnrstrip(char * szTarget, size_t nSize)
{
	size_t nCounter = 0;
	for (size_t i = nSize - 1; i > 0; i--)
	{
		switch (szTarget[i])
		{
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			szTarget[i] = '\0';
		case '\0':
			nCounter++;
			break;
		default:
			return nCounter;
		}
	}
	return nCounter;
}
size_t strnstrip(char * szTarget, size_t nSize)
{
	return (strnlstrip(szTarget, nSize) - szTarget) + strnrstrip(szTarget, nSize);
}

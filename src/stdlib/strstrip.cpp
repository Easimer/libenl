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

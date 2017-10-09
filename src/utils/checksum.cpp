#include <enl/checksum.h>

uint16_t checksum_ip4(const uint8_t* pData, size_t nSize)
{
	uint32_t nSum = 0;
	size_t iIndex;
	
	for(iIndex = 0; iIndex < nSize - 1; iIndex += 2)
	{
		nSum += *(uint16_t*)(pData + iIndex);
	}
	
	if(nSize & 1)
	{
		nSum += (uint8_t)pData[iIndex];
	}
	
	while (nSum >> 16) nSum = (nSum & 0xFFFF)+(nSum >> 16);
	
	return ~nSum;
}

#pragma once

#include <cstdint>
#include <cstdlib>

uint16_t checksum_ip4(const uint8_t* pData, size_t nSize);

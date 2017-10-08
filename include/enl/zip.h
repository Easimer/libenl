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

#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <enl/platform.h>
#include <enl/hash_otp.h>

// enlzip

// enlzip is an uncompressed archive format.
// there are no directories and no filenames
// only the hashes of the file path are stored on write, therefore the
// user/program must know the "path" to a file to access it.
// an enlzip archive is read-only after it's written.
// hashes are calculated using a modified jenkins hash algorithm,
// but this may change in the future.

// an archive starts with all the file data, followed by the
// path hash table, then the hash OTP table and finally the header.

// enlzip
// ===
// data
// hash table
// hash otp table
// enlzip header

// file data is aligned on a 512-byte boundary. there may be unused space for
// future additions to the archive (if it's not closed).
// the hash table contains the hash of a file's path, where the file data begins
// and the size of that data.
// the OTP table is a "one-time pad". Used by the hashing algorithm.
// sizes of these two tables are stored in the header at the end.

// enlzip archives itself can be compressed using external compression
// utilities like gzip or xz.

#define IDENLZIP	(('P' << 24) + ('I' << 16) + ('Z' << 8) + 'E')

// Archive flags
// The archived is closed, adding further data and file entry is
// impossible without complete reconstruction.
#define ENLZIP_A_CLOSED 0x0001

// File flags
// Empty entry
#define ENLZIP_F_EMPTY 0x0001

// Header
// Ends an enlzip
// Contains the signature
START_PACK
struct ezip_header {
	uint32_t reserved4;
	uint32_t reserved3;
	uint32_t reserved2;
	uint32_t reserved1;

	uint32_t iFlags;
	uint32_t nOTPLen; // OTP table length (bytes)
	uint32_t nHTableLen; // Hash table length (pcs)
	uint32_t iID; // IDENLZIP
} PACKED;
END_PACK

// Hashmap entries
// path_hash is the hash of the path to the file
// calculated using the OTP table above.
// See <enl/hash_otp.h>.
START_PACK
struct ezip_hentry {
	uint32_t nPathHash;
	uint32_t nDataOffset; // MUST be aligned on 512 byte boundary
	uint32_t nDataSize;
	uint32_t iFlags; // see ENLZIP_F_ defines
	uint32_t iOwnerUser;
	uint32_t iOwnerGroup;
	uint32_t nPermissions;
} PACKED;
END_PACK

struct enlzip_write_request {
	uint32_t nHash;
	size_t nOffset;
	size_t nSize;
	uint32_t iOwnerUser;
	uint32_t iOwnerGroup;
	uint32_t nPermissions;
};

typedef struct enlzip_write_request enlzip_file;

class enlzip_writer {
public:
	enlzip_writer(const char* szFilename);
	~enlzip_writer();

	ssize_t write(const char* szPath, const char* pData, size_t nLength);
	ssize_t write(const char* szFilename, const char* pData, size_t nLength, uint32_t iOwnerUser, uint32_t iOwnerGroup, uint32_t nPermissions);

	void close();

private:
	std::ofstream m_file;
	std::vector<enlzip_write_request> m_reqs;
	size_t m_iByte = 0;

	hash m_hash;
};

class enlzip_reader {
public:
	enlzip_reader(const char* szFilename);

	uint32_t open(const char* szPath);
	size_t size(uint32_t pFile);
	ssize_t read(uint32_t pFile, uint8_t* pDst);

	uint32_t owner_user(uint32_t pFile);
	uint32_t owner_group(uint32_t pFile);
	uint32_t permissions(uint32_t pFile);

private:
	std::ifstream m_file;
	hash m_hash;
	std::vector<enlzip_file> m_files;

	size_t m_nOTPLen, m_nHTableLen;
	// Offset to HTable from end of file
	size_t m_iHTableOff;
};


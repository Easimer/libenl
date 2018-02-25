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

#include <mutex>
#include <atomic>

/*
	A thread-safe command buffer

	T - command struct, should be as small as possible
	nSize - size of the buffer
	bClearOnEndRead - should the front buffer be cleared on EndRead
	bSortBeforeCopy - should the back buffer be sorted before Copy; 
commands are sorted by the first 8 bytes of the commands
*/
template<class T, unsigned nSize, bool bClearOnEndRead = false, bool bSortBeforeCopy = false>
class cmdbuf
{
public:
	cmdbuf()
	{
		m_pBackBuffer = new T[nSize];
		m_pFrontBuffer = new T[nSize];
		m_bForceCopy = true;
		m_bClosed = false;
	}
	~cmdbuf()
	{
		delete m_pFrontBuffer;
		delete m_pBackBuffer;
	}

	void begin_write()
	{
		m_BackBufferLock.lock();
		//std::cerr << "CMDBUF-W: Back buffer locked" << std::endl;
		m_nBackBufferCount = 0;
	}
	void write(T& cmd)
	{
		if (m_nBackBufferCount == nSize)
		{
			std::cerr << "CMDBUF-W: Back buffer is full!!!" << std::endl;
			return;
		}
		memcpy(m_pBackBuffer + m_nBackBufferCount, &cmd, sizeof(T));

		unsigned char* pCmd = (unsigned char*)&cmd;
		for (int i = 0; i < sizeof(T); i++)
		{
			m_nChecksum = (m_nChecksum >> 1) + ((m_nChecksum & 1) << 15);
			m_nChecksum += pCmd[i];
			m_nChecksum &= 0xFFFF;
		}

		m_nBackBufferCount++;
	}
	void end_write()
	{
		m_BackBufferLock.unlock();
		//std::cerr << "CMDBUF-W: Back buffer unlocked" << std::endl;
		copy();
	}

	bool begin_read(T** pCmdBuf, size_t* nCount)
	{
		bool bLocked = m_FrontBufferLock.try_lock();
		if (!bLocked)
			return false;
		//std::cerr << "CMDBUF-R: Front buffer locked" << std::endl;
		*pCmdBuf = m_pFrontBuffer;
		*nCount = m_nFrontBufferCount;
		return true;
	}
	void end_read()
	{
		if(bClearOnEndRead)
			m_nFrontBufferCount = 0;
		m_FrontBufferLock.unlock();
		//std::cerr << "CMDBUF-R: Front buffer unlocked" << std::endl;
	}

	void close()
	{
		m_bClosed = true;
	}

	bool is_closed() const
	{
		return m_bClosed;
	}

	bool is_empty() const
	{
		return m_nFrontBufferCount == 0;
	}

private:
	void sort()
	{
		#define _BBSORTKEY(i) *((size_t*)&m_pBackBuffer[i])
		size_t i = 1;
		while (i < m_nBackBufferCount)
		{
			size_t j = i;
			while (j > 0 && _BBSORTKEY(j - 1) > _BBSORTKEY(j))
			{
				T t;
				memcpy(&t, m_pBackBuffer + j, sizeof(T));
				memcpy(m_pBackBuffer + j, m_pBackBuffer - 1, sizeof(T));
				memcpy(m_pBackBuffer + j, m_pBackBuffer + (j - 1), sizeof(T));
				memcpy(m_pBackBuffer + (j-1), &t, sizeof(T));
				j--;
			}
			i++;
		}
	}

	void copy()
	{
		m_BackBufferLock.lock();
		//std::cerr << "CMDBUF-C: Back buffer locked" << std::endl;
		m_FrontBufferLock.lock();
		//std::cerr << "CMDBUF-C: Front buffer locked" << std::endl;
		if (m_bForceCopy || (m_nChecksum != m_nLastChecksum))
		{
			if (bSortBeforeCopy)
			{
				if (m_nBackBufferCount != 0)
				{
					sort();
				}
			}
			memcpy(m_pFrontBuffer, m_pBackBuffer, m_nBackBufferCount * sizeof(T));
			m_nFrontBufferCount = m_nBackBufferCount;
			m_bForceCopy = false;
		}
		else
		{
			//std::cerr << "CMDBUF-C: No copy needed" << std::endl;
		}
		m_nFrontBufferCount = m_nBackBufferCount;
		m_nLastChecksum = m_nChecksum;
		m_FrontBufferLock.unlock();
		//std::cerr << "CMDBUF-C: Front buffer unlocked" << std::endl;
		m_BackBufferLock.unlock();
		//std::cerr << "CMDBUF-C: Back buffer unlocked" << std::endl;
	}

private:
	T* m_pBackBuffer;
	size_t m_nBackBufferCount;
	T* m_pFrontBuffer;
	size_t m_nFrontBufferCount;
	std::mutex m_BackBufferLock; // prevents copying to or from the back buffer; locked by BeginWrite or BeginCopy; opened by EndWrite or EndCopy
	std::mutex m_FrontBufferLock; // prevents copying to or from the front buffer; locked by BeginRead or BeginCopy; opened by EndRead or EndCopy

	unsigned long m_nChecksum;
	unsigned long m_nLastChecksum;
	bool m_bForceCopy;

	std::atomic<bool> m_bClosed;
};

// Store debug caller info in commands

#ifdef PLAT_DEBUG

typedef struct cmdbuf_debug_t {
	char szFunction[128];
	long iLine;
} cmdbuf_debug_t;

#define CMDBUF_DEBUG_DECLARE {__FUNCTION__, __LINE__}

#define CMDBUF_DEBUG cmdbuf_debug_t callerinfo;

#else

#define CMDBUG_DEBUG_DECLARE
#define CMDBUF_DEBUG

#endif

#define CMDBUF_BEGIN_CMD(name) typedef struct name {
#define CMDBUF_END_CMD(name) \
		CMDBUF_DEBUG; \
	} name;

#define CMDBUF_DEF(name, T, nSize, bClearOnEndRead, bSortBeforeCopy) class name : public cmdbuf<T, nSize, bClearOnEndRead, bSortBeforeCopy> {}

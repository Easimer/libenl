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

#include <enl/hash_otp.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <iostream>

template<class T>
struct hashmap_elem {
	hashmap_elem() : hash(0), data(NULL), next(NULL) {}
	uint32_t hash;
	T* data;
	void* next;
};

template<class T>
class hashmap {
public:
	hashmap() :	m_pHead(NULL), m_pTail(NULL), m_collcnt(0) {}
	~hashmap()
	{
		hashmap_elem<T>* next = NULL;
		hashmap_elem<T>* cur = NULL;
		next = m_pHead;
		cur = next;
		while(next)
		{
			cur = next;
			next = (hashmap_elem<T>*)cur->next;
			delete cur;
		}
		if(m_collcnt > 0)
		{
			std::cerr << "hashmap: " << m_collcnt << " collisions have happened" << std::endl;
		}
	}

	T& operator[](uint32_t key)
	{
		hashmap_elem<T>* cur = NULL;
		if(!m_pHead)
			throw std::out_of_range("empty hashmap");
		cur = m_pHead;
		while(cur)
		{
			if(cur->hash == key)
				return *cur->data;
			cur = (hashmap_elem<T>*)cur->next;
		}
		throw std::out_of_range("no such key");
	}

	T& operator[](const char* key)
	{
		uint32_t hash = m_hash.get_hash((uint8_t*)key, strlen(key) + 1);
		return (*this)[hash];
	}

	void push_back(const char* szKey, const T* data)
	{
		hashmap_elem<T>* elem = NULL;
		uint32_t hash;
		uint32_t nKeyLen;

		elem = new hashmap_elem<T>;
		if(!elem)
			return;
		if(!m_pTail)
		{
			m_pHead = elem;
			m_pTail = elem;
		}
		else
		{
			m_pTail->next = elem;
			m_pTail = elem;
		}
		nKeyLen = strlen(szKey) + 1;
		hash = m_hash.get_hash((uint8_t*)szKey, nKeyLen);

		elem->data = (T*)data;
		elem->hash = hash;

		// Sanity check

		hashmap_elem<T>* sc = m_pHead;
		while(sc)
		{
			if(sc != elem)
			{
				if(sc->hash == elem->hash)
				{
					std::cerr << "HASH COLLISION!" << std::endl;
					m_collcnt++;
				}
			}
			sc = (hashmap_elem<T>*)sc->next;
		}

		//std::cout << "Data with key " << szKey << " added!" << std::endl;
	}

	void dump()
	{
		hashmap_elem<T>* cur = NULL;
		if(!m_pHead)
			return;
		cur = m_pHead;
		while(cur)
		{
			std::cout << "Key-hash: " << cur->hash << " Data: " << cur->data << std::endl;
			cur = (hashmap_elem<T>*)cur->next;
		}
	}

private:
	hash m_hash;
	hashmap_elem<T>* m_pHead = NULL;
	hashmap_elem<T>* m_pTail = NULL;

	uint64_t m_collcnt;
};


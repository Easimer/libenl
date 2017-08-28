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
#include <enl/config.h>
#include <enl/strstrip.h>
#include <enl/stdlib.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

void CConfig::Load(const char * szPath)
{
	std::string line;
	std::ifstream file(szPath);

	if (!file.is_open())
		return;
	while (!file.eof())
	{
		std::getline(file, line);
		for (size_t i = 0; i < line.size(); i++)
		{
			if (line[i] == ' ')
			{
				char buf[512];
				enl_strncpy(buf, line.c_str(), 512);
				buf[i] = '\0';
				strnrstrip(&buf[i + 1], 511 - i);
				auto pair = m_map.emplace(buf, &buf[i + 1]);
				break;
			}
		}
	}
}

const char * CConfig::GetValue(const char * szKey)
{
	return m_map[szKey].c_str();
}

CConfig* gpConfig;

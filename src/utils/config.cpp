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

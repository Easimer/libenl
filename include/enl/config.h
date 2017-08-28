#pragma once

#include <string>
#include <map>

class CConfig
{
public:
	void Load(const char* szPath = "config.conf");
	const char* GetValue(const char* szKey);

private:
	std::map<std::string, std::string> m_map;
};

extern CConfig* gpConfig;

#define CONFIG_INIT() gpConfig = new CConfig();
#define CONFIG_SHUTDOWN() delete gpConfig;
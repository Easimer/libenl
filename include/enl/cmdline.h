#pragma once

#include <vector>
#include <string>

typedef struct cmdline_arg {
	char chFlag;
	const char* szArg;
} cmdline_arg;

class CCommandLine
{
public:
	CCommandLine(int argc, char** argv);

	bool IsFlagPresent(char chFlag);
	const char* GetArgument(char chFlag = '\0');
	const char* GetExecName();

private:
	void PushArgument(char chFlag, const char* szValue);

private:
	std::string m_execname;
	std::vector<cmdline_arg> m_args;
};

extern CCommandLine* gpCmdline;

#define CMDLINE_INIT() gpCmdline = new CCommandLine(argc, argv);
#define CMDLINE_SHUTDOWN() delete gpCmdline;

#pragma once

#include <enl/platform.h>
#include <vector>
#include <string>

typedef struct cmdline_arg {
	char chFlag;
	const char* szArg;
} cmdline_arg;

class CCommandLine
{
public:
	ENL_EXPORT CCommandLine(int argc, char** argv);

	ENL_EXPORT bool IsFlagPresent(char chFlag);
	ENL_EXPORT const char* GetArgument(char chFlag = '\0');
	ENL_EXPORT const char* GetExecName();

private:
	void PushArgument(char chFlag, const char* szValue);

private:
	std::string m_execname;
	std::vector<cmdline_arg> m_args;
};

ENL_EXPORT extern CCommandLine* gpCmdline;

#define CMDLINE_INIT() gpCmdline = new CCommandLine(argc, argv);
#define CMDLINE_SHUTDOWN() delete gpCmdline;

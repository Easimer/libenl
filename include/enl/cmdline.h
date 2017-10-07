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

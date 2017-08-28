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

#include <enl/cmdline.h>

CCommandLine::CCommandLine(int argc, char ** argv)
{
	const char* flag = NULL;
	m_execname = argv[0];
	for (int i = 1; i < argc; i++)
	{
		const char* arg = argv[i];
		if (arg[0] == '-') // current arg is a flag
		{
			if (flag) // last arg was a flag too, add it
			{
				PushArgument(arg[1], NULL);
			}
			flag = arg;
		}
		else
		{
			if (flag)
			{
				PushArgument(flag[1], arg);
				flag = NULL;
			}
			else
			{
				PushArgument('\0', arg);
			}
		}
	}
	if (flag)
	{
		PushArgument(flag[1], NULL);
	}
}

bool CCommandLine::IsFlagPresent(char chFlag)
{
	for (auto& arg : m_args)
	{
		if (arg.chFlag == chFlag)
			return true;
	}
	return false;
}

const char * CCommandLine::GetArgument(char chFlag)
{
	for (auto& arg : m_args)
	{
		if (arg.chFlag == chFlag)
			return arg.szArg;
	}
	return NULL;
}

const char * CCommandLine::GetExecName()
{
	return m_execname.c_str();
}

void CCommandLine::PushArgument(char chFlag, const char * szValue)
{
	cmdline_arg a;
	a.chFlag = chFlag;
	a.szArg = szValue;
	m_args.push_back(a);
}

CCommandLine* gpCmdline;

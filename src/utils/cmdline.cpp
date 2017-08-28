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

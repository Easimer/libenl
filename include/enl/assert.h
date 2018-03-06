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

#if defined(PLAT_DEBUG)

#include <iostream>

#define WIDE2(x) L##x
#define WIDE1(x) WIDE2(x)
#define WFILE WIDE1(__FILE__)
#define WSIGN WIDE1(__FUNCSIG__)

#if defined(PLAT_WINDOWS) // Win32

#include <windows.h>
#include <cwchar>

#define ASSERT(expr) \
	{ \
		if(!(expr)) \
		{ \
			wchar_t errormsg[512]; \
			swprintf(errormsg, 512, L"Assertion failed: %s\nBreak in debugger?\n\nLine: %u\nFile: %s\nFunction:\n%s\n", WIDE1(#expr), __LINE__, WFILE, WSIGN); \
			int nRet = MessageBoxW( NULL, errormsg, L"Engine Error",  MB_YESNOCANCEL | MB_ICONWARNING | MB_DEFBUTTON1); \
			if(nRet == IDYES) \
				DebugBreak(); \
			else if (nRet == IDCANCEL) \
				exit(1); \
		} \
	}

#define ASSERT_CUSTOM(expr, msg) \
	{ \
		if(!(expr)) \
		{ \
			wchar_t errormsg[512]; \
			swprintf(errormsg, 512, L"Assertion failed: %s\nBreak in debugger?\n\nLine: %u\nFile: %s\nFunction:\n%s\n\n%s", WIDE1(#expr), __LINE__, WFILE, WSIGN, msg); \
			int nRet = MessageBoxW( NULL, errormsg, L"Engine Error",  MB_YESNOCANCEL | MB_ICONWARNING | MB_DEFBUTTON1); \
			if(nRet == IDYES) \
				DebugBreak(); \
			else if (nRet == IDCANCEL) \
				exit(1); \
		} \
	}

#define ASSERT_ERRORFUNC(expr, errorfunc) \
	{ \
		if(!(expr)) \
		{ \
			wchar_t errormsg[512]; \
			swprintf(errormsg, 512, L"Assertion failed, SDL error printed in console.\nBreak in debugger?\n\nLine: %u\nFile: %s\nFunction:\n%s\n", __LINE__, WFILE, WSIGN); \
			std::cerr << #errorfunc << "(): " << errorfunc() << std::endl; \
			int nRet = MessageBoxW( NULL, errormsg, L"Engine Error", MB_YESNOCANCEL | MB_ICONWARNING | MB_DEFBUTTON1); \
			if(nRet == IDYES) \
				DebugBreak(); \
			else if (nRet == IDCANCEL) \
				exit(1); \
		} \
	}

#define ASSERT_SDL2(expr) ASSERT_ERRORFUNC(expr, SDL_GetError)
#define ASSERT_MIXER(expr) ASSERT_ERRORFUNC(expr, Mix_GetError)
#define ASSERT_IMAGE(expr) ASSERT_ERRORFUNC(expr, IMG_GetError)

#elif defined(PLAT_LINUX) || defined(PLAT_HTML5) // Linux/HTML5

#include <assert.h>

#define ASSERT(expr) assert(expr);
#define ASSERT_ERRORFUNC(expr, errorfunc) \
	if(!(expr)) \
	{ \
		std::cerr << #errorfunc << "(): " << errorfunc() << std::endl; \
		assert(expr); \
	}

#define ASSERT_SDL2(expr) ASSERT_ERRORFUNC(expr, SDL_GetError)
#define ASSERT_MIXER(expr) ASSERT_ERRORFUNC(expr, Mix_GetError)
#define ASSERT_IMAGE(expr) ASSERT_ERRORFUNC(expr, IMG_GetError)

#else

#warning Unknown platform!

#define ASSERT(expr) \
	if(!(expr)) \
	{ \
		std::cout << "Assertion failed: " << (#expr) << std::endl; \
	}

#define ASSERT_SDL2(expr) \
	if(!(expr)) \
	{ \
		std::cerr << "SDL_GetError(): " << SDL_GetError() << std::endl; \
		assert(expr); \
	}

#endif
#else
#define ASSERT(expr)
#define ASSERT_SDL2(expr)
#define ASSERT_MIXER(expr)
#define ASSERT_IMAGE(expr)
#endif

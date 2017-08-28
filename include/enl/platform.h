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

#if defined(_WIN32)
#define PLAT_WINDOWS
#endif

#if defined(__linux__)
#define PLAT_LINUX
#endif

#if defined(__EMSCRIPTEN__)
#define PLAT_HTML5
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define PLAT_OSX
#endif

#if defined(_DEBUG) || defined(DEBUG) || defined(NDEBUG)
#define PLAT_DEBUG
#endif

#if defined(PLAT_WINDOWS)
#if defined(ENL_LIBRARY)
#define ENL_EXPORT __declspec(dllexport)
#else
#define ENL_EXPORT __declspec(dllimport)
#endif
#else
#define ENL_EXPORT
#endif

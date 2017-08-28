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

#if defined(_DEBUG) || defined(DEBUG) || defined(NDEBUG)
#define PLAT_DEBUG
#endif

#if defined(PLAT_WINDOWS)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

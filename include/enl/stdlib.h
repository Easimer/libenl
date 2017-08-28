#pragma once
// Platform specific stdlib related stuff

#if defined(PLAT_WINDOWS)
#define enl_strncpy(d, s, n) strncpy_s(d, n, s, n)

#else
#define enl_strncpy(d, s, n) strncpy(d, s, n)
#endif

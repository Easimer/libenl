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
// Platform specific stdlib related stuff

#if defined(PLAT_WINDOWS)
#define enl_strncpy(d, s, n) strncpy_s(d, n, s, n)

#else
#define enl_strncpy(d, s, n) strncpy(d, s, n)
#endif

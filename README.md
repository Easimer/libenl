# libenl
Mostly C++ snippets, stuff like multi-plat TCP client, command,
line argument handler, header with platform defines, etc.

`#include <enl/assert.h>`

Defines assert macros. Multiplatform; on Windows it shows a 
messagebox, offering a debugger break. Has functions for SDL2 
and it's extensions, that also show the current error, a.k.a. the 
string returned by SDL_GetError, IMG_GetError, etc.

`#include <enl/stdlib.h>`

`#include <enl/platform.h>`

Defines PLAT_WINDOWS, PLAT_LINUX, PLAT_HTML5 on each platform. 
Defines PLAT_DEBUG when the project is in Debug mode.

`#include <enl/cmdline.h>`

Defines a class that handles command-line arguments. Programs can 
query the presence of flags, or get the argument string associated 
with that flag.

Offers macros to create and destroy a global copy.

`#include <enl/config.h>`

Defines a class that reads a basic key-value configuration file. 
Keys are the first words on a line. Value is the rest of the string. 
Each value string is stripped of whitespace.

Offers macros to create and destroy a global copy.

`#include <enl/strstrip.h>`

Defines `strnlstrip`, `strnrstrip` and `strnstrip`, that strip 
the whitespace from a string from the left, from the left and from 
both sides, respectively.

`#include <enl/tcpclient.h>`

Multiplatform TCP client.

Defines macros that initialize Winsock on Windows (and do nothing on 
other platforms).

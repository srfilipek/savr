#ifndef _savr_version_h_Included_
#define _savr_version_h_Included_
/*********************************************************************************
 Copyright (C) 2011 by Stefan Filipek

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*********************************************************************************/

/**
 * @file version.h
 */
 #define __str(s) #s
 #define __xstr(s) __str(s)


#define SAVR_MAJOR   1  ///< Major version number
#define SAVR_MINOR   2  ///< Minor version number
#define SAVR_DEVEL
#define SAVR_VERSION (((uint16_t)SAVR_MAJOR << 8) | SAVR_MINOR)  ///< Full version 16-bit word

#define SAVR_TARGET_STR     __xstr(MCU)
#if defined(SAVR_DEVEL)
#define SAVR_VERSION_STR    __xstr(SAVR_MAJOR) "." __xstr(SAVR_MINOR) "-Dev"
#else
#define SAVR_VERSION_STR    __xstr(SAVR_MAJOR) "." __xstr(SAVR_MINOR)
#endif

#endif /* _savr_version_h_Included_ */

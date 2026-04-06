/*

@TODO(Honza): Description etc.

=====================================================================

The MIT License (MIT)
---------------------

Copyright © 2025 Honza Kříž

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the “Software”),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

=====================================================================

*/

#include <stdint.h>

#if defined(_WIN64)
	#define WIN32_MEAN_AND_LEAN
	#include <Windows.h>
#else
	#error TODO: Implement for other platforms than 64-bit Windows!
#endif

int64_t get_miliseconds()
{
#if defined(_WIN64)
	LARGE_INTEGER ticks_per_second;
	LARGE_INTEGER ticks;

	// @NOTE(Honza): This should be fine for a metronome.
	// But if this should become a serious timing
	// function for other purposes, it should be refactored.
	//
	// For example, the QueryPerformanceFrequency gets us
	// a constant value, that can be cached, to avoid
	// calling it every time we call our get_miliseconds function.
	QueryPerformanceFrequency(&ticks_per_second);

/*
	Why is there no error checking here?
	Quote from Microsoft's documentation:

	"""
	If the function succeeds, the return value is nonzero.

	If the function fails, the return value is zero.
	To get extended error information, call GetLastError.
	On systems that run Windows XP or later, the function will
	always succeed when given valid parameters
	and will thus never return zero.
	"""
*/
	QueryPerformanceCounter(&ticks);
	return ticks.QuadPart / (ticks_per_second.QuadPart / 1000);

#else
	#error TODO: this function is currently not implemented for this platform!
#endif

}

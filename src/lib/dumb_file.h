/* ============================================================================

dumb_file.h - basic file I/O & utilities.

===============================================================================

version 0.1.3
Copyright © 2025 Honza Kříž

https://github.com/JKKross
https://x.com/honza_kriz_bass

===============================================================================

	Index of this file
	------------------

	|SECTION| - LICENSE
	|SECTION| - DOCUMENTATION
	|SECTION| - COMPILE CHECKS
	|SECTION| - INCLUDES
	|SECTION| - DECLARATIONS
		- |TYPES|
		- |FUNCTIONS|
	|SECTION| - IMPLEMENTATION

===============================================================================

	|SECTION| - LICENSE
	----------------------------

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

===============================================================================

	|SECTION| - DOCUMENTATION
	----------------------------

Most "documentation" for this library is (at least for now) the code itself.
But a few notes:

- 1)
You _MUST_ '#define DUMB_FILE_IMPLEMENTATION'
in EXACTLY _one_ C or C++ file that includes this header, BEFORE the
include, like this:

	#define DUMB_FILE_IMPLEMENTATION
	#include "dumb_file.h"

All other files should just #include "dumb_file.h" without the #define.

============================================================================ */

/*
	|SECTION| - COMPILE CHECKS
	--------------------------
*/

#if !(__GNUC__ || __CLANG__ || _MSC_VER || __TINYC__ )
	#error YOU ARE USING A COMPILER THIS LIBRARY WAS NOT TESTED WITH! \
If you wish to proceed, read through the source, run the tests & make sure everything works correctly!
#endif

#if !(defined(_WIN64) || defined(__linux__))
	#error THIS LIBRARY HAS NOT BEEN TESTED ON THIS PLATFORM! \
If you wish to proceed, read through the source, run the tests & make sure everything works correctly!
#endif

/*
	|SECTION| - INCLUDES
	--------------------
*/

/*
@NOTE(Honza): For file IO. I will most likely rewrite this
with some platform-specific APIs for the platforms I test on.
But maybe I'll keep stdio.h in as a somewhat safe default for
othe platforms.

Either way, the point is to abstract all the platform
specific behaviour away.
*/
#include <stdio.h>

/*
	|SECTION| - DECLARATIONS
	------------------------
*/

#ifdef __cplusplus
extern "C" {
#endif

/* --- |MACROS| --- */

#define DUMB_FILE_KB(n)  (n * 1024)
#define DUMB_FILE_MB(n)  (n * 1048576)
#define DUMB_FILE_GB(n)  (n * 1073741824)
#define DUMB_FILE_TB(n)  (n * 1099511627776)

/* --- |TYPES| --- */

typedef enum Dumb_File_Result
{
	DUMB_FILE_SUCCESS,
	DUMB_FILE_UNKNOWN_FAILURE,
	DUMB_FILE_COULD_NOT_BE_OPENED,
	DUMB_FILE_COULD_NOT_BE_CLOSED,
	DUMB_FILE_CANNOT_WRITE,
} Dumb_File_Result;

typedef enum Dumb_File_Write_Mode
{
	DUMB_FILE_APPEND,
	DUMB_FILE_OVER_WRITE,
} Dumb_File_Write_Mode;

/* --- |FUNCTIONS| --- */

void             dumb_file_windows_path_to_unix_path(char *path);
void             dumb_file_unix_path_to_windows_path(char *path);
int              dumb_file_exists(char *path);
Dumb_File_Result dumb_file_read_bytes(char *path, unsigned char *output_buffer, size_t output_buffer_capacity, size_t *bytes_read);
Dumb_File_Result dumb_file_save_bytes(char *path, unsigned char *bytes, size_t num_bytes, Dumb_File_Write_Mode write_mode);

/*
	|SECTION| - IMPLEMENTATION
	--------------------------
*/

#ifdef DUMB_FILE_IMPLEMENTATION

/* --- |FUNCTIONS| --- */

void
dumb_file_windows_path_to_unix_path(char *path)
{
	while (*path != '\0')
	{
		if (*path == '\\') { *path = '/'; }
		path++;
	}
}

void
dumb_file_unix_path_to_windows_path(char *path)
{
	while (*path != '\0')
	{
		if (*path == '/') { *path = '\\'; }
		path++;
	}
}

/*
Returns 1 if the file exists
Returns 0 if the file does not exist
Returns -1 on error (check implementation)
 */
int
dumb_file_exists(char *path)
{
	int result;
	FILE *file;

#ifdef _MSC_VER
	fopen_s(&file, path, "r");
#else
	file = fopen(path, "r");
#endif

	if (file == NULL)
	{
		return 0;
	}
	else
	{
		result = fclose(file);
		/* @TODO(Honza):
		   Should rewrite this in OS specific implementation,
		   since fclose can fail & the user should not get
		   an error when simply asking if a
		   file exists.
		*/
		if (result != 0) { return -1; }

		return 1;
	}
}

/*
   'bytes_read' is an optional argument - you can pass NULL.
*/
Dumb_File_Result
dumb_file_read_bytes(char *path, unsigned char *output_buffer, size_t output_buffer_capacity, size_t *bytes_read)
{
	size_t i, byte_count;
	int bin;
	unsigned char byte;

	FILE *file;

#ifdef _MSC_VER
	fopen_s(&file, path, "r");
#else
	file = fopen(path, "r");
#endif

	if (file == NULL) { return DUMB_FILE_COULD_NOT_BE_OPENED; }

	for (i = 0; i < output_buffer_capacity; i++)
	{
		bin = fgetc(file);
		if (bin == EOF) { break; }

		byte = (unsigned char)bin;
		output_buffer[i] = bin;
	}
	if (bytes_read != NULL) { *bytes_read = i; }

	i = fclose(file);
	if (i != 0) { return DUMB_FILE_COULD_NOT_BE_CLOSED; }

	return DUMB_FILE_SUCCESS;
}

Dumb_File_Result
dumb_file_save_bytes(char *path, unsigned char *bytes, size_t num_bytes, Dumb_File_Write_Mode write_mode)
{
	size_t i;
	int fputc_result;
	char *mode;
	FILE *file;

	switch (write_mode)
	{
		case DUMB_FILE_OVER_WRITE:
		{
			mode = "w";
		}
		break;
		case DUMB_FILE_APPEND:
		{
			mode = "a";
		}
		break;
		default:
		{
			return DUMB_FILE_CANNOT_WRITE;
		}
		break;
	}

#ifdef _MSC_VER
	fopen_s(&file, path, mode);
#else
	file = fopen(path, mode);
#endif

	if (file == NULL) { return DUMB_FILE_COULD_NOT_BE_OPENED; }

	for (i = 0; i < num_bytes; i++)
	{
		fputc_result = fputc((int)bytes[i], file);

		if (fputc_result == EOF) { return DUMB_FILE_CANNOT_WRITE; }
	}

	i = fclose(file);
	if (i != 0) { return DUMB_FILE_COULD_NOT_BE_CLOSED; }

	return DUMB_FILE_SUCCESS;
}


#endif /* DUMB_FILE_IMPLEMENTATION */

#ifdef __cplusplus
} /* extern "C" */
#endif

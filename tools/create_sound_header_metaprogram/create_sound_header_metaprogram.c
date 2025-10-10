/*

create_sound_header_metaprogram.c

A metaprogram that parses input WAV file(s), generates a struct
of the WAV data & creates a header file with the struct(s) for use in
the main program.

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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> // @NOTE: For "_set_fmode" on Windows

#define DUMB_LIB_IMPLEMENTATION
#include "dumb_lib.h"

#define HELP_MESSAGE "\
-h            - Display this help message.\n\
-f [FILE.wav] - Provide a file (or multiple files) to create a header file from.\n\
"

#define TEMPLATE "\n\
/*\n\
	THIS IS A GENERATED FILE, DO NOT CHANGE MANUALLY!!!\n\
*/\n\
\n\
typedef struct GM_Sound {\n\
	char           *name;\n\
	unsigned char   num_channels;\n\
	unsigned long   sample_rate;\n\
	unsigned long   byte_rate;\n\
	unsigned short  bits_per_sample;\n\
	unsigned long   data_size;\n\
	unsigned char  *data;\n\
} GM_Sound;\n\
\n\
void\n\
gm_init_sounds(Dumb_Arena *allocator, Dumb_Array *sounds_array);\n\
\n\
void\n\
gm_init_sounds(Dumb_Arena *arena, Dumb_Array *sounds_array)\n\
{\n\
	GM_Sound *gm_sound;\n\n\
"

typedef char                s8;
typedef short               s16;
typedef long                s32;
typedef long long           s64;

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned long       u32;
typedef unsigned long long  u64;

typedef float               f32;
typedef double              f64;

typedef struct WAV_FMT {
	/* The "fmt" sub-chunk */
	char SubchunkID[4];
	u32  SubchunkSize;
	u16  AudioFormat;
	u16  NumChannels;
	u32  SampleRate;
	u32  ByteRate;
	u16  BlockAlign;
	u16  BitsPerSample;
} WAV_FMT;

typedef struct WAV_DATA {
	/* The "data" sub-chunk */
	char SubchunkID[4];
	u32  SubchunkSize;
	u8  *data;
} WAV_DATA;

/*
   @NOTE(Honza): The RIFF format is little-endian,
   which should be fine for now. If we'd like to separate
   the parser into a standalone library though, there may
   be some issues in the future.
   RTFM in that case.
*/
typedef struct RIFF {
	/* The "RIFF" chunk descriptor */
	char     ChunkID[4];
	u32      ChunkSize;
	char     Format[4];
	WAV_FMT  fmt_chunk;
	WAV_DATA data_chunk;
} RIFF;

void read_file_into_buffer(Dumb_String *filename, Dumb_Array *file_buffer, Dumb_Arena *file_buffer_arena);

RIFF parse_wav_buffer(Dumb_Arena *allocator, Dumb_Array *file_buffer);

void save_dumb_string_to_file(const char *filename, Dumb_String *file_contents);

void
read_file_into_buffer(Dumb_String *filename, Dumb_Array *file_buffer, Dumb_Arena *file_buffer_arena)
{
	FILE *file;
	_set_fmode(_O_BINARY);
	fopen_s(&file, filename->_chars, "r");

	if (file == NULL)
	{
		printf("Couldn't read file \"%s\"\n", filename->_chars);
		fclose(file);
		return;
	}

	int bin = getc(file);
	u8  byte = (u8)bin;

	while (bin != EOF)
	{
		dumb_array_push(file_buffer_arena, file_buffer, &byte);

		bin = getc(file);
		byte = (u8)bin;
	}

	fclose(file);
}

RIFF
parse_wav_buffer(Dumb_Arena *allocator, Dumb_Array *file_buffer)
{
	size_t i;
	RIFF parsed_riff;

	// @TODO(Honza): All of this is extremely hacky, rewrite to be more fool proof!

	parsed_riff.ChunkID[0] = *(char *)dumb_array_get(file_buffer, 0);
	parsed_riff.ChunkID[1] = *(char *)dumb_array_get(file_buffer, 1);
	parsed_riff.ChunkID[2] = *(char *)dumb_array_get(file_buffer, 2);
	parsed_riff.ChunkID[3] = *(char *)dumb_array_get(file_buffer, 3);

	parsed_riff.ChunkSize = *(u32 *)dumb_array_get(file_buffer, 4);

	parsed_riff.Format[0] = *(char *)dumb_array_get(file_buffer, 8);
	parsed_riff.Format[1] = *(char *)dumb_array_get(file_buffer, 9);
	parsed_riff.Format[2] = *(char *)dumb_array_get(file_buffer, 10);
	parsed_riff.Format[3] = *(char *)dumb_array_get(file_buffer, 11);

	if (dumb_memcmp(parsed_riff.Format, "WAVE", 4))
	{
		// @TODO(Honza): Log? Skip? Crash?
		printf("ERROR: Wrong format! parsed_riff.Format == %s; %s %d\n", parsed_riff.Format, __FILE__, __LINE__);
	}

	parsed_riff.fmt_chunk.SubchunkID[0] = *(char *)dumb_array_get(file_buffer, 12);
	parsed_riff.fmt_chunk.SubchunkID[1] = *(char *)dumb_array_get(file_buffer, 13);
	parsed_riff.fmt_chunk.SubchunkID[2] = *(char *)dumb_array_get(file_buffer, 14);
	parsed_riff.fmt_chunk.SubchunkID[3] = *(char *)dumb_array_get(file_buffer, 15);

	if (dumb_memcmp(parsed_riff.fmt_chunk.SubchunkID, "fmt ", 4))
	{
		// @TODO(Honza): Log? Skip? Crash?
		printf("ERROR: parsed_riff.fmt_chunk.SubchunkID == %s; %s %d\n", parsed_riff.fmt_chunk.SubchunkID, __FILE__, __LINE__);
	}

	parsed_riff.fmt_chunk.SubchunkSize  = *(u32 *)dumb_array_get(file_buffer, 16);
	parsed_riff.fmt_chunk.AudioFormat   = *(u16 *)dumb_array_get(file_buffer, 20);

	if (dumb_memcmp(parsed_riff.fmt_chunk.SubchunkID, "fmt ", 4))
	{
		// @TODO(Honza): Log? Skip? Crash?
		printf("ERROR: parsed_riff.fmt_chunk.AudioFormat == %d, which suggest some level of compression. May not play correctly.\n", parsed_riff.fmt_chunk.AudioFormat);
	}

	parsed_riff.fmt_chunk.NumChannels   = *(u16 *)dumb_array_get(file_buffer, 22);
	parsed_riff.fmt_chunk.SampleRate    = *(u32 *)dumb_array_get(file_buffer, 24);
	parsed_riff.fmt_chunk.ByteRate      = *(u32 *)dumb_array_get(file_buffer, 28);
	parsed_riff.fmt_chunk.BlockAlign    = *(u16 *)dumb_array_get(file_buffer, 32);
	parsed_riff.fmt_chunk.BitsPerSample = *(u16 *)dumb_array_get(file_buffer, 34);

	parsed_riff.data_chunk.SubchunkID[0] = *(char *)dumb_array_get(file_buffer, 20 + parsed_riff.fmt_chunk.SubchunkSize + 0);
	parsed_riff.data_chunk.SubchunkID[1] = *(char *)dumb_array_get(file_buffer, 20 + parsed_riff.fmt_chunk.SubchunkSize + 1);
	parsed_riff.data_chunk.SubchunkID[2] = *(char *)dumb_array_get(file_buffer, 20 + parsed_riff.fmt_chunk.SubchunkSize + 2);
	parsed_riff.data_chunk.SubchunkID[3] = *(char *)dumb_array_get(file_buffer, 20 + parsed_riff.fmt_chunk.SubchunkSize + 3);

	if (dumb_memcmp(parsed_riff.data_chunk.SubchunkID, "data", 4))
	{
		// @TODO(Honza): Log? Skip? Crash?
		printf("ERROR: parsed_riff.data_chunk.SubchunkID == %s; %s %d\n", parsed_riff.data_chunk.SubchunkID, __FILE__, __LINE__);
	}

	parsed_riff.data_chunk.SubchunkSize = *(u32 *)dumb_array_get(file_buffer, 20 + parsed_riff.fmt_chunk.SubchunkSize + 4);

 	parsed_riff.data_chunk.data = dumb_arena_push(allocator, parsed_riff.data_chunk.SubchunkSize);
	parsed_riff.data_chunk.data = (u8 *)dumb_array_get(file_buffer, 20 + parsed_riff.fmt_chunk.SubchunkSize + 8);

	return parsed_riff;
}

void
save_dumb_string_to_file(const char *filename, Dumb_String *file_contents)
{
	FILE *file;
	fopen_s(&file, filename, "w");

	if (file == NULL)
	{
		printf("Couldn't write to file \"%s\"\n", filename);
		fclose(file);
		return;
	}
	fprintf(file, "%s\n", (char *)file_contents->_chars);

	fclose(file);
}

int
main(int argc, char *argv[])
{
	// It would make sense to make the global arena
	// (and also the arrays etc.) a bigger size from the begining.
	//
	// However:
	//
	// 1) This is a development tool, so I don't care about speed
	// or being efficient (at least not for now).
	//
	// 2) I do care about using this to stress-testing dumb_lib.h, though.
	//
	// That's why I initialize these with 0.
	Dumb_Arena *global_arena  = dumb_arena_create(0);
	Dumb_Arena *scratch_arena = dumb_arena_create(0);

	Dumb_String output = dumb_string_from(global_arena, TEMPLATE);

	Dumb_Array wav_filenames = dumb_array_create(global_arena, sizeof(Dumb_String));

	if (argc == 2)
	{
		Dumb_String arg    = dumb_string_from(scratch_arena, argv[1]);
		Dumb_String help   = dumb_string_from(scratch_arena, "-h");

		if (dumb_string_compare(&arg, &help))
		{
			printf(HELP_MESSAGE);
			return 0;
		}
		else
		{
			printf("Incorrect usage\n\n");
			printf(HELP_MESSAGE);
			return 0;
		}
	}
	else if (argc > 2)
	{
		Dumb_String files_option = dumb_string_from(scratch_arena, "-f");
		Dumb_String arg = dumb_string_from(scratch_arena, argv[1]);

		int start_loading_filenames = 0;

		if (dumb_string_compare(&arg, &files_option) == 0) { start_loading_filenames = 1; }

		for (int i = 2; i < argc; i++)
		{
			Dumb_String filename_string = dumb_string_from(global_arena, argv[i]);

			if (start_loading_filenames)
			{
				dumb_array_push(global_arena, &wav_filenames, (void *)&filename_string);
			}
		}
	}
	else
	{
		printf("Incorrect usage\n\n");
		printf(HELP_MESSAGE);
		return 0;
	}

	Dumb_Array parsed_files_array = dumb_array_create(global_arena, sizeof(RIFF));

	for (int i = 0; i < wav_filenames._count; i++)
	{
		Dumb_String *current_filename = (Dumb_String *) dumb_array_get(&wav_filenames, i);

		if (current_filename->_count < 1)
		{
			printf("ERROR: Tried to load empty filename at index 'i = %i'\n", i);
			continue;
		}

		Dumb_Array current_file_buffer = dumb_array_create(global_arena, sizeof(char));

		read_file_into_buffer(current_filename, &current_file_buffer, global_arena);
		RIFF parsed_file = parse_wav_buffer(global_arena, &current_file_buffer);

		// @TODO(Honza): the '\\' part presumes Windows-style path.
		// These appear only if the user escapes the path in quotes (ie.: -f "..\..\my_file.wav"
		//
		// Howevewr, if the user doesn't escape the path, C automatically "translates" them to UNIX-style
		// forward slashes.
		//
		// What do we do about that?
		Dumb_Array  filename_split = dumb_string_split_by_char(scratch_arena, current_filename, '\\');
		Dumb_String sound_name_dot_wav = *(Dumb_String *)dumb_array_get(&filename_split, (filename_split._count - 1));

		Dumb_Array  soundname_split = dumb_string_split_by_char(scratch_arena, &sound_name_dot_wav, '.');
		Dumb_String sound_name = *(Dumb_String *)dumb_array_get(&soundname_split, 0);

		// Next part of the code appends the desired contents of the output file
		// to Dumb_String 'output_string'.
		//
		// @TODO(Honza): Could this be made more simple? More readable?
		// This is a mess, but it works... at least for now.
		// However, splitting it into a different function would only hide it
		// and add another for loop...

		Dumb_String tmp_str = dumb_string_create_precise(scratch_arena, 1024);

		dumb_string_append(global_arena, &output, "\t/* GM_Sound no. ");

		dumb_string_clear(&tmp_str);
		sprintf(tmp_str._chars, "%d", i);
		dumb_string_append(global_arena, &output, tmp_str._chars);
		dumb_string_append(global_arena, &output, " */\n");

		dumb_string_append(global_arena, &output, "\tgm_sound = (GM_Sound *)dumb_arena_push(arena, sizeof(GM_Sound));\n\n");

		dumb_string_append(global_arena, &output, "\tgm_sound->name = (char *)dumb_arena_push(arena, 64);\n");

		dumb_string_append(global_arena, &output, "\tgm_sound->name = \"");
		dumb_string_append(global_arena, &output, (char *)sound_name._chars);
		dumb_string_append(global_arena, &output, "\";\n");

		dumb_string_append(global_arena, &output, "\tgm_sound->num_channels = ");
		dumb_string_clear(&tmp_str);
		sprintf(tmp_str._chars, "%d", parsed_file.fmt_chunk.NumChannels);
		dumb_string_append(global_arena, &output, tmp_str._chars);
		dumb_string_append(global_arena, &output, ";\n");

		dumb_string_append(global_arena, &output, "\tgm_sound->sample_rate = ");
		dumb_string_clear(&tmp_str);
		sprintf(tmp_str._chars, "%d", parsed_file.fmt_chunk.SampleRate);
		dumb_string_append(global_arena, &output, tmp_str._chars);
		dumb_string_append(global_arena, &output, ";\n");

		dumb_string_append(global_arena, &output, "\tgm_sound->byte_rate = ");
		dumb_string_clear(&tmp_str);
		sprintf(tmp_str._chars, "%d", parsed_file.fmt_chunk.ByteRate);
		dumb_string_append(global_arena, &output, tmp_str._chars);
		dumb_string_append(global_arena, &output, ";\n");

		dumb_string_append(global_arena, &output, "\tgm_sound->bits_per_sample = ");
		dumb_string_clear(&tmp_str);
		sprintf(tmp_str._chars, "%d", parsed_file.fmt_chunk.BitsPerSample);
		dumb_string_append(global_arena, &output, tmp_str._chars);
		dumb_string_append(global_arena, &output, ";\n");

		dumb_string_append(global_arena, &output, "\tgm_sound->data_size = ");
		dumb_string_clear(&tmp_str);
		sprintf(tmp_str._chars, "%d", parsed_file.data_chunk.SubchunkSize);
		dumb_string_append(global_arena, &output, tmp_str._chars);
		dumb_string_append(global_arena, &output, ";\n");

		dumb_string_append(global_arena, &output, "\tgm_sound->data = dumb_arena_push(arena, gm_sound->data_size);\n\n");

		for (int i = 0; i < parsed_file.data_chunk.SubchunkSize; i++)
		{
			dumb_string_append(global_arena, &output, "\tgm_sound->data[");

			dumb_string_clear(&tmp_str);
			sprintf(tmp_str._chars, "%d", i);
			dumb_string_append(global_arena, &output, tmp_str._chars);
			dumb_string_append(global_arena, &output, "] = 0x");

			dumb_string_clear(&tmp_str);
			sprintf(tmp_str._chars, "%X", parsed_file.data_chunk.data[i]);
			dumb_string_append(global_arena, &output, tmp_str._chars);
			dumb_string_append(global_arena, &output, ";\n");
		}
		dumb_string_append(global_arena, &output, "\n\tdumb_array_append(arena, sounds_array, gm_sound);\n");
		dumb_string_append(global_arena, &output, "\n");
	}
	dumb_string_append(global_arena, &output, "}");

	save_dumb_string_to_file("gm_sounds.h", &output);

	dumb_arena_destroy(global_arena);
	dumb_arena_destroy(scratch_arena);

	return 0;
}

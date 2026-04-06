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

#include <stdio.h> // @TODO(Honza): For debug only! Remove!!!
#include <stdint.h>
#include <math.h>

#define MINIAUDIO_IMPLEMENTATION
#include "lib/miniaudio.h"

#include "lib/dumb_lib.h"

#include "gasoline_audio.h"

static void
gasoline_audio_read_frames_from_buffer(Gasoline_Audio_Buffer *audio_buffer, void *output, uint32_t frame_count)
{
	if ((audio_buffer->current_index + frame_count) >= (audio_buffer->buffer_size))
	{
		audio_buffer->current_index = 0;
	}
	float *samples = audio_buffer->samples;
	size_t i       = audio_buffer->current_index;

	dumb_memcpy(output, (void *)&samples[i], frame_count);
	audio_buffer->current_index += frame_count;
}

static void
gasoline_audio_data_callback(ma_device *device, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
	// In playback mode copy data to pOutput. In capture mode read data from pInput. In full-duplex mode, both
	// pOutput and pInput will be valid and you can move data from pInput into pOutput. Never process more than
	// frameCount frames.

	Gasoline_Audio_Buffer *audio_buffer = (Gasoline_Audio_Buffer *)device->pUserData;
	if (audio_buffer == NULL)
	{
		// @TODO(Honza): More robust error handling.
		printf("gasoline_audio_data_callback(): something went horribly wrong [line %d]\n", __LINE__);
		return;
	}
	gasoline_audio_read_frames_from_buffer(audio_buffer, pOutput, frameCount);
	return;
}

void
gasoline_audio_change_tempo(Gasoline_Audio_Buffer *audio_buffer, int tempo)
{
	// @TODO(Honza):
	// This code is for PROTOTYPING ONLY for now.
	// Obviously it does nothing with the tempo parameter, it's here just so I can
	// get some sound from the speakers.
	int sample_rate = 48000; // Pass as an argument or store in the audio_buffer struct?
	int sound_length = sample_rate;
	float tone_frequency = 440.0;

	for (int i = 0; i < audio_buffer->buffer_size; i++)
	{
		if (i > sound_length)
		{
			audio_buffer->samples[i] = 0.0;
		}
		else
		{
			float phase = fmodf(((float)i * tone_frequency / (float)sample_rate), 1.0);
			audio_buffer->samples[i] = (phase < 0.5) ? 1.0f : -1.0f;
		}
	}
}

int
gasoline_audio_init(Gasoline_Audio_Device *device, Gasoline_Audio_Buffer *audio_buffer, int sample_rate, int num_channels)
{
	for (int i = 0; i < audio_buffer->buffer_size; i++)
	{
		// Clear the buffer, just in case.
		audio_buffer->samples[i] = 0.0;
	}

	ma_device_config config = ma_device_config_init(ma_device_type_playback);

	config.playback.format   = ma_format_f32;
	config.playback.channels = num_channels;
	config.sampleRate        = sample_rate;
	config.dataCallback      = gasoline_audio_data_callback;
	config.pUserData         = audio_buffer;

	if (ma_device_init(NULL, &config, &device->device) != MA_SUCCESS)
	{
		return -1; // Failed to initialize the device.
	}
	ma_device_start(&device->device); // The device is sleeping by default so it needs to be started it manually.
	return 0;
}

int
gasoline_audio_deinit(Gasoline_Audio_Device *device)
{
	ma_device_uninit(&device->device);
	return 0;
}

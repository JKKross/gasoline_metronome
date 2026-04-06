#include "lib/miniaudio.h"

typedef struct Gasoline_Audio_Device
{
	// Opaque struct to avoid linking clashes.
	ma_device device;
} Gasoline_Audio_Device;

typedef struct Gasoline_Audio_Buffer
{
	size_t  buffer_size;
	size_t  current_index;
	float  *samples;
} Gasoline_Audio_Buffer;

int gasoline_audio_init(Gasoline_Audio_Device *device, Gasoline_Audio_Buffer *audio_buffer, int sample_rate, int num_channels);
int gasoline_audio_deinit(Gasoline_Audio_Device *device);
void gasoline_audio_change_tempo(Gasoline_Audio_Buffer *audio_buffer, int tempo);

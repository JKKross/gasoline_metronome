/*

@TODO: Description etc.

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

#include "./lib/raylib/raylib.h"

#define RAYGUI_VALUEBOX_MAX_CHARS 3
#define RAYGUI_IMPLEMENTATION
#include "./lib/raygui.h"

#define DUMB_LIB_IMPLEMENTATION
#include "./lib/dumb_lib.h"

#include "gm_default_style.h"

#define MIN_TEMPO 5
#define MAX_TEMPO 500

#define WINDOW_WIDTH_NORMAL  320
#define WINDOW_HEIGHT_NORMAL 130

#define WINDOW_WIDTH_EXPANDED  WINDOW_WIDTH_NORMAL
#define WINDOW_HEIGHT_EXPANDED 330


void tap_tempo(int *tempo);
void playback(int *should_play, int *tempo);

int main()
{
	Color bg_color = GetColor(0x252525FF);

	Rectangle beats_per_bar_minus_button   = { .x = 10,  .y = 10, .width = 40,  .height = 40 };
	Rectangle beats_per_bar_value_box_rect = { .x = 60,  .y = 10, .width = 40,  .height = 40 };
	Rectangle beats_per_bar_plus_button    = { .x = 110, .y = 10, .width = 40,  .height = 40 };

	Rectangle tempo_value_box_rect = { .x = 160, .y = 10, .width = 100, .height = 40 };

	Rectangle play_pause_button_rect = { .x = 270, .y = 10, .width = 40, .height = 40 };

	Rectangle beats_per_bar_label  = { .x = 10,  .y = 55, .width = 140, .height = 20 };
	Rectangle tempo_label          = { .x = 160, .y = 55, .width = 100, .height = 20 };

	Rectangle show_more_button_rect  = { .x = 10, .y = 80, .width = 140, .height = 40 };
	Rectangle tap_tempo_button_rect  = { .x = 160, .y = 80, .width = 150, .height = 40 };

	int tempo = 80;

	int beats_per_bar = 4;
	int beats_per_bar_minus_clicked = 0;
	int beats_per_bar_plus_clicked  = 0;

	int is_playing = 0;
	int show_more = 0;

	int button_clicked = 0;

	InitWindow(WINDOW_WIDTH_NORMAL, WINDOW_HEIGHT_NORMAL, "Gasoline Metronome");

	SetTargetFPS(60);

	GuiLoadStyleGmDefaultStyle();
	GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		SetWindowSize(
			(show_more ? WINDOW_WIDTH_EXPANDED : WINDOW_WIDTH_NORMAL),
			(show_more ? WINDOW_HEIGHT_EXPANDED : WINDOW_HEIGHT_NORMAL)
		);

		BeginDrawing();
		ClearBackground(bg_color);

		GuiSetStyle(DEFAULT, TEXT_SIZE, 31);

		button_clicked = GuiButton(beats_per_bar_minus_button, "-");
		if (button_clicked && (beats_per_bar > 1)) { beats_per_bar -= 1; }

		GuiValueBox(beats_per_bar_value_box_rect, NULL, &beats_per_bar, 1, 32, 0);

		button_clicked = GuiButton(beats_per_bar_plus_button, "+");
		if (button_clicked && (beats_per_bar < 32)) { beats_per_bar += 1; }

		// @TODO: MIN_TEMPO & MAX_TEMPO are not working.
		GuiValueBox(tempo_value_box_rect, NULL, &tempo, MIN_TEMPO, MAX_TEMPO, 1);

		button_clicked = GuiButton(play_pause_button_rect, (is_playing ? "#132#" : "#131#"));
		if (button_clicked)
		{
			is_playing = (is_playing ? 0 : 1);
			playback(&is_playing, &tempo);
		}

		GuiSetStyle(DEFAULT, TEXT_SIZE, 27);
		button_clicked = GuiButton(show_more_button_rect, "Show more");
		if (button_clicked) { show_more = (show_more ? 0 : 1); }

		button_clicked = GuiButton(tap_tempo_button_rect, "Tap tempo");
		if (button_clicked) { tap_tempo(&tempo); }

		GuiSetStyle(DEFAULT, TEXT_SIZE, 23);
		GuiLabel(beats_per_bar_label, "No. of beats");
		GuiLabel(tempo_label, "BPM");


		EndDrawing();
	}

/*
	From raylib.h v4.0:

- WARNING: GuiLoadStyle() and GuiLoadStyle{Custom}() functions, allocate memory for
*         font atlas recs and glyphs, freeing that memory is (usually) up to the user,
*         no unload function is explicitly provided... but note that GuiLoadStyleDefault() unloads
*         by default any previously loaded font (texture, recs, glyphs)

	Hence:
*/
	GuiLoadStyleDefault();
	CloseWindow();
	return 0;
}

void tap_tempo(int *tempo)
{
	// @TODO: Implement
	printf("Tap tempo tapped!\n");
	return;
}

void playback(int *should_play, int *tempo)
{
	// @TODO: Implement
	if (*should_play)
	{
		printf("Playing beats at tempo %i\n", *tempo);
	}
	else
	{
		printf("Stopped playing\n");
	}
}

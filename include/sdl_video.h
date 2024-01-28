#ifndef DISPLAY_H
#define DISPLAY_H

#include "arcade.h"

#define GAME_WIDTH  256
#define GAME_HEIGHT 224
#define STRETCH_4_3 1.16375  // 4:3 = 1.33 = 1.16375 * 256 / 224

#define CELLOPHANE_GREEN 0x8000FF00  // ABGR
#define CELLOPHANE_RED 0xA0F5006D    // For Taito the filter cellophane is more magenta

// Display API
void initialize_video(arcade_system *system);
void draw_frame(arcade_system *system);

#endif
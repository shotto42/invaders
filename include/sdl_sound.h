#ifndef SOUND_H
#define SOUND_H

#include "arcade.h"

// Sound API
int initialize_audio(arcade_system *system);
void play_sound(int sample_num);
void clear_audio();

#endif
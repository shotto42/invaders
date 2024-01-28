#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL_mixer.h>
#include "sdl_sound.h"

Mix_Chunk *si_sound[10];
int audio_initialization_status = 0;

/**
 * Initialize the audio device
*/
int initialize_audio(arcade_system *system) {

    if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
        printf("Failed to initialize the audio device!\n");
        return -1;
    }

    for(int i = 0; i < 10; i++) {
        si_sound[i] = Mix_LoadWAV(system->sample_filepath[i]);
        if (si_sound[i] == NULL) {
            printf("Failed to load a sound sample: %s\n", system->sample_filepath[i]);
            return -1;
        }

    }
    audio_initialization_status = 1;

    return 0; 
}

/**
 * Play a single sample on the first free channel once
*/
void play_sound(int sample_num) {
    if (audio_initialization_status == 1) {
        Mix_PlayChannel(-1, si_sound[sample_num], 0);
    }
}

/**
 * Close the audio device
*/
void clear_audio() {
    for(int i = 0; i < 9; i++) {
        Mix_FreeChunk(si_sound[i]);
    }
    Mix_CloseAudio();
}
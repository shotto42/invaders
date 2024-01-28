#ifndef ARCADE_H
#define ARCADE_H

#include <SDL2/SDL.h>
#include "i8080.h"

typedef struct {
    Cpu_state state;                // CPU State (registers, sp, pc, memory, etc.)
    int left;
    int right;
    int shot;
    int start1;
    int start2;
    int coin;
    int tilt;
    int quit;
    uint16_t ext_shift_data;        // External shift register (shift data)
    uint8_t ext_shift_offset;       // External shift register (shift amount)
    uint8_t dip_switches[8];        // The original SI hardware DIP switches
    uint8_t arcade_mode[7];         // Configure: Color, Rotate, Flip, Fullscreen, Background, 2P_Vertical_Flip and Scaling_Mode
    char sample_filepath[10][64];   // Sound Sample filepaths
    uint8_t cocktail_vertical_screen_flip;  // Flip the screen vertically for a 2P SI cocktail table game
} arcade_system;

// Arcade API
void initialize_arcade_system(arcade_system *system);
void run_arcade_system(arcade_system *system);

#endif
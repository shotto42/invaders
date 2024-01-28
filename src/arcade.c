#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "arcade.h"
#include "i8080_ports.h"
#include "config_rom_loader.h"
#include "sdl_video.h"
#include "sdl_sound.h"
#include "sdl_input.h"

#define FRAMERATE 59.541985                         // ~60Hz Video refreshrate
#define CYCLES_PER_FRAME 1996800 / FRAMERATE        // ~2MHz 8080 CPU clock frequency
#define MICROSEC_PER_FRAME 1000000 / FRAMERATE      // 1µs emulation resolution

/**
 * Create the Invaders Arcade System
*/
void initialize_arcade_system(arcade_system *system) {
    // Initialize the CPU state
    system->state.pc = 0;
    system->state.sp = 0;
    system->state.int_enable = 0;
    system->state.cc.ac = 0;
    system->state.cc.cy = 0;
    system->state.cc.z = 0;
    system->state.cc.s = 0;
    system->state.cc.p = 0;
    for (int i = 0; i < 7; i++) {
        system->state.regs[i] = 0;
    }

    // Set inputs to 0
    system->left = 0;
    system->right = 0;
    system->shot = 0;
    system->start1 = 0;
    system->start2 = 0;
    system->coin = 0;
    system->tilt = 0;
    system->quit = 0;

    system->ext_shift_offset = 0;  // The external shift register shift amount
    system->ext_shift_data = 0;    // The external shift register data

    system->cocktail_vertical_screen_flip = 0;  // Flip the screen vertically for a 2P SI cocktail table game

    load_config_rom(system);       // Load the invaders.ini and the listed invader ROMs
    initialize_audio(system);      // Initialize the SDL Mixer
    initialize_video(system);      // Initialize the SDL video output
    set_arcade_system(system);     // Provide the arcade system pointer to the port handling
}

/**
 * Return a time stamp with microseconds resolution
*/
long long timeInMicroseconds(void) {
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000000) + tv.tv_usec;
}

/**
 * Arcade execution loop
*/
void run_arcade_system(arcade_system *system) {
    int cyc = 0, int_state = 0;
    uint64_t timer = 0, delta = 0;

    timer = timeInMicroseconds();
    while (!system->quit) {
        int_state = 0;

        handleInput(system);  // Input is read every 1/FRAMERATE

        while(int_state != 2) {
            // Let's execute as many CPU cycles as one video frame takes to be drawn
            // We always assume that the emulation speed for a single frame is faster than 1/FRAMERATE
            cyc += exec_opcode(&system->state); // Execute the next opcode

            if (cyc >= CYCLES_PER_FRAME / 2 && int_state == 0) {
                // 1st half of the video frame has been drawn => 1st interrupt vector RST 8
                int_state = 1;
                cyc += interrupt(&system->state, 1);
            }

            if (cyc >= CYCLES_PER_FRAME && int_state == 1) {
                // 2nd half of the video frame has been drawn => 2nd interrupt vector RST 10
                int_state = 2;
                cyc += interrupt(&system->state, 2);
            }
        }
        cyc = CYCLES_PER_FRAME - cyc;  // The emulation already used cycles beyond CYCLES_PER_FRAME caused by the 2nd interrupt

        // Now we must synchronize with the video timing by waiting until 1/FRAMERATE passed
        delta = timeInMicroseconds() - timer;
        if (delta > MICROSEC_PER_FRAME) {
            printf("Emulation for one frame was too slow:  %6.3f ms / Frame \n", delta/1000.0);
        } else {
            // printf("Emulation Cycle Time:  %6.3f ms / Frame \n", delta/1000.0);
            while (delta < MICROSEC_PER_FRAME) {
                delta = timeInMicroseconds() - timer;
            }
        }
        timer = timeInMicroseconds();

        draw_frame(system);  // Drawing the video frame in the emulation is much faster than on the original CRT
    }
    clear_audio();
}
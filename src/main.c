// ****************************************************************************************
// * Space Invaders Emulator running the original Arcade ROMs
// * Author:    Stephan Hotto
// * Date:      28.01.2024
// * Version:   1.0
// * Reference: The Intel 8080 CPU code (i8080.h, i8080.c) comes from the 
// *            MIT licensed GitHub project: intarga/i8080e (Ingrid Rebecca Abraham).
// *            It has been adapted by adding the port handling.
// ****************************************************************************************

#define SDL_MAIN_HANDLED

#include <string.h>
#include <stdio.h>
#include "arcade.h"

/**
 * Main
*/
int main(int argc, char *argv[]) {
    arcade_system system;

    if (argc > 0) {
        printf("%s\n", argv[0]);
    }

    initialize_arcade_system(&system);  // Setup the whole arcade system emulation (CPU, Video, Audio and Input)
    run_arcade_system(&system);         // Run the execution loop
}
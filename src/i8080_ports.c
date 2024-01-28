#include <stdio.h>
#include "i8080_ports.h"
#include "sdl_sound.h"

arcade_system *g_system;

/**
 * Setter to provide the arcade system pointer
*/
void set_arcade_system(arcade_system *system) {
    g_system = system;
}

/**
 * Called from the i8080.c cpu emulation to read the port input for the given port number
*/
uint8_t read_port(uint8_t port_number) {
    uint8_t port_data = 0;  // Holds the port data

    // ATTENTION: DIP switch inputs are inverted
    switch (port_number) {
    case 0:
        port_data = (1 - g_system->dip_switches[2])  // SW3 = ON for RAM & sound checking 
        | (g_system->dip_switches[4] << 1)           // Always 1
        | (g_system->dip_switches[5] << 2)           // Always 1
        | (g_system->dip_switches[6] << 3)           // Always 1
        | (g_system->shot << 4)                      // Fire button
        | (g_system->left << 5)                      // Left button
        | (g_system->right << 6);                    // Right button
        break;
    case 1:
        port_data = g_system->coin     // Coin Slot
            | (g_system->start2 << 1)  // Two players button
            | (g_system->start1 << 2)  // One player button
            | (1 << 3)                 // ?
            | (g_system->shot << 4)    // Player one - Fire button
            | (g_system->left << 5)    // Player one - Left button
            | (g_system->right << 6)   // Player one - Right button
            | (1 << 7);                // ?
        break;
    case 2:
        // SW 1 & 2 switches: number of ships (11 = 3 ships, 01 = 5 ships, 10 = 4 ships, 00 = 6 ships)
        port_data = (1 - g_system->dip_switches[0])    // SW1
            | ((1 - g_system->dip_switches[1]) << 1)   // SW2 
            | ((g_system->tilt) << 2)                  // TILT Switch
            | ((1 - g_system->dip_switches[3]) << 3)   // SW4   0 = extra ship at 1500, 1 = extra ship at 1000
            | (g_system->shot << 4)                    // Player two - Fire button
            | (g_system->left << 5)                    // Player two - Left button
            | (g_system->right << 6)                   // Player two - Right button
            | ((1 - g_system->dip_switches[7]) << 7);  // SW8 Coin info displayed in demo screen 0 = ON
        break;
    case 3:
        port_data = g_system->ext_shift_data >> (8 - g_system->ext_shift_offset); // 0-7 external shift register data input
        break;
    }

    return port_data;
}

/**
 * Called from the i8080.c cpu emulation to write data to the specific port number.
 * Play sound when the port bit changes from 0 to 1.
*/
void write_port(uint8_t port_number, uint8_t port_data) {
    static uint8_t port_data_mem[2] = {0, 0};

    switch (port_number) {
    case 2:
        g_system->ext_shift_offset = port_data & 0x07;  // bit 0,1,2 the shifting amount requested
        break;
    case 3:
        if ((port_data & 0x01) && !(port_data_mem[0] & 0x01)) play_sound(0);  // UFO_F
        if ((port_data & 0x02) && !(port_data_mem[0] & 0x02)) play_sound(1);  // MISSL (Player shot)
        if ((port_data & 0x04) && !(port_data_mem[0] & 0x04)) play_sound(2);  // LAU_H (Flash)
        if ((port_data & 0x08) && !(port_data_mem[0] & 0x08)) play_sound(3);  // INV_H (Invader hit)
        if ((port_data & 0x10) && !(port_data_mem[0] & 0x10)) play_sound(4);  // EXTRA (Extended play)
        port_data_mem[0] = port_data;
        break;
    case 4:
        g_system->ext_shift_data = (g_system->ext_shift_data >> 8) | (port_data << 8); // bit 0-7 shift data (LSB on 1st write, MSB on 2nd)
        break;
    case 5:
        if ((port_data & 0x01) && !(port_data_mem[1] & 0x01)) play_sound(5);  // INV_1 (Fleet movement 1)
        if ((port_data & 0x02) && !(port_data_mem[1] & 0x02)) play_sound(6);  // INV_2 (Fleet movement 2)
        if ((port_data & 0x04) && !(port_data_mem[1] & 0x04)) play_sound(7);  // INV_3 (Fleet movement 3)
        if ((port_data & 0x08) && !(port_data_mem[1] & 0x08)) play_sound(8);  // INV_4 (Fleet movement 4)
        if ((port_data & 0x10) && !(port_data_mem[1] & 0x10)) play_sound(9);  // UFO_H (UFO Hit)
        if (port_data & 0x20) {
            g_system->cocktail_vertical_screen_flip = 1;                      // Flip the screen vertically for a 2P SI cocktail table game
        } else {
            g_system->cocktail_vertical_screen_flip = 0;
        }

        break;
    case 6:
        // printf("Port 6 (Watchdog): %d\n", port_data);
        break;
    }
}
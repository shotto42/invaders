#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "config_rom_loader.h"

/**
 * Load rom file into memory location
*/
void load_rom_file(char *filename, uint8_t *memory) {
    int bytes_read;
    struct stat st;
    FILE *file;

    stat(filename, &st);

    if (st.st_size == 0) {
        printf("Failed to open the rom file: %s\n", filename);
        exit(-1);
    }

    file = fopen(filename, "rb");
    bytes_read = fread(memory, 1, st.st_size, file);
    fclose(file);

    if (bytes_read != st.st_size) {
        printf("Failed to read the rom file: %s\n", filename);
        exit(-1);
    }
}

/**
 * Load the configuration (e.g. dip switch positions) and rom files
*/
void load_config_rom(arcade_system *system) {
    int i = 0, j = 0;
    int rom_addresses[10];
    char filepath[64];
    char buffer[512];
    char *filename = "invaders.ini";
    char *pch;
    FILE *file;

    file = fopen(filename, "r");
    if (!file) {
        printf("Could not open the invaders.ini file!\n");
        exit(1);
    }

    // Read the 5 configuration lines from the invaders.ini file
    while(fgets(buffer, 512, file) && i < 5) {
        buffer[strcspn(buffer, "\r\n")] = 0;           // Remove return and new line
        for (size_t i = 0; i < strlen(buffer); i++) {  // Replace tabs with spaces
            if (buffer[i] == 9) {
                buffer[i] = 32;
            }
        }
        pch = strtok (buffer, " ");
        if (pch != NULL) {
            printf("%s\n", pch);
            j = 0;
            while (pch != NULL)
            {
                if (j > 0) {   // Skip the 1st element in each line
                    if(i==0) { // Read ROM Addresses
                        rom_addresses[j - 1] = (int)strtol(pch, 0, 16);
                        printf("%d\n", rom_addresses[j - 1]);
                    }
                    if(i==1) { // Read ROM filenames and load the ROMs into the associated memory address
                        strcpy(filepath, "rom/");
                        strcat(filepath, pch);
                        printf("%s\n", filepath);
                        load_rom_file(filepath, &system->state.memory[rom_addresses[j - 1]]);
                    }
                    if(i==2) { // Read sound sample filenames to load the samples in the sound module
                        strcpy(filepath, "samples/");
                        strcat(filepath, pch);
                        strcpy(system->sample_filepath[j - 1], filepath);
                        printf("Sound Sample: %s\n", system->sample_filepath[j - 1]);
                    }
                    if(i==3) { // Read the DIP Switches
                        system->dip_switches[j - 1] = (int)atoi(pch);
                        printf("SW%d: %d\n", j, system->dip_switches[j - 1]);
                    }
                    if(i==4) { // Read the Arcade Mode 
                        system->arcade_mode[j - 1] = (int)atoi(pch);
                        printf("Arcade Mode %d: %d\n", j, system->arcade_mode[j - 1]);
                    }
                }
                pch = strtok (NULL, " ");
                j++;
            }
            printf("-----------------\n");
            i++;
        }
    }
}
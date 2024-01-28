#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include "sdl_video.h"

SDL_Texture *background_texture;
SDL_Texture *game_texture;
SDL_Texture *filter_texture;
SDL_Texture *target_texture;  // Used to render game and cellophane filter first to be drawn above the background image
SDL_Renderer *renderer;
SDL_Window *window;
uint32_t pixels[GAME_WIDTH * GAME_HEIGHT];

/**
 * SDL2 Video Initialization
*/
void initialize_video(arcade_system *system) {
    int screen_width = 0, screen_height=0;
    int target_texture_support = 0;
    int window_flags = SDL_WINDOW_RESIZABLE;
    char *scaling_mode[3] = {"nearest", "linear", "best"};
    uint32_t color = 0, pitch = 0;
    uint32_t filter_pixels[GAME_WIDTH * GAME_HEIGHT];
    SDL_RendererInfo info;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("SDL video initialization failed: %s\n", SDL_GetError());
        exit(-1);
    }

    if(system->arcade_mode[1] == 0) {  // Original mapping (non-rotated)
        screen_width = GAME_WIDTH * STRETCH_4_3;
        screen_height = GAME_HEIGHT;
    } else {                           // Rotated by -90 degrees
        screen_width = GAME_HEIGHT;
        screen_height = GAME_WIDTH * STRETCH_4_3;
    }

    if(system->arcade_mode[3] == 1) {  // Fullscreen mode
        window_flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
        SDL_ShowCursor(SDL_DISABLE);   // Hide the mouse pointer
    }

    if (system->arcade_mode[6] <= 2) {  // Set the render scaling type
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, scaling_mode[system->arcade_mode[6]]);
    }

    window = SDL_CreateWindow("Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, window_flags);
    SDL_SetWindowMinimumSize(window, 2 * screen_width, 2 * screen_height); 

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    SDL_RenderSetLogicalSize(renderer, screen_width, screen_height);

    background_texture = IMG_LoadTexture(renderer, "background.jpg" );
    game_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, GAME_WIDTH, GAME_HEIGHT);
    filter_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, GAME_WIDTH, GAME_HEIGHT);
    target_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, GAME_WIDTH, GAME_HEIGHT);

    SDL_SetTextureBlendMode(game_texture, SDL_BLENDMODE_BLEND);    // Blend mode required to display the background image
    SDL_SetTextureBlendMode(filter_texture, SDL_BLENDMODE_MUL);    // Overlay mode for the CRT cellophane filter
    SDL_SetTextureBlendMode(target_texture, SDL_BLENDMODE_BLEND);  // Blend mode required for the background image

    // Draw the overlay cellophane filter texture
    for(int y=0; y < GAME_HEIGHT; y++) {
        pitch = y * GAME_WIDTH;
        for(int x=0; x < GAME_WIDTH; x++)  {
            color = 0x00000000;
            if (x < 64 && x > 15) {   // The green area consists of two rectangles
                color = CELLOPHANE_GREEN;
            } else if (x < 16 && y > 25 && y < 136) {
                    color = CELLOPHANE_GREEN; 
            }
            if (x > 191 && x < 224) {  // Upper read area
                 color = CELLOPHANE_RED; 
            }
            filter_pixels[pitch + x] = color;
        }
    }
    SDL_UpdateTexture(filter_texture, NULL, &filter_pixels, sizeof(uint32_t) * GAME_WIDTH);

    SDL_GetRendererInfo(renderer, &info);
    target_texture_support = info.flags & SDL_RENDERER_TARGETTEXTURE;
    if(!target_texture_support) {
        printf("Renderer does not provide target texture support!\n");
        exit(-1);
    }
}

/**
 * Draw the game video RAM content in original orientation.
 * Any coloring and rotation happens on the render textures.
*/
void draw_frame(arcade_system *system) {
    uint8_t byte = 0;
    uint16_t i = 0;
    uint32_t color = 0;
    uint32_t pitch = 0;
    SDL_Rect dstrect;
    int flip = SDL_FLIP_NONE;
    int angle = 0;
   
    // Scan the Space Invaders memory sequentially and map the CRT line drawing on the display game_texture.
    // This results in a 90° clockwise rotated image because the monitor in the arcade cabinet is rotated by 90° counter clockwise. 
    for(int y=0; y < GAME_HEIGHT; y++) {
        pitch = y * GAME_WIDTH;
        for(int x=0; x < GAME_WIDTH; x=x+8)  {
            byte = system->state.memory[i + 0x2400];
            i++;
            
            for(int bit = 0; bit < 8; bit++) {
                if ((byte >> bit) & 0x01) { 
                    color = 0xFFFFFFFF;
                } else {
                    color = 0x00000000;
                }
                pixels[pitch + x + bit] = color;
            }
        }
    }

    if (system->cocktail_vertical_screen_flip && system->arcade_mode[5]) {
        flip = flip | SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL;  // If the cocktail table mode is active then the screen flips for a 2P game
    }

    if (system->arcade_mode[2] == 1) {  // If flip is activated then the output is mirrored to be used in an arcade with a semi-transparend mirror
        flip = flip | SDL_FLIP_VERTICAL;
    }

    if (system->arcade_mode[1] == 0) {  // Original screen where the game is rotated by 90°
        angle = 0;
        dstrect.x = 0;
        dstrect.y = 0;
        dstrect.w = GAME_WIDTH * STRETCH_4_3;
        dstrect.h = GAME_HEIGHT;
    } else {                            // Screen (CRT) rotated by 90° counter-clockwise
        angle = -90;
        dstrect.x = (GAME_HEIGHT - GAME_WIDTH * STRETCH_4_3) / 2 - 1;
        dstrect.y = (GAME_WIDTH * STRETCH_4_3 - GAME_HEIGHT) / 2;
        dstrect.w = GAME_WIDTH * STRETCH_4_3;
        dstrect.h = GAME_HEIGHT;
    }

    SDL_UpdateTexture(game_texture, NULL, &pixels, sizeof(uint32_t) * GAME_WIDTH);  // Map the pixels to the game texture

    SDL_SetRenderTarget(renderer, target_texture);      // Switch the renderer to the target texture
    SDL_RenderClear(renderer);                          // Clear the target_texture renderer
    SDL_RenderCopy(renderer, game_texture, NULL, NULL);
    if (system->arcade_mode[0] == 1) {                  // If color is activated then the CRT cellophane is simulated
            SDL_RenderCopy(renderer, filter_texture, NULL, NULL);
    }
    SDL_SetRenderTarget(renderer, NULL);                // Switch the renderer back to the window
    SDL_RenderClear(renderer);                          // Clear the window renderer

    if (system->arcade_mode[4] == 1) {                  // Draw background image
        SDL_RenderCopyEx(renderer, background_texture, NULL, &dstrect, angle, NULL, flip);
    }
    SDL_RenderCopyEx(renderer, target_texture, NULL, &dstrect, angle, NULL, flip);    
   
    SDL_RenderPresent(renderer);
}
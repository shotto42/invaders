#include <stdio.h>
#include "sdl_input.h"

SDL_GameController *controller[2] = {NULL, NULL};

/**
 * Search for connected gamepads
*/
int findController() {
    int n = 0;
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            controller[n] = SDL_GameControllerOpen(i);
            n++;
            if (n >= 2) break;  // We handle a maximum of two gamepads
        }
    }
    return n;
}

/**
 * Handle keyboard inputs.
 * Player 1 & 2 are mapped to the same keys.
*/
void keyHandler(SDL_KeyboardEvent *event, arcade_system *system, int state) {
    if (event->repeat == 0) {
        switch (event->keysym.scancode) {
            case SDL_SCANCODE_LEFT:
                system->left = state;
                break;
            case SDL_SCANCODE_RIGHT:
                system->right = state;
                break;
            case SDL_SCANCODE_SPACE:
                system->shot = state;
                break;
            case SDL_SCANCODE_1:
                system->start1 = state;
                break;
            case SDL_SCANCODE_2:
                system->start2 = state;
                break;
            case SDL_SCANCODE_C:
                system->coin = state;
                break;
            case SDL_SCANCODE_T:
                system->tilt = state;
                break;
            case SDL_SCANCODE_ESCAPE:
                system->quit = state;
                break;
            default:
                break;
        }
    }
}

/**
 * Handle gamepad inputs.
 * Any game controller delivers the input for player 1 & 2.
*/
void gamepadHandler(SDL_Event event, arcade_system *system, int state) {
    switch (event.cbutton.button) {
        case SDL_CONTROLLER_BUTTON_START:
            system->coin = state;
            break;
        case SDL_CONTROLLER_BUTTON_BACK:
            system->quit = state;
        break;
        case SDL_CONTROLLER_BUTTON_Y:
            system->start1 = state;
            break;
        case SDL_CONTROLLER_BUTTON_B:
            system->start2 = state;
            break;
        case SDL_CONTROLLER_BUTTON_X:
            system->shot = state;
            break;
        case SDL_CONTROLLER_BUTTON_A:
            system->shot = state;
        break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            system->left = state;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            system->right = state;
            break;
        default:
            break;
    }
}

/**
 * Get all events from the queue.
*/
void handleInput(arcade_system *system) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            // Key event handling
            case SDL_QUIT: system->quit = 1; break;
            
            case SDL_KEYDOWN: 
                keyHandler(&event.key, system, 1);
            break;
            
            case SDL_KEYUP:
                keyHandler(&event.key, system, 0);
            break;

            // Gamepad event handling
            case SDL_CONTROLLERDEVICEADDED:
                printf("Number of Gamepads: %d\n", findController());
            break;
            case SDL_CONTROLLERDEVICEREMOVED:
                if (controller[0] && event.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller[0]))) {
                    SDL_GameControllerClose(controller[0]);  
                }
                if (controller[1] && event.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller[1]))) {
                    SDL_GameControllerClose(controller[1]);  
                }
                printf("Number of Gamepads: %d\n", findController());
            break;

            case SDL_CONTROLLERBUTTONDOWN:
                if (controller[0] && event.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller[0]))) {
                    gamepadHandler(event, system, 1);
                }
                if (controller[1] && event.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller[1]))) {
                    gamepadHandler(event, system, 1);
                }
            break;

            case SDL_CONTROLLERBUTTONUP:
                if (controller[0] && event.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller[0]))) {
                    gamepadHandler(event, system, 0);
                }
                if (controller[1] && event.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller[1]))) {
                    gamepadHandler(event, system, 0);
                }
            break;
        }
    }
}
# Project Webpage:
https://hotto.de/software-hardware/emulation-of-the-arcade-game-space-invaders/

# Emulation of the Space Invaders Arcade Hardware

**The Invaders Emulator runs the arcade machine ROMs on simulated arcade hardware.**

<img src="https://hotto.de/wp-content/uploads/2024/01/color_rotated.jpg" width="250" /> <img src="https://hotto.de/wp-content/uploads/2024/01/si_background.jpg" width="250" />

## Motivation and why just another SI emulator?
SI has been released more than 45 years ago. This is a great reason to pay tribute by implementing an emulator that handles the arcade cabinets hardware (CPU, controls, etc.) down to all game relevant details.  
Despite of the already existing SI emulations (e.g. MAME [5]) it is still worth to build an **open source application [1]** that is easy to configure to build DIY arcade cabinets or just for playing the game which wrote history that must be preserved.  
Furthermore, where is the fun when we don't dig deeper into the inner workings of the SI arcade machine.  
  
## Space Invaders, a deeper look:
Referencing the Wikipedia article [2] SI is commonly considered as one of the most influential video game of all time.
By the end of 1979, an estimated 750,000 Space Invaders machines were installed worldwide. In the same year, it had become the arcade game industry's all-time best-seller. It remained the top arcade game for three years through 1980. In 1982, SI had grossed $3.8 billion ($13 billion adjusted for inflation as of 2016).  

<img src="https://hotto.de/wp-content/uploads/2024/01/cocktail_table_2_player.jpg" width="200" /> <img src="https://hotto.de/wp-content/uploads/2024/01/arcade_cabinet.jpg" width="200" />  

  

computerarcheology.com [3] provides a great documentation of the SI hardware and software.

| Category        | Technical details (1st release): |
|-----------------|-------------------------------------------------------|
| Release:	  | 1st of April and mass-production in July 1978 |
| Vendor:	  | Taito (licensed to Midway); Lead Developer: Tomohiro Nishikado |
| Form factor:	  | Upright arcade cabinet and cocktail table (table-top) cabinet |
| CPU:            | Intel 8080 / 1.9968 MHz Clock (Released in 1974) |
| Barrel Shifter: | The 8080 does not provide a fast and wide enough shift operation to handle the Invader shifts in video RAM. Therefore, a 2-byte external shift register mapped to input- and output-ports supports the CPU. |
| Sound:          | Texas Instrument SN76477 and analog circuits |
| Video Monitor:  | Black-and-white cathode-ray tube (CRT) rotated counter-clockwise by 90° |
| Color:          | Cellophane overlays create green laser bases and a magenta (Taito) or orange (Midway) UFO |
| Game box:       | Resolution: 256 x 224 pixels | 
| Display box:    | Non-interlaced 320 Lines x 262 Dots |
| Frame Rate:     | 59,541985Hz at 15.6kHz CRT line frequency (pixel clock 4.992 MHz) |
| Graphic Memory: | 1bit per pixel => 256 x 224 / 8 = 7Kbytes |

  
**Interrupt Handling:**  
The game software is only allowed to write into the graphics memory when the CRTs electron beam is not drawing the object to be updated.  
Two interrupts (vector RST 8 in the middle of the screen and RST 10 at the end) tell the code when to update the already refreshed part of the screen.  

**The memory mapping:**  
```
ROM Mapping (depends on the ROM sizes which are mainly 2K):  
0000 - 07FF  
0800 - 0FFF  
1000 - 17FF  
1800 - 1FFF  
  
RAM Mapping  
2000 - 23FF 1K RAM  
2400 - 3FFF 7K Video RAM  

Due to partial adressing the rest of the memory address space appears as shadow images of the ROM and RAM:  
4000 - 5FFF - ROM shadow  
6000 - 7FFF - RAM shadow  
8000 - 9FFF - ROM shadow  
A000 - BFFF - RAM shadow  
C000 - DFFF - ROM shadow  
E000 - FFFF - RAM shadow  
```

**CPU Input/Output Ports:**  
The 8080 CPU uses input and output ports to communicate with the outside world.  
DIP switches are used to configure the game, whereas several port inputs handle the buttons to control the gameplay.  
The output ports are mainly used to control the sound generation. A notable exception is the communication towards the external shift register to realize the invader movements.  

<img src="https://hotto.de/wp-content/uploads/2024/01/dip_switches.jpg" width="300" />

The SW1 ... SW8 inputs are inverted before going into the CPU ports.  
If for example SW3 is set to ON then the signal at bit 0 of port 0 is set to 0.  
  
**Input Ports:**
```
Port 0  
bit 0 = SW3 (1 = RAM & Sound self-test-request at power up) (inverted)  
bit 1 = Always 1  
bit 2 = Always 1  
bit 3 = Always 1  
bit 4 = Fire  
bit 5 = Left  
bit 6 = Right  
bit 7 = n.a.  
  
Port 1  
bit 0 = CREDIT  
bit 1 = 2P start  
bit 2 = 1P start  
bit 3 = Always 1  
bit 4 = 1P shot  
bit 5 = 1P left  
bit 6 = 1P right  
bit 7 = n.a.  
  
Port 2  
bit 0 = SW1  11 = 3 ships  10 = 5 ships (inverted)  
bit 1 = SW2  01 = 4 ships  00 = 6 ships (inverted)  
bit 2 = Tilt switch  
bit 3 = SW4  1 = extra ship at 1500, 0 = extra ship at 1000 (inverted)  
bit 4 = 2P shot  
bit 5 = 2P left  
bit 6 = 2P right  
bit 7 = SW8  1 = Coin info displayed in demo screen (inverted)  
  
Port 3  
bit 0-7 External shift register data input  
```
  
**Output Ports:**  
```
Port 2:  
bit 0,1,2 Provides the shift amount to the external shift register  

Port 3:  
bit 0 = UFO  
bit 1 = Shot  
bit 2 = Player has been hit  
bit 3 = Invader has been hit  
bit 4 = Extended play  
bit 5 = AMP enable  
bit 6 = n.a.  
bit 7 = n.a.  

Port 4:  
bit 0-7 External shift register data output (LSB on 1st write, MSB on 2nd)  

Port 5:  
bit 0 = Fleet 1  
bit 1 = Fleet 2  
bit 2 = Fleet 3  
bit 3 = Fleet 4  
bit 4 = UFO Hit  
bit 5 = Flip the screen vertically for the cocktail table version in 2 player mode   
bit 6 = n.a.  
bit 7 = n.a.

Port 6:  
Watchdog signal
```
  
## Invaders Emulator Features:  
**Emulation of the hardware:**  
+ Intel 8080 CPU, RAM and ROM
+ Full screen mode to be used in DIY arcade cabinets
+ Vertical screen flip in 2 player cocktail table mode
+ Video graphics handling by using texture overlays (cellophane simulation for color) and SDL2 texture rotation/flipping
+ Sound output loading wav samples
+ External Bit-Shifter to move the invaders in video memory
+ Arcade cabinet DIP switches for the game configuration
+ Tilt switch simulation. Yes, the arcade cabinet used a tilt detection.
	
**Fully configurable by an invaders.ini file:**  
+ Load different ROMs and configure the memory mapping
+ Load configurable SI audio samples
+ Set the arcade DIP switches to configure the number of laser bases and the bonus point level for awarding extra laser bases
+ Configure the video graphics output (B&W vs. color, rotation, flipping, fullscreen as well as the background image)

**Control inputs via keys and up to 2 gamepads**  
+ Because the game is alternating between player 1 & 2, their controls are mapped on all input devices in parallel

  
**Keyboard controls:**
| Key      | Function
|----------|---------------|
| c        | Coin          |
| 1        | 1 player game | 
| 2        | 2 player game |
| <-       | Move laser base to the left | 
| ->       | Move laser base to the right | 
| Space    | Fire          |
| t        | Simulate the tilting of the arcade machine | 

  
**PlayStation/Xbox style gamepad button mapping:**

<img src="https://hotto.de/wp-content/uploads/2024/01/game_controller.jpg" width="400" />  

<img src="https://hotto.de/wp-content/uploads/2024/01/cocktail_table_player_1.jpg" width="300" /> <img src="https://hotto.de/wp-content/uploads/2024/01/cocktail_table_player_2.jpg" width="300" />

<img src="https://hotto.de/wp-content/uploads/2024/01/self_check.jpg" width="400" /> <img src="https://hotto.de/wp-content/uploads/2024/01/tilt.jpg" width="231" />


## The Software Implementation:
The emulation has been written in C and makes use of SDL2 (video and inputs) [9], SDL2_mixer (sound) [10] as well as SDL2_image (background image) [11]  
Because there are already working Intel 8080 CPU emulations like MAME (cpu/i8085/i8085.cpp) [6], the decision has been taken to go for a well working MIT licensed GitHub project intarga/i8080 [4]. The CPU code has been adapted by moving the port input/output handling into the CPU emulation to avoid opcode interpretation outside of the CPU.  
The arcade system simulation has been freshly implemented to cover the above mentioned features.


**Game ROMs:**  
For copyright reasons it is not allowed to distribute the ROM files.  
The Invaders Emulation works with MAME ROMs (Google ...).  
Unzip the ROM files and copy the content into the bin/rom folder.  
  
The folder **ini_file_templates** contains configuration templates for each of the below listed ROM sets.  
Copy the ROM matching ini file (e.g. invaders.sitv1) into the invaders bin/ folder and rename it to invaders.ini to make sure that the ROMs are correctly loaded and memory mapped. Details of the memory mapping are well documented in the MAME source code: midw8080/8080bw.cpp [7]

**The following MAME ROM versions have been tested:**
| MAME      | Description                                                     |
| ----------|-----------------------------------------------------------------|
| sitv1     | TV revision 1 (including self test option - DIP SW3)            |
| sisv2     | SV revision 2 (black & white)                                   |
| invaders  | Midway version                                                  |
| sitv      | TV revision 2 (including self test option - DIP SW3)            |
| sisv3     | SV revision 3 (black & white)                                   |
| sisv      | SV revision 4 (black & white)                                   |
| tst_invd  | Test Rom to execute the arcade self check beside of the TV revision 1 & 2 versions |  
  
  
**Emulator Audio Output:**  
For copyright reasons it is not possible to provide the sound samples.  
Find (Google ...) and add the wav files to the bin/samples folder to activate the audio output.  
Configure the mapping between the SI sound effects and the sample filenames in the invaders.ini file.  
  
  
## Emulator performance und supported hardware:
The emulator must be able to execute "CPU clock / video frames per second" (1.9968 MHz / 59.541985Hz) opcode cycles within "1 / video frames per second" to make sure that the video RAM is fully refreshed in realtime.  
In numbers: 33,536 cycles in **16.8ms**  

The Invaders Emulator is a single core application and has been tested on the following CPUs:
```
CPU                                        Execution time per video frame
Intel Core i7 6700HQ (old ThinkPad)                 < 1.5ms
ARM Cortex-A76       (Orange PI 5B)                 < 2ms
```

## Configuration file (invaders.ini):
The ini file allows the full configuration of the application. For example the DIP switch settings of the arcade cabinet or the video graphics output mode.  
The configuration lines start with a keyword and each line item is separated by a space or tab.  
Please refer to the **ini_file_templates** folder provided by the download and GitHub [1].

```
ROM_ADDRESSES:  0x0000 0x0800 0x1000 0x1800
ROM_FILES:      invaders.h invaders.g invaders.f invaders.e
SOUND_FILES:    UFO_F.wav MISSL.wav LAU_H.wav INV_H.wav EXTRA.wav INV_1.wav INV_2.wav INV_3.wav INV_4.wav UFO_H.wav
DIP_SWITCHES:   0 0 0 0 1 1 1 1
ARCADE_MODE:    0 0 0 0 0 1 0
			    

Description:

ROM_ADDRESSES:  Memory start addresses of the associated rom files.


ROM_FILES:  The rom file names to be loaded and mapped to the memory addresses.


SOUND_FILES: UFO  Player_Shot  Flash Invader_Hit  Fleet_1  Fleet_2  Fleet_3  Fleet_4  UFO_Hit Extended_Play


DIP_SWITCHES:  SW1 ... SW8

SW1  SW2
 1    1    3 Laser bases per game
 0    1    4 Laser bases per game
 1    0    5 Laser bases per game
 0    0    6 Laser bases per game

SW3 RAM & Game Sound check (only with CPU board (A) No. CUN00006). The game version must support the function: TV revision 1 & 2.
1   Checking
0   Normal Play

SW4 sets bonus point level for awarding extra laser bases
1    1500 Points
0    1000 Points

SW5 Always 1 (Factory setting solid state modules)
SW6 Always 1 (Factory setting solid state modules)
SW7 Always 1 (Factory setting solid state modules)

SW8 for displaying play pricing on screen
1   "1 coin - 1 player     2 coins - 2 players"
0   Nothing is displayed


ARCADE_MODE: Color Rotate Flip Fullscreen Background 2P_Vertical_Flip Scaling_Mode

Color:
1 = Emulates the cellophane (Taito: green and magenta) overlayed over the Cathode-Ray Tube (CRT).

Rotate:
0 = The game output is directly mapped resulting in a 90° clockwise rotated game image because in the original arcade the monitor has been installed in a vertical orientation.
1 = The game display is rotated by 90° counter-clockwise.

Flip:
1 = Flips the display because the original arcade used a semi-transparent mirror to reflect the game in front of a background image of space and a moon.
Don't use that option together with the 2P_Vertical_Flip. The SI cocktail table version does not use a semi-transparent mirror.

Fullscreen:
1 = Activates the fullscreen mode not showing any window decoration.

Background:
1 = Displays a background image like in the original arcade.

2P_Vertical_Flip
1 = The screen flips vertically for a 2 player game on the SI cocktail table version.
Don't use that option together with the flip because the table mode has no semi-transparent mirror.

Scaling_Mode:
0 = Nearest
1 = Linear
2 = Best  
```

## Building and running  
**Download, Source Code Compilation and SDL2 Libraries:**  
Keep in mind that you need the appropriate MAME ROMs (pls. see above) to play the game.  
Furthermore, add SI sound samples (pls. see above) to the samples folder to activate the games audio output.
  
**Windows:**  
Either download the pre-compiled Invaders Emulator or use MSYS2/MINGW64 [12] to compile from source [1].  
To compile against the SDL2 DLL libraries (SDL2.dll, SDL2_mixer.dll, SDL2_image.dll) copy them into the lib folder.  
Furthermore, to make it easy just copy the DLLs into the bin folder for application execution.
  
  
**Linux:**  
As pre-requisite compile and install the SDL2 [9], SDL2_mixer [10] and SDL2_image [11] libraries.
For each of the three libraries enter the associated folder and execute the following:  
```
$ ./configure
$ make
$ sudo make install
```
  
The libraries are installed under /usr/local/lib/  
  
**Compile the Invaders Emulator:**  
Clone the project from GitHub [1] and type make to compile the application.  
Go into the bin/ folder and type ./invaders to start the application.  
Make sure that the library path /usr/local/lib is part of the LD_LIBRARY_PATH system variable.  
If the application exits with a "symbol not found" error than add the lib path temporarily by executing the following command:
```
$ export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib
```

**macOS:**  
Up to now it hasn't been tested whether the compilation works on Apple machines but there shouldn't be any reason that prevents it.  

  
## References:  
[1]  https://github.com/shotto42/invaders  
[2]  https://en.wikipedia.org/wiki/Space_Invaders  
[3]  https://computerarcheology.com/Arcade/SpaceInvaders/  
[4]  https://github.com/intarga/i8080e/tree/master  
[5]  https://www.mamedev.org/  
[6]  https://github.com/mamedev/mame/blob/master/src/devices/cpu/i8085/i8085.cpp  
[7]  https://github.com/mamedev/mame/blob/master/src/mame/midw8080/8080bw.cpp  
[8]  https://www.libsdl.org/  
[9]  https://github.com/libsdl-org/SDL/releases  
[10] https://github.com/libsdl-org/SDL_mixer/releases  
[11] https://github.com/libsdl-org/SDL_image/releases  
[12] https://www.msys2.org/



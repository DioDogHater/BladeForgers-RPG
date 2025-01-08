#-- About this project --
I am making this project as a way to test my game making abilities while also wasting time as a no-life.

# Why the hell is it in C?
Because I want to affirm my sigma-ness in a world of beta noobs. Also because I enjoy C more than other languages.

# Did you smoke before coding or something? This code is ass!
Remind yourself before posting any vulgar comments or death threats that I am still in high school, and I consider myself an intermediate at programming

# BUGS!!! HELP ME I GOT 50 FPS!!!
This game is supposed to run at 120 fps by default.
If the game runs badly on your system or it simply doesn't fucking work, please attempt to contact me or something.
- NOTE: I am very lazy, so if you do reach me, I might give up on fixing the issue if it is too problematic

# Note to the people who stumbled on this page
Thank you for finding this project, and I hope you enjoy looking at my code.
If there any improvements you would make, please tell me so I could add them.
- NOTE: I don't want any dumb shit asking me to add multiplayer or something thinking I'm some kind of networking genius.

# Code structure
I've attempted to make my code more readable, but I am sorry if some of it might be impossible to decipher to other people.
Anyways here's the information on my project's program structure:

- Both source files are independant! That means that you should build them INDIVIDUALLY! Both need SDL2, SDL2_ttf, SDL2_image libraries linked using commands.
- The code is portable when compiled (the .exe or other type of executable), as long as (for Windows) SDL2.dll, SDL2_image.dll, SDL2_image.dll are in the same folder
- The code is not OS locked! That means that it can run on any OS capable of compiling C and that is compatible with SDL2.

- INCLUDE/BLADELIB.H:
It's a header containing my own library that both of my source files share. It builds on top of the SDL2 library.
It has multiple structs, functions and type definitions / preprocessor defines.
CAUTION -> the head file uses the keywork BLADELIB_H to determine if it is defining itself more than once, so avoid using it in your programs

- MAIN.C
The main source file, holds the source code of the game. The program uses the SDL2 library to render things onscreen.
The FPS cap uses the milliseconds until last frame to determine if a frame should not yet be computated.
Yes! The deltaTime is indeed calculated each frame and used in motion calculations in case of lag or something.
Sadly, I don't wanna make a complicated resizable window system, so I'll stick to constant 800x800 windows.
The game will open normally if default.bin is present in folder /maps/, but if it is not there, the game will ask you
to input your custom map's file name at the start.

- MAPBUILDER.C
Same thing as MAIN.C, except the player doesnt exist, and you build maps. The map building system is not modular though,
so be careful when editing it. The map saving/loading system uses binary files and writes the raw map on the file you entered when
the program started. If you need anyhelp with the controls, check the text displayed on the lower left area of the window. The window might
crash when adding a new chunk so my bad. Chunks' positions are of a scale of 16 grid blocks, since the chunks are 16x16. I might add the ability
to remove a chunk in the future.

#-----
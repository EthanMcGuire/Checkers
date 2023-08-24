# Description

This is a simple checkers game made using the SFML library. SFML is used for the graphics. In addition, a mini-max algorithm is used for the enemy AI. Based on the depth of the mini-max tree,
the AI becomes more intelligient and thus more difficult to defeat.

# How to run

To run the game just run main.exe. 

If you would like to edit any of the game settings or the code you will need MinGW32 in order to recompile the game.

In addition, you will need to edit the program path in the Makefile (NOT IN QUOTES):

    ROOT_DIR = Checkers_Source_Directory

If the SFML path is set up and MinGW32 is installed and in your path, run this command in your project directory to recompile:

    MinGW32-make

# Editing the Game Settings

The game settings can be configured in checkers.hh and main.cpp.

To change the AI difficulty, in checkers.hh change the MAX_DEPTH value:

    //AI
    const int MAX_DEPTH = 6;  //Max depth for minimax

I would recommend not going above 7 depth, as the time to wait for the AI increases DRASTICALLY.

In main.cpp the team settings can also be changed:

    //Team settings
    teams[BLACK] = "ai";    //Team name should be "ai" or "player"
    teams[WHITE] = "player";
    teamDir[BLACK] = "down";
    teamDir[WHITE] = "up";
    teamColor[BLACK] = "black";
    teamColor[WHITE] = "white";

Both teams can be set to "ai" if you want to see two AI's fight each other. Both can also be set to "player" if you want to challenge a friend.

teamDir affects which direction a team is moving on the board. One team should be "up" and the other should be "down".

teamColor affects the internal color of each team. This is used for the program to know which color is which team. These should not be changed.

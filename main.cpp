#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>

#include "checkers.hh"

//-mwindows
//ming32-make to compile and link the program

int main()
{
   ///Declare variables

   //Main
   states state = getAction;
   std::string currentTurn = "white";
   std::string playerTeam = "white";
   std::string aiTeam = "black";
   std::string winner;
   bool canKill = false;
   bool twoPlayer = false;

   //Window
   sf::RenderWindow window;

   sf::Vector2i globalMousePos;
   sf::Vector2f globalMousePosWorld;

   //Audio
   sf::Music music;  //Music
   sf::SoundBuffer bufferPieceMove, bufferWin; //Sound buffers
   sf::Sound sndPieceMove, sndWin; //Sound entities

   //Graphical
   sf::Texture background;
   sf::Texture boardTexture;
   sf::Texture blackFlagTexture, whiteFlagTexture;
   sf::Texture blackStoneTexture, blackKingTexture;
   sf::Texture whiteStoneTexture, whiteKingTexture;
   sf::Sprite spriteBackground;
   sf::Sprite spriteBoard; //592 by 592
   sf::Sprite spriteWhiteFlag, spriteBlackFlag;
   sf::Sprite killHighlight;  //Highlight on pieces to kill
   sf::Sprite canKillHighlight;  //Highlight on pieces that can kill
   sf::Sprite pieceHighlight; //Highlight on a currently selected piece
   sf::Sprite moveHighlight; //Highlight for a pieces movement
   sf::RectangleShape winRect;

   //Font/text
   sf::Font font;
   sf::Text textWin; //Displays when someone wins

   //Piece Information
   std::vector<stone> black;
   std::vector<stone> white;
   unsigned int blackCount = PIECE_COUNT;
   unsigned int whiteCount = PIECE_COUNT;

   //Move selection
   bool selected = false;  //True when a piece is pressed on
   stone* selectedPiece;   //Reference to the currently selected piece
   unsigned int mouseMoveIndex;  //The movePositions index the mouse is highlighted over

   //Move info
   sf::Vector2f killSpots[PIECE_COUNT];
   unsigned int killMoveCount = 0;
   sf::Vector2i movePositions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS];   //The coordinates on the board where this piece can move
   unsigned int movePointCount[MAX_POSSIBLE_MOVES];   //The number of points within a move
   unsigned int moveCount; //Total moves found for a piece
   bool killChain;   //Whether a kill chain can be executed

   //Action Control
   bool executingAction = false; //Whether an action is currently being executed (e.g. piece move)
   bool killAction = false;   //Whether the current action is a kill move
   unsigned int moveIndex; //The movePositions index that is being executed
   sf::Vector2f actionPointStart, actionPointGoalBoard, actionPointGoal;  //The x and y point where the piece will start and land after their action
   sf::Vector2f actionLength; //The length from action start to goal
   float actionLengthReal;
   stone* pieceToKill;  //The piece to kill for a kill move

   //Game timing control
   sf::Clock deltaClock;
   float deltaTime = 0;

   //Other
   int i, j, k;

   //Create the window
   window.create( sf::VideoMode( WINDOW_WIDTH, WINDOW_HEIGHT ), "Checkers", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize );
   window.setFramerateLimit( 60 );

   //Load audio

   if ( !music.openFromFile( SND_MUSIC ) )
      return EXIT_FAILURE;

   if ( !bufferPieceMove.loadFromFile( SND_PIECE_MOVE ) )
      return EXIT_FAILURE;

   if ( !bufferWin.loadFromFile( SND_WIN ) )
      return EXIT_FAILURE;

   sndPieceMove.setBuffer( bufferPieceMove );
   sndWin.setBuffer( bufferWin );

   //Play music
   music.setLoop( true );
   music.play();

   //Load font and set up text

   if ( !font.loadFromFile( FONT ) )
      return EXIT_FAILURE;

   //Win text
   textWin.setPosition( window.getView().getCenter() );
   textWin.setFont( font );
   textWin.setCharacterSize( 50 );
   textWin.setFillColor( sf::Color( 255, 255, 255 ) );
   textWin.setStyle( sf::Text::Bold | sf::Text::Italic );

   //Win rectangle
   winRect.setPosition( window.getView().getCenter() );
   winRect.setFillColor( sf::Color( 139, 69, 19, 224 ) ); //Saddlebrown
   winRect.setOutlineColor( sf::Color( 245, 222, 179, 224 ) ); //Wheat
   winRect.setOutlineThickness( 4 );

   //Create the game textures

   if ( !background.loadFromFile( SPR_BACKGROUND ) )
      return EXIT_FAILURE;

   if ( !boardTexture.loadFromFile( SPR_BOARD ) )
      return EXIT_FAILURE;

   if ( !blackFlagTexture.loadFromFile( SPR_BLACK_FLAG ) )
      return EXIT_FAILURE;

   if ( !whiteFlagTexture.loadFromFile( SPR_WHITE_FLAG ) )
      return EXIT_FAILURE;

   if ( !blackStoneTexture.loadFromFile( SPR_BLACK_PIECE ) )
      return EXIT_FAILURE;

   if ( !whiteStoneTexture.loadFromFile( SPR_WHITE_PIECE ) )
      return EXIT_FAILURE;

   if ( !blackKingTexture.loadFromFile( SPR_BLACK_KING ) )
   return EXIT_FAILURE;

   if ( !whiteKingTexture.loadFromFile( SPR_WHITE_KING ) )
      return EXIT_FAILURE;

   //Get the sprite textures
   spriteBackground.setTexture( background );
   spriteBoard.setTexture( boardTexture );
   spriteWhiteFlag.setTexture( whiteFlagTexture );
   spriteBlackFlag.setTexture( blackFlagTexture );

   moveHighlight.setTexture( whiteStoneTexture );
   pieceHighlight.setTexture( whiteStoneTexture );
   canKillHighlight.setTexture( whiteStoneTexture );
   killHighlight.setTexture( whiteStoneTexture );

   //Set the board position
   spriteBoard.setPosition( BOARD_ORIGIN_X, BOARD_ORIGIN_Y ); //Center the checkers board

   //Set the flag positions
   sf::FloatRect flagBounds;

   flagBounds = spriteBlackFlag.getLocalBounds();
   spriteBlackFlag.setPosition( BOARD_ORIGIN_X - flagBounds.width - 16, BOARD_ORIGIN_Y - 32 );

   flagBounds = spriteWhiteFlag.getLocalBounds();
   spriteWhiteFlag.setPosition( BOARD_ORIGIN_X + BOARD_SIZE + 16, BOARD_ORIGIN_Y + BOARD_SIZE - 192 );

   //Set the piece highlight color
   pieceHighlight.setColor( sf::Color( 0, 255, 0, 128 ) );  //50% opacity green
   canKillHighlight.setColor( sf::Color( 255, 255, 0, 128 ) );  //50% opacity yellow

   //Create the pieces
   unsigned int offset; //Offset based on shifted rows (On the board)
   unsigned int boardX, boardY;

   offset = 1;
   boardX = 0;
   boardY = 0;

   //Create the black pieces
   for ( i = 0 ; i < blackCount ; i++ )
   {
      black.push_back( *(new stone) );

       //Load black piece texture
      if ( !( black.at(i).load( blackStoneTexture, &blackKingTexture, "black", "down" ) ) )
         return EXIT_FAILURE;

      black.at(i).setBoardPosition( offset + boardX * 2, boardY ); //Set the black positions

      boardX += 1;

      if ( boardX >= ROW_SIZE )
      {
         boardX = 0;
         boardY += 1;

         offset = !offset;
      }
   }

   offset = 0;
   boardX = 0;
   boardY = 0;

   //Create the white pieces
   for ( i = 0 ; i < whiteCount ; i++ )
   {
      white.push_back( *(new stone) );

      //Load white piece texture
      if ( !( white.at(i).load( whiteStoneTexture, &whiteKingTexture, "white", "up" ) ) )
         return EXIT_FAILURE;

      white.at(i).setBoardPosition( offset + boardX * 2, WHITE_PLAYER_ROW_OFFSET + boardY ); //Set the black positions

      boardX += 1;

      if ( boardX >= ROW_SIZE )
      {
         boardX = 0;
         boardY += 1;

         offset = !offset;
      }
   }

   //Start the game loop
   while ( window.isOpen() )
   {
      //Get delta time
      deltaTime = deltaClock.restart().asSeconds();

      //Get the mouse position
      globalMousePos = sf::Mouse::getPosition( window );
      globalMousePosWorld = window.mapPixelToCoords( globalMousePos );

      sf::Event event;

      while ( window.pollEvent( event ) )
      {
         //Close window
         if ( event.type == sf::Event::Closed )
         {
            window.close();
         }

         //Do NOT get mouse inputs, if it is not the players turn (Unless playing 2 player)
         if ( ( playerTeam == currentTurn || twoPlayer ) && ( state == getAction || state == getChainKill ) )
         {
            //Mouse moved
            if ( event.type == sf::Event::MouseMoved )
            {
               //Check if the mouse is over a move
               if ( selected )
               {
                  mouseMoveIndex = checkMouseOverMove( movePositions, movePointCount, moveCount, globalMousePosWorld.x, globalMousePosWorld.y );
               }
            }

            //Mouse pressed
            if ( event.type == sf::Event::MouseButtonPressed )
            {
               if ( event.mouseButton.button == sf::Mouse::Left )
               {
                  //Check for an action execution
                  if ( selected )
                  {
                     //Is a move hovered over?
                     if ( mouseMoveIndex != -1 )
                     {
                        //START THE ACTION
                        //This is executed during the getAction or getChainKill state

                        selected = false;
                        state = startAction;
                        moveIndex = mouseMoveIndex;
                     }
                  }

                  //These are only executed during getAction state
                  //The getChainKill state does not allow selecting another piece
                  if ( state == getAction )
                  {
                     selected = false;
                     selectedPiece = nullptr;
                     mouseMoveIndex = -1; //Reset the mouseMoveIndex

                     //Check for piece selection

                     if ( currentTurn == "black" )
                     {
                        for ( i = 0 ; i < black.size() ; i++ )
                        {
                           //Select a black piece
                           if ( black.at(i).meetingPoint( globalMousePosWorld.x, globalMousePosWorld.y ) )
                           {
                              //Piece selected
                              selected = true;
                              selectedPiece = &black.at(i);
                           } 
                        }
                     }
                     else
                     {
                        for ( i = 0 ; i < white.size()  ; i++ )
                        {
                           //Select a white piece
                           if ( white.at(i).meetingPoint( globalMousePosWorld.x, globalMousePosWorld.y ) )
                           {
                              //Piece selected
                              selected = true;
                              selectedPiece = &white.at(i);
                           } 
                        }
                     }

                     //Cancel the selection if canKill and no kill moves found
                     if ( selected && canKill && !hasKillMoves( *selectedPiece, black, white ) )
                     {
                        selected = false;
                        selectedPiece = nullptr;
                     }

                     //Piece was selected
                     if ( selected )
                     {
                        //Set up move information
                        moveCount = getMovePositions( movePositions, movePointCount, *selectedPiece, black, white );  //Get the possible move count

                        pieceHighlight.setPosition( selectedPiece -> getPosition() );

                        //Check for mouse over move
                        mouseMoveIndex = checkMouseOverMove( movePositions, movePointCount, moveCount, globalMousePosWorld.x, globalMousePosWorld.y );
                     }
                  }
               }
            }
         }
      }

      //Step Actions

      if ( !twoPlayer && currentTurn != playerTeam )
      {
         //Get the AI's move

         if ( state == getAction || state == getChainKill )
         {
            sf::Vector2i piecePosition;
            move finalMove;

            getAIMove( &finalMove, black, white, piecePosition, aiTeam, killChain, selectedPiece );

            //Get the stone for the selected move
            selectedPiece = getStoneFromPosition( piecePosition.x, piecePosition.y, black, white );

            std::cout << "HUH " << finalMove.goalPosition.x;



            selected = false;
            moveIndex = 0;
            state = startAction;
         }
      }
      else if ( state == startAction )
      {
         //Starts an action

         state = action;

         killAction = false;

         //Is this is a kill move?
         if ( movePointCount[moveIndex] > 1 )
         {
            //First point is the enemies location
            pieceToKill = getStoneFromPosition( movePositions[moveIndex][0].x, movePositions[moveIndex][0].y, black, white );

            killAction = true;
         }

         //Set the action start and goal positions
         actionPointStart = selectedPiece -> getPosition();

         actionPointGoalBoard.x = movePositions[moveIndex][movePointCount[moveIndex] - 1].x;
         actionPointGoalBoard.y = movePositions[moveIndex][movePointCount[moveIndex] - 1].y;

         actionPointGoal.x = BOARD_ORIGIN_X + BOARD_SQUARE_OFFSET + PIECE_OFFSET + ( BOARD_SQUARE_SIZE * actionPointGoalBoard.x );
         actionPointGoal.y = BOARD_ORIGIN_Y + BOARD_SQUARE_OFFSET + PIECE_OFFSET + ( BOARD_SQUARE_SIZE * actionPointGoalBoard.y );

         actionLength = actionPointGoal - actionPointStart;
         actionLengthReal = sqrt( pow( actionLength.x, 2 ) + pow( actionLength.y, 2 ) );
      }
      else if ( state == action )
      {
         //Executing an action
         
         //Move the selected piece to the point goal
         bool pointReached = false;
         bool king = selectedPiece -> isKing();
         float scale = 1;
         float progress, lengthTraveledReal;
         sf::Vector2f lengthTraveled;

         //Move the piece
         pointReached = ( selectedPiece -> moveTowardsPoint( actionPointGoal, PIECE_MOVE_SPEED, deltaTime ) );

         //Get the length traveled of this action
         lengthTraveled = selectedPiece -> getPosition() - actionPointStart;
         lengthTraveledReal = sqrt( pow( lengthTraveled.x, 2 ) + pow( lengthTraveled.y, 2 ) );

         progress = lengthTraveledReal / actionLengthReal;  //Get the percentage of the action completed

         //Set scale

         //Make the piece go "above" the rest
         if ( progress < 0.5 )
         {
            //Increase scale
            scale += 0.5 * ( progress / 0.5 );
         }
         else
         {
            //Decrease sacle back to normal
            scale += 0.5 - ( 0.5 * ( ( progress - 0.5 ) / 0.5 ) );
         }

         selectedPiece -> setScale( scale, scale );

         //Goal reached
         if ( pointReached )
         {
            //Set the pieces position and scale
            selectedPiece -> setBoardPosition( actionPointGoalBoard.x, actionPointGoalBoard.y );
            selectedPiece -> setScale( 1, 1 );

            //Play move sound
            sndPieceMove.play();

            killChain = false;

            if ( killAction )
            {
               //Kill the piece
               pieceToKill -> kill();

               removeDeadPieces( black, white );

               //A kill chain CANNOT be executed once a piece becomes king
               if ( selectedPiece -> isKing() == king )
               {
                  //Check for kill chains
                  moveCount = getKillMoves( movePositions, movePointCount, *selectedPiece, black, white, actionPointGoalBoard.x, actionPointGoalBoard.y );

                  if ( moveCount != 0 )
                  {
                     //Set up move information

                     //AI doesn't need this
                     if ( twoPlayer || currentTurn == playerTeam )
                     {
                        //Set the piece highlight on the current pieces location
                        pieceHighlight.setPosition( selectedPiece -> getPosition() );

                        //Check for mouse over this new move
                        mouseMoveIndex = checkMouseOverMove( movePositions, movePointCount, moveCount, globalMousePosWorld.x, globalMousePosWorld.y );
                     }

                     killChain = true;
                  }
               }
            }

            if ( killChain )
            {
               //Start the getChainKill state

               //AI moves don't get highlighted
               if ( twoPlayer || currentTurn == playerTeam )
               {
                  selected = true;
               }

               state = getChainKill;
            }
            else
            {
               //Check for game win
               if ( black.size() == 0 )
               {
                  winner = "White";
                  state = gameOver;
               }
               else if ( white.size() == 0 )
               {
                  winner = "Black";
                  state = gameOver;
               }

               selectedPiece = nullptr;

               if ( state != gameOver )
               {
                  //Next turn
                  state = getAction;

                  getNextTurn( currentTurn, canKill, black, white );
                  killMoveCount = getPiecesThatCanKill( black, white, killSpots, currentTurn );
               }
               else
               {
                  sf::FloatRect textBounds, rectBounds;

                  //Set the win text
                  textWin.setString( winner + " Wins!" );

                  //Center the win text
                  textBounds = textWin.getLocalBounds();

                  textWin.setOrigin( textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f );

                  //Center and get the win rectangle
                  winRect.setSize( sf::Vector2f( textBounds.width + 48, textBounds.height + 32 ) );

                  rectBounds = winRect.getLocalBounds();

                  winRect.setOrigin( rectBounds.left + rectBounds.width / 2.0f, rectBounds.top + rectBounds.height / 2.0f  );

                  //Play the victory sound!
                  sndWin.play();
               }
            }
         }
      }

      //Clear the window
      window.clear();

      //Draw

      //Background sprite
      window.draw( spriteBackground );

      //Board sprite
      window.draw( spriteBoard );

      //Draw black pieces
      for ( i = 0 ; i < black.size()  ; i++ )  //Rows
      {
         window.draw( black.at(i) );
      }

      //Draw white pieces
      for ( i = 0 ; i < white.size()  ; i++ )  //Rows
      {
         window.draw( white.at(i) );
      }

      //Moving piece gets drawn over the other pieces
      if ( state == action ) window.draw( *selectedPiece );

      //Draw the team flags
      window.draw( spriteWhiteFlag ) ;
      window.draw( spriteBlackFlag ) ;
   
      //Draw the possible move positions and piece highlights
      if ( selected )
      {
         for ( int i = 0 ; i < moveCount ; i++ )
         {
            for ( int j = 0 ; j < movePointCount[i] ; j++ )
            {
               //Get the position for this point
               unsigned int xPos = movePositions[i][j].x;
               unsigned int yPos = movePositions[i][j].y;
               int xReal, yReal;
               int opacity;

               xReal = BOARD_ORIGIN_X + BOARD_SQUARE_OFFSET + ( BOARD_SQUARE_SIZE * xPos );
               yReal = BOARD_ORIGIN_Y + BOARD_SQUARE_OFFSET + ( BOARD_SQUARE_SIZE * yPos );

               //Set color opacity
               opacity = 128;

               //Check if this point is highlighted by the mouse
               if ( mouseMoveIndex == i )
               {
                  opacity = 192;
               }

               //Draw
               if ( !isPositionFree( xPos, yPos, black, white ) )
               {
                  //Kill highlight

                  killHighlight.setColor( sf::Color( 255, 0, 0, opacity ) );   //Red

                  //Draw the kill highlight
                  killHighlight.setPosition( xReal + PIECE_OFFSET, yReal + PIECE_OFFSET );
                  window.draw( killHighlight );
               }
               else
               {
                  //Piece movement highlight

                  moveHighlight.setColor( sf::Color( 255, 255, 0, opacity ) );  //Yellow

                  //Draw the piece movement highlight
                  moveHighlight.setPosition( xReal + PIECE_OFFSET, yReal + PIECE_OFFSET );
                  window.draw( moveHighlight );
               }
            }
         }

         //Draw the selected piece highlight
         window.draw( pieceHighlight );
      }
      else
      {
         if ( state == getAction )
         {
            //Highlight pieces that can kill
            if ( canKill )
            {
               for ( i = 0 ; i < killMoveCount ; i++ )
               {
                  canKillHighlight.setPosition( killSpots[i] );
                  window.draw( canKillHighlight );
               }
            }
         }
      }

      //Draw win text
      if ( state == gameOver )
      {
         window.draw( winRect );
         window.draw( textWin );
      }
      
      //Update the window display
      window.display();
   }

   return EXIT_SUCCESS;
}

/*****************************************************************************************************************************************************************************/



//Checks if value is within the range low to high
//Returns true if so
//Param:
//value - The value to check the range of
//low - The low range
//high - The high range
bool inRange( unsigned int value, unsigned int low, unsigned int high )
{
    if ( value >= low && value <= high )
        return true;

    return false;
}

//Returns the distance moved from pointA to pointB by speed
//Uses normalization of points for a direction
//Param:
//pointA - Starting point
//pointB - Point to move to
//speed - Speed to move at
sf::Vector2f interpolate( sf::Vector2f pointA, sf::Vector2f pointB, float speed )
{
   sf::Vector2f distance, direction, velocity;
   float magnitude;

   //Get the total distance
   distance = pointB - pointA;
   
   //Normalize
   magnitude = sqrt( pow( distance.x, 2 ) + pow( distance.y, 2 ) );
   direction = sf::Vector2f( distance.x / magnitude, distance.y / magnitude );

   //Get the movement amount
   velocity = speed * direction;

   return velocity;
}

//Gets the next turn
//Param:
//currentTurn - The current color whose team it is
//canKill - Whether a kill move can be performed
//black - The array of black pieces
//white - The array of white pieces
void getNextTurn( std::string& currentTurn, bool& canKill, std::vector<stone>& black, std::vector<stone>& white )
{
   int i = 0;

   canKill = false;

   //Get the next turn
   if ( currentTurn == "black" )
   {
      currentTurn = "white";
   }
   else
   {
      currentTurn = "black";
   }

   canKill = canTeamKill( currentTurn, black, white );
}

//Gets the next move for the AI using the getTeamUtilityValue and min/max functions
//Param:
//positions - The array of x and y positions for possible moves
//movePointCount - The number of points in a move
//black - The array of black pieces
//white - The array of white pieces
//piecePosition - x and y position of the piece to move
//teamName - The team to get a move for (The AI's team)
//canKill - Whether this team can currently kill
//chainKillPiece - The piece that can chainKill
void getAIMove( 
   move* finalMove, 
   std::vector<stone>& black, std::vector<stone>& white, sf::Vector2i& piecePosition,
   std::string aiTeam,
   bool chainKill, stone* chainKillPiece )
{
   struct teamDatabase teams;

   teams.black = black;
   teams.white = white;
   teams.chainKill = chainKill;
   teams.chainKillPiece = chainKillPiece;

   teams.value = aiMax( 0, teams, aiTeam, finalMove, true );

   std::cout << "Value :" << teams.value << "\n";

   return;
}

unsigned int aiMax( 
   unsigned int depth,
   struct teamDatabase teams, 
   std::string aiTeam,
   move* finalMove, bool first )
{
   sf::Vector2i newPositions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS];
   unsigned int newMovePointCount[MAX_POSSIBLE_MOVES];

   //Final move info
   sf::Vector2i maxPosition[MAX_MOVE_POINTS];
   unsigned int maxPointCount;
   sf::Vector2i position;  //Piece to move position

   struct teamDatabase newTeams;
   std::vector<stone>* team;
   stone* piece;  //Current piece to look at moves for
   stone* pieceDatabase, *pieceKill;  //Piece to mess with in the database

   unsigned int moveCount;
   unsigned int i, j, k;
   unsigned int maxValue;
   bool canKill;

   maxValue = -999;

   std::cout << depth << "\n";

   if ( depth >= MAX_DEPTH )
   {
      return getTeamUtilityValue( &teams, aiTeam );
   }

   //Determine possible moves and databases

   if ( teams.chainKill )
   {
      std::cout << "Max Chain Kill\n";

      return aiMaxChainKill( depth, teams, aiTeam, finalMove, first, maxValue );
   }  
   else
   {
      //Get the AI team
      if ( aiTeam == "black" )
      {
         team = &teams.black;
      }
      else
      {
         team = &teams.white;
      }

      //Are kills possible?
      canKill = canTeamKill( aiTeam, teams.black, teams.white );

      if ( canKill )
      {
         std::cout << "Max kill Moves\n";

         //Only look at pieces that can kill for moves
         for ( i = 0 ; i < team -> size() ; i++ )
         {
            if ( hasKillMoves( team -> at(i), teams.black, teams.white ) )
            {
               piece = &team -> at(i);

               //Get all kill moves for this piece
               moveCount = getKillMoves( newPositions, newMovePointCount, *piece, teams.black, teams.white, piece -> getBoardXPos(), piece -> getBoardYPos() );

               //Get databases for these move
               for ( j = 0 ; j < moveCount ; j++ )
               {
                  unsigned int temp = maxValue;
                  bool king = false;

                  //Get newTeams
                  newTeams = teams;
                  newTeams.chainKill = false;
                  newTeams.chainKillPiece = nullptr;
                  
                  //Move the moving piece to the end of their move
                  pieceDatabase = getStoneFromPosition( piece -> getBoardXPos(), piece -> getBoardYPos(), newTeams.black, newTeams.white );
                  pieceDatabase -> setBoardPosition( newPositions[j][ newMovePointCount[j] - 1 ].x, newPositions[j][ newMovePointCount[j] - 1 ].y );

                  king = pieceDatabase -> isKing();

                  pieceKill = getStoneFromPosition( newPositions[j][0].x, newPositions[j][0].y, newTeams.black, newTeams.white );
                  pieceKill -> kill();

                  removeDeadPieces( newTeams.black, newTeams.white );

                  //Check for chain kill
                  if ( king == pieceDatabase -> isKing() )
                  {
                     if ( hasKillMoves( *pieceDatabase, newTeams.black, newTeams.white ) )
                     {
                        newTeams.chainKill = true;
                        newTeams.chainKillPiece = pieceDatabase;

                        maxValue = std::max( maxValue, aiMaxChainKill( depth, newTeams, aiTeam, finalMove, first, maxValue ) );
                     }
                  }

                  if ( !newTeams.chainKill )
                  {
                     //Call min for this database
                     maxValue = std::max( maxValue, aiMin( depth + 1, newTeams, aiTeam ) );

                     //Check for best value
                     if ( first && maxValue != temp ) 
                     {
                        //Get the best move positions
                        finalMove -> goalPosition.x = newPositions[j][newMovePointCount[j] - 1].x;
                        finalMove -> goalPosition.y = newPositions[j][newMovePointCount[j] - 1].y;

                        //Piece to moves position
                        finalMove -> piecePosition.x = piece -> getBoardXPos();
                        finalMove -> piecePosition.y = piece -> getBoardYPos();
                     }
                  }
               }
            }
         }
      }
      else
      {
         std::cout << "Default Max moves\n";

         //Look at EVERY piece for moves
         for ( i = 0 ; i < team -> size() ; i++ )
         {
            piece = &team -> at(i);

            //Get all moves for this piece
            moveCount = getMovePositions( newPositions, newMovePointCount, *piece, teams.black, teams.white );

            //Get databases for these move
            for ( j = 0 ; j < moveCount ; j++ )
            {
               unsigned int temp = maxValue;

               //Get newTeams
               newTeams = teams;
               newTeams.chainKill = false;
               newTeams.chainKillPiece = nullptr;
               
               //Move the moving piece to the end of their move
               pieceDatabase = getStoneFromPosition( piece -> getBoardXPos(), piece -> getBoardYPos(), newTeams.black, newTeams.white );
               pieceDatabase -> setBoardPosition( newPositions[j][ newMovePointCount[j] - 1 ].x, newPositions[j][ newMovePointCount[j] - 1 ].y );

               //Call min for this database
               maxValue = std::max( maxValue, aiMin( depth + 1, newTeams, aiTeam ) );

               //Check for best value
               if ( first && maxValue != temp ) 
               {
                  //Get the best move positions
                  finalMove -> goalPosition.x = newPositions[j][newMovePointCount[j] - 1].x;
                  finalMove -> goalPosition.y = newPositions[j][newMovePointCount[j] - 1].y;

                  //Piece to moves position
                  finalMove -> piecePosition.x = piece -> getBoardXPos();
                  finalMove -> piecePosition.y = piece -> getBoardYPos();
               }
            }
         }
      }
   }

   //Return the best utility value
   return maxValue;
}

unsigned int aiMin( 
   unsigned int depth,
   struct teamDatabase teams,
   std::string aiTeam )
{
   sf::Vector2i newPositions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS];
   unsigned int newMovePointCount[MAX_POSSIBLE_MOVES];

   struct teamDatabase newTeams;
   std::vector<stone>* team;
   stone* piece;  //Current piece to look at moves for
   stone* pieceDatabase, *pieceKill;  //Piece to mess with in the database

   std::string opponentTeam;

   unsigned int moveCount;
   unsigned int i, j, k;
   unsigned int minValue;
   bool canKill;

   minValue = 999;

   std::cout << depth << "\n";

   if ( depth >= MAX_DEPTH )
   {
      return getTeamUtilityValue( &teams, aiTeam );
   }

   //Get the opponent team
   if ( aiTeam == "black" )
   {
      opponentTeam = "white";
   }
   else
   {
      opponentTeam = "black";
   }

   //Determine possible moves and databases

   if ( teams.chainKill )
   {
      std::cout << "Min Chain Kill\n";

      return aiMinChainKill( depth, teams, aiTeam, minValue );
   }  
   else
   {
      //Get the opponent team
      if ( opponentTeam == "black" )
      {
         team = &teams.black;
      }
      else
      {
         team = &teams.white;
      }

      //Are kills possible?
      canKill = canTeamKill( opponentTeam, teams.black, teams.white );

      if ( canKill )
      {
         std::cout << "Max kill moves\n";

         //Only look at pieces that can kill for moves
         for ( i = 0 ; i < team -> size() ; i++ )
         {
            if ( hasKillMoves( team -> at(i), teams.black, teams.white ) )
            {
               piece = &team -> at(i);

               //Get all kill moves for this piece
               moveCount = getKillMoves( newPositions, newMovePointCount, *piece, teams.black, teams.white, piece -> getBoardXPos(), piece -> getBoardYPos() );

               //Get databases for these move
               for ( j = 0 ; j < moveCount ; j++ )
               {
                  unsigned int temp = minValue;
                  bool king = false;

                  //Get newTeams
                  newTeams = teams;
                  newTeams.chainKill = false;
                  newTeams.chainKillPiece = nullptr;
                  
                  //Move the moving piece to the end of their move
                  pieceDatabase = getStoneFromPosition( piece -> getBoardXPos(), piece -> getBoardYPos(), newTeams.black, newTeams.white );
                  pieceDatabase -> setBoardPosition( newPositions[j][ newMovePointCount[j] - 1 ].x, newPositions[j][ newMovePointCount[j] - 1 ].y );

                  king = pieceDatabase -> isKing();

                  pieceKill = getStoneFromPosition( newPositions[j][0].x, newPositions[j][0].y, newTeams.black, newTeams.white );
                  pieceKill -> kill();

                  removeDeadPieces( newTeams.black, newTeams.white );

                  //Check for chain kill
                  if ( king == pieceDatabase -> isKing() )
                  {
                     if ( hasKillMoves( *pieceDatabase, newTeams.black, newTeams.white ) )
                     {
                        newTeams.chainKill = true;
                        newTeams.chainKillPiece = pieceDatabase;

                        //Call chain kill min
                        minValue = std::min( minValue, aiMinChainKill( depth, newTeams, aiTeam, minValue ) );
                     }
                  }

                  if ( !newTeams.chainKill )
                  {
                     //Call max for this database
                     minValue = std::min( minValue, aiMax( depth + 1, newTeams, aiTeam, nullptr, false ) );
                  }
               }
            }
         }
      }
      else
      {
         std::cout << "Default min moves\n";

         //Look at EVERY piece for moves
         for ( i = 0 ; i < team -> size() ; i++ )
         {
            piece = &team -> at(i);

            //Get all moves for this piece
            moveCount = getMovePositions( newPositions, newMovePointCount, *piece, teams.black, teams.white );

            //Get databases for these move
            for ( j = 0 ; j < moveCount ; j++ )
            {
               unsigned int temp = minValue;

               //Get newTeams
               newTeams = teams;
               newTeams.chainKill = false;
               newTeams.chainKillPiece = nullptr;
               
               //Move the moving piece to the end of their move
               pieceDatabase = getStoneFromPosition( piece -> getBoardXPos(), piece -> getBoardYPos(), newTeams.black, newTeams.white );
               pieceDatabase -> setBoardPosition( newPositions[j][ newMovePointCount[j] - 1 ].x, newPositions[j][ newMovePointCount[j] - 1 ].y );

               //Call min for this database
               minValue = std::min( minValue, aiMax( depth + 1, newTeams, aiTeam, nullptr, false ) );
            }
         }
      }
   }

   //Return the min utility value
   return minValue;
}

unsigned int aiMaxChainKill( 
   unsigned int depth,
   struct teamDatabase teams, 
   std::string aiTeam,
   move* finalMove, bool first,
   unsigned int maxValue )
{
   sf::Vector2i newPositions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS];
   unsigned int newMovePointCount[MAX_POSSIBLE_MOVES];
  
   sf::Vector2i maxPosition[MAX_MOVE_POINTS];
   unsigned int maxPointCount;
   sf::Vector2i position;  //Piece to move position

   struct teamDatabase newTeams;
   stone* piece;  //Current piece to look at moves for
   stone* pieceDatabase, *pieceKill;  //Piece to mess with in the database

   unsigned int moveCount;
   unsigned int i, j, k;

   piece = teams.chainKillPiece;

   //Only look at kill moves for this one piece
   moveCount = getKillMoves( newPositions, newMovePointCount, *piece, teams.black, teams.white, piece -> getBoardXPos(), piece -> getBoardYPos() );

   //Get databases for these move
   for ( j = 0 ; j < moveCount ; j++ )
   {
      unsigned int temp = maxValue;
      bool king = false;

      //Get newTeams
      newTeams = teams;
      newTeams.chainKill = false;
      newTeams.chainKillPiece = nullptr;
      
      //Move the moving piece to the end of their move
      pieceDatabase = getStoneFromPosition( piece -> getBoardXPos(), piece -> getBoardYPos(), newTeams.black, newTeams.white );
      pieceDatabase -> setBoardPosition( newPositions[j][ newMovePointCount[j] - 1 ].x, newPositions[j][ newMovePointCount[j] - 1 ].y );

      king = pieceDatabase -> isKing();

      //Kill the piece
      pieceKill = getStoneFromPosition( newPositions[j][0].x, newPositions[j][0].y, newTeams.black, newTeams.white );
      pieceKill -> kill();

      removeDeadPieces( newTeams.black, newTeams.white );

      //Check for chain kill
      if ( king == ( pieceDatabase -> isKing() ) )
      {
         if ( hasKillMoves( *pieceDatabase, newTeams.black, newTeams.white ) )
         {
            newTeams.chainKill = true;
            newTeams.chainKillPiece = pieceDatabase;

            maxValue = std::max( maxValue, aiMaxChainKill( depth, newTeams, aiTeam, finalMove, first, maxValue ) );
         }
      }

      if ( !newTeams.chainKill )
      {
         //Call min for this database
         maxValue = std::max( maxValue, aiMin( depth + 1, newTeams, aiTeam ) );

         if ( first && maxValue != temp )
         {
            //Get the best move positions
            finalMove -> goalPosition.x = newPositions[j][newMovePointCount[j] - 1].x;
            finalMove -> goalPosition.y = newPositions[j][newMovePointCount[j] - 1].y;

            //Piece to moves position
            finalMove -> piecePosition.x = piece -> getBoardXPos();
            finalMove -> piecePosition.y = piece -> getBoardYPos();
         }
      }
   }

   return maxValue;
}

unsigned int aiMinChainKill( 
   unsigned int depth,
   struct teamDatabase teams, 
   std::string aiTeam,
   unsigned int minValue )
{
   sf::Vector2i newPositions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS];
   unsigned int newMovePointCount[MAX_POSSIBLE_MOVES];
  
   sf::Vector2i position;  //Piece to move position

   struct teamDatabase newTeams;
   stone* piece;  //Current piece to look at moves for
   stone* pieceDatabase, *pieceKill;  //Piece to mess with in the database

   unsigned int moveCount;
   unsigned int j;

   piece = teams.chainKillPiece;

   //Only look at kill moves for this one piece
   moveCount = getKillMoves( newPositions, newMovePointCount, *piece, teams.black, teams.white, piece -> getBoardXPos(), piece -> getBoardYPos() );

   //Get databases for these move
   for ( j = 0 ; j < moveCount ; j++ )
   {
      unsigned int temp = minValue;
      bool king = false;

      //Get newTeams
      newTeams = teams;
      newTeams.chainKill = false;
      newTeams.chainKillPiece = nullptr;
      
      //Move the moving piece to the end of their move
      pieceDatabase = getStoneFromPosition( piece -> getBoardXPos(), piece -> getBoardYPos(), newTeams.black, newTeams.white );
      pieceDatabase -> setBoardPosition( newPositions[j][ newMovePointCount[j] - 1 ].x, newPositions[j][ newMovePointCount[j] - 1 ].y );

      king = pieceDatabase -> isKing();

      //Kill the piece
      pieceKill = getStoneFromPosition( newPositions[j][0].x, newPositions[j][0].y, newTeams.black, newTeams.white );
      pieceKill -> kill();

      removeDeadPieces( newTeams.black, newTeams.white );

      //Check for chain kill
      if ( king == ( pieceDatabase -> isKing() ) )
      {
         if ( hasKillMoves( *pieceDatabase, newTeams.black, newTeams.white ) )
         {
            newTeams.chainKill = true;
            newTeams.chainKillPiece = pieceDatabase;

            minValue = std::min( minValue, aiMinChainKill( depth, newTeams, aiTeam, minValue ) );
         }
      }

      if ( !newTeams.chainKill )
      {
         //Call min for this database
         minValue = std::min( minValue, aiMax( depth + 1, newTeams, aiTeam, nullptr, false ) );
      }
   }

   return minValue;
}

//Returns the utility value of the white or black team
//This value is determined by weighted piece points
//Param:
//teams - The database for the current teams
//team - Team to determine value for
unsigned int getTeamUtilityValue( struct teamDatabase* teams, std::string team )
{
   unsigned int weightPiece, weightKing, value;
   unsigned int i;

   weightPiece = 1;
   weightKing = 2;

   value = 0;
   i = 0;

   //Add up the weighted value of every piece in the black team
   for ( i = 0 ; i < teams -> black.size() ; i++ )
   {
      if ( teams -> black.at(i).isKing() )
      {
         value += weightKing;
      }
      else
      {
         value += weightPiece;
      }
   }

   teams -> valueBlack = value;

   value = 0;
   i = 0;

   //Add up the weighted value of every piece in the white team
   for ( i = 0 ; i < teams -> white.size() ; i++ )
   {
      if ( teams -> white.at(i).isKing() )
      {
         value += weightKing;
      }
      else
      {
         value += weightPiece;
      }
   }

   teams -> valueWhite = value;

   if ( team == "black" )
   {
      teams -> value = teams -> valueBlack - teams -> valueWhite;

      std::cout << teams -> value << " ???\n";
   }
   else
   {
      teams -> value = teams -> valueWhite - teams -> valueBlack;
   }

   return teams -> value;
}

//Returns true if this position on the board is free
//Otherwise returns false
//Param:
//x - Row on the board
//y - Column on the board
//black - The array of black pieces
//white - The array of white pieces
bool isPositionFree( unsigned int x, unsigned int y, std::vector<stone>& black, std::vector<stone>& white )
{
   int i;

   //Check black pieces
    for ( i = 0 ; i < black.size() ; i++ )  //Rows
    {
      if ( black.at(i).meetingPosition( x, y ) )
         return false;
    }

    //Check white pieces
    for ( i = 0 ; i < white.size() ; i++ )  //Rows
    {
      if ( white.at(i).meetingPosition( x, y ) )
         return false;
    }

    return true;
}

//Desc: Returns the stone located at this position on the board
//Param:
//x - Row on the board
//y - Column on the board
//black - The array of black pieces
//white - The array of white pieces
stone* getStoneFromPosition( unsigned int x, unsigned int y, std::vector<stone>& black, std::vector<stone>& white )
{
   int i;

   //Check black pieces
   for ( i = 0 ; i < black.size() ; i++ )  //Rows
   {
      if ( black.at(i).meetingPosition( x, y ) )
         return &black.at(i);
   }

   //Check white pieces
   for ( i = 0 ; i < white.size() ; i++ )  //Rows
   {
      if ( white.at(i).meetingPosition( x, y ) )
         return &white.at(i);
   }

    return nullptr;
}

//Gets all possible x and y positions of movement for a stone
//Modifies the positions array with the x and y positions
//Modifies the movePointCount with the number of points in each move
//Returns the number of possible moves
//Param:
//positions - The array of x and y positions for possible moves
//movePointCount - The number of points in a move
//piece - The piece to check the possible moves for
//black - The array of black pieces
//white - The array of white pieces
unsigned int getMovePositions( 
   sf::Vector2i positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES], 
   stone piece, std::vector<stone>& black, std::vector<stone>& white )
{
   unsigned int index = 0;
   unsigned int count = 0;
   unsigned int countChange = 0;

   bool king = piece.isKing();
   std::string moveDir = piece.getMoveDir();
   unsigned int boardX = piece.getBoardXPos();
   unsigned int boardY = piece.getBoardYPos();

   if ( king || moveDir == "down" )
   {
         //Down left
         if ( ( countChange = confirmPosition( positions, movePointCount, piece, black, white, index, boardX - 1, boardY + 1, -1, 1 ) ) != 0 )
         {
            index += countChange;
            count += countChange;
         }

         //Down right
         if ( ( countChange = confirmPosition( positions, movePointCount, piece, black, white, index, boardX + 1, boardY + 1, 1, 1 ) ) != 0 )
         {
            index += countChange;
            count += countChange;
         }
   }

   if ( king || moveDir == "up" )
   {
         //Up left
         if ( ( countChange = confirmPosition( positions, movePointCount, piece, black, white, index, boardX - 1, boardY - 1, -1, -1 ) ) != 0 )
         {
            index += countChange;
            count += countChange;
         }

         //Up right
         if ( ( countChange = confirmPosition( positions, movePointCount, piece, black, white, index, boardX + 1, boardY - 1, 1, -1 ) ) != 0 )
         {
            index += countChange;
            count += countChange;
         }
   }

   //Check for kill moves, and if so remove any other moves
   if ( count != 0 )
   { 
      bool found = false;
      int i, j, k;

      //Check if any kill moves were found
      for ( i = 0 ; i < index ; i++ )
      {
         if ( movePointCount[i] > 1 )
         {
            //Kill move found
            found = true;

            break;
         }
      }

      if ( found )
      {
         //Remove any non-kill moves (Since kills are forced)
         i = 0;

         while ( i < count )
         {
            if ( movePointCount[i] == 1 )
            {
               for ( j = i ; j < index - 1 ; j++ )
               {
                  for ( k = 0 ; k < movePointCount[j + 1] ; k++ )
                  {
                     positions[j][k] = positions[j + 1][k];
                  }

                  movePointCount[j] = movePointCount[j + 1];
               }

               count--;
            }
            else
            {
               //Only increment i when a move isn't removed (Since we would then skip moves)
               i++;
            }
         }
      }
   }

   return count;
}

//Confirms the position for this stone
//If confirmed, places the position in positions and returns the found position count
//Also gets the movePointCount points for each move
//Param:
//positions - The array of x and y positions for possible moves
//movePointCount - The number of points in a move
//piece - The piece to check the possible moves for
//black - The array of black pieces
//white - The array of white pieces
//index - The current index in the positions array
//x - Row on the board
//y - Column on the board 
//xChange - The x direction for this move (-1, or 1 )
//yChange - The y direction for this move (-1, or 1 )
unsigned int confirmPosition( 
   sf::Vector2i positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES],
   stone piece, std::vector<stone>& black, std::vector<stone>& white, 
   unsigned int index, 
   unsigned int x, unsigned int y, 
   unsigned int xChange, unsigned int yChange )
{
   if ( inRange( x, BOARD_RANGE_LOW, BOARD_RANGE_HIGH ) && inRange( y, BOARD_RANGE_LOW, BOARD_RANGE_HIGH ) )
   {
         if ( isPositionFree( x, y, black, white ) )
         {
            positions[index][0].x = x;
            positions[index][0].y = y;

            movePointCount[index] = 1;

            return 1;
         }
         else
         {
            //Check if the intruding piece is killable
            if ( isKillablePiece( piece, black, white, x, y, xChange, yChange ) )
            {
                  //Get the move to kill this piece

                  //Get the position of the enemy to kill
                  positions[index][0].x = x;
                  positions[index][0].y = y;

                  //Get the position after the kill
                  positions[index][1].x = x + xChange;
                  positions[index][1].y = y + yChange;

                  //2 points in a kill move
                  movePointCount[index] = 2;

                  return 1;
            }

            //Otherwise no valid move
         }

         //0 moves added
         return 0;
   }
}

//Gets all possible kill moves from position x, y
//Will add possible kill moves into the positions array
//Point count in a kill move are saved in movePointCount
//Param:
//positions - The array of x and y positions for possible moves
//movePointCount - The number of points in a move
//piece - The piece to check the possible kill moves for
//black - The array of black pieces
//white - The array of white pieces
//x - Row on the board
//y - Column on the board 
unsigned int getKillMoves( 
   sf::Vector2i positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES],
   stone piece, std::vector<stone>& black, std::vector<stone>& white, 
   unsigned int x, unsigned int y )
{
   //Get my piece information
   bool king;
   std::string moveDir;
   unsigned int boardX;
   unsigned int boardY;

   king = piece.isKing();
   moveDir = piece.getMoveDir();
   boardX = piece.getBoardXPos();
   boardY = piece.getBoardYPos();

   unsigned int moveCount;
   unsigned int index, pointIndex;

   moveCount = 0;
   index = 0;

   //Check for possible kill chains
   if ( king || moveDir == "down" )
   {
         //Down left
         if ( !isPositionFree( x - 1, y + 1, black, white ) )
         {
            //Is this piece killable?
            if ( isKillablePiece( piece, black, white, x - 1, y + 1, -1, 1 ) )
            {
               //Get the position of the enemy to kill
               positions[index][0].x = x - 1;
               positions[index][0].y = y + 1;

               //Get the position after the kill
               positions[index][1].x = x - 2;
               positions[index][1].y = y + 2;

               movePointCount[index] = 2; //2 points in a kill move

               moveCount += 1;
               index += 1;
            }
         }

         //Down right
         if ( !isPositionFree( x + 1, y + 1, black, white ) )
         {
            //Is this piece killable?
            if ( isKillablePiece( piece, black, white, x + 1, y + 1, 1, 1 ) )
            {
               //Get the position of the enemy to kill
               positions[index][0].x = x + 1;
               positions[index][0].y = y + 1;

               //Get the position after the kill
               positions[index][1].x = x + 2;
               positions[index][1].y = y + 2;

               movePointCount[index] = 2; //2 points in a kill move

               moveCount += 1;
               index += 1;
            }
         }
   }

   if ( king || moveDir == "up" )
   {
         //Up left
         if ( !isPositionFree( x - 1, y - 1, black, white ) )
         {
            //Is this piece killable?
            if ( isKillablePiece( piece, black, white, x - 1, y - 1, -1, -1 ) )
            {
               //Get the position of the enemy to kill
               positions[index][0].x = x - 1;
               positions[index][0].y = y - 1;

               //Get the position after the kill
               positions[index][1].x = x - 2;
               positions[index][1].y = y - 2;

               movePointCount[index] = 2; //2 points in a kill move

               moveCount += 1;
               index += 1;
            }
         }

         //Up right
         if ( !isPositionFree( x + 1, y - 1, black, white ) )
         {
            //Is this piece killable?
            if ( isKillablePiece( piece, black, white, x + 1, y - 1, 1, -1 ) )
            {
               //Get the position of the enemy to kill
               positions[index][0].x = x + 1;
               positions[index][0].y = y - 1;

               //Get the position after the kill
               positions[index][1].x = x + 2;
               positions[index][1].y = y - 2;

               movePointCount[index] = 2; //2 points in a kill move

               moveCount += 1;
               index += 1;
            }
         }
   }

   //Return the number of kill moves found
   return moveCount;
}

//Returns the positions of pieces that can kill
//Param:
//black - The array of black pieces
//white - The array of white pieces
//killspots - Array of positions for kills
//currentTurn - The team whose turn it is
unsigned int getPiecesThatCanKill( std::vector<stone>& black, std::vector<stone>& white, sf::Vector2f *killSpots, std::string& currentTurn )
{
   int i, index;
   std::vector<stone>* team;

   i = 0;
   index = 0;

   if ( currentTurn == "black" )
   {
      team = &black;
   }
   else
   {
      team = &white;
   }

   for ( i = 0 ; i < team -> size() ; i++ )
   {
      if ( hasKillMoves( team -> at(i), black, white ) )
      {
         killSpots[index] = team -> at(i).getPosition();

         index++;
      }
   }

   return index;
}

//Returns true if piece has kill moves
//Param:
//piece - The piece to check the possible kill moves for
//black - The array of black pieces
//white - The array of white pieces
bool hasKillMoves( stone piece, std::vector<stone>& black, std::vector<stone>& white )
{
   //Get my piece information
   bool king;
   std::string moveDir;
   unsigned int boardX;
   unsigned int boardY;
   unsigned int x, y;

   king = piece.isKing();
   moveDir = piece.getMoveDir();
   boardX = piece.getBoardXPos();
   boardY = piece.getBoardYPos();
   
   x = piece.getBoardXPos();
   y = piece.getBoardYPos();

   //Check for possible kill chains
   if ( king || moveDir == "down" )
   {
         //Down left
         if ( !isPositionFree( x - 1, y + 1, black, white ) )
         {
            //Is this piece killable?
            if ( isKillablePiece( piece, black, white, x - 1, y + 1, -1, 1 ) )
            {
               return true;
            }
         }

         //Down right
         if ( !isPositionFree( x + 1, y + 1, black, white ) )
         {
            //Is this piece killable?
            if ( isKillablePiece( piece, black, white, x + 1, y + 1, 1, 1 ) )
            {
               return true;
            }
         }
   }

   if ( king || moveDir == "up" )
   {
         //Up left
         if ( !isPositionFree( x - 1, y - 1, black, white ) )
         {
            //Is this piece killable?
            if ( isKillablePiece( piece, black, white, x - 1, y - 1, -1, -1 ) )
            {
               return true;
            }
         }

         //Up right
         if ( !isPositionFree( x + 1, y - 1, black, white ) )
         {
            //Is this piece killable?
            if ( isKillablePiece( piece, black, white, x + 1, y - 1, 1, -1 ) )
            {
               return true;
            }
         }
   }

   return false;
}

//Returns true if the chosen team can kill
//Param:
//teamName - "black" or "white"
//black - The array of black pieces
//white - The array of white pieces
bool canTeamKill( std::string teamName, std::vector<stone>& black, std::vector<stone>& white )
{
   std::vector<stone>* team;
   unsigned int i;

   i = 0;

   if ( teamName == "black" )
   {
      team = &black;
   }
   else
   {
      team = &white;
   }

   //Check all pieces for a kill move
   for ( i = 0 ; i < team -> size() ; i++ )
   {
      if ( hasKillMoves( team -> at(i), black, white ) )
      {
         return true;
      }
   }

   return false;
}

//Checks if piece can the piece at x, y in the direction of xChange + yChange
//Returns true if a kill can be executed
//piece - The piece to check the kill for
//black - The array of black pieces
//white - The array of white pieces
//x - Row on the board
//y - Column on the board 
//xChange - The x direction for this kill (-1, or 1 )
//yChange - The y direction for this kill (-1, or 1 )
bool isKillablePiece( 
   stone piece, std::vector<stone>& black, std::vector<stone>& white, 
   unsigned int x, unsigned int y, 
   unsigned int xChange, unsigned int yChange )
{
   //Get the piece
   stone* pieceInWay = getStoneFromPosition( x, y, black, white );

   //This should not happen
   if ( pieceInWay == nullptr )
      return -1;

   //Is this an enemy piece?
   if ( pieceInWay -> getColor() != piece.getColor() )
   {
      //This is an enemy piece

      //Can I jump over?
      //Must be within the board range and not intruded by another piece
      if ( inRange( x + xChange, BOARD_RANGE_LOW, BOARD_RANGE_HIGH ) && inRange( y + yChange, BOARD_RANGE_LOW, BOARD_RANGE_HIGH ) && isPositionFree( x + xChange, y + yChange, black, white ) )
      {
            return true;
      }
   }

   return false;
}

//Checks if the mouse is over a move
//This is done by checking each move point
//Returns the index of the pressed move
//Param:
//positions - The array of x and y positions for possible moves
//movePointCount - Number of points in a move
//moveCount - The number of possible moves for the current piece
//mouseX - The mouse x position
//mouseY - The mouse y position
unsigned int checkMouseOverMove( 
   sf::Vector2i positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES], 
   unsigned int moveCount,
   int mouseX, int mouseY )
{
   int x, y, i, j;
   int xLeft, yTop;

   for ( i = 0 ; i < moveCount ; i++ )
   {
      for ( j = 0 ; j < movePointCount[i] ; j++ )
      {
         x = positions[i][j].x;
         y = positions[i][j].y;

         //Check the mouse coordinate
         xLeft = BOARD_ORIGIN_X + BOARD_SQUARE_OFFSET + ( x * BOARD_SQUARE_SIZE );
         yTop = BOARD_ORIGIN_Y + BOARD_SQUARE_OFFSET + ( y * BOARD_SQUARE_SIZE );

         if ( ( xLeft <= mouseX && xLeft + BOARD_SQUARE_SIZE >= mouseX ) && ( yTop <= mouseY && yTop + BOARD_SQUARE_SIZE >= mouseY ) )
         {
            return i;
         }
      }
   }

   return -1;
}

//Removes dead pieces from the black and white vectors
//Param:
//black - Black pieces
//white - White pieces
void removeDeadPieces( std::vector<stone>& black, std::vector<stone>& white )
{
   int i;

   i = 0;

   //Remove dead black pieces
   while ( i < black.size() )
   {
      if ( !( black.at(i).isAlive() ) )
      {
         black.erase( black.begin() + i );
      }
      else
      {
         //Only increment i when a piece isn't removed
         i++;
      }
   }

   i = 0;
   
   //Remove white black pieces
   while ( i < white.size() )
   {
      if ( !( white.at(i).isAlive() ) )
      {
         white.erase( white.begin() + i );
      }
      else
      {
         //Only increment i when a piece isn't removed
         i++;
      }
   }

   return;
}
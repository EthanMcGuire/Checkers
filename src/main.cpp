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
#include <time.h>

#include "checkers.hh"

//-mwindows
//ming32-make to compile and link the program

bool cheater = false;

int main()
{
   //Randomize
   srand( time( NULL ) );

   ///Declare variables

   //Main
   states state = getAction;
   std::string winner;
   std::string teams[2], teamDir[2], teamColor[2];
   enum turn currentTurn = WHITE;
   bool canKill = false;
   bool twoPlayer = false;

   //Team settings
   teams[BLACK] = "ai";    //Team name should be "ai" or "player"
   teams[WHITE] = "player";
   teamDir[BLACK] = "down";
   teamDir[WHITE] = "up";
   teamColor[BLACK] = "black";
   teamColor[WHITE] = "white";

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

   //AI Move
   std::vector<move> aiMoveList;

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

   //Create the teams
   
   if ( createTeam( black, "black", teamDir[BLACK], blackStoneTexture, &blackKingTexture ) == EXIT_FAILURE )
      return EXIT_FAILURE;

   if ( createTeam( white, "white", teamDir[WHITE], whiteStoneTexture, &whiteKingTexture ) == EXIT_FAILURE )
      return EXIT_FAILURE;

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

         //Do NOT get mouse inputs, if its an AIs turn
         if ( ( teams[currentTurn] == "player" ) && ( state == getAction || state == getChainKill ) )
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

                     if ( currentTurn == BLACK )
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

      if ( teams[currentTurn] == "ai" && ( state == getAction || state == getChainKill ) )
      {
         //AIs turn

         //A chain kill can cancel out into a new move in cheater mode
         if ( cheater )
         {
            while ( aiMoveList.size() > 0 )
            {
               aiMoveList.pop_back();
            }
         }

         if ( aiMoveList.size() == 0 )
         {
            //Get the move list for this AIs turn
            getAIMove( &aiMoveList, black, white, teamColor[currentTurn], teamDir[BLACK], teamDir[WHITE] );
         }

         if ( aiMoveList.size() != 0 )
         {
            //Get the AI's next move
            int moveX, moveY;
            struct move nextMove;

            nextMove = aiMoveList.at(0);

            //Get the stone for the selected move
            selectedPiece = getStoneFromPosition( nextMove.piecePosition.x, nextMove.piecePosition.y, black, white );

            //Get the move positions for this move
            moveX = nextMove.goalPosition.x - nextMove.piecePosition.x;
            moveY = nextMove.goalPosition.y - nextMove.piecePosition.y;

            //Get the point count in this move
            movePointCount[0] = std::max( abs( moveX ), abs( moveY ) );

            moveX = sgn(moveX);
            moveY = sgn(moveY);

            for ( i = 1 ; i <= movePointCount[0] ; i++ )
            {
               movePositions[0][i-1].x = nextMove.piecePosition.x + moveX * i;
               movePositions[0][i-1].y = nextMove.piecePosition.y + moveY * i;
            }

            selected = false;
            moveIndex = 0;
            state = startAction;

            //Remove the move from the vector
            aiMoveList.erase( aiMoveList.begin() );
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
                  //AI doesn't need this
                  if ( teams[currentTurn] == "player" )
                  {
                     //Check for kill chains
                     moveCount = getKillMoves( movePositions, movePointCount, *selectedPiece, black, white, actionPointGoalBoard.x, actionPointGoalBoard.y );

                     if ( moveCount != 0 )
                     {
                        //Set up move information

                        //Set the piece highlight on the current pieces location
                        pieceHighlight.setPosition( selectedPiece -> getPosition() );

                        //Check for mouse over this new move
                        mouseMoveIndex = checkMouseOverMove( movePositions, movePointCount, moveCount, globalMousePosWorld.x, globalMousePosWorld.y );

                        killChain = true;
                     }
                  }
                  else
                  {
                     //For AI, just check if they have remaining moves
                     if ( aiMoveList.size() != 0 )
                     {
                        killChain = true;
                     }
                  }
               }
            }

            if ( killChain )
            {
               //Start the getChainKill state

               //AI moves don't get highlighted
               if ( teams[currentTurn] == "player" )
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
               else
               {
                  std::string otherTeam;

                  if ( teamColor[currentTurn] == "black" )
                  {
                     otherTeam = "white";
                  }
                  else
                  {
                     otherTeam = "black";
                  }

                  //Check if the other team is softlocked
                  if ( !canTeamKill( otherTeam, black, white ) && !canTeamMove( otherTeam, black, white ) )
                  {
                     //Game over
                     winner = teamColor[currentTurn];
                     winner[0] = toupper( winner[0] );
                     state = gameOver;
                  }
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
         //Only highlight for non-AI players
         if ( teams[currentTurn] == "player" )
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

//Checks if the x, y position is within the boards bounds
//Returns true if so
//Param:
//x - x position on board
//y - y position on board
bool inBounds( unsigned int x, unsigned int y )
{
   if ( !inRange( x, BOARD_RANGE_LOW, BOARD_RANGE_HIGH ) )
   {
      return false;
   }

   if ( !inRange( y, BOARD_RANGE_LOW, BOARD_RANGE_HIGH ) )
   {
      return false;
   }

   return true;
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

//Creates a checkers team and pushes each piece into team
//Param:
//team - The vector of pieces/stones for the team
//teamColor - The color of this team "black" or "white"
//teamDir - The direction this team moves "up" or "down"
//texture - The texture for this teams pieces
//textureKing - The texture for this teams kings
int createTeam( std::vector<stone>& team, std::string teamColor, std::string teamDir, sf::Texture& texture, sf::Texture* textureKing )
{
   //Create the pieces
   unsigned int offset; //Offset based on shifted rows (On the board)
   unsigned int boardX, boardY, yPos;
   unsigned int i;

   offset = 0;

   //Adjust for being at the top of the board
   if ( teamDir == "down" ) offset = 1;

   boardX = 0;
   boardY = 0;

   //Create the black pieces
   for ( i = 0 ; i < PIECE_COUNT ; i++ )
   {
      team.push_back( *(new stone) );

       //Load black piece texture
      if ( !( team.at(i).load( texture, textureKing, teamColor, teamDir ) ) )
         return EXIT_FAILURE;

      yPos = boardY;

      //Adjust for begin at the bottom of the board
      if ( teamDir == "up" ) yPos += BOTTOM_PLAYER_ROW_OFFSET;

      team.at(i).setBoardPosition( offset + boardX * 2, yPos ); //Set the black positions

      boardX += 1;

      if ( boardX >= ROW_SIZE )
      {
         boardX = 0;
         boardY += 1;

         offset = !offset;
      }
   }

   return EXIT_SUCCESS;
}

//Gets the next turn
//Param:
//currentTurn - The current team
//canKill - Whether a kill move can be performed
//black - The array of black pieces
//white - The array of white pieces
void getNextTurn( enum turn& currentTurn, bool& canKill, std::vector<stone>& black, std::vector<stone>& white )
{
   int i = 0;
   std::string name;

   canKill = false;

   //Get the next turn
   if ( currentTurn == BLACK )
   {
      currentTurn = WHITE;

      name = "white";
   }
   else
   {
      currentTurn = BLACK;

      name = "black";
   }

   canKill = canTeamKill( name, black, white );
}

//Gets the next move for the AI using the getTeamUtilityValue and min/max functions
//Param:
//moveList - List of moves to perform
//black - The array of black pieces
//white - The array of white pieces
//aiTeam - The team to get a move for (The AI's team)
//blackDir - black teams direction of movement "up" or "down"
//whiteDir - black teams direction of movement "up" or "down"
void getAIMove( 
   std::vector<move>* moveList, 
   std::vector<stone>& black, std::vector<stone>& white,
   std::string aiTeam, std::string blackDir, std::string whiteDir )
{
   unsigned int i, j;
   int moveCount = 0;
   int value;
   struct teamDatabase teams, finalDatabase;
   stone* piece;

   //Initialize the database
   for ( i = 0 ; i < BOARD_RANGE_HIGH + 1 ; i++ )
   {
      for ( j = 0 ; j < BOARD_RANGE_HIGH + 1 ; j++ )
      {
         teams.blackTeam[i][j] = 0;
         teams.whiteTeam[i][j] = 0;
      }
   }

   //Get team directions
   teams.blackDir = blackDir;
   teams.whiteDir = whiteDir;

   //Initialize the database
   //Pawns set to 1
   //Kings set to 2
   for ( i = 0 ; i < black.size() ; i++ )
   {
      piece = &black.at(i);
      teams.blackTeam[piece -> getBoardXPos()][piece -> getBoardYPos()] = 1 + ( piece -> isKing() );
   }

   for ( i = 0 ; i < white.size() ; i++ )
   {
      piece = &white.at(i);
      teams.whiteTeam[piece -> getBoardXPos()][piece -> getBoardYPos()] = 1 + ( piece -> isKing() );
   }

   //Get the best database to move to using max
   value = aiMax( 0, teams, &finalDatabase, aiTeam, true );

   //Confirm a move is possible
   if ( finalDatabase.moves.size() > 0 )
   {
      //Get the move list
      *moveList = finalDatabase.moves;
   }
   else
   {
      moveList -> clear();
   }

   //std::cout << value << " Value\n";
}

//Returns the max utility value for every possible move from the teams database
//Param:
//depth - Depth in the tree
//teams - The current teams database
//aiTeam - The ai team color to get a move for
//finalDatabase - The database to return at the end
//first - Whether this is the first max call
int aiMax( 
   unsigned int depth,
   struct teamDatabase teams, 
   struct teamDatabase* finalDatabase,
   std::string aiTeam, bool first )
{
   //Return the utility value if the game is over or MAX_DEPTH is reached
   if ( databaseGameEnd( &teams ) || depth >= MAX_DEPTH )
   {
      int value = getTeamUtilityValue( &teams, aiTeam );

      return value;
   }

   std::vector<teamDatabase> newTeams;
   std::vector<int> values;
   unsigned int i, j, k;
   unsigned int maxIndex = 0;
   int maxValue, temp;

   maxValue = -999;

   //Get the new teams
   getNewDatabases( newTeams, teams, aiTeam );

   //No moves available?
   if ( newTeams.size() == 0 )
   {
      if ( first )
      {
         *finalDatabase = teams;
      }

      int value = getTeamUtilityValue( &teams, aiTeam );

      return value;
   }

   //Call min on the teams
   for ( i = 0 ; i < newTeams.size() ; i++ )
   {
      temp = aiMin( depth + 1, newTeams.at(i), aiTeam );
      
      if ( temp > maxValue )
      {
         maxValue = temp;
         maxIndex = i;
      }
   }

   if ( first )
   {
      *finalDatabase = newTeams.at( maxIndex );
   }

   return maxValue;
}

//Returns the min utility value for every possible move from the teams database
//Param:
//depth - Depth in the tree
//teams - The current teams database
//aiTeam - The ai team color to get a move for
int aiMin( 
   unsigned int depth,
   struct teamDatabase teams,
   std::string aiTeam )
{
   //Return the utility value if the game is over or MAX_DEPTH is reached
   if ( databaseGameEnd( &teams ) || depth >= MAX_DEPTH )
   {
      return getTeamUtilityValue( &teams, aiTeam );
   }

   std::vector<teamDatabase> newTeams;
   std::string enemyTeam;
   unsigned int i;
   int minValue = 999;

   if ( aiTeam == "black" )
   {
      enemyTeam = "white";
   }
   else
   {
      enemyTeam = "black";
   }

   //Get the new teams
   getNewDatabases( newTeams, teams, enemyTeam );

   //No moves available?
   if ( newTeams.size() == 0 )
   {
      return getTeamUtilityValue( &teams, aiTeam );
   }

   //Call max on the teams
   for ( i = 0 ; i < newTeams.size() ; i++ )
   {
      minValue = std::min( minValue, aiMax( depth + 1, newTeams.at(i), nullptr, aiTeam, false ) );
   }

   return minValue;
}

//Returns the utility value of the white or black team
//This value is determined by weighted piece points
//Param:
//teams - The database for the current teams
//team - Team to determine value for
int getTeamUtilityValue( struct teamDatabase* teams, std::string team )
{
   //Relaxed cheating problem
      //Assume I go 2 moves in a row
      //If this does not result in a good position, this path can be discarded
   //Precompute openings and endings strategies to save time

   //ALways have moving foward valued?

   int weightPiece, weightKing;
   int value, blackValue, whiteValue;
   int black, white;
   unsigned int blackPawns, whitePawns, blackKings, whiteKings;
   unsigned int whiteTotal, blackTotal;
   unsigned int i, j;

   weightPiece = 3;
   weightKing = 6;

   blackValue = 0;
   whiteValue = 0;

   blackPawns = 0;
   whitePawns = 0;
   blackKings = 0;
   whiteKings = 0;

   //Get the value of for every piece on the board
   for ( i = 0 ; i <= BOARD_RANGE_HIGH ; i++ )
   {
      for ( j = 0 ; j <= BOARD_RANGE_HIGH ; j++ )
      {
         black = teams -> blackTeam[i][j];
         white = teams -> whiteTeam[i][j];

         //Get the pieces value
         if ( black != 0 )
         {
            if ( black == 1 )
            {
               blackPawns++;

               //Add value based on how close they are to becoming king
               /*
               if ( teams -> blackDir == "down" )
               {
                  blackValue += std::max( 0, ( int ) ( j - 2 ) );
               }
               else
               {
                  blackValue += std::max( 0, ( int ) ( 5 - j ) );
               }
               */
            }
            else if ( black == 2 )
            {
               blackKings++;
            }
         }
        
         if ( white != 0 )
         {
            if ( white == 1 )
            {
               whitePawns++;

               //Add value based on how close they are to becoming king
               /*
               if ( teams -> whiteDir == "down" )
               {
                  whiteValue += std::max( 0, ( int ) ( j - 2 ) );
               }
               else
               {
                  whiteValue += std::max( 0, ( int ) ( 5 - j ) );
               }
               */
            }
            else if ( white == 2 )
            {
               whiteKings++;
            }
         }
      }
   }

   blackTotal = blackPawns + blackKings;
   whiteTotal = whitePawns + whiteKings;

   blackValue += ( blackPawns * weightPiece ) + ( blackKings * weightKing );
   whiteValue += ( whitePawns * weightPiece ) + ( whiteKings * weightKing );

   //Check how far each piece is on the board

   //Add value to the team with more pieces
      //This position is favoured for trades
      //+3
   if ( blackTotal != whiteTotal )
   {
      if ( blackTotal > whiteTotal + 1 )
      {
         //blackValue += 3;
      }
      else if ( whiteTotal > blackTotal + 1 )
      {
         //whiteValue += 3;
      }
   }

   //Check if a team has lost (either by having no pieces or being softlocked)
      //+10 to the winning team

   //Get the final utitility value for this team
   if ( team == "black" )
   {
      value = blackValue - whiteValue;
   }
   else
   {
      value = whiteValue - blackValue;
   }
   
   value += ( rand() % 5 ) - 2;  //Add -2 to 2 randomness

   return value;
}

//Gets the new databases after teamColor performs any possible move
//Param:
//newTeams - The vector of teams databases to put the new databases in
//teams - The current database of teams
//teamColor - The current team color whose turn it is
void getNewDatabases( std::vector<teamDatabase>& newTeams, struct teamDatabase teams, std::string teamColor )
{
   unsigned int i, j;
   unsigned int teamMove[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1];
   unsigned int teamOther[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1];
   std::string dir, dirOther;
   bool canKill = false;

   //Copy the team arrays based on whose turn it is
   if ( teamColor == "black" )
   {
      //Black teams move
      std::copy( &teams.blackTeam[0][0], &teams.blackTeam[0][0] + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &teamMove[0][0] );
      std::copy( &teams.whiteTeam[0][0], &teams.whiteTeam[0][0] + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &teamOther[0][0] );

      dir = teams.blackDir;
      dirOther = teams.whiteDir;
   }
   else
   {
      //White teams move
      std::copy( &teams.whiteTeam[0][0], &teams.whiteTeam[0][0] + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &teamMove[0][0] );
      std::copy( &teams.blackTeam[0][0], &teams.blackTeam[0][0] + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &teamOther[0][0] );

      dir = teams.whiteDir;
      dirOther = teams.blackDir;
   }

   //Check if kill moves can be performed
   if ( !cheater )
   {
      canKill = checkDatabaseForKills( teamMove, teamOther, dir );
   }

   //Perform every possible move for each piece
   for ( i = 0 ; i <= BOARD_RANGE_HIGH ; i++ )
   {
      for ( j = 0 ; j <= BOARD_RANGE_HIGH ; j++ )
      {
         if ( teamMove[i][j] != 0 )
         {
            //Perform every move for this piece

            //Down right
            performDatabaseMove( newTeams, teamMove, teamOther, teamColor, dir, dirOther, i, j, 1, 1, canKill );

            //Down left
            performDatabaseMove( newTeams, teamMove, teamOther, teamColor, dir, dirOther, i, j, -1, 1, canKill );

            //Up right
            performDatabaseMove( newTeams, teamMove, teamOther, teamColor, dir, dirOther, i, j, 1, -1, canKill );

            //Up left
            performDatabaseMove( newTeams, teamMove, teamOther, teamColor, dir, dirOther, i, j, -1, -1, canKill );
         }
      }
   }
}

//Performs a move and adds the new database to newTeams
//Param:
//newTeams - The vector of teams databases to put the new databases in
//teamMove - The database array for the moving team
//teamOther - The database array for the enemy team
//teamColor - The current team color whose turn it is
//teamDir - The current team dir whose turn it is
//teamDirOther - The dir of the other team
//boardX - The x position of the moving piece
//boardY - The y position of the moving piece
//moveX - The x change for this piece
//moveY - The y change for this piece
//canKill - Whether this database can perform kill moves
void performDatabaseMove( 
   std::vector<teamDatabase>& newTeams, 
   unsigned int teamMove[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1], 
   unsigned int teamOther[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1],
   std::string teamColor, std::string teamDir, std::string teamDirOther,
   unsigned int boardX, unsigned int boardY,
   int moveX, int moveY,
   bool canKill )
{
   bool addToDatabase = false;

   if ( !inBounds( boardX + moveX, boardY + moveY ) )
   {
      //Move out of range
      return;
   }

   //Return if this piece cannot move in this direction
   //Kings can move in any direction
   if ( teamMove[boardX][boardY] != 2 )
   {
      if ( moveY == 1 && teamDir != "down" )
      {
         return;
      }

      if ( moveY == -1 && teamDir != "up" )
      {
         return;
      }
   }

   //Dont wanna change original array values
   unsigned int teamMoveTemp[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1];
   unsigned int teamOtherTemp[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1];

   std::copy( &teamMove[0][0], &teamMove[0][0] + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &teamMoveTemp[0][0] );
   std::copy( &teamOther[0][0], &teamOther[0][0] + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &teamOtherTemp[0][0] );

   //Check if this position is free
   if ( teamMoveTemp[boardX + moveX][boardY + moveY] == 0 && teamOtherTemp[boardX + moveX][boardY + moveY] == 0 )
   {
      if ( !canKill )
      {
         //Move the piece
         teamMoveTemp[boardX + moveX][boardY + moveY] = teamMoveTemp[boardX][boardY];
         teamMoveTemp[boardX][boardY] = 0;

         //Check if this piece became king
         if ( teamMoveTemp[boardX + moveX][boardY + moveY] != 2 )
         {
            if ( teamDir == "up" && ( boardY + moveY ) == BOARD_RANGE_LOW )
            {
               teamMoveTemp[boardX + moveX][boardY + moveY] = 2;
            }
            else if ( teamDir == "down" && ( boardY + moveY ) == BOARD_RANGE_HIGH )
            {
               teamMoveTemp[boardX + moveX][boardY + moveY] = 2;
            }
         }

         addToDatabase = true;
      }
   }
   else
   {
      //Check if an enemy piece is in the way
      if ( teamOtherTemp[boardX + moveX][boardY + moveY] != 0 )
      {
         if ( inBounds( boardX + moveX * 2, boardY + moveY * 2 ) )
         {
            //Check if the land position is free
            if ( teamMoveTemp[boardX + moveX * 2][boardY + moveY * 2] == 0 && teamOtherTemp[boardX + moveX * 2][boardY + moveY * 2] == 0 )
            {
               //Prepare the database for the chain kill
               struct teamDatabase nextTeam;

               //Check for chain kill and perform the kill move
               databaseChainKill( newTeams, nextTeam, teamMoveTemp, teamOtherTemp, teamColor, teamDir, teamDirOther, boardX, boardY, boardX + moveX * 2, boardY + moveY * 2 );
            }
         }
      }
   }

   //Add this new database to the database vector
   if ( addToDatabase )
   {
      struct teamDatabase next;
      struct move nextMove;

      //Check the moving team color
      if ( teamColor == "black" )
      {
         std::copy( &teamMoveTemp[0][0], &teamMoveTemp[0][0]  + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &next.blackTeam[0][0]  );
         std::copy( &teamOtherTemp[0][0], &teamOtherTemp[0][0]  + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &next.whiteTeam[0][0]  );

         next.blackDir = teamDir;
         next.whiteDir = teamDirOther;
      }
      else
      {
         std::copy( &teamMoveTemp[0][0], &teamMoveTemp[0][0]  + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &next.whiteTeam[0][0]  );
         std::copy( &teamOtherTemp[0][0], &teamOtherTemp[0][0]  + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &next.blackTeam[0][0]  );

         next.whiteDir = teamDir;
         next.blackDir = teamDirOther;
      }
      
      //Get the specific move info
      nextMove.piecePosition = sf::Vector2i( boardX, boardY );
      nextMove.goalPosition = sf::Vector2i( boardX + moveX, boardY + moveY );

      next.moves.push_back( nextMove );

      newTeams.push_back( next );
   }

   return;
}

//Check for a follow up kill for the currently moving team in this database
//and adds a new database when no more kills are available
//Param:
//newTeams - The vector of teams databases to put the new databases in
//nextTeam - The next team to get a database chain kill move for
//teamMove - The database array for the moving team
//teamOther - The database array for the enemy team
//teamColor - The current team color whose turn it is
//teamDir - The current team dir whose turn it is
//teamDirOther - The dir of the other team
//startX - The x of the moving piece
//startY - The y of the moving piece
//boardX - The landing x position of the moving piece
//boardY - The landing y position of the moving piece
void databaseChainKill( 
   std::vector<teamDatabase>& newTeams, 
   struct teamDatabase nextTeam, 
   unsigned int teamMove[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1], 
   unsigned int teamOther[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1],
   std::string teamColor, std::string teamDir, std::string teamDirOther,
   int startX, int startY, int boardX, int boardY )
{
   struct move nextMove;
   bool addToDatabase = true;
   bool canKill = true;
   int moveX[4] = {-1, -1, 1, 1};
   int moveY[4] = {-1, 1, -1, 1};
   unsigned int k;

   //Get the specific move info
   nextMove.piecePosition = sf::Vector2i( startX, startY );
   nextMove.goalPosition = sf::Vector2i( boardX, boardY );

   nextTeam.moves.push_back( nextMove );

   //Don't wanna change original array values
   unsigned int teamMoveTemp[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1];
   unsigned int teamOtherTemp[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1];

   std::copy( &teamMove[0][0], &teamMove[0][0] + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &teamMoveTemp[0][0] );
   std::copy( &teamOther[0][0], &teamOther[0][0] + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &teamOtherTemp[0][0] );

   //ACTUALLY perform the kill now

   int offsetX, offsetY;

   offsetX = boardX - startX;
   offsetX = offsetX / 2;

   offsetY = boardY - startY;
   offsetY = offsetY / 2;

   //Kill the piece
   teamOtherTemp[startX + offsetX][startY + offsetY] = 0;

   //Move the piece
   teamMoveTemp[boardX][boardY] = teamMoveTemp[startX][startY];
   teamMoveTemp[startX][startY] = 0;

   //Check if this piece became king
   if ( teamMoveTemp[boardX][boardY] != 2 )
   {
      if ( teamDir == "up" && boardY == BOARD_RANGE_LOW )
      {
         teamMoveTemp[boardX][boardY] = 2;
         canKill = false;  //Cant kill after becoming king
      }
      else if ( teamDir == "down" && boardY == BOARD_RANGE_HIGH )
      {
         teamMoveTemp[boardX][boardY] = 2;
         canKill = false;  //Cant kill after becoming king
      }
   }

   //Check for possible kills in each direction
   //Kings move in any direction
   if ( canKill )
   {
      for ( k = 0 ; k < 4 ; k++ )
      {
         //Down kills
         if ( teamMoveTemp[boardX][boardY] == 2 || ( ( moveY[k] == 1 && teamDir == "down" ) || ( moveY[k] == -1 && teamDir == "up" ) ) )
         {
            if ( inRange( boardY + moveY[k] * 2, BOARD_RANGE_LOW, BOARD_RANGE_HIGH ) )
            {
               //Down right
               if ( inRange( boardX + moveX[k] * 2, BOARD_RANGE_LOW, BOARD_RANGE_HIGH ) )
               {
                  //Check for enemy piece
                  if ( teamOtherTemp[boardX + moveX[k]][boardY + moveY[k]] != 0 )
                  {
                     //Confirm the destination is clear
                     if ( teamMoveTemp[boardX + moveX[k] * 2][boardY + moveY[k] * 2] == 0 && teamOtherTemp[boardX + moveX[k] * 2][boardY + moveY[k] * 2] == 0 )
                     {
                        //Can kill
                        addToDatabase = false;  //Will check for another chain kill

                        //Check for chain kill
                        databaseChainKill( newTeams, nextTeam, teamMoveTemp, teamOtherTemp, teamColor, teamDir, teamDirOther, boardX, boardY, boardX + moveX[k] * 2, boardY + moveY[k] * 2 ); 
                     }
                  }
               }
            }
         }
      }
   }

   //Add this database
   if ( addToDatabase )
   {
      //Check the moving team color
      if ( teamColor == "black" )
      {
         std::copy( &teamMoveTemp[0][0], &teamMoveTemp[0][0]  + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &nextTeam.blackTeam[0][0]  );
         std::copy( &teamOtherTemp[0][0], &teamOtherTemp[0][0]  + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &nextTeam.whiteTeam[0][0]  );

         nextTeam.blackDir = teamDir;
         nextTeam.whiteDir = teamDirOther;
      }
      else
      {
         std::copy( &teamMoveTemp[0][0], &teamMoveTemp[0][0]  + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &nextTeam.whiteTeam[0][0]  );
         std::copy( &teamOtherTemp[0][0], &teamOtherTemp[0][0]  + (int) pow( BOARD_RANGE_HIGH + 1, 2 ), &nextTeam.blackTeam[0][0]  );

         nextTeam.whiteDir = teamDir;
         nextTeam.blackDir = teamDirOther;
      }

      newTeams.push_back( nextTeam );
   }
}

bool checkDatabaseForKills( 
   unsigned int teamMove[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1], 
   unsigned int teamOther[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1],
   std::string teamDir )
{
   unsigned int i, j, k;
   int moveX[4] = { 1, 1, -1, -1 };
   int moveY[4] = { 1, -1, 1, -1 };

   for ( i = 0 ; i <= BOARD_RANGE_HIGH ; i++ )
   {
      for ( j = 0 ; j <= BOARD_RANGE_HIGH ; j++ )
      {
         if ( teamMove[i][j] != 0 )
         {
            //Check each direction for a kill
            for ( k = 0 ; k < 4 ; k++ )
            {
               //Can this piece move in this direction
               //Kings move in any direction
               if ( teamMove[i][j] == 2 || ( ( moveY[k] == 1 && teamDir == "down" ) || ( ( moveY[k] == -1 && teamDir == "up" ) ) ) )
               {
                  //Check if this position is on the board
                  if ( inBounds( i + moveX[k] * 2, j + moveY[k] * 2 ) )
                  {
                     //Enemy in this direction?
                     if ( teamOther[i + moveX[k]][j + moveY[k]] != 0 )
                     {
                        //End space not blocked off?
                        if ( teamMove[i + moveX[k] * 2][j + moveY[k] * 2] == 0 && teamOther[i + moveX[k] * 2][j + moveY[k] * 2] == 0 )
                        {
                           //Kill move can be made
                           return true;
                        }
                     }
                  } 
               }
            }
         }
      }
   }

   return false;
}

//Returns true if this team database has reached the game end
//Either the black or white team are all dead
//Param:
//teams - The database for the current teams
bool databaseGameEnd( struct teamDatabase* teams )
{
   unsigned int i, j;
   bool blackDead, whiteDead;

   blackDead = true;
   whiteDead = true;

   //Check each board position for remaining black/white pieces
   for ( i = 0 ; i < BOARD_RANGE_HIGH + 1 ; i++ )
   {
      for ( j = 0 ; j < BOARD_RANGE_HIGH + 1 ; j++ )
      {
         //Check for alive black pieces
         if ( teams -> blackTeam[i][j] != 0 )
         {
            blackDead = false;
         }

         //Check for alive white pieces
         if ( teams -> whiteTeam[i][j] != 0 )
         {
            whiteDead = false;
         }
      }
   }

   //Return true if the black or white team are dead
   if ( blackDead || whiteDead )
   {
      return true;
   }

   return false;
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
   if ( inBounds( x, y ) )
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
unsigned int getPiecesThatCanKill( std::vector<stone>& black, std::vector<stone>& white, sf::Vector2f *killSpots, enum turn currentTurn )
{
   int i, index;
   std::vector<stone>* team;

   i = 0;
   index = 0;

   if ( currentTurn == BLACK )
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

//Returns true if piece can move
//Param:
//piece - The piece to check the possible kill moves for
//black - The array of black pieces
//white - The array of white pieces
bool canMove( stone piece, std::vector<stone>& black, std::vector<stone>& white )
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

   //Check for possible moves
   if ( king || moveDir == "down" )
   {
         //Down left
         if ( inBounds( x - 1, y + 1 ) && isPositionFree( x - 1, y + 1, black, white ) )
         {
            return true;
         }

         //Down right
         if ( inBounds( x + 1, y + 1 ) && isPositionFree( x + 1, y + 1, black, white ) )
         {
            return true;
         }
   }

   if ( king || moveDir == "up" )
   {
         //Up left
         if ( inBounds( x - 1, y - 1 ) && isPositionFree( x - 1, y - 1, black, white ) )
         {
            return true;
         }

         //Up right
         if ( inBounds( x + 1, y - 1 ) && isPositionFree( x + 1, y - 1, black, white ) )
         {
            return true;
         }
   }

   return false;
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
         if ( inBounds( x - 1, y + 1 ) && !isPositionFree( x - 1, y + 1, black, white ) )
         {
            //Is this piece killable?
            if ( isKillablePiece( piece, black, white, x - 1, y + 1, -1, 1 ) )
            {
               return true;
            }
         }

         //Down right
         if ( inBounds( x + 1, y + 1 ) && !isPositionFree( x + 1, y + 1, black, white ) )
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
         if ( inBounds( x - 1, y - 1 ) && !isPositionFree( x - 1, y - 1, black, white ) )
         {
            //Is this piece killable?
            if ( isKillablePiece( piece, black, white, x - 1, y - 1, -1, -1 ) )
            {
               return true;
            }
         }

         //Up right
         if ( inBounds( x + 1, y - 1 ) && !isPositionFree( x + 1, y - 1, black, white ) )
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

//Returns true if the chosen team can move
//Param:
//teamName - "black" or "white"
//black - The array of black pieces
//white - The array of white pieces
bool canTeamMove( std::string teamName, std::vector<stone>& black, std::vector<stone>& white )
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
      if ( canMove( team -> at(i), black, white ) )
      {
         return true;
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
      if ( inBounds( x + xChange, y + yChange ) && isPositionFree( x + xChange, y + yChange, black, white ) )
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
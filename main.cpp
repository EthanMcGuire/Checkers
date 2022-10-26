#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include <string>

#include "checkers.hh"

//ming32-make to compile and link the program

int main()
{
   ///Declare variables

   //Window
   sf::RenderWindow window;

   //Music
   sf::Music music;

   //Graphical
   sf::Texture boardTexture;
   sf::Texture blackStoneTexture, blackKingTexture;
   sf::Texture whiteStoneTexture, whiteKingTexture;
   sf::Sprite killHighlight;  //Highlight on pieces to kill
   sf::Sprite pieceHighlight; //Highlight on a currently selected piece
   sf::Sprite moveHighlight; //Highlight for a pieces movement

   //Piece Information
   stone black[ROW_SIZE][COLUMN_SIZE];
   stone white[ROW_SIZE][COLUMN_SIZE];

   bool selected = false;  //True when a piece is pressed on
   stone* selectedPiece;   //Reference to the currently selected piece
   unsigned int mouseMoveIndex;  //The movePositions index the mouse is highlighted over

   point movePositions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS];   //The coordinates on the board where this piece can move
   unsigned int moveCountMatrix[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1];   //The number of moves at each point on the board
   unsigned int movePointCount[MAX_POSSIBLE_MOVES];   //The number of points within a move
   unsigned int moveCount;

   int i, j, k;

   //Create the window
   window.create( sf::VideoMode(1280, 720), "Checkers" );   //1280p window
   window.setFramerateLimit( 60 );

   //Play music

   if ( !music.openFromFile( SND_MUSIC ) )
      return EXIT_FAILURE;

   music.setLoop( true );
   music.play();

   //Create the game textures

   if ( !boardTexture.loadFromFile( SPR_BOARD ) )
      return EXIT_FAILURE;

   if ( !blackStoneTexture.loadFromFile( SPR_BLACK_PIECE ) )
      return EXIT_FAILURE;

   if ( !whiteStoneTexture.loadFromFile( SPR_WHITE_PIECE ) )
      return EXIT_FAILURE;

   if ( !blackKingTexture.loadFromFile( SPR_BLACK_KING ) )
   return EXIT_FAILURE;

   if ( !whiteKingTexture.loadFromFile( SPR_WHITE_KING ) )
      return EXIT_FAILURE;


   //Get the checkers board sprite
   sf::Sprite boardSprite( boardTexture ); //592 by 592

   boardSprite.setPosition( BOARD_ORIGIN_X, BOARD_ORIGIN_Y ); //Center the checkers board

   //Set up piece move highlight
   moveHighlight.setTexture( whiteStoneTexture );
   moveHighlight.setColor( sf::Color( 255, 255, 0, 64 ) );  //25% opacity yellow

   //Set up the piece highlight
   pieceHighlight.setTexture( whiteStoneTexture );
   pieceHighlight.setColor( sf::Color( 0, 255, 0, 128 ) );  //50% opacity yellow

   //Set up the kill highlight
   killHighlight.setTexture( whiteStoneTexture );
   killHighlight.setColor( sf::Color( 255, 0, 0, 128 ) );  //50% opacity red

   //Create the pieces
   bool offset = true; //Offset based on shifted rows (On the board)

   for ( int i = 0 ; i < COLUMN_SIZE ; i++ )  //Rows
   {
      for ( int j = 0 ; j < ROW_SIZE ; j++ )  //Columns
      {
         //Create the black pieces

         //Load black piece texture
         if ( !black[j][i].load( blackStoneTexture, &blackKingTexture, "black", "down" ) )
            return EXIT_FAILURE;

         black[j][i].setBoardPosition( offset + j * 2, i ); //Set the black positions

         //Create the white pieces
         
         //Load white piece texture
         if ( !white[j][i].load( whiteStoneTexture, &whiteKingTexture, "white", "up" ) )
            return EXIT_FAILURE;

         white[j][i].setBoardPosition( !offset + j * 2, WHITE_PLAYER_ROW_OFFSET + i ); //Set the white positions
      }

      offset = !offset; //Offset based on shifted rows
   }

   //Start the game loop
   while ( window.isOpen() )
   {
      sf::Event event;

      while ( window.pollEvent( event ) )
      {
         //Close window
         if ( event.type == sf::Event::Closed )
         {
            window.close();
         }

         //Mouse moved
         if ( event.type == sf::Event::MouseMoved )
         {
            //Check if the mouse is over a move
            if ( selected )
            {
               //Get the mouse position
               auto mousePos = sf::Mouse::getPosition( window );
               auto mousePosWorld = window.mapPixelToCoords( mousePos );

               //Check for mouse over move
               mouseMoveIndex = checkMouseOverMove( movePositions, movePointCount, moveCountMatrix, moveCount, mousePosWorld.x, mousePosWorld.y );
            }
         }

         //Mouse pressed
         if ( event.type == sf::Event::MouseButtonPressed )
         {
            if ( event.mouseButton.button == sf::Mouse::Left )
            {
               //Get the mouse position
               auto mousePos = sf::Mouse::getPosition( window );
               auto mousePosWorld = window.mapPixelToCoords( mousePos );

               bool actionComplete = false;

               //Check for a piece move action
               if ( selected )
               {
                  //Is a move hovered over?
                  if ( mouseMoveIndex != -1 )
                  {
                     //START THE ACTION
                     //executingAction = true;
                     //Check if this is a kill move
                     //If so, get the piece to kill from position[index][0]
                     //goalX = getX;
                     //goalY = getY;
                     //Progress towards the goalX and goalY until the move is complete
                     //Once the action is complete:
                        //If kill move, kill the piece then check for kill chains
                           //Set selected to true when kill chains are available
                              //Also fill the arrays
                           //If no kills chains, end the ACTION
                        //Otherwise, end the ACTION

                     //Move the currently selected piece
                     selectedPiece -> setBoardPosition( movePositions[mouseMoveIndex][movePointCount[mouseMoveIndex] - 1].x, movePositions[mouseMoveIndex][movePointCount[mouseMoveIndex] - 1].y );

                     //No longer do this
                     selected = false; //This WILL be false
                     selectedPiece = nullptr;   //BUT this WILL point to our piece


                     actionComplete = true;
                  }
               }

               //Check if pieces were pressed on
               if ( !actionComplete )
               {
                  for ( int i = 0 ; i < COLUMN_SIZE ; i++ )  //Rows
                  {
                     for ( int j = 0 ; j < ROW_SIZE ; j++ )  //Columns
                     {
                        //Select a black piece
                        if ( black[j][i].meetingPoint( mousePosWorld.x, mousePosWorld.y ) )
                        {
                           //Piece selected
                           selected = true;
                           selectedPiece = &black[j][i];
                           moveCount = getMovePositions( movePositions, movePointCount, *selectedPiece, black, white );  //Get the possible move count
                           getTotalMovesPerPoint( movePositions, movePointCount, moveCountMatrix, moveCount );   //Get the total moves on each board point

                           //Set the piece highlight on the current pieces location
                           pieceHighlight.setPosition( selectedPiece -> getPosition() );

                           mouseMoveIndex = -1; //Reset the mouseMoveIndex
                           actionComplete = true;
                        }

                        ///Select a white piece
                        if ( white[j][i].meetingPoint( mousePosWorld.x, mousePosWorld.y ) )
                        {
                           //Piece selected
                           selected = true;
                           selectedPiece = &white[j][i];
                           moveCount = getMovePositions( movePositions, movePointCount, *selectedPiece, black, white );  //Get the possible move count
                           getTotalMovesPerPoint( movePositions, movePointCount, moveCountMatrix, moveCount );   //Get the total moves on each board point

                           //Set the piece highlight on the current pieces location
                           pieceHighlight.setPosition( selectedPiece -> getPosition() );

                           mouseMoveIndex = -1; //Reset the mouseMoveIndex
                           actionComplete = true;
                        }
                     }
                  }
               }

               //Blank space was pressed
               if ( !actionComplete )
               {
                  selected = false;
                  selectedPiece = nullptr;
                  mouseMoveIndex = -1; //Reset the mouseMoveIndex
               }
            }
         }
      }

      //Step Actions
      /***
       * None yet
      ***/

      //Clear the window
      window.clear();

      //Draw Sprites
      window.draw( boardSprite );
      
      for ( int i = 0 ; i < COLUMN_SIZE ; i++ )  //Rows
      {
         for ( int j = 0 ; j < ROW_SIZE ; j++ )  //Columns
         {
            //Draw black pieces
            if ( black[j][i].isAlive() )
               window.draw( black[j][i] );

            //Draw white pieces
            if ( white[j][i].isAlive() )
               window.draw( white[j][i] );
         }
      }
   
      //Draw the possible move positions and piece highlights
      if ( selected )
      {
         for ( int i = 0 ; i <= BOARD_RANGE_HIGH ; i++ )
         {
            for ( int j = 0 ; j <= BOARD_RANGE_HIGH ; j++ )
            {
               //If there is a move point at this board point
               if ( moveCountMatrix[i][j] > 0 )
               {
                  //Get the position for this point
                  unsigned int xPos = i;
                  unsigned int yPos = j;
                  int xReal, yReal;
                  int opacity;
                  bool highlighted = false;

                  xReal = BOARD_ORIGIN_X + BOARD_SQUARE_OFFSET + ( BOARD_SQUARE_SIZE * xPos );
                  yReal = BOARD_ORIGIN_Y + BOARD_SQUARE_OFFSET + ( BOARD_SQUARE_SIZE * yPos );

                  //Check if this point is highlighted by the mouse
                  if ( mouseMoveIndex != -1 )
                  {
                     for ( k = 0 ; k < movePointCount[mouseMoveIndex] ; k++ )
                     {
                        if ( movePositions[mouseMoveIndex][k].x == xPos && movePositions[mouseMoveIndex][k].y == yPos )
                        {
                           //The highlighted move contains this point
                           highlighted = true;
                           break;
                        }
                     }
                  }

                  //Draw
                  if ( !isPositionFree( xPos, yPos, black, white ) )
                  {
                     //Kill highlight

                     //Set color opacity
                     opacity = 128;

                     if ( highlighted )
                     {
                        opacity = 192;
                     }

                     killHighlight.setColor( sf::Color( 255, 0, 0, opacity ) );   //Red

                     //Draw the kill highlight
                     killHighlight.setPosition( xReal + PIECE_OFFSET, yReal + PIECE_OFFSET );
                     window.draw( killHighlight );
                  }
                  else
                  {
                     //Piece movement highlight

                     //Set color opacity
                     opacity = 64;
                  
                     if ( highlighted )
                     {
                        opacity = 128;
                     }

                     moveHighlight.setColor( sf::Color( 255, 255, 0, opacity ) );  //Yellow

                     //Draw the piece movement highlight
                     moveHighlight.setPosition( xReal + PIECE_OFFSET, yReal + PIECE_OFFSET );
                     window.draw( moveHighlight );
                  }
               }
            }
         }

         //Draw the selected piece highlight
         window.draw( pieceHighlight );
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

//Returns true if this position on the board is free
//Otherwise returns false
//Param:
//x - Row on the board
//y - Column on the board
//black - The array of black pieces
//white - The array of white pieces
bool isPositionFree( unsigned int x, unsigned int y, stone black[ROW_SIZE][COLUMN_SIZE], stone white[ROW_SIZE][COLUMN_SIZE] )
{
    for ( int i = 0 ; i < COLUMN_SIZE ; i++ )  //Rows
    {
        for ( int j = 0 ; j < ROW_SIZE ; j++ )  //Columns
        {
            //Check black pieces
            if ( black[j][i].meetingPosition( x, y ) )
            return false;

            //Check white pieces
            if ( white[j][i].meetingPosition( x, y )  )
            return false;
        }
    }

    return true;
}

//Desc: Returns the stone located at this position on the board
//Param:
//x - Row on the board
//y - Column on the board
//black - The array of black pieces
//white - The array of white pieces
stone* getStoneFromPosition( unsigned int x, unsigned int y, stone black[ROW_SIZE][COLUMN_SIZE], stone white[ROW_SIZE][COLUMN_SIZE] )
{
    for ( int i = 0 ; i < COLUMN_SIZE ; i++ )  //Rows
    {
        for ( int j = 0 ; j < ROW_SIZE ; j++ )  //Columns
        {
            //Check black pieces
            if ( black[j][i].meetingPosition( x, y ) )
            return &black[j][i];

            //Check white pieces
            if ( white[j][i].meetingPosition( x, y )  )
            return &white[j][i];
        }
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
   point positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES], 
   stone piece, stone black[ROW_SIZE][COLUMN_SIZE], stone white[ROW_SIZE][COLUMN_SIZE] )
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
   point positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES],
   stone piece, stone black[ROW_SIZE][COLUMN_SIZE], stone white[ROW_SIZE][COLUMN_SIZE], 
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
                  
                  //return getKillMove( positions, movePointCount, piece, black, white, index, x + xChange, y + yChange, xChange, yChange, 0 );
            }

            //Otherwise no valid move
         }

         //0 moves added
         return 0;
   }
}

//Gets all positions for a kill move (This is started with a kill move)
//Will add whatever possible kills their are into the positions array
//Points in a kill move are saved in movePointCount
//This is a RECURSIVE function
//Calls again at every kill found, attempting to find new kills or paths
//Param:
//positions - The array of x and y positions for possible moves
//movePointCount - The number of points in a move
//piece - The piece to check the possible kill moves for
//black - The array of black pieces
//white - The array of white pieces
//index - The current index in the positions array
//x - Row on the board
//y - Column on the board 
//preKillDirX - The x direction for the previous kill (-1, or 1 )
//preKillDirY - The x direction for the previous kill (-1, or 1 )
unsigned int getKillMove( 
   point positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES],
   stone piece, stone black[ROW_SIZE][COLUMN_SIZE], stone white[ROW_SIZE][COLUMN_SIZE], 
   unsigned int index, 
   unsigned int x, unsigned int y, 
   unsigned int preKillDirX, unsigned int preKillDirY,
   unsigned int pointIndex )
{
   //Get my piece information
   bool king = piece.isKing();
   std::string moveDir = piece.getMoveDir();
   unsigned int boardX = piece.getBoardXPos();
   unsigned int boardY = piece.getBoardYPos();

   bool killFound = false;
   unsigned int moveCount, moveCountAdd;
   unsigned int currentIndex, currentPointIndex;
   point tempPositions[MAX_MOVE_POINTS];

   moveCount = 0;
   currentIndex = index;
   currentPointIndex = pointIndex;

   //Get the position of the enemy to kill
   positions[currentIndex][currentPointIndex].x = x - preKillDirX;
   positions[currentIndex][currentPointIndex].y = y - preKillDirY;

   currentPointIndex += 1;

   //Get the position after the kill
   positions[currentIndex][currentPointIndex].x = x;
   positions[currentIndex][currentPointIndex].y = y;

   //Get the number of points in this kill move
   movePointCount[currentIndex] = currentPointIndex + 1;

   //No longer searching for key chains here
   //Just return the current kill
   //Kill chains will be searched for after each kill

   /*
   //Make a kill path backup
   for ( int i = 0 ; i <= currentPointIndex ; i++ )
   {
      tempPositions[i] = positions[currentIndex][i];
   }

   //Check for possible kill chains
   if ( king || moveDir == "down" )
   {
         //Down left
         //Can't kill down left if we killed up right previously
         if ( !( preKillDirX == 1 && preKillDirY == -1 ) && !isPositionFree( x - 1, y + 1, black, white ) )
         {
            //Is this piece killable?
            if ( isKillablePiece( piece, black, white, x - 1, y + 1, -1, 1 ) )
            {
               killFound = true;

               moveCountAdd = getKillMove( positions, movePointCount, piece, black, white, currentIndex, x - 2, y + 2, -1, 1, currentPointIndex + 1 );

               moveCount += moveCountAdd;
               currentIndex += moveCountAdd;
               movePointCount[currentIndex] = currentPointIndex + 1;

               for ( int i = 0 ; i <= currentPointIndex ; i++ )
               {
                  positions[currentIndex][i] = tempPositions[i];
               }
            }
         }

         //Down right
         //Can't kill down right if we killed up left previously
         if ( !( preKillDirX == -1 && preKillDirY == -1 ) && !isPositionFree( x + 1, y + 1, black, white ) )
         {
            //Is this piece killable?
            if ( isKillablePiece( piece, black, white, x + 1, y + 1, 1, 1 ) )
            {
               killFound = true;

               moveCountAdd = getKillMove( positions, movePointCount, piece, black, white, currentIndex, x + 2, y + 2, 1, 1, currentPointIndex + 1 );

               moveCount += moveCountAdd;
               currentIndex += moveCountAdd;
               movePointCount[currentIndex] = currentPointIndex + 1;

               for ( int i = 0 ; i <= currentPointIndex ; i++ )
               {
                  positions[currentIndex][i] = tempPositions[i];
               }
            }
         }
   }

   if ( king || moveDir == "up" )
   {
         //Up left
         //Can't kill up left if we killed down right previously
         if ( !( preKillDirX == 1 && preKillDirY == 1 ) && !isPositionFree( x - 1, y - 1, black, white ) )
         {
            //Is this piece killable?
            if ( isKillablePiece( piece, black, white, x - 1, y - 1, -1, -1 ) )
            {
               killFound = true;

               moveCountAdd = getKillMove( positions, movePointCount, piece, black, white, currentIndex, x - 2, y - 2, -1, -1, currentPointIndex + 1 );

               moveCount += moveCountAdd;
               currentIndex += moveCountAdd;
               movePointCount[currentIndex] = currentPointIndex + 1;

               for ( int i = 0 ; i <= currentPointIndex ; i++ )
               {
                  positions[currentIndex][i] = tempPositions[i];
               }
            }
         }

         //Up right
         //Can't kill up right if we killed down left previously
         if ( !( preKillDirX == -1 && preKillDirY == 1 ) && !isPositionFree( x + 1, y - 1, black, white ) )
         {
            //Is this piece killable?
            if ( isKillablePiece( piece, black, white, x + 1, y - 1, 1, -1 ) )
            {
               killFound = true;

               moveCountAdd = getKillMove( positions, movePointCount, piece, black, white, currentIndex, x + 2, y - 2, 1, -1, currentPointIndex + 1 );

               moveCount += moveCountAdd;
               currentIndex += moveCountAdd;
               movePointCount[currentIndex] = currentPointIndex + 1;

               for ( int i = 0 ; i <= currentPointIndex ; i++ )
               {
                  positions[currentIndex][i] = tempPositions[i];
               }
            }
         }
   }
   */

   //If no kill was found, this is the end of this kill chain
   if ( !killFound )
   {
      //This move is done, return
      return 1;
   }
   else
   {
      return moveCount;
   }
}

//Gets the numbers of moves for each point on the board based on positions
//moveCountMatrix will contain the number of moves per point
//Param:
//positions - The array of x and y positions for possible moves
//movePointCount - The number of points in a move
//moveCountMatrix - The number of moves at each point on the board
//moveCount - The number of possible moves for the current piece
void getTotalMovesPerPoint( 
   point positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES], 
   unsigned int moveCountMatrix[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1],
   unsigned int moveCount )
{
   int i, j, x, y;

   //Reset moveCountMatrix
   for ( i = 0 ; i < BOARD_RANGE_HIGH + 1 ; i++ )
   {
      for ( j = 0 ; j < BOARD_RANGE_HIGH + 1 ; j++ )
      {
         moveCountMatrix[i][j] = 0;
      }
   }

   //Get the total moves on each board point
   for ( i = 0 ; i < moveCount ; i++ )
   {
      for ( j = 0 ; j < movePointCount[i] ; j++ )
      {
         //Get the board point
         x = positions[i][j].x;
         y = positions[i][j].y;

         //Increment the move count
         moveCountMatrix[x][y] += 1;
      }
   }

   return;
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
   stone piece, stone black[ROW_SIZE][COLUMN_SIZE], stone white[ROW_SIZE][COLUMN_SIZE], 
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
//moveCountMatrix - The number of moves at each point on the board
//moveCount - The number of possible moves for the current piece
//mouseX - The mouse x position
//mouseY - The mouse y position
unsigned int checkMouseOverMove( 
   point positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES], 
   unsigned int moveCountMatrix[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1],
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

         //Confirm only one move is at this point
         if ( moveCountMatrix[x][y] == 1 )
         {
            //Check the mouse coordinate
            xLeft = BOARD_ORIGIN_X + BOARD_SQUARE_OFFSET + ( x * BOARD_SQUARE_SIZE );
            yTop = BOARD_ORIGIN_Y + BOARD_SQUARE_OFFSET + ( y * BOARD_SQUARE_SIZE );

            if ( ( xLeft <= mouseX && xLeft + BOARD_SQUARE_SIZE >= mouseX ) && ( yTop <= mouseY && yTop + BOARD_SQUARE_SIZE >= mouseY ) )
            {
               return i;
            }
         }
      }
   }

   return -1;
}

//Sets the position of the yellow movement squares for a selected stone
//Param:
//positions - The possible row and column move positions for the current piece
//movePointCount - Number of points in a move
//movementSquares - The RectangleShape squares to set the positions of
//moveCount - The number of possible moves for the current piece
/*
void setMovementRectPositions(
   point positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS],
   unsigned int movePointCount[MAX_POSSIBLE_MOVES],
   sf::RectangleShape movementSquares[MAX_POSSIBLE_MOVES], 
   unsigned int moveCount )
{
   //Set the movement rectangles positions
   for ( int i = 0 ; i < moveCount ; i++ )
   {
      int xPos = BOARD_ORIGIN_X + BOARD_SQUARE_OFFSET + ( positions[i][movePointCount[i] - 1].x * BOARD_SQUARE_SIZE );
      int yPos = BOARD_ORIGIN_Y + BOARD_SQUARE_OFFSET + ( positions[i][movePointCount[i] - 1].y * BOARD_SQUARE_SIZE );

      movementSquares[i].setPosition( xPos, yPos );
   }

   return;
}
*/
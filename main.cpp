#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include <string>

#include "checkers.hh"

//ming32-make to compile and link the program

stone black[rowSize][columnSize];
stone white[rowSize][columnSize];

int main()
{
   sf::RenderWindow window;
   sf::Music music;
   sf::Texture boardTexture;
   sf::Texture blackStoneTexture;
   sf::Texture whiteStoneTexture;
   sf::RectangleShape movementSquares[maxPossibleMoves];

   bool selected = false;  //True when a piece is pressed on
   stone* selectedPiece;   //Reference to the currently selected piece
   unsigned int selectedPiecePositions[maxPossibleMoves][2];   //The coordinates on the board where this piece can move
   unsigned int moveCount;

   //Create the window
   window.create( sf::VideoMode(1280, 720), "Checkers" );

   //Play music

   if ( !music.openFromFile( "Audio\\DanseMacabre.ogg" ) )
      return EXIT_FAILURE;

   music.setLoop( true );
   music.play();

   //Create the game textures

   if ( !boardTexture.loadFromFile( "Sprites\\CheckersBoard.png" ) )
      return EXIT_FAILURE;

   if ( !blackStoneTexture.loadFromFile( "Sprites\\CheckersBlack.png" ) )
      return EXIT_FAILURE;

   if ( !whiteStoneTexture.loadFromFile( "Sprites\\CheckersWhite.png" ) )
      return EXIT_FAILURE;

   //Get the checkers board sprite
   sf::Sprite boardSprite( boardTexture ); //592 by 592

   boardSprite.setPosition( boardOriginX, boardOriginY ); //Center the checkers board

   //Set up the movement draw squares
   for ( int i = 0 ; i < maxPossibleMoves ; i++ )
   {
      movementSquares[i].setSize( sf::Vector2f( boardSquareSize, boardSquareSize ) );
      movementSquares[i].setFillColor( sf::Color( 255, 255, 0, 128 ) );
      movementSquares[i].setOutlineColor( sf::Color( 255, 255, 0, 128 ) );
   }

   //Create the pieces
   bool offset = true; //Offset based on shifted rows (On the board)

   for ( int i = 0 ; i < columnSize ; i++ )  //Rows
   {
      for ( int j = 0 ; j < rowSize ; j++ )  //Columns
      {
         //Create the black pieces

         //Load black piece texture
         if ( !black[j][i].load( blackStoneTexture, "black", "down", pieceSize, pieceSize ) )
            return EXIT_FAILURE;

         black[j][i].setBoardPosition( offset + j * 2, i ); //Set the black positions

         //Create the white pieces
         
         //Load white piece texture
         if ( !white[j][i].load( whiteStoneTexture, "white", "up", pieceSize, pieceSize ) )
            return EXIT_FAILURE;

         white[j][i].setBoardPosition( !offset + j * 2, whitePlayerRowOffset + i ); //Set the white positions
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

         //Mouse pressed
         if ( event.type == sf::Event::MouseButtonPressed )
         {
            if ( event.mouseButton.button == sf::Mouse::Left )
            {
               bool actionComplete = false;

               //Move the currently selected piece
               if ( selected )
               {
                  
               }

               //Check if pieces were pressed on
               if ( !actionComplete )
               {
                  //Get the mouse position
                  auto mousePos = sf::Mouse::getPosition( window );
                  auto mousePosWorld = window.mapPixelToCoords( mousePos );

                  for ( int i = 0 ; i < columnSize ; i++ )  //Rows
                  {
                     for ( int j = 0 ; j < rowSize ; j++ )  //Columns
                     {
                        //Select a black piece
                        if ( black[j][i].meetingCoordinates( mousePosWorld.x, mousePosWorld.y ) )
                        {
                           //Disable the previous pieces selection
                           if ( selected )
                           {
                              selected = false;

                              (*selectedPiece).endSelection();
                           }

                           //Piece selected
                           selected = true;
                           selectedPiece = &black[j][i];
                           black[j][i].setRed();   //Set the red selection color
                           moveCount = ( selectedPiece -> getMovePositions( selectedPiecePositions ) );

                           actionComplete = true;
                        }

                        ///Select a white piece
                        if ( white[j][i].meetingCoordinates( mousePosWorld.x, mousePosWorld.y ) )
                        {
                           //Disable the previous pieces selection
                           if ( selected )
                           {
                              selected = false;

                              (*selectedPiece).endSelection();
                           }

                           //Piece selected
                           selected = true;
                           selectedPiece = &white[j][i];
                           white[j][i].setRed();   //Set the red selection color
                           moveCount = ( selectedPiece -> getMovePositions( selectedPiecePositions ) );

                           actionComplete = true;
                        }
                     }
                  }
               }

               //Blank space was pressed
               if ( !actionComplete )
               {
                  //Disable the previous pieces selection
                  if ( selected )
                  {
                     selected = false;

                     (*selectedPiece).endSelection();
                  }
               }
            }
         }
      }

      //Clear the window
      window.clear();

      //Draw Sprites
      window.draw( boardSprite );
      
      for ( int i = 0 ; i < columnSize ; i++ )  //Rows
      {
         for ( int j = 0 ; j < rowSize ; j++ )  //Columns
         {
            //Draw black pieces
            if ( black[j][i].isAlive() )
               window.draw( black[j][i] );

            //Draw white pieces
            if ( white[j][i].isAlive() )
               window.draw( white[j][i] );
         }
      }
   
      //Draw the possible move positions
      if ( selected )
      {
         for ( int i = 0 ; i < moveCount ; i++ )
         {
            //Set the rectangles positions
            int xPos = boardOriginX + boardSquareOffset + selectedPiecePositions[i][0] * boardSquareSize;
            int yPos = boardOriginY + boardSquareOffset + selectedPiecePositions[i][1] * boardSquareSize;

            movementSquares[i].setPosition( xPos, yPos );

            //Draw the rectangle
            window.draw( movementSquares[i] );
         }

         //Add a CircleShape
         //Draw a red circle over the selected pieces location
         //Easy money
      }

      //Update the window display
      window.display();
   }

   return EXIT_SUCCESS;
}

/*****************************************************************************************************************************************************************************/

bool stone::load( sf::Texture& texture, std::string pieceColor, std::string pieceDir, unsigned int width, unsigned int height )
{
   //Get the texture
   m_texture = texture;

   //Set the piece color
   color = pieceColor;

   //Set the movement direction
   moveDir = pieceDir;

   // resize the vertex array to fit the level size
   m_vertices.setPrimitiveType( sf::Quads);
   m_vertices.resize( 4 );

   //Define its 4 corners
   m_vertices[0].position = sf::Vector2f(0.f, 0.f);
   m_vertices[1].position = sf::Vector2f(pieceSize, 0.f);
   m_vertices[2].position = sf::Vector2f(pieceSize, pieceSize);
   m_vertices[3].position = sf::Vector2f(0.f, pieceSize);

   //Define its 4 texture coordinates
   m_vertices[0].texCoords = sf::Vector2f(0.f, 0.f);
   m_vertices[1].texCoords = sf::Vector2f(pieceSize, 0.f);
   m_vertices[2].texCoords = sf::Vector2f(pieceSize, pieceSize);
   m_vertices[3].texCoords = sf::Vector2f(0.f, pieceSize);

   return true;
}

//Returns true if this piece is alive
bool stone::isAlive()
{
   return alive;
}

//Returns true if this piece is within these coordinates
//Returns false if not alive
//Used for mouse collisions
bool stone::meetingCoordinates( int x, int y )
{
   if ( !alive )
         return false;

   if ( ( getPosition().x <= x && getPosition().x + pieceSize >= x ) && ( getPosition().y <= y && getPosition().y + pieceSize >= y ) )
   {
         return true;
   }

   return false;
}

//Returns true if this piece is on this board position
bool stone::meetingPosition( unsigned int x, unsigned int y )
{
   if ( boardX == x && boardY == y )
   {
         return true;
   }

   return false;
}

//Sets this pieces position relative to the board
//0 - 7, 0 - 7
//Each square is 36 x 36 pixels large
void stone::setBoardPosition( unsigned int x, unsigned int y )
{
   boardX = x;
   boardY = y;

   setPosition( boardOriginX + boardSquareOffset + 4 + boardX * boardSquareSize, boardOriginY + boardSquareOffset + 4 + boardY * boardSquareSize );
}

//Shifts this pieces position relative to their current board position
void stone::shiftPosition( unsigned int x, unsigned int y )
{
   boardX += x;
   boardY += y;

   setPosition( boardOriginX + boardSquareOffset + 4 + boardX * boardSquareSize, boardOriginY + boardSquareOffset + 4 + boardY * boardSquareSize );
}

//Returns this pieces color as a string
//"black" or "white"
std::string stone::getColor()
{
   return color;
}

//Gets all possible x and y positions of movement
//Returns the number of possible moves
unsigned int stone::getMovePositions( unsigned int positions[4][2] )
{
   unsigned int index = 0;
   unsigned int count = 0;

   if ( isKing || moveDir == "down" )
   {
         //Down left
         if ( confirmPosition( positions, index, boardX - 1, boardY + 1, -1, 1 ) )
         {
            index++;
            count++;
         }

         //Down right
         if ( confirmPosition( positions, index, boardX + 1, boardY + 1, 1, 1 ) )
         {
            index++;
            count++;
         }
   }

   if ( isKing || moveDir == "up" )
   {
         //Down left
         if ( confirmPosition( positions, index, boardX - 1, boardY - 1, -1, -1 ) )
         {
            index++;
            count++;
         }

         //Down right
         if ( confirmPosition( positions, index, boardX + 1, boardY - 1, 1, -1 ) )
         {
            index++;
            count++;
         }
   }

   return count;
}

bool stone::confirmPosition( unsigned int positions[4][2], unsigned int index, unsigned int x, unsigned int y, unsigned int xChange, unsigned int yChange )
{
   if ( inRange( x, boardRangeLow, boardRangeHigh ) && inRange( y, boardRangeLow, boardRangeHigh ) )
   {
         if ( isPositionFree( x, y ) )
         {
            positions[index][0] = x;
            positions[index][1] = y;

            return true;
         }
         else
         {
            //Get the piece
            stone* pieceInWay = getStoneFromPosition( x, y );

            //This should not happen
            if ( pieceInWay == NULL )
               return -1;

            if ( pieceInWay -> getColor() != color )
            {
               //This is an enemy piece
               //Jump over?
               if ( inRange( x + xChange, boardRangeLow, boardRangeHigh ) && inRange( y + yChange, boardRangeLow, boardRangeHigh ) && isPositionFree( x + xChange, y + yChange ) )
               {
                     positions[index][0] = x + xChange;
                     positions[index][1] = y + yChange;

                     return true;
               }
            }
            //Otherwise no valid move
         }

         return false;
   }
}

//Make the piece vertices red
void stone::setRed()
{
   m_vertices[0].color = sf::Color::Red;
   m_vertices[1].color = sf::Color::Red;
   m_vertices[2].color = sf::Color::Red;
   m_vertices[3].color = sf::Color::Red;
}

//Get rid of the selected color
void stone::endSelection()
{
   m_vertices[0].color = sf::Color::White;
   m_vertices[1].color = sf::Color::White;
   m_vertices[2].color = sf::Color::White;
   m_vertices[3].color = sf::Color::White;
}

void stone::draw( sf::RenderTarget& target, sf::RenderStates states ) const
{
   //Apply the transform
   states.transform *= getTransform();

   //Apply the texture
   states.texture = &m_texture;

   //Draw the vertex array
   target.draw(m_vertices, states);
}

 //Checks if value is within the range low to high
//Returns true if so
bool inRange( unsigned int value, unsigned int low, unsigned int high )
{
    if ( value >= low && value <= high )
        return true;

    return false;
}

//Returns true if this position on the board is free
//Otherwise returns false
bool isPositionFree( unsigned int x, unsigned int y )
{
    for ( int i = 0 ; i < columnSize ; i++ )  //Rows
    {
        for ( int j = 0 ; j < rowSize ; j++ )  //Columns
        {
            //Draw black pieces
            if ( black[j][i].meetingPosition( x, y ) )
            return false;

            //Draw white pieces
            if ( white[j][i].meetingPosition( x, y )  )
            return false;
        }
    }

    return true;
}

//Returns the stone located at this position on the board
stone* getStoneFromPosition( unsigned int x, unsigned int y )
{
    for ( int i = 0 ; i < columnSize ; i++ )  //Rows
    {
        for ( int j = 0 ; j < rowSize ; j++ )  //Columns
        {
            //Draw black pieces
            if ( black[j][i].meetingPosition( x, y ) )
            return &black[j][i];

            //Draw white pieces
            if ( white[j][i].meetingPosition( x, y )  )
            return &white[j][i];
        }
    }

    return NULL;
}
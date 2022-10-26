#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include <string>

#include "checkers.hh"

//Initializer
//Param:
//texture - The texture for this piece
//pieceColor - "black" or "white"
//pieceDir - "down" or "up"
//width - width in pixels of this piece
//height - height in pixels of this piece
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
   m_vertices[1].position = sf::Vector2f(PIECE_SIZE, 0.f);
   m_vertices[2].position = sf::Vector2f(PIECE_SIZE, PIECE_SIZE);
   m_vertices[3].position = sf::Vector2f(0.f, PIECE_SIZE);

   //Define its 4 texture coordinates
   m_vertices[0].texCoords = sf::Vector2f(0.f, 0.f);
   m_vertices[1].texCoords = sf::Vector2f(PIECE_SIZE, 0.f);
   m_vertices[2].texCoords = sf::Vector2f(PIECE_SIZE, PIECE_SIZE);
   m_vertices[3].texCoords = sf::Vector2f(0.f, PIECE_SIZE);

   return true;
}

///Methods/Actions

//Sets this pieces position relative to the board
//0 - 7, 0 - 7
//Param:
//x - Row on the board
//y - Column on the board
void stone::setBoardPosition( unsigned int x, unsigned int y )
{
   boardX = x;
   boardY = y;

   setPosition( BOARD_ORIGIN_X + BOARD_SQUARE_OFFSET + PIECE_OFFSET + boardX * BOARD_SQUARE_SIZE, BOARD_ORIGIN_Y + BOARD_SQUARE_OFFSET + PIECE_OFFSET + boardY * BOARD_SQUARE_SIZE );
}

//Shifts this pieces position relative to their current board position
//Param:
//x - Row on the board
//y - Column on the board
void stone::shiftPosition( unsigned int x, unsigned int y )
{
   boardX += x;
   boardY += y;

   setPosition( BOARD_ORIGIN_X + BOARD_SQUARE_OFFSET + PIECE_OFFSET + boardX * BOARD_SQUARE_SIZE, BOARD_ORIGIN_Y + BOARD_SQUARE_OFFSET + PIECE_OFFSET + boardY * BOARD_SQUARE_SIZE );
}

///Checks

//Returns true if this piece is within these coordinates
//Returns false if not alive
//Used for mouse collisions
//Param:
//x - Mouse X coordinate
//y - Mouse Y coordinate
bool stone::meetingPoint( int x, int y )
{
   if ( !alive )
         return false;

   if ( ( getPosition().x <= x && getPosition().x + width >= x ) && ( getPosition().y <= y && getPosition().y + height >= y ) )
   {
         return true;
   }

   return false;
}

//Returns true if this piece is on this board position
//Param:
//x - Row on the board
//y - Column on the board
bool stone::meetingPosition( unsigned int x, unsigned int y )
{
   if ( !alive )
      return false;
      
   if ( boardX == x && boardY == y )
   {
         return true;
   }

   return false;
}

///Getters

//Returns true if this piece is alive
bool stone::isAlive()
{
   return alive;
}

//Returns true if this piece is a king
bool stone::isKing()
{
   return king;
}

//Returns this pieces row on the board
unsigned int stone::getBoardXPos()
{
   return boardX;
}

//Returns this pieces column on the board
unsigned int stone::getBoardYPos()
{
   return boardY;
}

//Returns this pieces color as a string
//"black" or "white"
std::string stone::getColor()
{
   return color;
}

//Returns this pieces move direction as a string
//"down" or "up"
std::string stone::getMoveDir()
{
   return moveDir;
}

//Draw override 
void stone::draw( sf::RenderTarget& target, sf::RenderStates states ) const
{
   //Apply the transform
   states.transform *= getTransform();

   //Apply the texture
   states.texture = &m_texture;

   //Draw the vertex array
   target.draw(m_vertices, states);
}
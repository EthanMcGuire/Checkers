#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>

#include "checkers.hh"

//Constructor
//Initializer
//Param:
//texture - The texture for this piece
//textureKing - The texture for this pieces king
//pieceColor - "black" or "white"
//pieceDir - "down" or "up"
bool stone::load( sf::Texture& texture, sf::Texture* textureKing, std::string pieceColor, std::string pieceDir )
{
   //Get the textures
   setTexture( texture );
   kingTexture = textureKing;

   //Set the piece color
   color = pieceColor;

   //Set the movement direction
   moveDir = pieceDir;

   //Resize the vertex array to fit a rectangular image
   m_vertices.setPrimitiveType( sf::Quads );
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

   tryToBecomeKing( boardY );

   return;
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

   tryToBecomeKing( boardY );
   
   return;
}

//Moves this piece towards point by speed
//Returns true once the point is reached
//Param:
//point - The x and y point to move to
//speed - The speed to move towards the point
//deltaTime - The time passed
bool stone::moveTowardsPoint( sf::Vector2f point, float speed, float deltaTime )
{
   //Move towards point
   sf::Vector2f moveAmount = interpolate( getPosition(), point, speed ) * ( deltaTime * 60 );

   setPosition( getPosition() + ( moveAmount ) );

   //Check if the point was reached
   sf::Vector2f distCheck = getPosition() - point;
   distCheck.x = std::abs( distCheck.x );
   distCheck.y = std::abs( distCheck.y );

   if ( distCheck.x < speed && distCheck.y < speed )
   {  
      setPosition( point );

      return true;
   }

   return false;
}

//Sets this piece to a king, if not already a king
//Param:
//y - y position on the board
void stone::tryToBecomeKing( unsigned int y )
{
   if ( !king )
   {
      if ( moveDir == "up" )
      {
         if ( y == BOARD_RANGE_LOW )
         {
            king = true;
         }
      }
      else
      {
         if ( y == BOARD_RANGE_HIGH )
         {
            king = true;
         }
      }
      
      //Set the texture to be a king
      if ( king )
      {
         setTexture( *kingTexture );
      }
   }
}

//Kills this piece
void stone::kill()
{
   alive = false;

   //Remove from the piece array?
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

   return;
}

///Setters

//Sets the texture of this piece
//Param:
//texture - The texture for this piece
void stone::setTexture( sf::Texture& texture )
{
   m_texture = texture;

   return;
}
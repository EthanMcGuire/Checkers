#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include <string>

const int boardOriginX = 640 - 296; //Board X and Y centered on the window
const int boardOriginY = 360 - 296;
const int boardSquareSize = 72;
const int boardSquareOffset = 8;
const int pieceSize = 64;
const int whitePlayerRowOffset = 5; //5 rows down from the last black row
const int pieceCount = 12;  //12 pieces per player
const int rowSize = 4;  //4 pieces started in a row
const int columnSize = 3;   //3 pieces started in a column (per player)
const int boardRangeLow = 0;
const int boardRangeHigh = 7;
const int maxPossibleMoves = 4;

#ifndef CHECKERS_H
#define CHECKERS_H

//Define the stone class (checkers pieces)
class stone : public sf::Drawable, public sf::Transformable
{
   public:
    
      bool load( sf::Texture& texture, std::string pieceColor, std::string pieceDir, unsigned int width, unsigned int height );

      //Returns true if this piece is alive
      bool isAlive();

      //Returns true if this piece is within these coordinates
      //Returns false if not alive
      //Used for mouse collisions
      bool meetingCoordinates( int x, int y );

      //Returns true if this piece is on this board position
      bool meetingPosition( unsigned int x, unsigned int y );

      //Sets this pieces position relative to the board
      //0 - 7, 0 - 7
      //Each square is 36 x 36 pixels large
      void setBoardPosition( unsigned int x, unsigned int y );

      //Shifts this pieces position relative to their current board position
      void shiftPosition( unsigned int x, unsigned int y );

      //Returns this pieces color as a string
      //"black" or "white"
      std::string getColor();

      //Gets all possible x and y positions of movement
      //Returns the number of possible moves
      unsigned int getMovePositions( unsigned int positions[4][2] );

      //Confirms a position to move to is valid
      bool confirmPosition( unsigned int positions[4][2], unsigned int index, unsigned int x, unsigned int y, unsigned int xChange, unsigned int yChange );

      //Make the piece vertices red
      void setRed();

      //Get rid of the selected color
      void endSelection();

   private:
      bool isKing = false;
      bool alive = true;
      int boardX = 0;
      int boardY = 0;

      std::string color;    //"black" or "white"
      std::string moveDir;  //"down" or "up"

      sf::VertexArray m_vertices;
      sf::Texture m_texture;

   virtual void draw( sf::RenderTarget& target, sf::RenderStates states ) const;
};

bool inRange( unsigned int value, unsigned int low, unsigned int high );
bool isPositionFree( unsigned int x, unsigned int y );
stone* getStoneFromPosition( unsigned int x, unsigned int y );

#endif

extern stone black[rowSize][columnSize];
extern stone white[rowSize][columnSize];
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include <string>

const int BOARD_ORIGIN_X = 640 - 296; //Board X and Y centered on the window
const int BOARD_ORIGIN_Y = 360 - 296;
const int BOARD_SQUARE_SIZE = 72;
const int BOARD_SQUARE_OFFSET = 8;
const int PIECE_OFFSET = 4;   //Offset for a stone/piece from the top left corner of a square
const int PIECE_SIZE = 64;
const int WHITE_PLAYER_ROW_OFFSET = 5; //5 rows down from the last black row
const int PIECE_COUNT_PER_PLAYER = 12;  //12 pieces per player
const int ROW_SIZE = 4;  //4 pieces started in a row
const int COLUMN_SIZE = 3;   //3 pieces started in a column (per player)
const int BOARD_RANGE_LOW = 0;   //The low and high range of squares on the board
const int BOARD_RANGE_HIGH = 7;
const int MAX_POSSIBLE_MOVES = 20;   //Max 20 possible moves for a single checkers piece
const int MAX_MOVE_POINTS = 24;  //24 possible points in 1 moves (if you SOMEHOW kill all 12 pieces)

//Files
const std::string SND_MUSIC = "Audio\\DanseMacabre.ogg";
const std::string SPR_BOARD = "Sprites\\CheckersBoard.png";
const std::string SPR_BLACK_PIECE = "Sprites\\CheckersBlack.png";
const std::string SPR_WHITE_PIECE = "Sprites\\CheckersWhite.png";

#ifndef CHECKERS_H
#define CHECKERS_H

//Define a x, y point
struct point
{
   int x, y;
};

//Define the stone class (checkers pieces)
class stone : public sf::Drawable, public sf::Transformable
{
   public:
    
      bool load( sf::Texture& texture, std::string pieceColor, std::string pieceDir, unsigned int width, unsigned int height );

      ///Methods/Actions

      //Sets this pieces position relative to the board
      //0 - 7, 0 - 7
      //Each square is 36 x 36 pixels large
      void setBoardPosition( unsigned int x, unsigned int y );

      //Shifts this pieces position relative to their current board position
      void shiftPosition( unsigned int x, unsigned int y );

      ///Checks

      //Returns true if this piece is within these coordinates
      //Returns false if not alive
      //Used for mouse collisions
      bool meetingPoint( int x, int y );

      //Returns true if this piece is on this board position
      bool meetingPosition( unsigned int x, unsigned int y );

      ///Getters

      //Returns true if this piece is alive
      bool isAlive();

      //Returns true if this piece is a king
      bool isKing();

      //Returns this pieces row on the board
      unsigned int getBoardXPos();

      //Returns this pieces column on the board
      unsigned int getBoardYPos();

      //Returns this pieces color as a string
      //"black" or "white"
      std::string getColor();

      //Returns this pieces move direction as a string
      //"down" or "up"
      std::string getMoveDir();

   private:
   bool alive = true;
      bool king = false;
      unsigned int boardX = 0;
      unsigned int boardY = 0;
      unsigned int width = PIECE_SIZE;
      unsigned int height = PIECE_SIZE;

      std::string color;    //"black" or "white"
      std::string moveDir;  //"down" or "up"

      sf::VertexArray m_vertices;
      sf::Texture m_texture;

   virtual void draw( sf::RenderTarget& target, sf::RenderStates states ) const;
};

bool inRange( unsigned int value, unsigned int low, unsigned int high );
bool isPositionFree( unsigned int x, unsigned int y, stone black[ROW_SIZE][COLUMN_SIZE], stone white[ROW_SIZE][COLUMN_SIZE] );
stone* getStoneFromPosition( unsigned int x, unsigned int y, stone black[ROW_SIZE][COLUMN_SIZE], stone white[ROW_SIZE][COLUMN_SIZE] );

unsigned int getMovePositions( 
   point positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES], 
   stone piece, stone black[ROW_SIZE][COLUMN_SIZE], stone white[ROW_SIZE][COLUMN_SIZE] );

unsigned int confirmPosition( 
   point positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS],
   unsigned int movePointCount[MAX_POSSIBLE_MOVES], 
   stone piece, stone black[ROW_SIZE][COLUMN_SIZE], stone white[ROW_SIZE][COLUMN_SIZE], 
   unsigned int index, 
   unsigned int x, unsigned int y, 
   unsigned int xChange, unsigned int yChange );

unsigned int getKillMove( 
   point positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES],
   stone piece, stone black[ROW_SIZE][COLUMN_SIZE], stone white[ROW_SIZE][COLUMN_SIZE], 
   unsigned int index, 
   unsigned int x, unsigned int y, 
   unsigned int preKillDirX, unsigned int preKillDirY,
   unsigned int pointIndex );

void getTotalMovesPerPoint( 
   point positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES], 
   unsigned int moveCountMatrix[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1],
   unsigned int moveCount );

bool isKillablePiece( 
   stone piece, stone black[ROW_SIZE][COLUMN_SIZE], stone white[ROW_SIZE][COLUMN_SIZE], 
   unsigned int x, unsigned int y, 
   unsigned int xChange, unsigned int yChange );

unsigned int checkMouseOverMove( 
   point positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES], 
   unsigned int moveCountMatrix[BOARD_RANGE_HIGH + 1][BOARD_RANGE_HIGH + 1],
   unsigned int moveCount,
   int mouseX, int mouseY );

/*
void setMovementRectPositions( 
   point positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS],
   unsigned int movePointCount[MAX_POSSIBLE_MOVES],
   sf::RectangleShape movementSquares[MAX_POSSIBLE_MOVES], 
   unsigned int moveCount );
   */

#endif
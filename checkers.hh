#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include <vector>
#include <string>

//Constants

const int FRAME_RATE = 60; //Frames per second

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
const int MAX_POSSIBLE_MOVES = 4;   //Max 4 possible moves for a single checkers piece
const int MAX_MOVE_POINTS = 2;  //2 possible points in 1 moves (killing a piece is 2 points)
const int PIECE_MOVE_SPEED = 5;  //The speed pieces move

//Files
const std::string SND_MUSIC = "Audio\\DanseMacabre.ogg";
const std::string SPR_BACKGROUND = "Sprites\\Background.jpg";
const std::string SPR_BOARD = "Sprites\\CheckersBoard.png";
const std::string SPR_BLACK_PIECE = "Sprites\\CheckersBlack.png";
const std::string SPR_WHITE_PIECE = "Sprites\\CheckersWhite.png";
const std::string SPR_BLACK_KING = "Sprites\\CheckersBlackKing.png";
const std::string SPR_WHITE_KING = "Sprites\\CheckersWhiteKing.png";

//Globals
extern sf::Vector2i globalMousePos;
extern sf::Vector2f globalMousePosWorld;

#ifndef CHECKERS_H
#define CHECKERS_H

//States
enum states
{
   getAction,  //Get an action
   getChainKill,  //Get the next piece to kill in a kill chain
   pieceSelected, //A stone/piece is selected currently
   action   //Executing an action
};

//Define the stone class (checkers pieces)
class stone : public sf::Drawable, public sf::Transformable
{
   public:

      //Load values (like a constructor)    
      bool load( sf::Texture& texture, sf::Texture* textureKing, std::string pieceColor, std::string pieceDir );

      ///Methods/Actions

      //Sets this pieces position relative to the board
      //0 - 7, 0 - 7
      //Each square is 36 x 36 pixels large
      void setBoardPosition( unsigned int x, unsigned int y );

      //Shifts this pieces position relative to their current board position
      void shiftPosition( unsigned int x, unsigned int y );

      //Moves this piece towards point by speed
      //Returns true once the point is reached
      bool moveTowardsPoint( sf::Vector2f point, float speed, float deltaTime );

      //Sets this piece to a king
      //Param:
      //y - y position on the board
      void tryToBecomeKing( unsigned int y );

      //Kills this piece
      void kill();

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

      ///Setters

      //Set the texture for this piece
      void setTexture( sf::Texture& texture );

   private:
      unsigned int boardX = 0;
      unsigned int boardY = 0;
      unsigned int width = PIECE_SIZE;
      unsigned int height = PIECE_SIZE;

      bool alive = true;
      bool king = false;

      std::string color;    //"black" or "white"
      std::string moveDir;  //"down" or "up"

      sf::VertexArray m_vertices;
      sf::Texture m_texture;
      sf::Texture* kingTexture;

   virtual void draw( sf::RenderTarget& target, sf::RenderStates states ) const;
};

bool inRange( unsigned int value, unsigned int low, unsigned int high );
sf::Vector2f interpolate( sf::Vector2f pointA, sf::Vector2f pointB, float speed );

void getNextTurn( std::string& currentTurn, bool& canKill, 
                  sf::Vector2i positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
                  unsigned int movePointCount[MAX_POSSIBLE_MOVES], 
                  std::vector<stone>& black, std::vector<stone>& white );

bool isPositionFree( unsigned int x, unsigned int y, std::vector<stone>& black, std::vector<stone>& white );
stone* getStoneFromPosition( unsigned int x, unsigned int y, std::vector<stone>& black, std::vector<stone>& white );

unsigned int getMovePositions( 
   sf::Vector2i positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES], 
   stone piece, std::vector<stone>& black, std::vector<stone>& white );

unsigned int confirmPosition( 
   sf::Vector2i positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS],
   unsigned int movePointCount[MAX_POSSIBLE_MOVES], 
   stone piece, std::vector<stone>& black, std::vector<stone>& white, 
   unsigned int index, 
   unsigned int x, unsigned int y, 
   unsigned int xChange, unsigned int yChange );

unsigned int getKillMoves( 
   sf::Vector2i positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES],
   stone piece, std::vector<stone>& black, std::vector<stone>& white, 
   unsigned int x, unsigned int y );

bool hasKillMoves( 
   sf::Vector2i positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES],
   stone piece, std::vector<stone>& black, std::vector<stone>& white );

bool isKillablePiece( 
   stone piece, std::vector<stone>& black, std::vector<stone>& white, 
   unsigned int x, unsigned int y, 
   unsigned int xChange, unsigned int yChange );

unsigned int checkMouseOverMove( 
   sf::Vector2i positions[MAX_POSSIBLE_MOVES][MAX_MOVE_POINTS], 
   unsigned int movePointCount[MAX_POSSIBLE_MOVES], 
   unsigned int moveCount,
   int mouseX, int mouseY );

void removeDeadPieces( std::vector<stone>& black, std::vector<stone>& white );

#endif
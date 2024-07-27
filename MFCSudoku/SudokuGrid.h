#pragma once
#include <sstream>
#include <vector>
#include "ExactCover.h"

class SudokuGrid
{
    #pragma region Dancing Links

  #define MAX_K 1000

  // Linked list structure for the Dancing Links algorithm
  struct Node
  {
    Node* left;
    Node* right;
    Node* up;
    Node* down;
    Node* head;

    int size;     //used for Column header
    int rowID[3]; //used to identify row in order to map solutions to a sudoku grid
    //ID Format: (Candidate, Row, Column)
  };

  static const int GSIZE = 9;
  static const int SIZE_SQUARED = 81;
  static const int SIZE_SQRT = 3;
  static const int ROW_NB = GSIZE * GSIZE * GSIZE;
  static const int COL_NB = 4 * GSIZE * GSIZE;

  Node Head;
  Node* HeadNode = &Head;
  std::vector<Node*> solution;
  Node* orig_values[MAX_K];
  bool isSolved = false;

  #pragma endregion
  struct BlockValues
  {
    // Assuming a 32 bit unsigned int
    unsigned int _pencilMark : 9;       // 9 bits for pencil marks 1-9
    unsigned int _snyderNotation : 9;   // 9 bits for snyder notation 1-9
    unsigned int _value : 4;            // 4 bits for the value 1-9
    unsigned int _solution : 4;         // 4 bits for the solution 1-9
    unsigned int _reserved : 6;         // 6 bits reserved for future use

    //CRect _rect;
  };

  BlockValues cells[9][9] = {0};

  ExactCover ec;
public:
  #pragma region Constructors
  SudokuGrid()
  {
    // Initialize the grid
    for ( int row = 0; row < 9; ++row )
    {
      for ( int column = 0; column < 9; ++column )
      {
        cells[row][column]._value = 0;
        cells[row][column]._pencilMark = 0x1FF;  // 9 bits set to 1
        cells[row][column]._snyderNotation = 0;
        cells[row][column]._solution = 0;
      }
    }
  }

  // Destructor
  ~SudokuGrid()
  {}

  // Copy constructor
  SudokuGrid( const SudokuGrid& other )
  {
    for ( int row = 0; row < 9; ++row )
    {
      for ( int column = 0; column < 9; ++column )
      {
        cells[row][column] = other.cells[row][column];
      }
    }
  }

  // Assignment operator
  SudokuGrid& operator=( const SudokuGrid& other )
  {
    if ( this != &other )
    {
      for ( int row = 0; row < 9; ++row )
      {
        for ( int column = 0; column < 9; ++column )
        {
          cells[row][column] = other.cells[row][column];
        }
      }
    }

    return *this;
  }

  // Move constructor
  SudokuGrid( SudokuGrid&& other ) noexcept
  {
    for ( int row = 0; row < 9; ++row )
    {
      for ( int column = 0; column < 9; ++column )
      {
        cells[row][column] = other.cells[row][column];
      }
    }
  }

  // Move assignment operator
  SudokuGrid& operator=( SudokuGrid&& other ) noexcept
  {
    if ( this != &other )
    {
      for ( int row = 0; row < 9; ++row )
      {
        for ( int column = 0; column < 9; ++column )
        {
          cells[row][column] = other.cells[row][column];
        }
      }
    }

    return *this;
  }

  #pragma endregion

    // Clear the grid
  void clear()
  {
    for ( int row = 0; row < 9; ++row )
    {
      for ( int column = 0; column < 9; ++column )
      {
        cells[row][column]._value = 0;
        cells[row][column]._pencilMark = 0;
        cells[row][column]._snyderNotation = 0;
      }
    }
  }

  void GenerateRandomGrid();

  bool Solve();

  const std::ostringstream AsString();

  #pragma region Getters and Setters
    // Get the value of a cell
  int getValue( int row, int column )
  {
    return static_cast<int>(cells[row][column]._value);
  }

  // Get the pencil mark value of a cell
  int getPencilMarkValue( int row, int column, int pencilMark )
  {
    return static_cast<int>(cells[row][column]._pencilMark >> pencilMark);
  }

//   // Get the rectangle for a cell
//   CRect& getRect(int row, int column)
//   {
//     return cells[row][column]._rect;
//   }

  // Set the value of a cell
  void setValue( int row, int column, int value )
  {
    cells[row][column]._value = (unsigned int)value;
    cells[row][column]._pencilMark &= ( ~( 1 << value ) );
  }

  // Set the pencil mark value of a cell
  void setPencilMarkValue( int row, int column, int value )
  {
    cells[row][column]._pencilMark = 1u << value;
  }
  #pragma endregion

private:
  #pragma region DancingLinks Functions
//===============================================================================================================//
//---------------------------------------------DLX Functions-----------------------------------------------------//
//===============================================================================================================//

//   void coverColumn( Node* col );
//
//   void uncoverColumn( Node* col );
//
//   void Search( int k );

//===============================================================================================================//
//----------------------Functions to turn a Sudoku grid into an Exact Cover problem -----------------------------//
//===============================================================================================================//

//--------------------------BUILD THE INITIAL MATRIX CONTAINING ALL POSSIBILITIES--------------------------------//
//   void BuildSparseMatrix();
//
//   //-------------------BUILD A TOROIDAL DOUBLY LINKED LIST OUT OF THE SPARSE MATRIX-------------------------//
//   void BuildLinkedList();
//
//   //-------------------COVERS VALUES THAT ARE ALREADY PRESENT IN THE GRID-------------------------//
//   void TransformListToCurrentGrid();
//
//   void MapSolutionToGrid();

  #pragma endregion
};


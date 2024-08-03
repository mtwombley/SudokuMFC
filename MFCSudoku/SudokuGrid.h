#pragma once
#include <sstream>
#include <vector>
#include <iterator>
#include "ExactCover.h"

struct SudokuGrid
{
  struct BlockValue
  {
    // Assuming a 32 bit unsigned int
    unsigned int _pencilMark : 9;       // 9 bits for pencil marks 1-9
    unsigned int _snyderNotation : 9;   // 9 bits for snyder notation 1-9
    unsigned int _value : 4;            // 4 bits for the value 1-9
    unsigned int _solution : 4;         // 4 bits for the solution 1-9
    unsigned int _reserved : 6;         // 6 bits reserved for future use

    //CRect _rect;
  };

  struct CardShuffle
  {
    CardShuffle( SudokuGrid::BlockValue* head )
    {
      deck.reserve( 81 );
      for ( int i = 0; i < 9; ++i )
      {
        for ( int j = 0; j < 9; ++j )
        {
          // Temporarily store the row, column, and value in the deck
          head->_value = i;
          head->_reserved = j;
          deck.push_back( head++ );
        }
      }
    }
     // create a vector of 81 pointers pointing to a BlockValues object
    std::vector<SudokuGrid::BlockValue*> deck;

    void Shuffle();

    void DebugPrint()
    {
      for ( auto& card : deck )
      {
        PLOGD << "r" << card->_value << "c" << card->_reserved;
      }
    }
  };

  BlockValue cells[9][9] = {0};

  ExactCover ec;

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
        cells[row][column]._pencilMark = 0x1FF;
        cells[row][column]._snyderNotation = 0;
      }
    }
  }

  void GenerateRandomGrid();

  void Solve();

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

  // Set the value of a cell
  void setValue( int row, int column, int value )
  {
    cells[row][column]._value = (unsigned int)value;
    cells[row][column]._pencilMark &= ( ~( 1 << (value-1) ) );
  }

  // Set the value of a cell
  void setSolution( int row, int column, int value )
  {
    cells[row][column]._solution = (unsigned int)value;
    cells[row][column]._pencilMark &= ( ~( 1 << (value-1) ) );
  }

  // Set the pencil mark value of a cell
  void setPencilMarkValue( int row, int column, int value )
  {
    cells[row][column]._pencilMark = 1u << value;
  }

  bool CheckValue( unsigned int row, unsigned int column, unsigned int value )
  {
    if ( cells[row][column]._value != 0 )
      return false;

    // Check the row and column against the value
    for ( int i = 0; i < 9; ++i )
    {
      if ( cells[row][i]._value == value ||
           cells[i][column]._value == value )
        return false;
    }

    // Check the block against the value
    int blockRow = row / 3;
    int blockColumn = column / 3;
    for ( int i = blockRow * 3; i < blockRow * 3 + 3; ++i )
    {
      for ( int j = blockColumn * 3; j < blockColumn * 3 + 3; ++j )
      {
        if ( cells[i][j]._value == value )
          return false;
      }
    }

    return true;
  }

  bool CheckSolution( unsigned int row, unsigned int column, unsigned int value )
  {
    if ( cells[row][column]._solution != 0 )
    {
      PLOGD << "Solution already set r" << row << "c" << column << "#" << value;
      return false;
    }

    // Check the row and column against the value
    for ( int i = 0; i < 9; ++i )
    {
      if ( cells[row][i]._solution == value ||
           cells[i][column]._solution == value )
      {
        PLOGD << "Row or Column already set r" << row << "c" << column << "#" << value;
         return false;
      }
    }

    // Check the block against the value
    int blockRow = row / 3;
    int blockColumn = column / 3;
    for ( int i = blockRow * 3; i < blockRow * 3 + 3; ++i )
    {
      for ( int j = blockColumn * 3; j < blockColumn * 3 + 3; ++j )
      {
        if ( cells[i][j]._solution == value )
        {
          PLOGD << "Block already set r" << row << "c" << column << "#" << value;
          return false;
        }
      }
    }

    return true;
  }
  #pragma endregion

};


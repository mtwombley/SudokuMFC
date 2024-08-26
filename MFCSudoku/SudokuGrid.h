#pragma once
#include <sstream>
#include <vector>
#include <iterator>
#include <__msvc_bit_utils.hpp>
#include "ExactCover.h"

struct SudokuGrid
{
  struct BlockValue
  {
    using uint32_t = unsigned __int32;

    // Assuming a 32 bit unsigned int
    uint32_t _pencilMark : 9;       // 9 bits for pencil marks 1-9
    uint32_t _snyderNotation : 9;   // 9 bits for snyder notation 1-9
    uint32_t _value : 4;            // 4 bits for the value 1-9
    uint32_t _solution : 4;         // 4 bits for the solution 1-9
    uint32_t _reserved : 6;         // 6 bits reserved for future use

    uint32_t CountPencilMarks() const
    {
      return std::_Popcount( _pencilMark );
    }

    uint32_t CountSnyderNotation() const
    {
      return std::_Popcount( _snyderNotation );
    }

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

#pragma region Debug Features
    void DebugPrint()
    {
      for ( auto& card : deck )
      {
        PLOGD << "r" << card->_value << "c" << card->_reserved;
      }
    }
#pragma endregion Debug Features
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
  void clear();

  void GenerateRandomGrid();

  void Solve();

  const std::ostringstream AsString();

  #pragma region Getters and Setters
  // Get the value of a cell
  int getValue( int row, int column ) const
  {
    return static_cast<int>(cells[row][column]._value);
  }

  // Get the solution value of a cell
  int getSolution( int row, int column ) const
  {
    return static_cast<int>(cells[row][column]._solution);
  }

  // Get the pencil mark value of a cell
  int getPencilMarkValue( int row/*0-8*/, int column/*0-8*/, int pencilMark/*0-8*/ ) const
  {
    return static_cast<int>(cells[row][column]._pencilMark & 1 << (pencilMark-1));
  }

  // Set the value of a cell
  void setValue( int row, int column, int value )
  {
    cells[row][column]._value = (unsigned int)value;
    cells[row][column]._pencilMark &= ( ~( 1 << (value-1) ) );
  }

  // Set the solution value of a cell
  // Doing so will clear the pencil marks in the row, column, and block
  void setSolution( int row, int column, int value )
  {
    cells[row][column]._solution = (unsigned int)value;
    cells[row][column]._pencilMark &= ( ~( 1 << (value-1) ) );

    // remove the value from the pencilMarks in the home cells
    int blockRow = row / 3;
    int blockColumn = column / 3;
    for ( int i = blockRow * 3; i < blockRow * 3 + 3; ++i )
    {
      for ( int j = blockColumn * 3; j < blockColumn * 3 + 3; ++j )
      {
        cells[i][j]._pencilMark &= ( ~( 1 << (value-1) ) );
      }
    }
    // remove the value from the pencilMarks in the row cells
    // remove the value from the pencilMarks in the column cells
    for ( int i = 0; i < 9; ++i )
      for ( int j = 0; j < 9; ++j )
      {
        if ( i == row || j == column )
          cells[i][j]._pencilMark &= ( ~( 1 << (value-1) ) );
      }
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

    return getPencilMarkValue( row, column, value - 1 );

//     // Check the row and column against the value
//     for ( int i = 0; i < 9; ++i )
//     {
//       if ( cells[row][i]._solution == value ||
//            cells[i][column]._solution == value )
//       {
//         PLOGD << "Row or Column already set r" << row << "c" << column << "#" << value;
//          return false;
//       }
//     }
//
//     // Check the block against the value
//     int blockRow = row / 3;
//     int blockColumn = column / 3;
//     for ( int i = blockRow * 3; i < blockRow * 3 + 3; ++i )
//     {
//       for ( int j = blockColumn * 3; j < blockColumn * 3 + 3; ++j )
//       {
//         if ( cells[i][j]._solution == value )
//         {
//           PLOGD << "Block already set r" << row << "c" << column << "#" << value;
//           return false;
//         }
//       }
//     }

//     return true;
  }
  #pragma endregion

};


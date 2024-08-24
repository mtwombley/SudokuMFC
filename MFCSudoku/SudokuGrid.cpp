#include "pch.h"
#include "SudokuGrid.h"
#include "DLX.h"
#include <random>
#include <algorithm>
#include "RandomGen.h"

extern CString GeneratePuzzle();

void SudokuGrid::clear()
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

void SudokuGrid::GenerateRandomGrid()
{
  ec.Initialize();
  clear();

  CString puzzle = GeneratePuzzle();

  // convert CString puzzle to ours
  for ( int i = 0; i < 81; ++i )
  {
    if ( puzzle[i] != '.' && puzzle[i] != '0' )
    {
      setSolution( i / 9, i % 9, puzzle[i] - '0' );
    }
  }

// First attempt at generating a random grid
// #if 1
//   CardShuffle puzzleDeck( &cells[0][0] );
//   puzzleDeck.Shuffle();
//   puzzleDeck.DebugPrint();
//
//   RandomGenerator randGen;
//
//   // Create a list of possible values for each cell
//   std::vector<int> suggestions{1,2,3,4,5,6,7,8,9};
//
//   int solutionTest = 0;
//
//   // Sources have indicated that 17 is the minimum number of clues required for a unique solution
//   for ( int x = 0; x < 81; ++x )
//   {
//     auto cell = puzzleDeck.deck.back();
//     puzzleDeck.deck.pop_back();
//     int i = cell->_value;
//     int j = cell->_reserved;
//
//     //Clear the cell temp values
//     cell->_value = 0;
//     cell->_reserved = 0;
//
//     if ( x < 17 )
//     {
//       // Shuffle the suggestions, randomly selecting a value for the cell can cause long loops because of repeated values
//       randGen.Shuffle( suggestions );
//       bool found = false;
//       for ( auto& suggestion : suggestions )
//       {
//         if ( CheckSolution( i, j, suggestion ) )
//         {
//           setSolution( i, j, suggestion );
//           found = true;
//           break;
//         }
//         else
//         {
//           ++solutionTest;
//         }
//       }
//       if ( !found )
//       {
//         PLOGD << "No solution found";
//         break;
//       }
//     }
//   }
//   PLOGD << VARTRACE(solutionTest) << AsString().str().c_str();
// #else
//   //char puzzle[] = "...82.....1...5..2.....63..9.8...4.7..4....3.....6..5...967.....7..315......9.8..";
//   char puzzle[] = "...4..51.....5....3....2.8...69.7...5....3..7......4..895.....32..014...7.......2";
//   for ( int i = 0; i < 81; ++i )
//   {
//     if ( puzzle[i] != '.' )
//     {
//       setSolution( i / 9, i % 9, puzzle[i] - '0' );
//     }
//   }
// #endif
//   ec.CheckHeaders();
//
//   // We have a minimum of 17 clues, now we need to check for a unique solution
//   Solve();
//   // If we have a unique solution, we are done


}

void SudokuGrid::Solve()
{
    // Fill the exact cover matrix with the current grid
  for ( int row = 0; row < 9; ++row )
  {
    for ( int column = 0; column < 9; ++column )
    {
      if ( cells[row][column]._solution != 0 )
      {
        ec.SetMark( column + 1, row + 1, cells[row][column]._solution );
      }
    }
  }
  size_t originalSize = ec.workingSolution.size();
  ec.CheckHeaders();
  do
  {
    auto solutionSize = ec.workingSolution.size();
    ec.Search( 0 );
    if ( ec.solutionCount == 0 )
    {
      PLOGD << "No solution found";
      return;
    }
    else if ( ec.solutionCount > 1 )
    {
      PLOGD << "Multiple solutions found";

      // We have a solution, but we need to make it unique
      // We can do this by adding a solved cell to the grid and trying again
      // Lets work with the existing exact cover matrix
      // Uncover the cells that are already solved

      // Backup the first solution
      std::vector<Node*> firstSolutionBackup = ec.firstSolution;

      //ec.DebugOutPuzzleSolution( firstSolutionBackup );
      // Shuffle the remaining cells
      //std::shuffle( firstSolutionBackup.begin() + solutionSize, firstSolutionBackup.end(), std::mt19937{std::random_device{}( )} );
      ec.DebugOutPuzzleSolution( firstSolutionBackup );
      size_t counter = 0;
      for ( auto itAdded = firstSolutionBackup.begin() + solutionSize; itAdded != firstSolutionBackup.end(); ++itAdded, ++counter )
      {
        #pragma region  "Uncover the cells that are already solved"
        // Uncover the cells that are already solved
        auto itRev = ec.workingSolution.rbegin();
        auto itEnd = ec.workingSolution.rend() - solutionSize;
        while ( itRev != itEnd )
        {
          auto cell = *itRev;
          auto c = cell->head;
          for ( Node* j = cell->left; j != cell; j = j->left )
          {
            j->head->Uncover();
          }
          c->Uncover();
          ++itRev;
        }
        // Erase the last solution
        ec.workingSolution.erase( ec.workingSolution.begin() + solutionSize, ec.workingSolution.end() );
        #pragma endregion  "Uncover the cells that are already solved"

        // Add the new cell
        ec.SetMark( (*itAdded)->columnCounter, ( *itAdded )->rowCounter, ( *itAdded )->candidate );
        solutionSize = ec.workingSolution.size();

        ec.DebugOutPuzzleForTest( firstSolutionBackup, originalSize + counter );

        ec.solutionCount = 0;
        ec.Search( 0 );
        if ( ec.solutionCount == 0 )
        {
          __debugbreak();
          PLOGD << "No solution found";
        }
        else if ( ec.solutionCount == 1 )
        {
          PLOGD << "Unique solution found";
          ec.DebugOutPuzzleForTest( ec.firstSolution );
          ec.DebugOutPuzzleForTest( firstSolutionBackup );
          ec.DebugOutPuzzleForTest( firstSolutionBackup, 17 + counter );
          break;
        }
      }
    }
    else
    {
      PLOGD << "Unique solution found";
    }
  } while ( ec.solutionCount != 1 );
}

const std::ostringstream SudokuGrid::AsString()
{
  std::ostringstream ss;
  for ( int i = 0; i < 9; ++i )
  {
    for ( int j = 0; j < 9; ++j )
    {
      if ( cells[i][j]._solution != 0 )
        ss << cells[i][j]._solution;
      else
        ss << ".";
    }
  }
  return ss;
}

void SudokuGrid::CardShuffle::Shuffle()
{
  std::random_device rd;
  std::mt19937 g;
  g.seed( rd() );
  std::shuffle( deck.begin(), deck.end(), g );
  std::shuffle( deck.begin(), deck.end(), g );
  std::shuffle( deck.begin(), deck.end(), g );
  std::shuffle( deck.begin(), deck.end(), g );
  std::shuffle( deck.begin(), deck.end(), g );
}

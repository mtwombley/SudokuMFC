#include "pch.h"
#include "SudokuGrid.h"
#include "DLX.h"
#include <random>
#include <algorithm>
#include "RandomGen.h"

void SudokuGrid::GenerateRandomGrid()
{
  ec.Initialize();
  clear();

  CardShuffle puzzleDeck( &cells[0][0] );
  puzzleDeck.Shuffle();
  puzzleDeck.DebugPrint();

  RandomGenerator randGen;

  // Create a list of possible values for each cell
  std::vector<int> suggestions{1,2,3,4,5,6,7,8,9};

  int solutionTest = 0;

  // Sources have indicated that 17 is the minimum number of clues required for a unique solution
  for ( int x = 0; x < 81; ++x )
  {
    auto cell = puzzleDeck.deck.back();
    puzzleDeck.deck.pop_back();
    int i = cell->_value;
    int j = cell->_reserved;

    //Clear the cell temp values
    cell->_value = 0;
    cell->_reserved = 0;

    if ( x < 17 )
    {
      // Shuffle the suggestions, randomly selecting a value for the cell can cause long loops because of repeated values
      randGen.Shuffle( suggestions );
      bool found = false;
      for ( auto& suggestion : suggestions )
      {
        if ( CheckSolution( i, j, suggestion ) )
        {
          setSolution( i, j, suggestion );
          found = true;
          break;
        }
        else
        {
          ++solutionTest;
        }
      }
      if ( !found )
      {
        PLOGD << "No solution found";
        break;
      }
    }
  }
  PLOGD << VARTRACE(solutionTest) << AsString().str().c_str();


  // We have a minimum of 17 clues, now we need to check for a unique solution
  Solve();
  // If we have a unique solution, we are done


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

      // Shuffle the remaining cells
      std::shuffle( firstSolutionBackup.begin() + solutionSize, firstSolutionBackup.end(), std::mt19937{ std::random_device{}() } );
      for ( auto itAdded = firstSolutionBackup.begin() + solutionSize; itAdded != firstSolutionBackup.end(); ++itAdded )
      {
        // Uncover the cells that are already solved
        auto itRev = ec.workingSolution.rbegin();
        auto itEnd = ec.workingSolution.rend() - solutionSize;
        int counter = 0;
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
          ++counter;
        }
        // Erase the last solution
        ec.workingSolution.erase( ec.workingSolution.begin() + solutionSize, ec.workingSolution.end() );

        // Add the new cell
        ec.SetMark( firstSolutionBackup[solutionSize - 1]->columnCounter, firstSolutionBackup[solutionSize - 1]->rowCounter, firstSolutionBackup[solutionSize - 1]->candidate );
        solutionSize = ec.workingSolution.size();

        ec.solutionCount = 0;
        ec.Search( 0 );
        if (ec.solutionCount == 0 )
        {
          __debugbreak();
          PLOGD << "No solution found";
        }
      }
    }
    else
    {
      PLOGD << "Unique solution found";
    }
  } while ( ec.solutionCount != 1 );

//   std::ostringstream ss;
//   for ( int i = 0; i < 9; ++i )
//   {
//     for ( int j = 0; j < 9; ++j )
//     {
//       ss << grid[i][j]._solution;
//     }
//   }
//   PLOGD << ss.str().c_str();
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

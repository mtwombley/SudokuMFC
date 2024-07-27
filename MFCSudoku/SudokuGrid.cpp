#include "pch.h"
#include "SudokuGrid.h"
#include "DLX.h"

void SudokuGrid::GenerateRandomGrid()
{
  int i = 0;
  int j = 0;

  int solutionCount = 0;
  //ec.PrettyPrint();
  do
  {
    // Sources have indicated that 17 is the minimum number of clues required for a unique solution
    for ( int x = 0; x < 17; ++x )
    {
      unsigned int sol = rand() % 9;
      while ( cells[i = rand() % 9][j = rand() % 9]._solution ); // Find an empty cell
      cells[i][j]._solution = sol; // Mark the cell as a solution
      if (!ec.SetMark(i,j,sol)) // Mark the cell in the exact cover matrix
      {
        --x;
        cells[i][j]._solution = 0;
      }
    }
    PLOGD << AsString().str().c_str();
    ec.Search( 0 );

//    solve_puzzle( newGrid->AsString().str().c_str() );
//     if ( newGrid->Solve() )
//     {
//       solutionCount++;
//       *this = *newGrid;
//     }
  } while ( !solutionCount );
}

bool SudokuGrid::Solve()
{
  // Solve the grid
//   BuildSparseMatrix();
//   BuildLinkedList();
//   TransformListToCurrentGrid();
//   Search( 0 );
  return isSolved;
}

const std::ostringstream SudokuGrid::AsString()
{
  std::ostringstream ss;
  for ( int i = 0; i < 9; ++i )
  {
    for ( int j = 0; j < 9; ++j )
    {
      ss << cells[i][j]._solution;
    }
  }
  return ss;
}

// void SudokuGrid::coverColumn( Node* col )
// {
//   col->left->right = col->right;
//   col->right->left = col->left;
//   for ( Node* node = col->down; node != col; node = node->down )
//   {
//     for ( Node* temp = node->right; temp != node; temp = temp->right )
//     {
//       temp->down->up = temp->up;
//       temp->up->down = temp->down;
//       temp->head->size--;
//     }
//   }
// }
//
// void SudokuGrid::uncoverColumn( Node* col )
// {
//   for ( Node* node = col->up; node != col; node = node->up )
//   {
//     for ( Node* temp = node->left; temp != node; temp = temp->left )
//     {
//       temp->head->size++;
//       temp->down->up = temp;
//       temp->up->down = temp;
//     }
//   }
//   col->left->right = col;
//   col->right->left = col;
// }
//
// void SudokuGrid::Search( int k )
// {
//   if ( HeadNode->right == HeadNode )
//   {
//     MapSolutionToGrid();
//     isSolved = true;
//     return;
//   }
//
//   //Choose Column Object Deterministically: Choose the column with the smallest Size
//   Node* Col = HeadNode->right;
//   for ( Node* temp = Col->right; temp != HeadNode; temp = temp->right )
//     if ( temp->size < Col->size )
//       Col = temp;
//
//   coverColumn( Col );
//
//   for ( Node* temp = Col->down; temp != Col; temp = temp->down )
//   {
//     solution[k] = temp;
//     for ( Node* node = temp->right; node != temp; node = node->right )
//     {
//       coverColumn( node->head );
//     }
//
//     Search( k + 1 );
//
//     temp = solution[k];
//     solution[k] = NULL;
//     Col = temp->head;
//     for ( Node* node = temp->left; node != temp; node = node->left )
//     {
//       uncoverColumn( node->head );
//     }
//   }
//
//   uncoverColumn( Col );
// }
//
// void SudokuGrid::BuildLinkedList()
// {
//   Node* header = new Node;
//   header->left = header;
//   header->right = header;
//   header->down = header;
//   header->up = header;
//   header->size = -1;
//   header->head = header;
//   Node* temp = header;
//
//   //Create all Column Nodes
//   for ( int i = 0; i < COL_NB; i++ )
//   {
//     Node* newNode = new Node;
//     newNode->size = 0;
//     newNode->up = newNode;
//     newNode->down = newNode;
//     newNode->head = newNode;
//     newNode->right = header;
//     newNode->left = temp;
//     temp->right = newNode;
//     temp = newNode;
//   }
//
//   int ID[3] = {0,1,1};
//   //Add a Node for each 1 present in the sparse matrix and update Column Nodes accordingly
//   for ( int i = 0; i < ROW_NB; i++ )
//   {
//     Node* top = header->right;
//     Node* prev = NULL;
//
//     if ( i != 0 && i % SIZE_SQUARED == 0 )
//     {
//       ID[0] -= GSIZE - 1;
//       ID[1]++;
//       ID[2] -= GSIZE - 1;
//     }
//     else if ( i != 0 && i % GSIZE == 0 )
//     {
//       ID[0] -= GSIZE - 1;
//       ID[2]++;
//     }
//     else
//     {
//       ID[0]++;
//     }
//
//     for ( int j = 0; j < COL_NB; j++, top = top->right )
//     {
//       if ( matrix[i][j] )
//       {
//         Node* newNode = new Node;
//         newNode->candidate = ID[0];
//         newNode->rowCounter = ID[1];
//         newNode->columnCounter = ID[2];
//         if ( prev == NULL )
//         {
//           prev = newNode;
//           prev->right = newNode;
//         }
//         newNode->left = prev;
//         newNode->right = prev->right;
//         newNode->right->left = newNode;
//         prev->right = newNode;
//         newNode->head = top;
//         newNode->down = top;
//         newNode->up = top->up;
//         top->up->down = newNode;
//         top->size++;
//         top->up = newNode;
//         if ( top->down == top )
//           top->down = newNode;
//         prev = newNode;
//       }
//     }
//   }
//
//   HeadNode = header;
// }
//
// void SudokuGrid::TransformListToCurrentGrid()
// {
//   int index = 0;
//   for ( int i = 0; i < GSIZE; i++ )
//     for ( int j = 0; j < GSIZE; j++ )
//       if ( cells[i][j]._value > 0 )
//       {
//         Node* Col = NULL;
//         Node* temp = NULL;
//         for ( Col = HeadNode->right; Col != HeadNode; Col = Col->right )
//         {
//           for ( temp = Col->down; temp != Col; temp = temp->down )
//             if ( temp->candidate == static_cast<int>(cells[i][j]._value) && ( temp->rowCounter - 1 ) == i && ( temp->columnCounter - 1 ) == j )
//               goto ExitLoops; // Break out of nested loops if a match is found in the linked list, goto is the better method
//         }
// ExitLoops:
//         coverColumn( Col );
//         orig_values[index] = temp;
//         index++;
//         for ( Node* node = temp->right; node != temp; node = node->right )
//         {
//           coverColumn( node->head );
//         }
//       }
// }
//
// void SudokuGrid::MapSolutionToGrid()
// {
//   for ( int i = 0; solution[i] != nullptr; i++ )
//   {
//     solutions[solution[i]->rowCounter - 1][solution[i]->columnCounter - 1]._value = solution[i]->candidate;
//   }
//   for ( int i = 0; orig_values[i] != nullptr; i++ )
//   {
//     solutions[orig_values[i]->rowCounter - 1][orig_values[i]->columnCounter - 1]._value = orig_values[i]->candidate;
//   }
// }

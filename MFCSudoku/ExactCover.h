#pragma once

// Exact Cover problem using Dancing Links

#include "DLX.h"
#include <iostream>

// This exact cover implementation is for a 9x9 sudoku grid
// The grid is represented as a 729x324 matrix, 729 being 9x9x9 (9 rows, 9 columns, 9 numbers), 324 being 4x81 (4 constraints, 81 cells)
// The constraints are:
// 1. There can only be one value in any given cell
// 2. There can only be one instance of a number in any given row
// 3. There can only be one instance of a number in any given column
// 4. There can only be one instance of a number in any given region
// The exact cover problem is to find a set of rows such that each column has exactly one 1 in it
// The solution to the exact cover problem will be a set of rows that represent a valid sudoku grid


struct ExactCover
{
  Node* head;
  Node (*col)[324]; // Will be a 2D array of 729x324 nodes
  std::vector<Node*> solution;
  int solutionCount;

  // Constructor / Destructor
  ExactCover()
  {
    head = new Node();
    // Instead of calling new 33885 times like the example, we allocate a 2D array of 730x324 nodes
    col = new Node[730][324]; // 730 because row 0 is the column headers

    // Initialize the Column headers
    for ( int i = 0; i < 324; ++i )
    {
      col[0][i].left = &col[0][( i + 323 ) % 324];
      col[0][i].right = &col[0][( i + 1 ) % 324];
      col[0][i].up = &col[0][i];
      col[0][i].down = &col[0][i];
      col[0][i].head = &col[0][i];
      col[0][i].size = 0;
      col[0][i].candidate = 0;
      col[0][i].rowCounter = 0;
      col[0][i].columnCounter = 0;
    }

    // Initialize the head node
    head->left = &col[0][323];
    head->right = &col[0][0];
    head->up = head;
    head->down = head;
    head->head = head;
    head->size = 0;
    head->candidate = 0;
    head->rowCounter = 0;
    head->columnCounter = 0;

    // The last column header points back to the head
    col[0][323].right = col[0][0].left = head;

    int tmpCandidate = 0;
    int tmpRow = 1;
    int tmpColumn = 1;
    int rowCount = 0;

    for ( int column = 0; column < 81; ++column )
    {
      int calc1 = column * 9; //multiplication is done once per column not each row loop
      int calc2 = ( ( ( column / 3 ) % 3 ) + ( column / 27 ) * 3 ) * 9;
      for ( int row = 0; row < 9; ++row )
      {
        int y = calc1 + row + 1; // y is the row number, offseting 1 because 0 is the column header
        // Get the 4 groups that the cell belongs to

        auto columnGrp1 = column;
        auto columnGrp2 = 81 + 9 * ( column / 9) + row;
        auto columnGrp3 = ( row + calc1 ) % 81 + 162;
        auto columnGrp4 = 243 + row + calc2;

        auto grp1 = &col[y][columnGrp1];
        auto grp2 = &col[y][columnGrp2];
        auto grp3 = &col[y][columnGrp3];
        auto grp4 = &col[y][columnGrp4];

        // Set the head pointers for the 4 groups to the column header
        grp1->head = &col[0][columnGrp1];
        grp2->head = &col[0][columnGrp2];
        grp3->head = &col[0][columnGrp3];
        grp4->head = &col[0][columnGrp4];

        grp1->head->size++;
        grp2->head->size++;
        grp3->head->size++;
        grp4->head->size++;

        grp1->size = 0;
        grp2->size = 0;
        grp3->size = 0;
        grp4->size = 0;

        // Set the candidate, row, and column counters
        if (rowCount != 0 && rowCount % 81 == 0)
        {
          tmpCandidate -= 9 - 1;
          tmpRow += 1;
          tmpColumn -= 9 - 1;
        }
        else if ( rowCount != 0 && rowCount % 9 == 0 )
        {
          tmpCandidate -= 9 -1;
          tmpColumn += 1;
        }
        else
        {
          tmpCandidate += 1;
        }

        grp1->candidate = tmpCandidate;
        grp2->candidate = tmpCandidate;
        grp3->candidate = tmpCandidate;
        grp4->candidate = tmpCandidate;

        grp1->rowCounter = tmpRow;
        grp2->rowCounter = tmpRow;
        grp3->rowCounter = tmpRow;
        grp4->rowCounter = tmpRow;

        grp1->columnCounter = grp1->head->columnCounter = tmpColumn;
        grp2->columnCounter = grp2->head->columnCounter = tmpColumn;
        grp3->columnCounter = grp3->head->columnCounter = tmpColumn;
        grp4->columnCounter = grp4->head->columnCounter = tmpColumn;

        // Set the right and left pointers for the columns
        grp1->right = grp2;
        grp2->right = grp3;
        grp3->right = grp4;
        grp4->right = grp1;

        grp4->left = grp3;
        grp3->left = grp2;
        grp2->left = grp1;
        grp1->left = grp4;

        // Set the up and down pointers for the rows
        SetUpDownPointers( grp1 );
        SetUpDownPointers( grp2 );
        SetUpDownPointers( grp3 );
        SetUpDownPointers( grp4 );

        rowCount++;
      }
    }

    solutionCount = 0;
    //PrettyPrint();
  }

  ~ExactCover()
  {
    delete head;
    delete[] col;
  }

  void Solve()
  {
    Search( 0 );
  }

  void Search( int k )
  {
    if ( head->right == head )
    {
      solutionCount++;
      return;
    }

    Node* c = head->ChooseColumn();
    c->Cover();

    for ( Node* r = c->down; r != c; r = r->down )
    {
      solution[k] = r;
      for ( Node* j = r->right; j != r; j = j->right )
      {
        j->head->Cover();
      }

      Search( k + 1 );

      r = solution[k];
      c = r->head;
      for ( Node* j = r->left; j != r; j = j->left )
      {
        j->head->Uncover();
      }
    }
    c->Uncover();
  }

  void PrettyPrint()
  {
    PLOGD << "Pretty Print";
    PLOGD << "Column Headers";

    // Print the column headers
    plog::util::nostringstream ss;
    for ( int j = 0; j < 324; ++j )
    {
      ss << col[0][j].size << ",";
    }
    PLOGD << ss.str();

    // Print the rows
    for ( int i = 1; i < 730; ++i )
    {
      ss = plog::util::nostringstream();
      for ( int j = 0; j < 324; ++j )
      {
        if ( col[i][j].size == 0 )
        {
          ss << col[i][j].candidate << ",";
        }
        else
        {
          ss << ".,";
        }
      }
      PLOGD << ss.str();
    }
  }

private:
  void SetUpDownPointers( Node* grp1 )
  {
    if ( grp1->head->up == grp1->head )
    { //If the column header is pointing to itself, then this is the first row in the column
      grp1->head->up = grp1;
      grp1->head->down = grp1;
      grp1->up = grp1->head;
      grp1->down = grp1->head;
    }
    else
    { //Otherwise, add the row to the bottom of the column
      grp1->head->up = grp1;
      grp1->down = grp1->head;
      Node* i = nullptr;
      for ( i = grp1->head; i->down != grp1->head; i = i->down );
      i->down = grp1;
      grp1->up = i;
    }
  }
public:

  bool SetMark( unsigned int _col, unsigned int _row, unsigned int value )
  {
    bool bRet = false;
    Node* temp = nullptr;
    Node* column = nullptr;

    int i = _row*81-81+(_col-1)*9+value;
    int j = ( _row - 1 ) * 9 + _col - 1;

    int flag = 0;
    for ( column = head->right; column != head; column = column->right )
    {
      if ( column->down->columnCounter != _col )
        continue;
      for ( temp = column->down; temp != column; temp = temp->down )
        if ( temp->candidate == value )
        {
          flag = 1;
          break;
        }
      if ( flag )
        break;
    }
    if ( flag )
    {
      column->Cover();
      solution.push_back(temp);
      for ( auto rowNode = temp->right; rowNode != temp; rowNode = rowNode->right )
        rowNode->head->Cover();
      bRet = true;
    }
    return bRet;
  }
};
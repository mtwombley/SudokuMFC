#pragma once

// Exact Cover problem using Dancing Links

#include "DLX.h"
#include "MFCSudoku.h"
#include <iostream>
#include <strsafe.h>

// This exact cover implementation is for a 9x9 Sudoku grid
// The grid is represented as a 729x324 matrix, 729 being 9x9x9 (9 rows, 9 columns, 9 numbers), 324 being 4x81 (4 constraints, 81 cells)
// The constraints are:
// 1. There can only be one value in any given cell
// 2. There can only be one instance of a number in any given row
// 3. There can only be one instance of a number in any given column
// 4. There can only be one instance of a number in any given region
// The exact cover problem is to find a set of rows such that each column has exactly one 1 in it
// The solution to the exact cover problem will be a set of rows that represent a valid Sudoku grid

// I don't like defines (they do have their place)
namespace
{
  const int MAX_CANDIDATES = 9;
  const int MAX_ROWS = 730;
  const int MAX_COLS = 324;
}

struct ExactCover
{
  Node* head;
  Node( *col )[MAX_COLS]; // Will be a 2D array of 729x324 nodes
  std::vector<Node*> workingSolution;
  std::vector<Node*> firstSolution;
  int solutionCount;

  // Constructor / Destructor
  ExactCover() : head( nullptr ), col( nullptr ), solutionCount( 0 )
  {
    head = new Node();
    // Instead of calling new 33885 times like the example, I allocate a 2D array of 730x324 nodes
    col = new Node[MAX_ROWS][MAX_COLS]; // 730 because row 0 is the column headers

    workingSolution.reserve( 81 );

    Initialize();
  }

  ~ExactCover()
  {
    delete head;
    delete[] col;
  }

  void Initialize()
  {
    workingSolution.clear();
    // Initialize the Column headers
    for ( int i = 0; i < MAX_COLS; ++i )
    {
      col[0][i].left = &col[0][( i + ( MAX_COLS - 1 ) ) % MAX_COLS];
      col[0][i].right = &col[0][( i + 1 ) % MAX_COLS];
      col[0][i].up = &col[0][i];
      col[0][i].down = &col[0][i];
      col[0][i].head = &col[0][i];
      col[0][i].size = 0;
      col[0][i].candidate = 0;
      col[0][i].rowCounter = 0;
      col[0][i].columnCounter = 0;
      col[0][i].rowIndex = 0;
      col[0][i].columnIndex = i;
      StringCbPrintf( col[0][i].strValue, 20, L"r0c%d#0", i );
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

    // The first column header points to the head
    col[0][0].left = head;
    // The last column header points back to the head
    col[0][323].right = col[0][0].left = head;

    int tmpCandidate = 0;
    int tmpRow = 1;
    int tmpColumn = 1;
    int rowCount = 0;

    for ( int column = 0; column < 81; ++column )
    {
      int calc1 = column * MAX_CANDIDATES; //multiplication is done once per column not each row loop
      int calc2 = ( ( ( column / 3 ) % 3 ) + ( column / 27 ) * 3 ) * MAX_CANDIDATES;
      for ( int row = 0; row < MAX_CANDIDATES; ++row )
      {
        int y = calc1 + row + 1; // y is the row number, offsetting 1 because 0 is the column header
        // Get the 4 groups that the cell belongs to

        auto columnGrp1 = column;
        auto columnGrp2 = 81 + MAX_CANDIDATES * ( column / MAX_CANDIDATES ) + row;
        auto columnGrp3 = ( row + calc1 ) % 81 + 162;
        auto columnGrp4 = 243 + row + calc2;

        Node* grp1 = &col[y][columnGrp1];
        Node* grp2 = &col[y][columnGrp2];
        Node* grp3 = &col[y][columnGrp3];
        Node* grp4 = &col[y][columnGrp4];

        // Set the head pointers for the 4 groups to the column header
        grp1->head = &col[0][columnGrp1];
        grp2->head = &col[0][columnGrp2];
        grp3->head = &col[0][columnGrp3];
        grp4->head = &col[0][columnGrp4];

        grp1->head->size++;
        grp2->head->size++;
        grp3->head->size++;
        grp4->head->size++;

        grp1->size = -1;
        grp2->size = -1;
        grp3->size = -1;
        grp4->size = -1;

        // Set the candidate, row, and column counters
        if ( rowCount != 0 && rowCount % 81 == 0 )
        {
          tmpCandidate -= MAX_CANDIDATES - 1;
          tmpRow += 1;
          tmpColumn -= MAX_CANDIDATES - 1;
        }
        else if ( rowCount != 0 && rowCount % MAX_CANDIDATES == 0 )
        {
          tmpCandidate -= MAX_CANDIDATES - 1;
          tmpColumn += 1;
        }
        else
        {
          tmpCandidate += 1;
        }

        StringCbPrintf( grp1->strValue, 20, L"r%dc%d#%d", tmpRow, tmpColumn, tmpCandidate );
        StringCbPrintf( grp2->strValue, 20, L"r%dc%d#%d", tmpRow, tmpColumn, tmpCandidate );
        StringCbPrintf( grp3->strValue, 20, L"r%dc%d#%d", tmpRow, tmpColumn, tmpCandidate );
        StringCbPrintf( grp4->strValue, 20, L"r%dc%d#%d", tmpRow, tmpColumn, tmpCandidate );

        grp1->rowIndex = y - 1; // Making the row index 0 based
        grp2->rowIndex = y - 1;
        grp3->rowIndex = y - 1;
        grp4->rowIndex = y - 1;
        grp1->columnIndex = columnGrp1;
        grp2->columnIndex = columnGrp2;
        grp3->columnIndex = columnGrp3;
        grp4->columnIndex = columnGrp4;

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
    //OutputToBMP();
  }

  // Recursive search function
  void Search( int k )
  {
    if ( head->right == head )
    {
      solutionCount++;
      // On the first solution back it up and see if there are more solutions
      if ( solutionCount == 1 )
      {
        firstSolution = workingSolution;
      }
      //DebugOutPuzzleSolution();
      return;
    }

    Node* c = head->ChooseColumn();
    c->Cover();

    for ( Node* r = c->down; r != c; r = r->down )
    {
      //solution[k] = r;
      workingSolution.push_back( r );
      for ( Node* j = r->right; j != r; j = j->right )
      {
        j->head->Cover();
      }

      Search( k + 1 );
      // If we have more than one solution, we are done, but allow the recursion to unwind
      if ( solutionCount >= 2 )
      {
        return;
      }

      r = workingSolution.back();
      workingSolution.pop_back();
      c = r->head;
      for ( Node* j = r->left; j != r; j = j->left )
      {
        j->head->Uncover();
      }
    }
    c->Uncover();
  }

  void DebugOutPuzzleSolution()
  {
    std::ostringstream ss;
    std::ostringstream ss1;
    std::vector<Node*> backupSolution = this->workingSolution;
    std::sort( backupSolution.begin(), backupSolution.end(), [] ( Node* a, Node* b )
    {
      if ( a->rowCounter == b->rowCounter )
        return a->columnCounter < b->columnCounter;
      else
        return a->rowCounter < b->rowCounter;
    } );

    for ( auto node : backupSolution )
    {
      ss << node->candidate;
      ss1 << "r" << node->rowCounter << "c" << node->columnCounter << "#" << node->candidate << " ";
    }
    PLOGD << ss.str();
    PLOGD << ss1.str();
  }

  // Print the solution to debug output using plog
  void PrettyPrint()
  {
    PLOGD << "Pretty Print";
    PLOGD << "Column Headers";

    // Print the column headers
    plog::util::nostringstream ss;
    for ( int j = 0; j < MAX_COLS; ++j )
    {
      ss << " " << col[0][j].size << " ";
    }
    PLOGD << ss.str();

    // Print the rows
    for ( int i = 1; i < MAX_ROWS; ++i )
    {
      plog::util::nostringstream line1;
      plog::util::nostringstream line2;
      plog::util::nostringstream line3;
      for ( int j = 0; j < MAX_COLS; ++j )
      {
        if ( col[i][j].up != nullptr )
          line1 << " ^ ";
        else
          line1 << "   ";

        if ( col[i][j].left != nullptr )
          line2 << "<" << col[i][j].candidate;
        else
          line2 << "  ";
        if ( col[i][j].right != nullptr )
          line2 << ">";
        else
          line2 << " ";

        if ( col[i][j].down != nullptr )
          line3 << " v ";
        else
          line3 << "   ";
      }
      PLOGD << line1.str();
      PLOGD << line2.str();
      PLOGD << line3.str();
    }
  }

  void OutputToBMP()
  {
    int xOffset = 50;
    int yOffset = 50;
    // Create a device independent bitmap
    // Each cell of the cover matrix will be 20x20
    const int width = 20 * MAX_COLS + xOffset;
    const int height = 20 * MAX_ROWS + yOffset;

    CDC memoryDC;
    memoryDC.CreateCompatibleDC( ( theApp.GetMainWnd()->GetDC() ) );

    CBitmap bmp;
    bmp.CreateBitmap( width, height, 1, 32, nullptr/* &memoryDC, width, height*/ );
    CBitmap* oldBmp = memoryDC.SelectObject( &bmp );
    memoryDC.FloodFill( 0, 0, RGB( 255, 255, 255 ) );

    CPen blkPen( PS_SOLID, 1, RGB( 0, 0, 0 ) );
    CPen redPen( PS_SOLID, 2, RGB( 255, 0, 0 ) );

    //CBrush* oldBrush = (CBrush*)memoryDC.SelectStockObject( NULL_BRUSH );
    CPen* oldPen = memoryDC.SelectObject( &blkPen );

    // Font selection and setup
    // Get the system font
    NONCLIENTMETRICS nonClientMetrics;
    nonClientMetrics.cbSize = sizeof( NONCLIENTMETRICS );

    CFont sysFont;
    if ( !SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( NONCLIENTMETRICS ), &nonClientMetrics, 0 ) )
    { //We failed to get the system font so we should create something
    }
    else
    {
      // set the font to the one we got from the system
      sysFont.CreateFontIndirect( &nonClientMetrics.lfCaptionFont );
    }

    nonClientMetrics.lfCaptionFont.lfOrientation = 900;
    nonClientMetrics.lfCaptionFont.lfEscapement = 900;

    CFont vFont;
    vFont.CreateFontIndirect( &nonClientMetrics.lfCaptionFont );

    CFont* oldFont = memoryDC.SelectObject( &sysFont );

    //        x = 6400  y = 14520  i = 726  j = 320 0000009DEE0FE878 0000009DEE0FE898 0000009DEE0FE8B8 0000009DEE0FE8D8
//    PLOGD << "x,        y,         i,       j,       Head,            THIS,            UP,              LEFT,             RIGHT,            DOWN";
//    PLOGD << ",,,, " << head << ",,,,,";

    for ( int i = 0; i < MAX_ROWS; ++i )
    {
      for ( int j = 0; j < MAX_COLS; ++j )
      {
        int x = j * 20;
        int y = i * 20;

        Node* current = &col[i][j];
        Node* up = current->up; // up should point to a lower address
        Node* left = current->left;
        Node* right = current->right;
        Node* down = current->down; // down should point to a higher address

        if ( i == 0 )
        {
          memoryDC.SelectObject( &vFont );
          wchar_t buffer[10];
          swprintf( buffer, 10, L"%d", j );
          //memoryDC.DrawText( buffer, CRect( j * 20 + xOffset, 0, j * 20 + 20 + yOffset, yOffset ), DT_RIGHT | DT_BOTTOM );
          memoryDC.TextOut( j * 20 + xOffset, yOffset, buffer );
        }

        if ( left != nullptr && right != nullptr )
        {
          // Center of the cell for the move to point and draw to point
          int xMPos = xOffset + x + 10;
          int yMPos = yOffset + y + 10;
          int xDPos = xMPos;
          int yDPos = yMPos;

          if ( current - up < 0 )
          {
            yDPos = yOffset - 4; // draw to the top of the matrix area
          }
          else
          {
            yDPos = yMPos - ( (int)( current - up ) / MAX_COLS ) * 20;
          }

//           PLOGD << "'" << x << ",'" << y << ",'" << i << ",'" << j << ",'" << &col[i][0] << ",'" << current << ",'" << yMPos << " " << yDPos << ",'" << left << ",'" << right << ",'" << down;

          auto lambdaA = [&memoryDC] ( int xMPos, int yMPos, int xDPos, int yDPos, CPen* cPen )
          {
            memoryDC.SelectObject( cPen );
            memoryDC.MoveTo( CPoint( xMPos, yMPos ) );
            memoryDC.LineTo( CPoint( xDPos, yDPos ) );
          };

          if ( current->left == current )
          {
            memoryDC.SelectObject( &redPen );
            CRect rect( xMPos, yMPos, xMPos + 1, yMPos + 1 );
            rect.InflateRect( CSize( 7, 7 ) );
            memoryDC.Rectangle( rect );
          }
          else
          {
            memoryDC.SelectObject( &blkPen );
            CRect rect( xMPos, yMPos, xMPos, yMPos );
            rect.InflateRect( CSize( 5, 5 ) );
            memoryDC.Rectangle( rect );
          }


          // Draw the line up
          lambdaA( xMPos, yMPos - 5, xDPos, yDPos + 4, &blkPen );

          // Draw the line down only for the wrap around
          if ( down == current->head )
          {
            lambdaA( xMPos, yMPos + 5, xDPos, height, &blkPen );
          }

          yDPos = yMPos;
          // Draw the line left
          if ( i == 0 ) // Column headers
          {
            if ( left == head )
            {
              xDPos = xOffset - 5;
            }
            else
            {
              xDPos -= (int)( current - left ) * 20;
            }
            lambdaA( xMPos - 5, yMPos, xDPos + 4, yDPos, &blkPen );
          }
          else
          {
            if ( current - left < 0 )
            {
              xDPos = xOffset - 5; // draw to the right of the matrix area
            }
            else
            {
              xDPos -= (int)( current - left ) * 20;
            }
            lambdaA( xMPos - 5, yMPos, xDPos + 4, yDPos, &blkPen );
          }

          // Draw the line right only for the wrap around
          if ( right - current < 0 )
          {
            lambdaA( xMPos + 5, yMPos, width, yDPos, &blkPen );
          }
        }

      }
      if ( i > 0 )
      {
        memoryDC.SelectObject( &sysFont );
        wchar_t buffer[10];
        swprintf( buffer, 10, L"r%dc%d#%d", ( i - 1 ) / 81 + 1, ( ( i - 1 ) / MAX_CANDIDATES ) % MAX_CANDIDATES + 1, ( i - 1 ) % MAX_CANDIDATES + 1 );
        memoryDC.DrawText( buffer, CRect( 0, i * 20 + yOffset, xOffset, i * 20 + 20 + yOffset ), DT_RIGHT | DT_BOTTOM );
      }
    }

    memoryDC.SelectObject( oldFont );
    memoryDC.SelectObject( oldPen );
    memoryDC.SelectObject( oldBmp );
    //memoryDC.SelectObject( oldBrush );

    // copy the bitmap to the clipboard
    OpenClipboard( nullptr );
    EmptyClipboard();
    SetClipboardData( CF_BITMAP, bmp.GetSafeHandle() );
    CloseClipboard();
  }

  bool SetMark( unsigned int _col/*1-9*/, unsigned int _row/*1-9*/, unsigned int value/*1-9*/ )
  {
    bool bRet = false;
    Node* temp = nullptr;
    Node* column = nullptr;

    int i = _row * 81 - 81 + ( _col - 1 ) * MAX_CANDIDATES + value/* Row from 1-730 */;
    int j = ( _row - 1 ) * MAX_CANDIDATES + _col - 1; // Column from 0-80 for the column

    // Is the cell already marked?
    if ( col[i][j].left != nullptr && col[i][j].right != nullptr &&
         col[i][j].up->down == &col[i][j] )
    {
      int flag = 0;
      column = col[i][j].head;
        // column is the header node for the column
        // temp is the row node for the column
      for ( temp = column->down; temp != column; temp = temp->down )
      {
        if ( temp->candidate == value )
        {
          flag = 1;
          break;
        }
      }
      if ( flag )
      {
        column->Cover();
        workingSolution.push_back( temp );
        for ( auto rowNode = temp->right; rowNode != temp; rowNode = rowNode->right )
          rowNode->head->Cover();
        bRet = true;
      }
    }

    //OutputToBMP();
    return bRet;
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

};
// CGridCtrl.cpp : implementation file
//

#include "pch.h"
#include "CGridCtrl.h"

#define SUDOKU_GRID_CLASS_NAME _T( "CGridCtrl" )

// CGridCtrl

IMPLEMENT_DYNAMIC(CGridCtrl, CWnd)

CGridCtrl::CGridCtrl()
{
  RegisterWindowClass();
  pencilMarkRects.reserve( 729 );
}

CGridCtrl::~CGridCtrl()
{
  if (homeCellPen.GetSafeHandle() != nullptr)
  {
    homeCellPen.DeleteObject();
  }
  if (blockPen.GetSafeHandle() != nullptr)
  {
    blockPen.DeleteObject();
  }
  if (paddingPen.GetSafeHandle() != nullptr)
  {
    paddingPen.DeleteObject();
  }
  if (cellPen.GetSafeHandle() != nullptr)
  {
    cellPen.DeleteObject();
  }

  if ( fontSml.GetSafeHandle() != nullptr )
  {
    fontSml.DeleteObject();
  }
  if ( fontSmlBold.GetSafeHandle() != nullptr )
  {
    fontSmlBold.DeleteObject();
  }
  if ( fontLrg.GetSafeHandle() != nullptr )
  {
    fontLrg.DeleteObject();
  }
  if ( fontLrgBold.GetSafeHandle() != nullptr )
  {
    fontLrgBold.DeleteObject();
  }
}

enum
{
  ThickLine1,  // 0
  ValueCell1,  // 1
  ThinLine1,   // 2
  ValueCell2,  // 3
  ThinLine2,   // 4
  ValueCell3,  // 5
  ThickLine2,  // 6
  ValueCell4,  // 7
  ThinLine3,   // 8
  ValueCell5,  // 9
  ThinLine4,   // 10
  ValueCell6,  // 11
  ThickLine3,  // 12
  ValueCell7,  // 13
  ThinLine5,   // 14
  ValueCell8,  // 15
  ThinLine6,   // 16
  ValueCell9,  // 17
  ThickLine4,  // 18
};

// CGridCtrl message handlers
BEGIN_MESSAGE_MAP(CGridCtrl, CWnd)
  ON_WM_CHANGEUISTATE()
  ON_WM_ERASEBKGND()
  ON_WM_MOUSEMOVE()
  ON_WM_PAINT()
  ON_WM_MOUSELEAVE()
  ON_WM_KEYDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

COLORREF CLR_PENCILMARKS    = RGB( 0, 0, 0 );
COLORREF CLR_GRID_NUMBERS   = RGB( 0, 0, 0 );
COLORREF CLR_WHITE          = RGB( 255, 255, 255 );
COLORREF CLR_SOLUTIONS      = RGB( 69, 92, 186 );
COLORREF CLR_CELL_HIGHLIGHT = RGB( 255, 242, 0 );  //cell highlight yellow
COLORREF CLR_CELL_VAL       = RGB( 21, 173, 76 );  //cell value number
COLORREF CLR_BLUE           = RGB( 0, 0, 255 );
COLORREF CLR_RED            = RGB( 255, 0, 0 );
COLORREF CLR_LGT_GREEN      = RGB( 21, 173, 76 );

BOOL CGridCtrl::RegisterWindowClass()
{
  WNDCLASS wndcls;
  HINSTANCE hInst = AfxGetInstanceHandle();

  if ( !( ::GetClassInfo( hInst, SUDOKU_GRID_CLASS_NAME, &wndcls ) ) )
  {
    // otherwise we need to register a new class
    wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wndcls.lpfnWndProc = ::DefWindowProc;
    wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
    wndcls.hInstance = hInst;
    wndcls.hIcon = NULL;
    wndcls.hCursor = AfxGetApp()->LoadStandardCursor( IDC_ARROW );
    wndcls.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
    wndcls.lpszMenuName = NULL;
    wndcls.lpszClassName = _T( "CGridCtrl" );

    if ( !AfxRegisterClass( &wndcls ) )
    {
      AfxThrowResourceException();
      return FALSE;
    }
  }

  return TRUE;
}

void CGridCtrl::OnChangeUIState( UINT nAction, UINT nUIElement )
{
  // This feature requires Windows 2000 or greater.
  // The symbols _WIN32_WINNT and WINVER must be >= 0x0500.
  CWnd::OnChangeUIState( nAction, nUIElement );

  // TODO: Add your message handler code here
}

BOOL CGridCtrl::OnEraseBkgnd( CDC* pDC )
{
  // TODO: Add your message handler code here and/or call default

  return CWnd::OnEraseBkgnd( pDC );
}

void CGridCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
  if (!m_mouseInGrid )
  {
    // We have entered the grid area, start tracking for mouse leave
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof( tme );
    tme.dwFlags = TME_LEAVE/* | TME_HOVER*/;
    tme.hwndTrack = GetSafeHwnd();
    m_mouseInGrid = TrackMouseEvent( &tme );
    if (!m_mouseInGrid )
    {
      auto err = GetLastError();
      PLOGD << "TrackMouseEvent failed with error: " << err;
    }
  }
  m_queryPoint = CPoint( -1,-1 ); //Invalidate the last query point
  switch ( nFlags )
  {
  case MK_LBUTTON:
    //PLOGD << "Left button pressed";
    break;
  case MK_RBUTTON:
    //PLOGD << "Right button pressed";
    break;
  case MK_MBUTTON:
    //PLOGD << "Middle button pressed";
    break;
  case MK_CONTROL:
    //PLOGD << "Control button pressed";
    break;
  case MK_SHIFT:
    //PLOGD << "Shift button pressed";
    break;
  case MK_XBUTTON1:
    //PLOGD << "X1 button pressed";
    break;
  case MK_XBUTTON2:
    //PLOGD << "X2 button pressed";
    break;
  case 0:
  {
    // if we are in pencil mode, draw a rect around the pencil mark cell
    // find the cell that we are in
    if (m_pencilMode )
    {
      auto foundRects = m_quadtree.query( point );
      if ( foundRects.size() == 0 )
      {
        PLOGD << "No cell found";
      }
      for ( auto rect : foundRects )
      {
        if ( m_highlightedCell != *rect/* && m_unHighlightCell != *rect */ )
        {
          m_unHighlightCell = m_highlightedCell;
          m_highlightedCell.CopyRect( *rect );
          m_queryPoint = point;
          m_currentRow = rect->row;
          m_currentCol = rect->col;
          m_currentPencilMark = rect->value;
          InvalidateRect( m_unHighlightCell );
          InvalidateRect( m_highlightedCell );
        }
      }
    }
    else
    {
      auto foundRects = m_quadtree.query( point );
      if ( !foundRects.empty() )
      {
        int cellRow = foundRects[0]->row;
        int cellCol = foundRects[0]->col;

        int left   = gridCoordinates[1 + cellCol * 2]+1;
        int top    = gridCoordinates[1 + cellRow * 2]+1;
        int right  = gridCoordinates[2 + cellCol * 2]-1;
        int bottom = gridCoordinates[2 + cellRow * 2]-1;
        CRect homeBlockRect( left, top, right, bottom );
        if ( homeBlockRect != m_highlightedCell )
        {
          //PLOGD << homeBlockRect.left << " " << homeBlockRect.right << " " << homeBlockRect.top << " " << homeBlockRect.bottom;
          m_unHighlightCell = m_highlightedCell;
          m_highlightedCell = homeBlockRect;
          m_currentRow = cellRow;
          m_currentCol = cellCol;
          m_queryPoint = point;
          InvalidateRect( m_unHighlightCell );
          InvalidateRect( m_highlightedCell );
        }
        //PLOGD << "Found cell: " << foundRects[0]->row << "," << foundRects[0]->col << "," << foundRects[0]->value;
      }
      else
        PLOGD << "No cell found";
    }
  }
    break;
  default:
    PLOGD << "Unknown button pressed";
    break;
  }

  CWnd::OnMouseMove( nFlags, point );
}

void CGridCtrl::OnPaint()
{
  CPaintDC dc( this );

  // On first paint set the small and large fonts and get the sizes
  if ( fontSml.GetSafeHandle() == nullptr )
  {
    // Get the system font
    NONCLIENTMETRICS nonClientMetrics;
    nonClientMetrics.cbSize = sizeof( NONCLIENTMETRICS );

    if ( !SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( NONCLIENTMETRICS ), &nonClientMetrics, 0 ) )
    { //We failed to get the system font so we should create something
    }
    else
    {
      // set the font to the one we got from the system
      fontSml.CreateFontIndirect( &nonClientMetrics.lfCaptionFont );
      nonClientMetrics.lfCaptionFont.lfHeight = -42;
      fontLrg.CreateFontIndirect( &nonClientMetrics.lfCaptionFont );
    }
    dc.SelectObject( &fontSml );

    // Get the text metrics
    TEXTMETRIC textMetric;
    dc.GetTextMetrics( &textMetric );

    // Define sizes and spacings
    m_pencilCellCharSize = max( (int)textMetric.tmAveCharWidth, (int)textMetric.tmHeight ); // Size of each pencil mark cell

    dc.SelectObject( &fontLrg );
    dc.GetTextMetrics( &textMetric );
  }

  // If the coordinates and Quad are not initialized, do so.
  if (m_recalculateGrid )
  {
    PreComputeCoordinates();

    m_recalculateGrid = false;
  }

  // Draw all text first
  DrawGridText( dc );
  // Draw all lines and they might overlap text
  DrawGridLines( dc );

  if ( !m_highlightedCell.IsRectEmpty() )
  {
    //PLOGD << "m_highlightCell: " << m_highlightedCell.left << "," << m_highlightedCell.top << "," << m_highlightedCell.right << "," << m_highlightedCell.bottom;
    dc.DrawFocusRect( m_highlightedCell );
  }

  // Do not call CWnd::OnPaint() for painting messages
}

void CGridCtrl::PreComputeCoordinates()
{
  // calculate the coordinates of elements of the grid
  gridCoordinates[ThickLine1] += m_labelTextAreaSize;                                   // Draw thick line
  gridCoordinates[ValueCell1] += gridCoordinates[ThickLine1] + m_thickBoarderPenSize;   // draw value cell
  gridCoordinates[ThinLine1]  += gridCoordinates[ValueCell1] + m_valueCellSize;         // draw thin line
  gridCoordinates[ValueCell2] += gridCoordinates[ThinLine1]  + m_thinBoarderPenSize;    // draw value cell
  gridCoordinates[ThinLine2]  += gridCoordinates[ValueCell2] + m_valueCellSize;         // draw thin line
  gridCoordinates[ValueCell3] += gridCoordinates[ThinLine2]  + m_thinBoarderPenSize;    // draw value cell
  gridCoordinates[ThickLine2] += gridCoordinates[ValueCell3] + m_valueCellSize;         // draw thick line
  gridCoordinates[ValueCell4] += gridCoordinates[ThickLine2] + m_thickBoarderPenSize;   // draw value cell
  gridCoordinates[ThinLine3]  += gridCoordinates[ValueCell4] + m_valueCellSize;         // draw thin line
  gridCoordinates[ValueCell5] += gridCoordinates[ThinLine3]  + m_thinBoarderPenSize;    // draw value cell
  gridCoordinates[ThinLine4]  += gridCoordinates[ValueCell5] + m_valueCellSize;         // draw thin line
  gridCoordinates[ValueCell6] += gridCoordinates[ThinLine4]  + m_thinBoarderPenSize;    // draw value cell
  gridCoordinates[ThickLine3] += gridCoordinates[ValueCell6] + m_valueCellSize;         // draw thick line
  gridCoordinates[ValueCell7] += gridCoordinates[ThickLine3] + m_thickBoarderPenSize;   // draw value cell
  gridCoordinates[ThinLine5]  += gridCoordinates[ValueCell7] + m_valueCellSize;         // draw thin line
  gridCoordinates[ValueCell8] += gridCoordinates[ThinLine5]  + m_thinBoarderPenSize;    // draw value cell
  gridCoordinates[ThinLine6]  += gridCoordinates[ValueCell8] + m_valueCellSize;         // draw thin line
  gridCoordinates[ValueCell9] += gridCoordinates[ThinLine6]  + m_thinBoarderPenSize;    // draw value cell
  gridCoordinates[ThickLine4] += gridCoordinates[ValueCell9] + m_valueCellSize;         // draw thick line

  //Place the coordinates into a quad tree for faster searching of elements.
  gridBounds = {gridCoordinates[ThickLine1], gridCoordinates[ThickLine1], gridCoordinates[ThickLine4], gridCoordinates[ThickLine4]};
  m_quadtree = Quadtree( gridBounds, 6, 4 );

  // Add the coordinate information for the pencil marks
  //PLOGD << "row,col,i,j,l,t,r,b,w,h";
  for ( int cellCoordIndexRow = ValueCell1, row = 0; cellCoordIndexRow <= ValueCell9; cellCoordIndexRow += 2, ++row )
  {
    // value cell coordinates
    for ( int cellCoordIndexCol = ValueCell1, col = 0; cellCoordIndexCol <= ValueCell9; cellCoordIndexCol += 2, ++col )
    {
      for ( int i = 0; i < 3; ++i )
      {
        for ( int j = 0; j < 3; ++j )
        {
            // l, t, r, b
          int l = gridCoordinates[cellCoordIndexCol] + j * m_pencilCellCharSize + j * m_pencilCellPadding;
          int t = gridCoordinates[cellCoordIndexRow] + i * m_pencilCellCharSize + i * m_pencilCellPadding;
          int r = gridCoordinates[cellCoordIndexCol] + j * m_pencilCellCharSize + j * m_pencilCellPadding + m_pencilCellCharSize;
          int b = gridCoordinates[cellCoordIndexRow] + i * m_pencilCellCharSize + i * m_pencilCellPadding + m_pencilCellCharSize;

          //PLOGD << row << "," << col << "," << i << "," << j << "," << l << "," << t << "," << r << "," << b << "," << r - l << "," << b - t;
          CRectInfo rect( {l, t, r, b}, row, col, i*3+j );
          //pencilMarkRects.push_back( rect );
          m_quadtree.insert( rect );
        }
      }
    }
  }
}

void CGridCtrl::DrawGridLines( CPaintDC& dc )
{
  int edgeOffset = m_labelTextAreaSize; // Offset from the edge of the window

  int lineStart = edgeOffset;
  int lineEnd = ( m_thickBoarderPenSize * 3 ) +
                ( m_valueCellSize * 10 ) + 8/*(counting from 0 adjustment)*/;

  CPen* pOldPen = dc.SelectObject( &homeCellPen );
  CFont* oldFont = dc.SelectObject( &fontSml );

  int x = gridCoordinates[ThinLine1]; // Start at the first thin line, placed before the lambda's for scope reasons

  auto drawThickLine = [&, this] ()
  {
    // Draw the thick grid lines
    dc.SelectObject( &homeCellPen );
    dc.MoveTo( x + 1, lineStart + 1 );
    dc.LineTo( x + 1, lineEnd );
    dc.MoveTo( lineStart + 1, x + 1 );
    dc.LineTo( lineEnd, x + 1 );
    x += m_thickBoarderPenSize;
  };

  auto drawPaddingLine = [&, this] ()
  {
    dc.SelectObject( &paddingPen );
    dc.MoveTo( x, lineStart );
    dc.LineTo( x, lineEnd );
    dc.MoveTo( lineStart, x );
    dc.LineTo( lineEnd, x );
    x += m_pencilCellPadding;
  };

  auto drawBlockLine = [&, this] ()
  {
    dc.SelectObject( &blockPen );
    dc.MoveTo( x, lineStart );
    dc.LineTo( x, lineEnd );
    dc.MoveTo( lineStart, x );
    dc.LineTo( lineEnd, x );
    x += m_thinBoarderPenSize;
  };

  auto drawCell = [&, this] ()
  {
    dc.SelectObject( &cellPen );
//     dc.FillSolidRect( x, y, m_pencilCellCharSize, m_pencilCellCharSize, RGB( 34, 177, 76 ) );
    x += m_pencilCellCharSize;
  };

  // Draw the thin grid lines
  drawBlockLine();
  x= gridCoordinates[ThinLine2];
  drawBlockLine();
  x = gridCoordinates[ThinLine3];
  drawBlockLine();
  x = gridCoordinates[ThinLine4];
  drawBlockLine();
  x = gridCoordinates[ThinLine5];
  drawBlockLine();
  x = gridCoordinates[ThinLine6];
  drawBlockLine();

  // Draw the thick grid lines
  x = gridCoordinates[ThickLine1];
  drawThickLine();
  x = gridCoordinates[ThickLine2];
  drawThickLine();
  x = gridCoordinates[ThickLine3];
  drawThickLine();
  x = gridCoordinates[ThickLine4];
  drawThickLine();

  // Draw the grid numbering system
  dc.SetTextColor( CLR_GRID_NUMBERS ); // Set the text color to black
  wchar_t pencilTextRow[][10] = {L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9"};
  wchar_t pencilTextCol[][10] = {L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I"};

  for ( int k = 0, y = 0; k < 3; ++k )
  {
    for ( int j = 0; j < 3; ++j )
    {
      CRect rec( y + edgeOffset + 3, edgeOffset - m_pencilCellCharSize, y + edgeOffset + m_valueCellSize + 3, edgeOffset );
      dc.DrawText( pencilTextCol[j + 3 * k], 1, &rec, DT_CENTER );
      y += m_valueCellSize + 1; // 13 is an aesthetic adjustment
    }
    y += m_thickBoarderPenSize;
  }

  for ( int k = 0, y = m_valueCellSize / 2 - 4; k < 3; ++k )
  {
    for ( int j = 0; j < 3; ++j )
    {
      dc.TextOut( edgeOffset - m_pencilCellCharSize, y + edgeOffset, pencilTextRow[j + 3 * k] );
      y += m_valueCellSize; // 13 is an aesthetic adjustment
    }
    y += m_thickBoarderPenSize + 2;
  }
  dc.SelectObject( pOldPen ); // Restore the old pen
  dc.SelectObject( oldFont ); // Restore the old font
}

void CGridCtrl::DrawGridText( CPaintDC& dc )
{
  CFont* oldFont = dc.SelectObject( &fontSml );
  CRect paintRect( dc.m_ps.rcPaint );

  //PLOGD << "Grid Draw Text";
  int boldValue = -1;
  PLOGD << VARTRACE( m_queryPoint.x ) << VARTRACE( m_queryPoint.y );

  auto pntTest = m_quadtree.query( m_queryPoint );

  for ( int i = 0; i < 9; ++i )
  {
    int y = gridCoordinates[i * 2 + 1] + m_pencilCellPadding;
    for ( int j = 0; j < 9; ++j )
    {
      int value = m_grid->getValue( i, j );
      int x = gridCoordinates[j * 2 + 1] + m_pencilCellPadding;
// Painting outside the rcPaint rect does nothing, see if we can skip the calculations when not necessary
//       if ( !paintRect.PtInRect( CPoint( x, y ) ) )
//       {
//         PLOGD << VARSTRACE(x, y) << "Rect: " << dc.m_ps.rcPaint.left << "," << dc.m_ps.rcPaint.top << "," << dc.m_ps.rcPaint.right << "," << dc.m_ps.rcPaint.bottom;
//         continue;
//       }
      int solution = m_grid->getSolution( i, j );

      //PLOGD << "pntTest size: " << pntTest.size();

      if ( pntTest.size() == 1 )
      {
        if ( pntTest[0]->row == i && pntTest[0]->col == j )
        {
          boldValue = pntTest[0]->value;
        }
      }

      if ( m_highlightedValue != 0 &&
           (m_grid->getPencilMarkValue(i,j,m_highlightedValue) ||
           m_grid->getValue(i,j) == m_highlightedValue ||
           m_grid->getSolution(i,j) == m_highlightedValue)
         )
      {
        dc.FillSolidRect( x, y, m_valueCellSize, m_valueCellSize, CLR_CELL_HIGHLIGHT );
      }
      else
        dc.SetBkColor( CLR_WHITE );

      // Draw the solution in the large font
      if ( solution != 0 )
      {
        dc.SelectObject( &fontLrg );
        dc.SetTextColor( CLR_SOLUTIONS );
        CString str;
        str.Format( L"%d", solution );
        //PLOGD << "Reg " << str;
        dc.DrawText( str, 1, CRect( x, y, x + m_valueCellSize, y + m_valueCellSize ), DT_CENTER | DT_VCENTER );
      }
      else
      {
        // Draw the value in the large font
        if ( value != 0 )
        {
          dc.SelectObject( &fontLrg );
          dc.SetTextColor( CLR_CELL_VAL );
          CString str;
          str.Format( L"%d", value );
          dc.DrawText( str, 1, CRect( x, y, x + m_valueCellSize, y + m_valueCellSize ), DT_CENTER | DT_VCENTER );
        }
        // Draw the pencil marks in the small font
        else
        {
          dc.SetTextColor( CLR_PENCILMARKS );
          for ( int k = 0; k < 9; ++k )
          {
            if (boldValue == k && m_pencilMode)
            {
              dc.SelectObject( &fontSmlBold );
              CString str;
              str.Format( L"%d", k + 1 );
              //PLOGD << "Bold " << str;
              dc.DrawText( str, 1, CRect( x + ( k % 3 ) * ( m_pencilCellCharSize + m_pencilCellPadding ), y + m_pencilCellCharSize * ( k / 3 ), x + ( k % 3 ) * ( m_pencilCellCharSize + m_pencilCellPadding ) + m_pencilCellCharSize, y + m_pencilCellCharSize * ( k / 3 ) + m_pencilCellCharSize ), DT_CENTER );
              boldValue = -1;
            }
            else if ( m_grid->getPencilMarkValue( i, j, k + 1 ) )
            {
              dc.SelectObject( &fontSml );
              CString str;
              str.Format( L"%d", k + 1 );
              //PLOGD << "Norm " << str;
              dc.DrawText( str, 1, CRect( x + ( k % 3 ) * ( m_pencilCellCharSize + m_pencilCellPadding ), y + m_pencilCellCharSize * ( k / 3 ), x + ( k % 3 ) * ( m_pencilCellCharSize + m_pencilCellPadding ) + m_pencilCellCharSize, y + m_pencilCellCharSize * ( k / 3 ) + m_pencilCellCharSize ), DT_CENTER );
            }
          }
        }
      }
    }
  }

  dc.SelectObject( oldFont ); // Restore the old font
}

void CGridCtrl::PreSubclassWindow()
{
  // TODO: Add your specialized code here and/or call the base class

  CWnd::PreSubclassWindow();
}

BOOL CGridCtrl::Create( CWnd* pParentWnd, RECT& rect, UINT nId, DWORD dwStyle )
{
  if (pParentWnd == NULL)
  {
    pParentWnd = AfxGetMainWnd();
  }

  // calculate the window size, rect will have the top left corner and the bottom right corner will be calculated
  CDC memoryDC;
  memoryDC.CreateCompatibleDC( pParentWnd->GetDC() );

  // Get the system font
  NONCLIENTMETRICS nonClientMetrics;
  nonClientMetrics.cbSize = sizeof( NONCLIENTMETRICS );

  if ( !SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( NONCLIENTMETRICS ), &nonClientMetrics, 0 ) )
  { //We failed to get the system font so we should create something
  }
  else
  {
    // set the font to the one we got from the system
    fontSml.CreateFontIndirect( &nonClientMetrics.lfCaptionFont );
    nonClientMetrics.lfCaptionFont.lfWeight = FW_BOLD;
    fontSmlBold.CreateFontIndirect( &nonClientMetrics.lfCaptionFont );
    nonClientMetrics.lfCaptionFont.lfHeight = 48;
    nonClientMetrics.lfCaptionFont.lfWeight = FW_NORMAL;
    fontLrg.CreateFontIndirect( &nonClientMetrics.lfCaptionFont );
    nonClientMetrics.lfCaptionFont.lfWeight = FW_BOLD;
    fontLrgBold.CreateFontIndirect( &nonClientMetrics.lfCaptionFont );

    //Test all fonts for success
    if ( fontSml.GetSafeHandle() == nullptr ||
         fontSmlBold.GetSafeHandle() == nullptr ||
         fontLrg.GetSafeHandle() == nullptr ||
         fontLrgBold.GetSafeHandle() == nullptr )
    {
      //Failure to create the fonts is pretty bad.
      return FALSE;
    }

  }
  memoryDC.SelectObject( &fontSml );

  // Get the text metrics
  TEXTMETRIC textMetric;
  memoryDC.GetTextMetrics( &textMetric );

  // Define sizes and spacings
  m_pencilCellCharSize = max( (int)textMetric.tmAveCharWidth, (int)textMetric.tmHeight ); // Size of each pencil mark cell

  // Layout details
  //          Column Labels--->
  //
  // Row      Thick boarder valueCell Thin boarder valueCell Thin boarder valueCell Thick boarder ... repeat 2 more times
  // Labels
  // -
  // -
  // V

  m_valueCellSize = 3 * m_pencilCellCharSize + 4 * m_pencilCellPadding; // Size of each block (3x3 grid of pencil cells)

  int dim = 4 * m_thickBoarderPenSize + 9 * m_valueCellSize + 6 * m_thinBoarderPenSize + m_labelTextAreaSize;

  rect.right = rect.left + dim;
  rect.bottom = rect.top + dim;

  // Create pens for drawing
  LOGBRUSH brush1;
  brush1.lbColor = CLR_BLUE;
  brush1.lbStyle = BS_SOLID;
  homeCellPen.CreatePen( PS_SOLID | PS_ENDCAP_SQUARE | PS_GEOMETRIC, m_thickBoarderPenSize, &brush1 ); // Blue pen, thick, for home cell borders
  brush1.lbColor = CLR_RED;
  blockPen.CreatePen( PS_SOLID | PS_ENDCAP_SQUARE | PS_GEOMETRIC, m_thinBoarderPenSize, &brush1 ); // Red pen, thin, for block borders
  brush1.lbColor = CLR_CELL_HIGHLIGHT;
  paddingPen.CreatePen( PS_SOLID | PS_ENDCAP_SQUARE | PS_GEOMETRIC, m_thinBoarderPenSize, &brush1 ); // Yellow pen, thin, for block borders
  brush1.lbColor = CLR_LGT_GREEN;
  cellPen.CreatePen( PS_SOLID | PS_ENDCAP_SQUARE | PS_GEOMETRIC, m_valueCellSize, &brush1 ); // Green pen, thin, for block borders


  return CWnd::Create( SUDOKU_GRID_CLASS_NAME, _T(""), dwStyle, rect, pParentWnd, nId);
}

void CGridCtrl::TogglePencilMode()
{
  m_pencilMode = !m_pencilMode;
  Invalidate();
}

void CGridCtrl::OnMouseLeave()
{
  m_mouseInGrid = FALSE;
  InvalidateRect( m_highlightedCell );
  InvalidateRect( m_unHighlightCell );
  m_unHighlightCell = {0, 0, 0, 0};
  m_highlightedCell = {0, 0, 0, 0};
  m_queryPoint = CPoint( -1, -1 );
  m_currentCol = -1;
  m_currentRow = -1;
  m_currentPencilMark = -1;
  Invalidate( FALSE );
  CWnd::OnMouseLeave();
}

void CGridCtrl::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
  PLOGD << "Key pressed: " << nChar;
  bool ctrlDown = ( GetKeyState( VK_CONTROL ) & 0x8000 ) != 0;
  if ( ctrlDown && nChar >= VK_NUMPAD1 && nChar <= VK_NUMPAD9)
  {
    m_highlightedValue = nChar - VK_NUMPAD1 + 1;
    Invalidate();
  }
  else
  {
    switch ( nChar )
    {
    case VK_NUMPAD0:
      // zero out the value currently highlighted
      if ( m_pencilMode )
      {
      }
      else
      {
        m_grid->setValue( m_currentRow, m_currentCol, 0 );
        m_grid->resetPencilMarks( m_currentRow, m_currentCol );
        Invalidate();
      }
      break;
    }
  }
  CWnd::OnKeyDown( nChar, nRepCnt, nFlags );
}

void CGridCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
  TogglePencilMode();
  InvalidateRect( m_unHighlightCell );
  InvalidateRect( m_highlightedCell );
  m_highlightedCell = {0,0,0,0};
  m_unHighlightCell = {0,0,0,0};
  CWnd::OnRButtonUp( nFlags, point );
}

void CGridCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
  // Can the mouse be outside the window when we click?
  if ( !m_mouseInGrid )
  {
    PLOGD << "Mouse is outside the grid, ignoring click";
    return;
  }
  // If we are in pencil mode, we need to add the pencil mark to the cell
  if ( m_pencilMode )
  {
    // find the cell that we are in
    auto foundRects = m_quadtree.query( point );
    if ( foundRects.size() == 1 )
    {
      //PLOGD << "Found cell: " << foundRects[0]->row << "," << foundRects[0]->col << "," << foundRects[0]->value;
      m_grid->togglePencilMarkValue( foundRects[0]->row, foundRects[0]->col, foundRects[0]->value );
      //InvalidateRect( *foundRects[0] );
      Invalidate();
    }
  }
  else
  {
    // find the cell that we are in
    auto foundRects = m_quadtree.query( point );
    if ( foundRects.size() == 1 )
    {
      //PLOGD << "Found cell: " << foundRects[0]->row << "," << foundRects[0]->col << "," << foundRects[0]->value;
      m_grid->setValue( foundRects[0]->row, foundRects[0]->col, foundRects[0]->value + 1 );
      //InvalidateRect( *foundRects[0] );
      Invalidate();
    }
  }
  CWnd::OnLButtonUp( nFlags, point );
}
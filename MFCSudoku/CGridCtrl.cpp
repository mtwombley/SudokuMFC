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


// CGridCtrl message handlers
BEGIN_MESSAGE_MAP(CGridCtrl, CWnd)
  ON_WM_CHANGEUISTATE()
  ON_WM_ERASEBKGND()
  ON_WM_MOUSEMOVE()
  ON_WM_PAINT()
  ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

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
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof( tme );
    tme.dwFlags = TME_LEAVE/* | TME_HOVER*/;
    tme.hwndTrack = GetSafeHwnd();
    m_mouseInGrid = TrackMouseEvent( &tme );
    if (!m_mouseInGrid )
    {
      auto err = GetLastError();
      PLOGD << "TrackMouseEvent failed";
    }
  }

  switch ( nFlags )
  {
  case MK_LBUTTON:
    PLOGD << "Left button pressed";
    break;
  case MK_RBUTTON:
    PLOGD << "Right button pressed";
    break;
  case MK_MBUTTON:
    PLOGD << "Middle button pressed";
    break;
  case MK_CONTROL:
    PLOGD << "Control button pressed";
    break;
  case MK_SHIFT:
    PLOGD << "Shift button pressed";
    break;
  case MK_XBUTTON1:
    PLOGD << "X1 button pressed";
    break;
  case MK_XBUTTON2:
    PLOGD << "X2 button pressed";
    break;
  case 0:
  {
    // if we are in pencil mode, draw a rect around the pencil mark cell
    // find the cell that we are in
    auto foundRects = m_quadtree.query( point );
    for ( auto rect : foundRects )
    {
      if (m_highlightedCell != *rect/* && m_unHighlightCell != *rect */)
      {
        // unhighlight the cell
        m_unHighlightCell = m_highlightedCell;
        m_highlightedCell = *rect;
        InvalidateRect( m_unHighlightCell );
        InvalidateRect( m_highlightedCell );
      }
    }
  }
    break;
  default:
    PLOGD << "Unknown button pressed";
    break;
  }

  PLOGD << VARSTRACE( point.x, point.y );

  CWnd::OnMouseMove( nFlags, point );
}

void CGridCtrl::OnPaint()
{
  CPaintDC dc( this );

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

  if (m_recalculateGrid )
  {
    PreComputeCoordinates();

    m_recalculateGrid = false;
  }

  DrawGridText( dc );
  DrawGridLines( dc );

  if ( !m_highlightedCell.IsRectEmpty() )
  {
    dc.DrawFocusRect( m_highlightedCell );
  }

  // Do not call CWnd::OnPaint() for painting messages
}

void CGridCtrl::PreComputeCoordinates()
{
  gridCoordinates[0] += m_labelTextAreaSize;                          // Draw thick line
  gridCoordinates[1] += m_thickBoarderPenSize + gridCoordinates[0];   // draw value cell
  gridCoordinates[2] += m_valueCellSize + gridCoordinates[1];         // draw thin line
  gridCoordinates[3] += m_thinBoarderPenSize + gridCoordinates[2];    // draw value cell
  gridCoordinates[4] += m_valueCellSize + gridCoordinates[3];         // draw thin line
  gridCoordinates[5] += m_thinBoarderPenSize + gridCoordinates[4];    // draw value cell
  gridCoordinates[6] += m_valueCellSize + gridCoordinates[5];         // draw thick line
  gridCoordinates[7] += m_thickBoarderPenSize + gridCoordinates[6];   // draw value cell
  gridCoordinates[8] += m_valueCellSize + gridCoordinates[7];         // draw thin line
  gridCoordinates[9] += m_thinBoarderPenSize + gridCoordinates[8];    // draw value cell
  gridCoordinates[10] += m_valueCellSize + gridCoordinates[9];         // draw thin line
  gridCoordinates[11] += m_thinBoarderPenSize + gridCoordinates[10];   // draw value cell
  gridCoordinates[12] += m_valueCellSize + gridCoordinates[11];        // draw thick line
  gridCoordinates[13] += m_thickBoarderPenSize + gridCoordinates[12];  // draw value cell
  gridCoordinates[14] += m_valueCellSize + gridCoordinates[13];        // draw thin line
  gridCoordinates[15] += m_thinBoarderPenSize + gridCoordinates[14];   // draw value cell
  gridCoordinates[16] += m_valueCellSize + gridCoordinates[15];        // draw thin line
  gridCoordinates[17] += m_thinBoarderPenSize + gridCoordinates[16];   // draw value cell
  gridCoordinates[18] += m_valueCellSize + gridCoordinates[17];        // draw thick line

  gridBounds = {gridCoordinates[0], gridCoordinates[0], gridCoordinates[18], gridCoordinates[18]};
  m_quadtree = Quadtree( gridBounds, 6, 4 );

  //PLOGD << "row,col,i,j,l,t,r,b,w,h";
  for ( int cellCoordIndexRow = 1, row = 0; cellCoordIndexRow <= 17; cellCoordIndexRow += 2, ++row )
  {
    // value cell coordinates
    for ( int cellCoordIndexCol = 1, col = 0; cellCoordIndexCol <= 17; cellCoordIndexCol += 2, ++col )
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

  //PLOGD << VARSTRACE( thickBoarderPenSize, thinBoarderPenSize, m_pencilCellCharSize, valueCellSize, homeColX, homeColY, lineStart, lineEnd, edgeOffset );

  // Create pens for drawing
  LOGBRUSH brush1;
  brush1.lbColor = RGB( 0, 0, 255 );
  brush1.lbStyle = BS_SOLID;
  CPen homeCellPen( PS_SOLID | PS_ENDCAP_SQUARE | PS_GEOMETRIC, m_thickBoarderPenSize, &brush1 ); // Blue pen, thick, for home cell borders
  brush1.lbColor = RGB( 255, 0, 0 );
  CPen blockPen( PS_SOLID | PS_ENDCAP_SQUARE | PS_GEOMETRIC, m_thinBoarderPenSize, &brush1 ); // Red pen, thin, for block borders
  brush1.lbColor = RGB( 255, 242, 0 );
  CPen paddingPen( PS_SOLID | PS_ENDCAP_SQUARE | PS_GEOMETRIC, m_thinBoarderPenSize, &brush1 ); // Yellow pen, thin, for block borders
  brush1.lbColor = RGB( 34, 177, 76 );
  CPen cellPen( PS_SOLID | PS_ENDCAP_SQUARE | PS_GEOMETRIC, m_valueCellSize, &brush1 ); // Green pen, thin, for block borders

  CPen* pOldPen = dc.SelectObject( &homeCellPen );
  CFont* oldFont = dc.SelectObject( &fontSml );
  PLOGD << "Grid Draw";

   int x = gridCoordinates[2];
//   int y = edgeOffset + m_thickBoarderPenSize + m_thinBoarderPenSize;

  auto drawThickLine = [&, this] ()
  {
    // Draw the thick grid lines
    dc.SelectObject( &homeCellPen );
    dc.MoveTo( x + 1, lineStart + 1 );
    dc.LineTo( x + 1, lineEnd );
    dc.MoveTo( lineStart + 1, x + 1 );
    dc.LineTo( lineEnd, x + 1 );
    {
      int x1 = x + 1;
      int y1 = lineStart + 1;
      int y2 = lineEnd;
      PLOGD << VARSTRACE( x1, y1, y2 );
    }
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
    {
      int x1 = x;
      int y1 = lineStart;
      int y2 = lineEnd;
      PLOGD << VARSTRACE( x1, y1, y2 );
    }
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
  x= gridCoordinates[4];
  drawBlockLine();
  x = gridCoordinates[8];
  drawBlockLine();
  x = gridCoordinates[10];
  drawBlockLine();
  x = gridCoordinates[14];
  drawBlockLine();
  x = gridCoordinates[16];
  drawBlockLine();

  // Draw the thick grid lines
  x = gridCoordinates[0];
  drawThickLine();
  x = gridCoordinates[6];
  drawThickLine();
  x = gridCoordinates[12];
  drawThickLine();
  x = gridCoordinates[18];
  drawThickLine();

  // Draw the grid numbering system
  dc.SetTextColor( RGB( 0, 0, 0 ) ); // Set the text color to black
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
//   // Create pens for drawing
//   CPen homeCellPen( PS_SOLID, m_thickBoarderPenSize, RGB( 0, 0, 255 ) ); // Blue pen, 2 pixels thick, for home cell borders
//   CPen blockPen( PS_SOLID, m_thinBoarderPenSize, RGB( 255, 0, 0 ) ); // Red pen, 1 pixel thick, for block borders
//
//   CPen* pOldPen = dc.SelectObject( &homeCellPen );
  CFont* oldFont = dc.SelectObject( &fontSml );
  CRect paintRect( dc.m_ps.rcPaint );

  PLOGD << "Grid Draw Text";
  int boldValue = -1;
  auto pntTest = m_quadtree.query( CPoint( dc.m_ps.rcPaint.left, dc.m_ps.rcPaint.top ) );

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
      if ( pntTest.size() == 1 )
      {
        if ( pntTest[0]->row == i && pntTest[0]->col == j )
        {
          boldValue = pntTest[0]->value;
        }
      }

      if ( solution != 0 )
      { // Draw the solution in the large font
        dc.SelectObject( &fontLrg );
        dc.SetTextColor( RGB( 255, 0, 0 ) );
        CString str;
        str.Format( L"%d", solution );
        dc.DrawText( str, 1, CRect( x, y, x + m_valueCellSize, y + m_valueCellSize ), DT_CENTER | DT_VCENTER );
      }
      else
      {
        if ( value != 0 )
        { // Draw the value in the large font
          dc.SelectObject( &fontLrg );
          dc.SetTextColor( RGB( 255, 255, 0 ) );
          CString str;
          str.Format( L"%d", value );
          dc.DrawText( str, 1, CRect( x, y, x + m_valueCellSize, y + m_valueCellSize ), DT_CENTER | DT_VCENTER );
        }
        else
        { // Draw the pencil marks in the small font
          dc.SetTextColor( RGB( 0, 0, 0 ) );
          for ( int k = 0; k < 9; ++k )
          {
            if (boldValue == k)
            {
              dc.SelectObject( &fontSmlBold );
              CString str;
              str.Format( L"%d", k + 1 );
              dc.DrawText( str, 1, CRect( x + ( k % 3 ) * ( m_pencilCellCharSize + m_pencilCellPadding ), y + m_pencilCellCharSize * ( k / 3 ), x + ( k % 3 ) * ( m_pencilCellCharSize + m_pencilCellPadding ) + m_pencilCellCharSize, y + m_pencilCellCharSize * ( k / 3 ) + m_pencilCellCharSize ), DT_CENTER );
              boldValue = -1;
            }
            else if ( m_grid->getPencilMarkValue( i, j, k + 1 ) )
            {
              dc.SelectObject( &fontSml );
              CString str;
              str.Format( L"%d", k + 1 );
              dc.DrawText( str, 1, CRect( x + ( k % 3 ) * ( m_pencilCellCharSize + m_pencilCellPadding ), y + m_pencilCellCharSize * ( k / 3 ), x + ( k % 3 ) * ( m_pencilCellCharSize + m_pencilCellPadding ) + m_pencilCellCharSize, y + m_pencilCellCharSize * ( k / 3 ) + m_pencilCellCharSize ), DT_CENTER );
            }
          }
        }
      }
    }
  }

//   dc.SelectObject( pOldPen ); // Restore the old pen
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
  m_unHighlightCell = {0, 0, 0, 0};
  CWnd::OnMouseLeave();
  InvalidateRect( m_highlightedCell );
  m_highlightedCell = {0, 0, 0, 0};
}

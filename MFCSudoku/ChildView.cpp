
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "MFCSudoku.h"
#include "ChildView.h"
#include "SudokuGrid.h"
#include "ExactCover.h"

// A local instance of the SudokuGrid class
//   Why is this not a member of the CChildView class?
//     Making it a member means the SudokuGrid class would need to be in the header file which increases the scope of the SudokuGrid class
//     Doing it this way keeps the SudokuGrid class private to the CChildView class because only it needs to know about it.

namespace
{
  PLOGInitializer plogInitializer;

  SudokuGrid grid;
//  ExactCover ec;

}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CChildView::CChildView()
{
  //PLOGD << "CChildView";
}

CChildView::~CChildView()
{}


// CChildView message handlers
BEGIN_MESSAGE_MAP( CChildView, CWnd )
  ON_WM_PAINT()
  ON_WM_CHANGEUISTATE()
  ON_COMMAND( ID_FILE_NEWRANDOM, &CChildView::OnFileNewrandom )
  ON_WM_CREATE()
  ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


BOOL CChildView::PreCreateWindow( CREATESTRUCT& cs )
{

  //PLOGD << "PreCreateWindow";

  if ( !CWnd::PreCreateWindow( cs ) )
    return FALSE;

  cs.dwExStyle |= WS_EX_CLIENTEDGE;
  cs.style &= ~WS_BORDER;
  cs.lpszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
                                      ::LoadCursor( nullptr, IDC_ARROW ), reinterpret_cast<HBRUSH>( COLOR_WINDOW + 1 ), nullptr );

  return TRUE;
}

void CChildView::OnPaint()
{
  CPaintDC dc( this ); // device context for painting

  // Do not call CWnd::OnPaint() for painting messages
}

void CChildView::OnChangeUIState( UINT nAction, UINT nUIElement )
{
  // This feature requires Windows 2000 or greater.
  // The symbols _WIN32_WINNT and WINVER must be >= 0x0500.
  CWnd::OnChangeUIState( nAction, nUIElement );

  // TODO: Add your message handler code here
}

void CChildView::OnFileNewrandom()
{
  grid.GenerateRandomGrid();
}

int CChildView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
  if ( CWnd::OnCreate( lpCreateStruct ) == -1 )
    return -1;

  CRect rect( 0, 0, 400, 400 );
  if ( m_gridCtrl.Create( this, rect, 1, WS_CHILD | WS_VISIBLE | WS_TABSTOP ) == -1 )
    return -1;

  m_gridCtrl.AssignGrid( grid );

  return 0;
}


void CChildView::OnRButtonUp( UINT nFlags, CPoint point )
{
  // Toggle pencil modes
  ((CGridCtrl*)GetDlgItem(1))->TogglePencilMode();
  CWnd::OnRButtonUp( nFlags, point );
}

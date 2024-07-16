
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "MFCSudoku.h"
#include "ChildView.h"
#include "SudokuGrid.h"

#define PLOG_CAPTURE_FILE
#define TRACEVARS
#include <plog/Log.h>
#include <plog/Init.h>
#include <plog/Formatters/CoutFormatter.h>
#include <plog/Appenders/DebugOutputAppender.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static plog::DebugOutputAppender<plog::COutFormatter>  debugOutputAppender;

// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
  ON_WM_PAINT()
  ON_WM_CHANGEUISTATE()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
  plog::init(plog::debug, &debugOutputAppender);
  LOGD << "PreCreateWindow";

  if (!CWnd::PreCreateWindow(cs))
    return FALSE;

  cs.dwExStyle |= WS_EX_CLIENTEDGE;
  cs.style &= ~WS_BORDER;
  cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
    ::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

  return TRUE;
}

void CChildView::OnPaint()
{
  CPaintDC dc(this); // device context for painting

  // Get the system font
  NONCLIENTMETRICS nonClientMetrics;
  nonClientMetrics.cbSize = sizeof(NONCLIENTMETRICS);

  CFont font;
  if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &nonClientMetrics, 0))
  { //We failed to get the system font so we should create something
  }
  else
  {
    // set the font to the one we got from the system
    font.CreateFontIndirect(&nonClientMetrics.lfCaptionFont);
  }
  dc.SelectObject(&font);

  // Get the text metrics
  TEXTMETRIC textMetric;
  dc.GetTextMetrics(&textMetric);

  // Define sizes and spacings
  int homeCellPenSize = 3; // Size of the home cell border pen
  int blockPenSize    = 1; // Size of the block border pen
  int pencilCellSize  = max((int)textMetric.tmAveCharWidth, (int)textMetric.tmHeight) + 1; // Size of each pencil mark cell
  int blockSize       = 3 * pencilCellSize; // Size of each block (3x3 grid of pencil cells)
  int homeColX        = pencilCellSize + 1;
  int homeColY        = pencilCellSize + 1;
  int lineStart       = pencilCellSize + 1;
  int lineEnd         = pencilCellSize * 27 + homeCellPenSize * 3 + blockPenSize * 6 + 1/*(counting from 0 adjustment)*/;
  int edgeOffset      = 50;

  // Create pens for drawing
  CPen homeCellPen(PS_SOLID, homeCellPenSize, RGB(0, 0, 255)); // Blue pen, 2 pixels thick, for home cell borders
  CPen blockPen(PS_SOLID, blockPenSize, RGB(255, 0, 0)); // Red pen, 1 pixel thick, for block borders

  CPen* pOldPen = dc.SelectObject(&homeCellPen);
  PLOGD << "Grid Draw";
  for (int i = 0; i <= 3; ++i)
  {
    PLOGD << VARSTRACE(homeColX, homeColY);
    // Draw vertical lines
    dc.MoveTo(homeColX + edgeOffset, lineStart + edgeOffset);
    dc.LineTo(homeColX + edgeOffset, lineEnd+ edgeOffset);
    homeColX += blockSize;

    // Draw horizontal lines
    dc.MoveTo(lineStart + edgeOffset, homeColY + edgeOffset);
    dc.LineTo(lineEnd + edgeOffset, homeColY + edgeOffset);
    homeColY += blockSize;

    if (i > 2) // don't draw the last block lines
      continue;

    dc.SelectObject(&blockPen);
    for (int j = 1; j < 3; ++j)
    {
      PLOGD << VARSTRACE(homeColX, homeColY);
      // Draw vertical lines
      dc.MoveTo(homeColX + edgeOffset, lineStart + 2 + edgeOffset);
      dc.LineTo(homeColX + edgeOffset, lineEnd + edgeOffset);
      homeColX += blockSize;

      // Draw horizontal lines
      dc.MoveTo(lineStart + 2 + edgeOffset, homeColY + edgeOffset);
      dc.LineTo(lineEnd + edgeOffset, homeColY + edgeOffset);
      homeColY += blockSize;
    }

    dc.SelectObject(&homeCellPen);
  }

  dc.SelectObject(pOldPen); // Restore the old pen

  // Draw the grid numbering system
  dc.SetTextColor(RGB(0, 0, 0)); // Set the text color to black
  //dc.SetBkColor(RGB(0, 255, 0));
  wchar_t pencilTextRow[][10] = { L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9" };
  wchar_t pencilTextCol[][10] = { L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I" };

  for (int i = 1,k=0; i <= 9; ++i, ++k)
  {
    int x = i * blockSize - 11; // 11 is an aesthetic adjustment
    dc.TextOut(x + edgeOffset, edgeOffset, pencilTextCol[k]);
  }
  for (int j = 1; j <= 9; ++j)
  {
    int y = j * blockSize - 13; // 13 is an aesthetic adjustment
    dc.TextOut(edgeOffset, y + edgeOffset, pencilTextRow[j-1]);
  }
  // Do not call CWnd::OnPaint() for painting messages
}

void CChildView::OnChangeUIState(UINT nAction, UINT nUIElement)
{
  // This feature requires Windows 2000 or greater.
  // The symbols _WIN32_WINNT and WINVER must be >= 0x0500.
  CWnd::OnChangeUIState(nAction, nUIElement);

  // TODO: Add your message handler code here
}

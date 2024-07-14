
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

// void CChildView::OnPaint()
// {
// 	CPaintDC dc(this); // device context for painting
//
// 	// Get the system font
// 	NONCLIENTMETRICS nonClientMetrics;
// 	nonClientMetrics.cbSize = sizeof(NONCLIENTMETRICS);
//
// 	CFont font;
// 	if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &nonClientMetrics, 0))
// 	{ //We failed to get the system font so we should create something
// 	}
// 	else
// 	{
// 		// set the font to the one we got from the system
// 		font.CreateFontIndirect(&nonClientMetrics.lfCaptionFont);
// 	}
// 	dc.SelectObject(&font);
//
// 	// Get the text metrics
// 	TEXTMETRIC textMetric;
// 	dc.GetTextMetrics(&textMetric);
//
// 	// Use the max width or height of the font to calculate the pencil cell size
// // 	int pencilCellSize = max((int)textMetric.tmAveCharWidth, (int)textMetric.tmHeight);
// //
// // 	// Calculate the size of a cell
// // 	int cellSize = 3 * pencilCellSize;
// //
// // 	// Calculate the size of a home cell
// // 	int homeCellSize = 3 * cellSize;
// //
// // 	// Calculate the size of the grid
// // 	int gridSize = 3 * homeCellSize;
//
// 	// Change the pen width to 1 pixel with a color of black
// // 	CPen pen2(PS_SOLID, 1, RGB(0, 0, 0));
// // 	dc.SelectObject(&pen2);
//
// 	// Draw the grid of cells (3x3)
// // 	for (int i = 0; i < 9; i++)
// // 	{
// // 		for (int j = 0; j < 9; j++)
// // 		{
// // 			dc.Rectangle(i * cellSize, j * cellSize, (i + 1) * cellSize, (j + 1) * cellSize);
// // 		}
// // 	}
//
// 	// Change the pen width to 2 pixels
// // 	CPen pen(PS_SOLID, 2, RGB(255, 0, 0));
// // 	dc.SelectObject(&pen);
//
// 	// Draw the grid of home cells (3x3)
// // 	for (int i = 0; i < 3; i++)
// // 	{
// // 		for (int j = 0; j < 3; j++)
// // 		{
// // 			dc.Rectangle(i * homeCellSize, j * homeCellSize, (i + 1) * homeCellSize, (j + 1) * homeCellSize);
// // 		}
// // 	}
//
// // 	wchar_t pencilText[][10] = { L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9" };
// //
// // 	int paddingX = 0;
// // 	// Draw the text 1-9 in each pencil cell
// // 	for (int i = 0; i < 9; i++) // Column
// // 	{
// // 		int paddingY = 0;
// // 		// At every 3rd column, add padding of pencilCellSize
// // 		if (i % 3 == 0)
// // 		{
// // 			paddingX += pencilCellSize;
// // 		}
// // 		for (int j = 0; j < 9; j++) // Row
// // 		{
// // 			// At every 3rd row, add padding of pencilCellSize
// // 			if (j % 3 == 0)
// // 			{
// // 				paddingY += pencilCellSize;
// // 			}
// // 			// Draw the 3x3 pencil cells
// // 			for (int k = 0; k < 9; k++)
// // 			{
// // 				// Calculate the position of the text
// // 				int x = i * cellSize + (k % 3) * pencilCellSize;
// // 				int y = j * cellSize + (k / 3) * pencilCellSize;
// // 				dc.TextOut(x + paddingX, y + paddingY, pencilText[k]);
// //
// // 			}
// // 		}
// // 	}
// //
//
// 	// I want to draw a grid of 3x3 cells where each cell is 3x3 pencil cells, draw a line every 3 pencil cells
// 	// I want to draw a grid of 3x3 home cells where each cell is 3x3 cells, draw a line every 3 cells
//
//
//
// 	// 	RECT rc;
// 	// 	GetClientRect(&rc);
// 	// 	dc.SetTextColor(RGB(0, 0, 0));
// 	// 	dc.DrawText(L"Hello World!", &rc, DT_CENTER);
//
// 	// Setup for drawing the grid
// 	int cellSize = max((int)textMetric.tmAveCharWidth, (int)textMetric.tmHeight) + 2; // Size of each pencil cell
// 	int gridSize = 3 * cellSize; // Size of each 3x3 cell
// 	int totalSize = 3 * gridSize; // Total size of the 9x9 grid
//
// 	// Create a pen for drawing the grid lines
// // 	CPen gridPen(PS_SOLID, 1, RGB(0, 0, 0)); // Black pen, 1 pixel thick
// //	CPen* pOldPen = dc.SelectObject(&gridPen);
//
// 	// Draw the 9x9 grid
// // 	for (int i = 0; i <= 9; ++i)
// // 	{
// // 		// Draw vertical lines
// // 		dc.MoveTo(i * cellSize, 0);
// // 		dc.LineTo(i * cellSize, totalSize);
// //
// // 		// Draw horizontal lines
// // 		dc.MoveTo(0, i * cellSize);
// // 		dc.LineTo(totalSize, i * cellSize);
// // 	}
//
// 	// Create a pen for drawing the bold lines every 3 cells
// 	CPen boldPen(PS_SOLID, 3, RGB(0, 0, 0)); // Black pen, 3 pixels thick
// 	CPen* pOldPen = dc.SelectObject(&boldPen);
//
// 	// Draw the bold lines to separate 3x3 cells
// 	for (int i = 0; i <= 3; ++i)
// 	{
// 		// Draw vertical bold lines
// 		dc.MoveTo(i * gridSize, 0);
// 		dc.LineTo(i * gridSize, totalSize);
//
// 		// Draw horizontal bold lines
// 		dc.MoveTo(0, i * gridSize);
// 		dc.LineTo(totalSize, i * gridSize);
// 	}
//
// 	// Restore the original pen
// 	dc.SelectObject(pOldPen);
//
// 		// Do not call CWnd::OnPaint() for painting messages
// }


void CChildView::OnPaint()
{
  CPaintDC dc(this); // device context for painting

  SudokuGrid sudokuGrid;
  sudokuGrid.setValue(0, 0, 1);

  // Define sizes and spacings
  int pencilCellSize = 15; // Size of each pencil cell
  int blockSpacing = 0; // Spacing between blocks
  int homeCellSpacing = 5; // Spacing between home cells
  int blockSize = 3 * pencilCellSize + 2 * blockSpacing; // Size of each block (3x3 grid of pencil cells)
  int homeCellSize = 3 * blockSize + 2 * homeCellSpacing; // Size of each home cell (3x3 grid of blocks)

  // Offset for the top and left side
  int offset = pencilCellSize;

  // Create pens for drawing
  CPen homeCellPen(PS_SOLID, 2, RGB(0, 0, 255)); // Blue pen, 2 pixels thick, for home cell borders
  CPen blockPen(PS_SOLID, 1, RGB(255, 0, 0)); // Red pen, 1 pixel thick, for block borders

  // first line is
  //x=15, 165, 315
  // y =15 - 315

  CPen* pOldPen = dc.SelectObject(&homeCellPen);
  // Draw the 3x3 grid of home cells with offset
  for (int homeRow = 0; homeRow < 3; ++homeRow)
  {
    dc.SelectObject(&homeCellPen);
    dc.MoveTo(15, homeRow * (homeCellSize + homeCellSpacing));
    dc.LineTo(420, homeRow * (homeCellSize + homeCellSpacing));

    dc.SelectObject(&blockPen);
    for (int blockRow = 0; blockRow < 3; ++blockRow)
    {
      dc.MoveTo(30, blockRow * (blockSize + blockSpacing));
      dc.LineTo(420, blockRow * (blockSize + blockSpacing));
    }
  }
  for (int homeCol = 0; homeCol < 3; ++homeCol)
  {
    dc.SelectObject(&homeCellPen);
    dc.MoveTo(homeCol * (homeCellSize + homeCellSpacing), 15);
    dc.LineTo(homeCol * (homeCellSize + homeCellSpacing), 420);

    dc.SelectObject(&blockPen);
    for (int blockCol = 0; blockCol < 3; ++blockCol)
    {
      dc.MoveTo(blockCol * (blockSize + blockSpacing), 30);
      dc.LineTo(blockCol * (blockSize + blockSpacing), 420);
    }
  }
  dc.SelectObject(pOldPen); // Restore the old pen


// Draw the 3x3 grid of blocks within the current home cell
//           // Calculate top-left corner of the current block with offset
//           int blockX = offset + homeCellX + blockCol * (blockSize + blockSpacing);
//           int blockY = offset + homeCellY + blockRow * (blockSize + blockSpacing);
//
//           sudokuGrid.getRect(homeRow * 3 + blockRow, homeCol * 3 + blockCol) = CRect(blockX, blockY, blockX + blockSize, blockY + blockSize);
//           // Draw border around the block
//           dc.Rectangle(blockX, blockY, blockX + blockSize, blockY + blockSize);
//         }
//       }

  // Do not call CWnd::OnPaint() for painting messages
}

void CChildView::OnChangeUIState(UINT nAction, UINT nUIElement)
{
  // This feature requires Windows 2000 or greater.
  // The symbols _WIN32_WINNT and WINVER must be >= 0x0500.
  CWnd::OnChangeUIState(nAction, nUIElement);

  // TODO: Add your message handler code here
}

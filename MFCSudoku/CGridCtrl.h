#pragma once
#include "SudokuGrid.h"
#include "QuadtreeNode.h"

// CGridCtrl

class CGridCtrl : public CWnd
{
	DECLARE_DYNAMIC(CGridCtrl)

public:
	CGridCtrl();
	virtual ~CGridCtrl();

  void AssignGrid( SudokuGrid& grid )
  {
    m_grid = &grid;
  };

protected:
	DECLARE_MESSAGE_MAP()
  BOOL RegisterWindowClass();
  afx_msg void OnChangeUIState( UINT nAction, UINT nUIElement );
  afx_msg BOOL OnEraseBkgnd( CDC* pDC );
  afx_msg void OnMouseMove( UINT nFlags, CPoint point );
  afx_msg void OnPaint();
  afx_msg void OnMouseLeave();
  afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
  afx_msg void OnRButtonUp( UINT nFlags, CPoint point );
  afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
  void PreComputeCoordinates();
  void PreSubclassWindow() override;
  void DrawGridLines( CPaintDC& dc );
  void DrawGridText( CPaintDC& dc );

  int m_labelTextAreaSize = 25; // Size of the area for the label
  int m_thickBoarderPenSize = 3; // Size of the home cell border pen
  int m_thinBoarderPenSize = 1;    // Size of the block border pen
  int m_pencilCellCharSize = 15; // Size of each pencil mark cell
  int m_pencilCellPadding = m_thinBoarderPenSize; // Padding between pencil cells
  int m_valueCellSize = 3 * m_pencilCellCharSize + 4 * m_pencilCellPadding; // Size of each block (3x3 grid of pencil cells)

  // The grid area is square, so we only need one dimension as x will be the same as y
  int gridCoordinates[19]{0}; // Coordinates of the grid lines
  std::vector<CRect> pencilMarkRects; // Rectangles for each pencil mark cell

  // Fonts and pens
  CFont fontSml;
  CFont fontSmlBold;
  CFont fontLrg;
  CFont fontLrgBold;
  CPen homeCellPen;
  CPen blockPen;
  CPen paddingPen;
  CPen cellPen;

  bool m_recalculateGrid = true;
  SudokuGrid* m_grid;

  CRect gridBounds{0, 0, 1000, 1000};
  Quadtree m_quadtree{gridBounds, 4, 4};
  CRect m_highlightedCell{0, 0, 0, 0}; //rectangle of currently highlighted cell. if m_pencilMode than it is the pencil mark highlighted
  CRect m_unHighlightCell{0, 0, 0, 0}; //rectangle to unhighlight as cursor moves
  int   m_currentRow;
  int   m_currentCol;
  int   m_currentPencilMark;
  int   m_highlightedValue = 0;
  BOOL m_mouseInGrid = FALSE;
  bool m_pencilMode = false;
  CPoint m_queryPoint;

public:
  // The passed rect is used for the top left corner of the grid and the bottom right corner is calculated
  virtual BOOL Create( CWnd* pParentWnd, RECT& rect, UINT nId, DWORD dwStyle );

  void TogglePencilMode();
};



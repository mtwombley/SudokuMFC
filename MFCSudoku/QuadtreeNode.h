#pragma once
#include <vector>
#include <sstream>

// Extend the CRect class to include row, column, and value
struct CRectInfo : public CRect
{
  CRectInfo( const CRect& _rect, int _row, int _col, int _val ) : CRect( _rect ), row(_row), col(_col), value(_val) {}

  int row = 0;
  int col = 0;
  int value = 0;

#pragma region Debug Features
  // Add a stream operator to make it easier to log the object
  friend plog::util::nostringstream& operator << ( plog::util::nostringstream& os, const CRectInfo& rect )
  {
    os << "CRectInfo: " << "Rect: " << rect.left << ", " << rect.top << ", " << rect.right << ", " << rect.bottom << " Row: " << rect.row << " Col: " << rect.col << " Value: " << rect.value;
    return os;
  }
#pragma endregion Debug Features

};

class QuadtreeNode {
public:
    CRect bounds;
    std::vector<CRectInfo> rects;
    std::unique_ptr<QuadtreeNode> children[4];

    QuadtreeNode(const CRect& bounds) : bounds(bounds) {}

    bool isLeaf() const {
        return !children[0];
    }
};

class Quadtree
{
public:
  Quadtree( const CRect& bounds, int maxDepth, int maxRectsPerNode )
    : root( std::make_unique<QuadtreeNode>( bounds ) ), maxDepth( maxDepth ), maxRectsPerNode( maxRectsPerNode )
  {}

  void insert( const CRectInfo& rect );
  std::vector<CRectInfo*> query( const CPoint& point );

private:
  std::unique_ptr<QuadtreeNode> root;
  int maxDepth;
  int maxRectsPerNode;

  void subdivide( QuadtreeNode* node );
  void insert( QuadtreeNode* node, const CRectInfo& rect, int depth );
  void query( QuadtreeNode* node, const CPoint& point, std::vector<CRectInfo*>& foundRects );
  bool Intersects( const CRect& rect1, const CRect& rect2 );
};

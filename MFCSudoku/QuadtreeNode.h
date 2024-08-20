#pragma once
#include <vector>


struct CRectInfo : public CRect
{
  CRectInfo( const CRect& _rect, int _row, int _col, int _val ) : CRect( _rect ), row(_row), col(_col), value(_val) {}

  int row = 0;
  int col = 0;
  int value = 0;
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

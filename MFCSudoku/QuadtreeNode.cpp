#include "pch.h"
#include "QuadtreeNode.h"

#pragma region Public Methods
void Quadtree::subdivide( QuadtreeNode* node )
{
  int midX = ( node->bounds.left + node->bounds.right ) / 2;
  int midY = ( node->bounds.top + node->bounds.bottom ) / 2;

  node->children[0] = std::make_unique<QuadtreeNode>( CRect( node->bounds.left, node->bounds.top, midX, midY ) );
  node->children[1] = std::make_unique<QuadtreeNode>( CRect( midX, node->bounds.top, node->bounds.right, midY ) );
  node->children[2] = std::make_unique<QuadtreeNode>( CRect( node->bounds.left, midY, midX, node->bounds.bottom ) );
  node->children[3] = std::make_unique<QuadtreeNode>( CRect( midX, midY, node->bounds.right, node->bounds.bottom ) );
}

void Quadtree::insert( const CRectInfo& rect )
{
  insert( root.get(), rect, 0 );
}

#pragma endregion Public Methods

#pragma region Private Methods
void Quadtree::insert( QuadtreeNode* node, const CRectInfo& rect, int depth )
{
  if ( node->isLeaf() )
  {
    if ( node->rects.size() < maxRectsPerNode || depth >= maxDepth )
    {
      node->rects.push_back( rect );
    }
    else
    {
      subdivide( node );
      for ( const auto& r : node->rects )
      {
        insert( node, r, depth + 1 );
      }
      node->rects.clear();
      insert( node, rect, depth + 1 );
    }
  }
  else
  {
    for ( auto& child : node->children )
    {
      if ( Intersects(child->bounds, rect ) )
      {
        insert( child.get(), rect, depth + 1 );
      }
    }
  }
}

std::vector<CRectInfo*> Quadtree::query( const CPoint& point )
{
  std::vector<CRectInfo*> foundRects;
  query( root.get(), point, foundRects );
  return foundRects;
}

void Quadtree::query( QuadtreeNode* node, const CPoint& point, std::vector<CRectInfo*>& foundRects )
{
  if ( node->bounds.PtInRect( point ) )
  {
    for ( auto& rect : node->rects )
    {
      if ( rect.PtInRect( point ) )
      {
        foundRects.push_back( &rect );
      }
    }
    if ( !node->isLeaf() )
    {
      for ( auto& child : node->children )
      {
        query( child.get(), point, foundRects );
      }
    }
  }
}

bool Quadtree::Intersects( const CRect& rect1, const CRect& rect2 )
{
  return !( rect1.right < rect2.left || rect1.left > rect2.right ||
            rect1.bottom < rect2.top || rect1.top > rect2.bottom );
}

#pragma endregion Private Methods

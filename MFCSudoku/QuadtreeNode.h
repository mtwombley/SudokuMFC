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

/**
 * @brief QuadtreeNode represents a node in a quad tree spatial index.
 *
 * Each QuadtreeNode covers a rectangular region of 2D space (`bounds`) and stores a collection of rectangles (`rects`)
 * with associated metadata (row, col, value) as `CRectInfo` objects. When the number of rectangles in a node exceeds
 * a threshold (see Quadtree), the node can be subdivided into four child nodes (`children`), each covering a quadrant
 * of the parent node's region. This enables efficient spatial queries such as point containment and intersection.
 *
 * Usage:
 *   - Nodes are managed by the Quadtree class and are not intended to be used directly.
 *   - Leaf nodes store rectangles; internal nodes have up to four children and may or may not store rectangles.
 *
 * Members:
 *   - bounds: The rectangular region this node covers.
 *   - rects: Rectangles (with metadata) stored in this node.
 *   - children: Four child nodes (unique_ptr), or nullptr if this node is a leaf.
 *
 * Methods:
 *   - isLeaf(): Returns true if the node has no children (i.e., is a leaf node).
 */
class QuadtreeNode
{
public:
  CRect bounds;                              ///< The rectangular region covered by this node.
  std::vector<CRectInfo> rects;              ///< Rectangles (with metadata) stored in this node.
  std::unique_ptr<QuadtreeNode> children[4]; ///< Child nodes (quadrants), or nullptr if leaf.

  /**
   * @brief Construct a new QuadtreeNode covering the given bounds.
   * @param bounds The rectangular region for this node.
   */
  QuadtreeNode( const CRect& bounds ) : bounds( bounds ) {}

  /**
   * @brief Check if this node is a leaf (has no children).
   * @return true if the node is a leaf, false otherwise.
   */
  bool isLeaf() const
  {
    return !children[0];
  }
};
// QuadtreeNode.h
//
// Quadtree and QuadtreeNode classes for efficient spatial indexing and search of rectangles (CRectInfo) in 2D space.
//
// The Quadtree is a tree data structure where each node represents a rectangular region of space and can have up to four children,
// each covering a quadrant of the parent node's region. This allows for efficient insertion and querying of rectangles,
// especially for point containment and intersection queries.
//
// Usage:
//   - Construct a Quadtree with a bounding rectangle, maximum depth, and maximum rectangles per node.
//   - Insert CRectInfo objects representing rectangles with associated metadata (row, col, value).
//   - Query the quad tree with a point to efficiently find all rectangles containing that point.
//
// Example:
//   CRect bounds(0, 0, 481, 481);
//   Quadtree qt(bounds, 5, 4);
//   qt.insert(CRectInfo(CRect(10,10,20,20), 0, 0, 1));
//   auto found = qt.query(CPoint(15, 15));
//
// Notes:
//   - The quad tree subdivides nodes as needed when the number of rectangles exceeds maxRectsPerNode and maxDepth is not reached.
//   - Rectangles may be stored in multiple nodes if they overlap quadrant boundaries.
//   - CRectInfo extends CRect to include row, column, and value for Sodoku.
//

class Quadtree
{
public:
  /**
   * @brief Construct a new Quadtree object.
   * @param bounds The bounding rectangle for the entire quad tree.
   * @param maxDepth The maximum depth to which the tree can subdivide.
   * @param maxRectsPerNode The maximum number of rectangles a node can hold before subdividing.
   */
  Quadtree( const CRect& bounds, int maxDepth, int maxRectsPerNode )
    : root( std::make_unique<QuadtreeNode>( bounds ) ), maxDepth( maxDepth ), maxRectsPerNode( maxRectsPerNode )
  {};

  /**
   * @brief Insert a rectangle with metadata into the quad tree.
   * @param rect The CRectInfo object to insert.
   */
  void insert( const CRectInfo& rect );

  /**
   * @brief Query the quad tree for rectangles containing a given point.
   * @param point The point to search for.
   * @return std::vector<CRectInfo*> A vector of pointers to rectangles containing the point.
   */
  std::vector<CRectInfo*> query( const CPoint& point );

private:
  std::unique_ptr<QuadtreeNode> root; ///< Root node of the quad tree.
  int maxDepth;                       ///< Maximum allowed depth of the tree.
  int maxRectsPerNode;                ///< Maximum rectangles per node before subdivision.

  /**
   * @brief Subdivide a node into four child quadrants.
   * @param node The node to subdivide.
   */
  void subdivide( QuadtreeNode* node );

  /**
   * @brief Recursive insert helper.
   * @param node The current node.
   * @param rect The rectangle to insert.
   * @param depth The current depth in the tree.
   */
  void insert( QuadtreeNode* node, const CRectInfo& rect, int depth );

  /**
   * @brief Recursive query helper.
   * @param node The current node.
   * @param point The point to search for.
   * @param foundRects Vector to collect found rectangles.
   */
  void query( QuadtreeNode* node, const CPoint& point, std::vector<CRectInfo*>& foundRects );

  /**
   * @brief Check if two rectangles intersect.
   * @param rect1 First rectangle.
   * @param rect2 Second rectangle.
   * @return true if the rectangles intersect, false otherwise.
   */
  bool Intersects( const CRect& rect1, const CRect& rect2 );
};
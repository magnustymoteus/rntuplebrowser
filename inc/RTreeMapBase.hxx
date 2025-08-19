//
// Created by patryk on 08.08.25.
//

#ifndef RTREEMAPBASE_HXX
#define RTREEMAPBASE_HXX

#include <cstdint>
#include <string>
#include <vector>

namespace ROOT::Experimental {
class RTreeMapBase {
public:
   struct Node {
      std::string fName, fType;
      uint64_t fSize;
      uint64_t fChildrenIdx;
      uint64_t fNChildren;
      Node() = default;
      Node(const std::string &name, const std::string &type, uint64_t size, uint64_t childrenIdx, uint64_t nChildren)
         : fName(name), fType(type), fSize(size), fChildrenIdx(childrenIdx), fNChildren(nChildren)
      {
      }
   };

   struct Vec2 {
      float x, y;
      Vec2(float xArg, float yArg) : x(xArg), y(yArg) {}
   };
   struct Rect {
      Vec2 fBottomLeft, fTopRight;
      Rect(const Vec2 &bottomLeftArg, const Vec2 &topRightArg) : fBottomLeft(bottomLeftArg), fTopRight(topRightArg) {}
   };
   struct RGBColor {
      uint8_t r, g, b, a;
      RGBColor(uint8_t rArg, uint8_t gArg, uint8_t bArg, uint8_t aArg = 255) : r(rArg), g(gArg), b(bArg), a(aArg) {}
   };
   std::vector<Node> fNodes;
   RTreeMapBase() = default;
   virtual ~RTreeMapBase() = default;

protected:
   void DrawTreeMap(const Node &elem, Rect rect, int depth) const;
   void DrawLegend() const;
   virtual void AddBox(const Rect &rect, const RGBColor &color, float borderWidth = 0.15f) const = 0;
   virtual void AddText(const Vec2 &pos, const std::string &content, float size,
                        const RGBColor &color = RGBColor(0, 0, 0), bool alignCenter = false) const = 0;
};
} // namespace ROOT::Experimental
#endif
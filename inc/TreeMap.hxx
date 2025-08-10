//
// Created by patryk on 08.08.25.
//

#ifndef RNTUPLEBROWSER_TREEMAP_HXX
#define RNTUPLEBROWSER_TREEMAP_HXX

#include <ROOT/RNTupleInspector.hxx>
#include <ROOT/RNTuple.hxx>

struct TreeMapNode {
    std::string fName;
    uint8_t fType;
    uint64_t fSize;
    uint64_t fChildrenIdx;
    uint64_t fNChildren;
    TreeMapNode() = default;
    TreeMapNode(const std::string &name, uint8_t type, uint64_t size, uint64_t childrenIdx,
                 uint64_t nChildren)
       : fName(name), fType(type), fSize(size), fChildrenIdx(childrenIdx), fNChildren(nChildren)
    {
    }
 };

class TreeMap {
public:
    struct Vec2 {
        float x, y;
        Vec2(float x, float y) : x(x), y(y) {}
    };
    struct Rect {
        Vec2 fBottomLeft, fTopRight;
        Rect(const Vec2 &bottomLeft, const Vec2 &topRight) : fBottomLeft(bottomLeft), fTopRight(topRight) {}
    };
    struct RGBColor {
        uint8_t r, g, b, a;
        RGBColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a=255) : r(r), g(g), b(b), a(a) {}
    };
    std::vector<TreeMapNode> fNodes;
    TreeMap(const ROOT::Experimental::RNTupleInspector &insp);
    TreeMap() = default;
protected:
    void DrawTreeMap(const TreeMapNode &elem, Rect rect, int depth) const;
    void DrawLegend() const;
    virtual void AddBox(const Rect &rect, const RGBColor &color, float borderWidth=0.15f) const = 0;
    virtual void AddText(const Vec2 &pos, const std::string &content, float size, const RGBColor &color=RGBColor(0,0,0), bool alignCenter=false) const = 0;
};
#endif //RNTUPLEBROWSER_TREEMAP_HXX
#ifndef TREEMAP_HXX
#define TREEMAP_HXX

#include <ROOT/RDrawable.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RNTuple.hxx>
#include <ROOT/RPad.hxx>
#include <set>

namespace ROOT::Experimental {

class RTreeMappable {
public:
   const std::string &GetName() const { return fName; }
   const std::string &GetType() const { return fType; }

   std::uint64_t GetSize() const { return fSize; }
   std::uint64_t GetNChildren() const { return fNChildren; }
   std::uint64_t GetChildrenIdx() const { return fChildrenIdx; }

   RTreeMappable(const std::string name, const std::string type, const std::uint64_t size,
                 const std::uint64_t childrenIdx, const std::uint64_t nChildren)
      : fName(name), fType(type), fSize(size), fChildrenIdx(childrenIdx), fNChildren(nChildren)
   {
   }

private:
   std::uint64_t fChildrenIdx, fNChildren, fSize;
   std::string fName, fType;
};

class RTreeMap : public RDrawable {

public:
   struct RVec2 {
      float x, y;
      RVec2(const float &x, const float &y) : x(x), y(y) {}
   };
   struct RRect {
      RVec2 p1, p2;
      RRect(const RVec2 &p1, const RVec2 &p2) : p1(p1), p2(p2) {}
   };

   RTreeMap(std::shared_ptr<RCanvas> canvas, const std::vector<RTreeMappable> &nodes,
            const std::set<std::string> &legend)
      : RDrawable("treemap"),
        fNodes(nodes),
        fBoxPad(canvas->AddPad(RPadPos(0, 0), RPadExtent(1, 1))),
        fTextPad(canvas->AddPad(RPadPos(0, 0), RPadExtent(1, 1)))
   {
      DrawTreeMap(fNodes[0], RVec2(0, 0), RVec2(1, 1), 0);
      DrawLegend(legend);
   }

private:
   std::vector<RTreeMappable> fNodes;
   std::shared_ptr<RPad> fBoxPad;
   std::shared_ptr<RPad> fTextPad;
   void DrawTreeMap(const RTreeMappable &elem, RVec2 begin, RVec2 end, int depth) const;
   void DrawLegend(const std::set<std::string> &legend) const;
};
} // namespace ROOT::Experimental

#endif
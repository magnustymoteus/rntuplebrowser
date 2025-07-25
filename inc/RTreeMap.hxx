#ifndef TREEMAP_HXX
#define TREEMAP_HXX

#include <ROOT/RDrawable.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleInspector.hxx>
#include <functional>

using namespace ROOT::Experimental;

class RTreeMappable {
public:
   const std::string &GetName() const { return fName; }
   std::uint64_t GetSize() const { return fSize; }
   std::uint64_t GetNChildren() const { return fNChildren; }
   std::uint64_t GetChildrenIdx() const { return fChildrenIdx; }
   RColor GetColor() const { return fColor; }

   RTreeMappable(const std::string name, const std::uint64_t size, const RColor color, const std::uint64_t childrenIdx,
                 const std::uint64_t nChildren)
      : fName(name), fSize(size), fColor(color), fChildrenIdx(childrenIdx), fNChildren(nChildren)
   {
   }

private:
   std::uint64_t fChildrenIdx, fNChildren, fSize;
   std::string fName;
   RColor fColor;
};

class RTreeMap : public RDrawable {
public:
   RTreeMap(std::shared_ptr<RCanvas> canvasArg, const std::vector<RTreeMappable> &nodes)
      : RDrawable("treemap"), fCanvas(canvasArg), fNodes(nodes)
   {
      DrawTreeMap(fNodes[0], {0, 0}, {1, 1}, 0);
   }

private:
   std::vector<RTreeMappable> fNodes;
   std::shared_ptr<RCanvas> fCanvas;
   void
   DrawTreeMap(const RTreeMappable &elem, std::pair<float, float> begin, std::pair<float, float> end, int depth) const;
};

#endif

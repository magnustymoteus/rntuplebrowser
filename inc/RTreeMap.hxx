#ifndef TREEMAP_HXX
#define TREEMAP_HXX

#include <ROOT/RPadBase.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleInspector.hxx>
#include <functional>

using namespace ROOT::Experimental;

class RTreeMappable {
public:
   const std::string &GetName() const { return fName; }
   const std::uint64_t GetSize() const { return fSize; }
   const std::uint64_t GetNChildren() const { return fNChildren; }
   const std::uint64_t GetChildrenIdx() const { return fChildrenIdx; }

   RTreeMappable(const std::string name, const std::uint64_t size, const std::uint64_t childrenIdx,
                 const std::uint64_t nChildren)
      : fName(name), fSize(size), fChildrenIdx(childrenIdx), fNChildren(nChildren)
   {
   }

private:
   const std::uint64_t fChildrenIdx, fNChildren, fSize;
   const std::string fName;
};

class RTreeMap : public RPadBase {
public:
   RTreeMap(std::shared_ptr<RCanvas> canvasArg, const std::vector<RTreeMappable> nodes)
      : RPadBase("treemap"), fCanvas(canvasArg), fNodes(nodes)
   {
      DrawTreeMap(fNodes[0], {0, 0}, {1, 1}, 0);
   }
   RCanvas *GetCanvas() override { return fCanvas.get(); }
   const RCanvas *GetCanvas() const override { return fCanvas.get(); }

private:
   const std::vector<RTreeMappable> fNodes;
   const std::shared_ptr<RCanvas> fCanvas;
   void DrawTreeMap(const RTreeMappable &elem, const std::pair<float, float> &begin, const std::pair<float, float> &end,
                    int depth) const;
};

#endif

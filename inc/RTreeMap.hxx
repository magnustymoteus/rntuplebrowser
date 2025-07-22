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
   const std::vector<std::shared_ptr<const RTreeMappable>> &GetChildren() const { return fChildren; }
   RTreeMappable(const std::string name, const std::uint64_t size,
                 const std::vector<std::shared_ptr<const RTreeMappable>> children)
      : fName(name), fSize(size), fChildren(children)
   {
   }

private:
   const std::vector<std::shared_ptr<const RTreeMappable>> fChildren;
   std::string fName;
   std::uint64_t fSize;
};

class RTreeMap : public RPadBase {
public:
   RTreeMap(std::shared_ptr<RCanvas> canvasArg, const std::shared_ptr<const RTreeMappable> &root)
      : RPadBase("treemap"), fRoot(root), fCanvas(canvasArg)
   {
      DrawTreeMap(fRoot, {0, 0}, {1, 1}, 0);
   }
   RCanvas *GetCanvas() override { return fCanvas.get(); }
   const RCanvas *GetCanvas() const override { return fCanvas.get(); }

private:
   const std::shared_ptr<const RTreeMappable> &fRoot;
   const std::shared_ptr<RCanvas> fCanvas;
   void DrawTreeMap(const std::shared_ptr<const RTreeMappable> &currentElem, const std::pair<float, float> &begin,
                    const std::pair<float, float> &end, int depth) const;
};

#endif

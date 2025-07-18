#ifndef TREEMAP_HXX
#define TREEMAP_HXX

#include <ROOT/RPadBase.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleInspector.hxx>

using namespace ROOT::Experimental;

class RTreeMap : public RPadBase {
public:
   RTreeMap(std::shared_ptr<RCanvas> canvasArg, const std::string &tupleName, const std::string &storage)
      : RPadBase("treemap"),
        fInspector(RNTupleInspector::Create(tupleName, storage)),
        fRootField(fInspector->GetDescriptor().GetFieldZero()),
        fCanvas(canvasArg)
   {
      DrawTreeMap(fRootField, {0, 0}, {1, 1}, 0);
   }
   RCanvas *GetCanvas() override { return fCanvas.get(); }
   const RCanvas *GetCanvas() const override { return fCanvas.get(); }

private:
   std::unique_ptr<RNTupleInspector> fInspector;
   const ROOT::RFieldDescriptor &fRootField;
   const std::shared_ptr<RCanvas> fCanvas;

   void DrawTreeMap(const ROOT::RFieldDescriptor &fieldDesc, const std::pair<float, float> &begin,
                    const std::pair<float, float> &end, int depth) const;
};

#endif

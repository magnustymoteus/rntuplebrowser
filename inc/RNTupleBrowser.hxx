//
// Created by patryk on 16.07.25.
//
#ifndef RNTUPLEBROWSERfHXX
#define RNTUPLEBROWSERfHXX

#include <ROOT/RNTuple.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RNTupleInspector.hxx>
#include "RTreeMap.hxx"

using namespace ROOT::Experimental;
class RNTupleBrowser {
private:
   const std::shared_ptr<RCanvas> fCanvas;
   const std::unique_ptr<RNTupleInspector> fInspector;
   std::shared_ptr<const RTreeMappable> CreateRTreeMappable(const ROOT::RFieldDescriptor &fieldDesc) const;

public:
   RNTupleBrowser(const std::string_view tupleName, const std::string_view storage)
      : fCanvas(RCanvas::Create("RNTupleBrowser")), fInspector(RNTupleInspector::Create(tupleName, storage))
   {
   }
   void Browse() const;
};

#endif // RNTUPLEBROWSERfHXX

//
// Created by patryk on 16.07.25.
//
#ifndef RNTUPLEBROWSERfHXX
#define RNTUPLEBROWSERfHXX

#include <ROOT/RNTuple.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RNTupleInspector.hxx>
using namespace ROOT::Experimental;
class RNTupleBrowser {
private:
   const std::shared_ptr<RCanvas> fCanvas;
   const std::string fTupleName, fStorage;

public:
   RNTupleBrowser(const std::string_view tupleName, const std::string_view storage)
      : fCanvas(RCanvas::Create("RNTupleBrowser")), fTupleName(tupleName), fStorage(storage)
   {
   }
   void Browse() const;
};

#endif // RNTUPLEBROWSERfHXX

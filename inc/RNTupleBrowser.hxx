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
   std::shared_ptr<RCanvas> fCanvas;
   std::unique_ptr<RNTupleInspector> fInspector;
   ROOT::DescriptorId_t fRootId;
   size_t fRootSize;

   std::vector<RTreeMappable> CreateRTreeMap(std::map<std::string, RColor> &columnLegend) const;
   RTreeMappable CreateRTreeMappable(const ROOT::RFieldDescriptor &fldDesc, const std::uint64_t &childrenIdx,
                                     const std::uint64_t &nChildren) const;
   RTreeMappable
   CreateRTreeMappable(const RNTupleInspector::RColumnInspector &colInsp, const std::uint64_t &childrenIdx) const;

public:
   RNTupleBrowser(const std::string_view tupleName, const std::string_view storage)
      : fCanvas(RCanvas::Create("RNTupleBrowser")),
        fInspector(RNTupleInspector::Create(tupleName, storage)),
        fRootId(fInspector->GetDescriptor().GetFieldZero().GetId()),
        fRootSize(0)
   {
      for (const auto &childId : fInspector->GetDescriptor().GetFieldDescriptor(fRootId).GetLinkIds()) {
         fRootSize += fInspector->GetFieldTreeInspector(childId).GetCompressedSize();
      }
   }
   void Browse() const;
};

#endif // RNTUPLEBROWSERfHXX
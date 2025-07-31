// Created by patryk on 16.07.25.
//
#ifndef RNTUPLEBROWSERfHXX
#define RNTUPLEBROWSERfHXX

#include <ROOT/RNTuple.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RNTupleInspector.hxx>
#include "RTreeMap.hxx"
#include <set>

namespace ROOT::Experimental {

class RNTupleBrowser {
private:
   std::shared_ptr<RCanvas> fCanvas = RCanvas::Create("RNTupleBrowser");
   std::unique_ptr<RNTupleInspector> fInspector;
   ROOT::DescriptorId_t fRootId = fInspector->GetDescriptor().GetFieldZero().GetId();
   size_t fRootSize = 0;

   std::vector<ROOT::Experimental::RTreeMappable> CreateTreeMap(std::set<std::string> &legend) const;
   ROOT::Experimental::RTreeMappable
   CreateTreeMappable(const ROOT::RFieldDescriptor &fldDesc, std::uint64_t childrenIdx, std::uint64_t nChildren) const;
   ROOT::Experimental::RTreeMappable
   CreateTreeMappable(const RNTupleInspector::RColumnInspector &colInsp, std::uint64_t childrenIdx) const;

public:
   RNTupleBrowser(const std::string_view tupleName, const std::string_view storage)
      : fInspector(RNTupleInspector::Create(tupleName, storage))
   {
      for (const auto &childId : fInspector->GetDescriptor().GetFieldDescriptor(fRootId).GetLinkIds()) {
         fRootSize += fInspector->GetFieldTreeInspector(childId).GetCompressedSize();
      }
   }
   void Browse() const;
};

} // namespace ROOT::Experimental

#endif // RNTUPLEBROWSERfHXX
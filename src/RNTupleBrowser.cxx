#include <RNTupleBrowser.hxx>
#include <ROOT/RCanvas.hxx>
#include <iostream>
#include <queue>
#include <cassert>
#include <ROOT/RColumnElementBase.hxx>

ROOT::Experimental::RTreeMappable
ROOT::Experimental::RNTupleBrowser::CreateTreeMappable(const ROOT::RFieldDescriptor &fldDesc,
                                                       const std::uint64_t &childrenIdx,
                                                       const std::uint64_t &nChildren) const
{
   uint64_t size =
      (fRootId != fldDesc.GetId()) ? fInspector->GetFieldTreeInspector(fldDesc.GetId()).GetCompressedSize() : fRootSize;
   return ROOT::Experimental::RTreeMappable(fldDesc.GetFieldName(), fldDesc.GetTypeName(), size, childrenIdx,
                                            nChildren);
}
ROOT::Experimental::RTreeMappable
ROOT::Experimental::RNTupleBrowser::CreateTreeMappable(const RNTupleInspector::RColumnInspector &colInsp,
                                                       const std::uint64_t &childrenIdx) const
{
   return ROOT::Experimental::RTreeMappable("",
                                            ROOT::Internal::RColumnElementBase::GetColumnTypeName(colInsp.GetType()),
                                            colInsp.GetCompressedSize(), childrenIdx, 0);
}

std::vector<ROOT::Experimental::RTreeMappable>
ROOT::Experimental::RNTupleBrowser::CreateTreeMap(std::set<std::string> &legend) const
{
   std::vector<ROOT::Experimental::RTreeMappable> nodes;
   const auto &descriptor = fInspector->GetDescriptor();

   std::queue<std::pair<uint64_t, bool>> queue; // (columnid/fieldid, isfield)
   queue.push({fRootId, true});
   while (!queue.empty()) {
      size_t levelSize = queue.size();
      size_t levelChildrenStart = nodes.size() + levelSize;
      for (size_t i = 0; i < levelSize; ++i) {
         const auto &current = queue.front();
         queue.pop();

         std::vector<uint64_t> children;
         size_t nChildren = 0;
         if (current.second) {
            const auto &fldDesc = descriptor.GetFieldDescriptor(current.first);
            children = fldDesc.GetLinkIds();
            for (const auto childId : children) {
               queue.push({childId, 1});
            }
            for (const auto &columnDesc : descriptor.GetColumnIterable(fldDesc.GetId())) {
               const auto &columnId = columnDesc.GetPhysicalId();
               children.push_back(columnId);
               queue.push({columnId, 0});
            }
            nChildren = children.size();
            const auto &node = CreateTreeMappable(fldDesc, levelChildrenStart, nChildren);
            nodes.push_back(node);
         } else {
            const auto &colInsp = fInspector->GetColumnInspector(current.first);
            const auto &node = CreateTreeMappable(colInsp, levelChildrenStart);
            nodes.push_back(node);
            legend.insert(ROOT::Internal::RColumnElementBase::GetColumnTypeName(colInsp.GetType()));
         }

         levelChildrenStart += nChildren;
      }
   }
   return nodes;
}

void ROOT::Experimental::RNTupleBrowser::Browse() const
{
   std::set<std::string> legend;
   const auto &treeMap = CreateTreeMap(legend);
   fCanvas->Draw<RTreeMap>(fCanvas, treeMap, legend);
   fCanvas->Show();
}
#include <RNTupleBrowser.hxx>
#include <ROOT/RCanvas.hxx>
#include <queue>
#include <ROOT/RColumnElementBase.hxx>

using namespace ROOT::Experimental;

RTreeMappable RNTupleBrowser::CreateTreeMappable(const ROOT::RFieldDescriptor &fldDesc, std::uint64_t childrenIdx,
                                                 std::uint64_t nChildren) const
{
   uint64_t size =
      (fRootId != fldDesc.GetId()) ? fInspector->GetFieldTreeInspector(fldDesc.GetId()).GetCompressedSize() : fRootSize;
   return RTreeMappable(fldDesc.GetFieldName(), fldDesc.GetTypeName(), size, childrenIdx, nChildren);
}
RTreeMappable
RNTupleBrowser::CreateTreeMappable(const RNTupleInspector::RColumnInspector &colInsp, std::uint64_t childrenIdx) const
{
   return RTreeMappable("", ROOT::Internal::RColumnElementBase::GetColumnTypeName(colInsp.GetType()),
                        colInsp.GetCompressedSize(), childrenIdx, 0);
}

std::vector<RTreeMappable> RNTupleBrowser::CreateTreeMap(std::set<std::string> &legend) const
{
   std::vector<RTreeMappable> nodes;
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

void RNTupleBrowser::Browse() const
{
   std::set<std::string> legend;
   const auto &treeMap = CreateTreeMap(legend);
   fCanvas->Draw<RTreeMap>(fCanvas, treeMap, legend);
   fCanvas->Show();
}
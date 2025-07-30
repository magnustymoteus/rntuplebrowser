#include <RNTupleBrowser.hxx>
#include <ROOT/RCanvas.hxx>
#include <iostream>
#include <queue>
#include <cassert>

static uint64_t ComputeFnv(uint64_t a)
{
   const uint64_t FNV_offset = 14695981039346656037ULL;
   const uint64_t FNV_prime = 1099511628211ULL;

   uint64_t h = FNV_offset;
   for (int i = 0; i < 8; ++i) {
      uint8_t octet = static_cast<uint8_t>(a >> (i * 8));
      h ^= octet;
      h *= FNV_prime;
   }
   return h;
}

RTreeMappable RNTupleBrowser::CreateTreeMappable(const ROOT::RFieldDescriptor &fldDesc,
                                                 const std::uint64_t &childrenIdx, const std::uint64_t &nChildren) const
{
   uint64_t size =
      (fRootId != fldDesc.GetId()) ? fInspector->GetFieldTreeInspector(fldDesc.GetId()).GetCompressedSize() : fRootSize;
   return RTreeMappable(fldDesc.GetFieldName(), size, RColor(100, 100, 100), childrenIdx, nChildren);
}
RTreeMappable RNTupleBrowser::CreateTreeMappable(const RNTupleInspector::RColumnInspector &colInsp,
                                                 const std::uint64_t &childrenIdx) const
{
   const uint64_t hash = ComputeFnv(static_cast<uint64_t>(colInsp.GetType()));
   const auto color = RColor((hash >> 16) & 0xFF, (hash >> 8) & 0xFF, hash & 0xFF);
   return RTreeMappable("", colInsp.GetCompressedSize(), color, childrenIdx, 0);
}

std::vector<RTreeMappable> RNTupleBrowser::CreateTreeMap(std::set<uint8_t> &legend) const
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
            legend.insert(static_cast<uint8_t>(colInsp.GetType()));
         }

         levelChildrenStart += nChildren;
      }
   }
   return nodes;
}

void RNTupleBrowser::Browse() const
{
   std::set<uint8_t> legend;
   const auto &treeMap = CreateTreeMap(legend);
   fCanvas->Draw<RTreeMap>(fCanvas, treeMap, legend);
   fCanvas->Show();
}
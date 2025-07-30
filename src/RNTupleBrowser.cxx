#include <RNTupleBrowser.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RColumnElementBase.hxx>
#include <iostream>
#include <queue>
#include <cassert>

/* hash string into RGB color with FNV-1a: used for speed and diffusion*/
static uint64_t ComputeFnv(const std::string &str)
{
   uint64_t h = 14695981039346656037ULL;
   for (char c : str)
      h = (h ^ static_cast<uint8_t>(c)) * 1099511628211ULL;
   return h;
}

static uint64_t ComputeFnv(const uint64_t &a)
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

RTreeMappable RNTupleBrowser::CreateRTreeMappable(const ROOT::RFieldDescriptor &fldDesc,
                                                  const std::uint64_t &childrenIdx,
                                                  const std::uint64_t &nChildren) const
{
   uint64_t size =
      (fRootId != fldDesc.GetId()) ? fInspector->GetFieldTreeInspector(fldDesc.GetId()).GetCompressedSize() : fRootSize;
   const uint64_t &hash = ComputeFnv(fldDesc.GetTypeName());
   const auto color = RColor((hash >> 16) & 0xFF, (hash >> 8) & 0xFF, hash & 0xFF);
   return RTreeMappable(fldDesc.GetFieldName(), size, color, childrenIdx, nChildren);
}
RTreeMappable RNTupleBrowser::CreateRTreeMappable(const RNTupleInspector::RColumnInspector &colInsp,
                                                  const std::uint64_t &childrenIdx) const
{
   const uint64_t &hash = ComputeFnv(static_cast<uint64_t>(colInsp.GetType()));
   const auto color = RColor((hash >> 16) & 0xFF, (hash >> 8) & 0xFF, hash & 0xFF);
   std::string str = "";
   return RTreeMappable(str, colInsp.GetCompressedSize(), color, childrenIdx, 0);
}

std::vector<RTreeMappable> RNTupleBrowser::CreateRTreeMap(std::map<std::string, RColor> &columnLegend) const
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
            const auto &node = CreateRTreeMappable(fldDesc, levelChildrenStart, nChildren);
            nodes.push_back(node);
         } else {
            const auto &colInsp = fInspector->GetColumnInspector(current.first);
            const auto &node = CreateRTreeMappable(colInsp, levelChildrenStart);
            columnLegend.insert(
               {ROOT::Internal::RColumnElementBase::GetColumnTypeName(colInsp.GetType()), node.GetColor()});
            nodes.push_back(node);
         }

         levelChildrenStart += nChildren;
      }
   }
   return nodes;
}

void RNTupleBrowser::Browse() const
{
   std::map<std::string, RColor> columnLegend;
   const auto &treeMap = CreateRTreeMap(columnLegend);
   fCanvas->Draw<RTreeMap>(fCanvas, treeMap, columnLegend);
   fCanvas->Show();
}
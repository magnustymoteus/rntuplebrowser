#include <RNTupleBrowser.hxx>
#include <ROOT/RCanvas.hxx>
#include <iostream>
#include <queue>

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
   return 14695981039346656037ULL ^ static_cast<uint8_t>(a) * 1099511628211ULL;
}

RTreeMappable RNTupleBrowser::CreateRTreeMappable(const ROOT::RFieldDescriptor &fldDesc,
                                                  const std::uint64_t &childrenIdx,
                                                  const std::uint64_t &nChildren) const
{
   uint64_t size = (fRootFld.GetId() != fldDesc.GetId())
                      ? fInspector->GetFieldTreeInspector(fldDesc.GetId()).GetCompressedSize()
                      : fRootSize;
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

std::vector<RTreeMappable> RNTupleBrowser::CreateRTreeMap() const
{
   std::vector<RTreeMappable> nodes;
   const auto &descriptor = fInspector->GetDescriptor();

   std::queue<std::pair<uint64_t, bool>> queue; // (columnid/fieldid, isfield)
   queue.push({fRootFld.GetId(), true});
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
            const auto &columns = fInspector->GetColumnIds(fldDesc.GetId());
            children.insert(children.end(), columns.begin(), columns.end());
            for (const auto &columnId : columns) {
               queue.push({columnId, 0});
            }
            nChildren = children.size();
            const auto &node = CreateRTreeMappable(fldDesc, levelChildrenStart, nChildren);
            nodes.push_back(node);
         } else
            nodes.push_back(CreateRTreeMappable(fInspector->GetColumnInspector(current.first), levelChildrenStart));

         levelChildrenStart += nChildren;
      }
   }
   return nodes;
}

void RNTupleBrowser::Browse() const
{

   fCanvas->Draw<RTreeMap>(fCanvas, CreateRTreeMap());
   fCanvas->Show();
}
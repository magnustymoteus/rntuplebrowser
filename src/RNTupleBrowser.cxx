//
// Created by patryk on 16.07.25.
//
#include <RNTupleBrowser.hxx>
#include <ROOT/RCanvas.hxx>
#include <iostream>
#include <queue>

std::vector<RTreeMappable> RNTupleBrowser::CreateRTreeMappable() const
{
   std::vector<RTreeMappable> nodes;
   const auto &descriptor = fInspector->GetDescriptor();
   const auto *root = &descriptor.GetFieldZero();

   size_t rootSize = 0;
   for (const auto &childId : root->GetLinkIds()) {
      rootSize += fInspector->GetFieldTreeInspector(childId).GetCompressedSize();
   }

   std::queue<const ROOT::RFieldDescriptor *> queue;
   queue.push(root);
   while (!queue.empty()) {
      size_t levelSize = queue.size();
      size_t levelChildrenStart = nodes.size() + levelSize;
      for (size_t i = 0; i < levelSize; ++i) {
         const auto *fldDesc = queue.front();
         queue.pop();

         const auto &children = fldDesc->GetLinkIds();
         size_t nChildren = children.size();
         size_t childrenIdx = levelChildrenStart;
         uint64_t size = (root->GetId() != fldDesc->GetId())
                            ? fInspector->GetFieldTreeInspector(fldDesc->GetId()).GetCompressedSize()
                            : rootSize;

         const auto hash = std::hash<std::string>()(fldDesc->GetTypeName());
         const auto color = RColor((hash & 0xFF0000) >> 16, (hash & 0x00FF00) >> 8,
                                   hash & 0x0000FF); // hash field type string and turn it into RGB
         nodes.push_back(RTreeMappable(fldDesc->GetFieldName(), size, color, childrenIdx, nChildren));

         for (const auto childId : children) {
            const auto *childFldDesc = &descriptor.GetFieldDescriptor(childId);
            queue.push(childFldDesc);
         }
         levelChildrenStart += nChildren;
      }
   }
   return nodes;
}

void RNTupleBrowser::Browse() const
{

   fCanvas->Draw<RTreeMap>(fCanvas, CreateRTreeMappable());
   fCanvas->Show();
}
//
// Created by patryk on 16.07.25.
//
#include <RNTupleBrowser.hxx>
#include <ROOT/RCanvas.hxx>
#include <iostream>

std::shared_ptr<const RTreeMappable> RNTupleBrowser::CreateRTreeMappable(const ROOT::RFieldDescriptor &fieldDesc) const
{
   std::vector<std::shared_ptr<const RTreeMappable>> children;
   const auto &descriptor = fInspector->GetDescriptor();
   const bool isRoot = fieldDesc.GetParentId() == ROOT::kInvalidDescriptorId;
   std::uint64_t size = 0;

   for (const auto &childId : fieldDesc.GetLinkIds()) {
      const auto &childFldDesc = descriptor.GetFieldDescriptor(childId);
      auto child = CreateRTreeMappable(childFldDesc);
      if (isRoot)
         size += child->GetSize();
      children.push_back(child);
   }

   if (!isRoot)
      size = fInspector->GetFieldTreeInspector(fieldDesc.GetId()).GetCompressedSize();
   return std::make_shared<RTreeMappable>(fieldDesc.GetFieldName(), size, children);
}

void RNTupleBrowser::Browse() const
{
   fCanvas->Draw<RTreeMap>(fCanvas, CreateRTreeMappable(fInspector->GetDescriptor().GetFieldZero()));
   fCanvas->Show();
}
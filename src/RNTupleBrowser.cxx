//
// Created by patryk on 16.07.25.
//
#include <RNTupleBrowser.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RBox.hxx>
#include <ROOT/RText.hxx>
#include <iostream>

void RNTupleBrowser::Treemap(const ROOT::RFieldDescriptor &fieldDesc, const float xBegin, const float xEnd,
                             const int &depth) const
{
   std::uint64_t size = 0;
   const auto &descriptor = _inspector->GetDescriptor();
   if (fieldDesc.GetParentId() == ROOT::kInvalidDescriptorId) {
      for (const auto &childFldId : fieldDesc.GetLinkIds()) {
         const auto &childFldDesc = descriptor.GetFieldDescriptor(childFldId);
         size += _inspector->GetFieldTreeInspector(childFldId).GetCompressedSize();
      }
   } else {
      size = _inspector->GetFieldTreeInspector(fieldDesc.GetId()).GetCompressedSize();
   }

   float currentX = xBegin;

   const float offset = 0.25f;           // offset for canvas coordinates
   const float scale = 0.5f;             // scaling from logical to canvas coordinates
   const float yBottom = 0.25f;          // fixed bottom y coordinate
   const float yTop = 0.75f;             // fixed top y coordinate
   const float textDepthOffset = 0.035f; // vertical offset per depth level for text.
   const float textSizeFactor = 0.005f;  // text size scaling factor

   const float canvasXBegin = offset + xBegin * scale;
   const float canvasXEnd = offset + xEnd * scale;

   _canvas->Draw<RBox>(RPadPos(canvasXBegin, yBottom), RPadPos(canvasXEnd, yTop));

   auto text = _canvas->Add<RText>(RPadPos(canvasXBegin, yTop - textDepthOffset * depth),
                                   fieldDesc.GetFieldName() + ": " + std::to_string(size));
   text->text.align = RAttrText::kLeftBottom;
   text->text.size = textSizeFactor * size;

   for (const auto &childFldId : fieldDesc.GetLinkIds()) {
      const auto &childFldDesc = descriptor.GetFieldDescriptor(childFldId);
      const std::uint64_t childSize = _inspector->GetFieldTreeInspector(childFldId).GetCompressedSize();
      if (childSize > 0) {
         const float nextX = currentX + static_cast<float>(childSize) / static_cast<float>(size);
         Treemap(childFldDesc, currentX, nextX, depth + 1);
         currentX = xEnd * nextX;
      }
   }
}

void RNTupleBrowser::Browse() const
{
   const auto &descriptor = _inspector->GetDescriptor();
   Treemap(descriptor.GetFieldZero());
   _canvas->Show();
}
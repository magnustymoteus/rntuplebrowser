//
// Created by patryk on 16.07.25.
//
#include <RNTupleBrowser.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RBox.hxx>
#include <ROOT/RText.hxx>
#include <iostream>

void RNTupleBrowser::Treemap(const ROOT::RFieldDescriptor &fieldDesc, const std::pair<float, float> begin,
                             const std::pair<float, float> end, const int &depth) const
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

   auto toPad = [&](float u) { return 0.25f + u * 0.5f; };
   const float textDepthOffset = 0.035f, textSizeFactor = 0.005f;

   const std::pair<float, float> drawBegin = {toPad(begin.first), toPad(begin.second)};
   const std::pair<float, float> drawEnd = {toPad(end.first), toPad(end.second)};
   _canvas->Draw<RBox>(RPadPos(drawBegin.first, drawBegin.second), RPadPos(drawEnd.first, drawEnd.second));
   auto text = _canvas->Add<RText>(RPadPos(drawBegin.first, drawBegin.second /*- textDepthOffset * depth*/),
                                   fieldDesc.GetFieldName() + ": " + std::to_string(size));
   text->text.align = RAttrText::kLeftBottom;
   text->text.size = textSizeFactor * size;

   std::pair<float, float> current = begin;
   const bool sliceVertical = depth % 2 == 0;
   for (const auto &childFldId : fieldDesc.GetLinkIds()) {
      const auto &childFldDesc = descriptor.GetFieldDescriptor(childFldId);
      const std::uint64_t childSize = _inspector->GetFieldTreeInspector(childFldId).GetCompressedSize();
      if (childSize > 0) {
         std::pair<float, float> nextBegin = begin, nextEnd = end;
         const float frac = static_cast<float>(childSize) / static_cast<float>(size);
         if (sliceVertical) {
            nextBegin.first = current.first;
            nextEnd.first = current.first + frac * (end.first - begin.first);
            current.first = nextEnd.first;
         } else {
            nextBegin.second = current.second;
            nextEnd.second = current.second + frac * (end.second - begin.second);
            current.second = nextEnd.second;
         }
         Treemap(childFldDesc, nextBegin, nextEnd, depth + 1);
      }
   }
}

void RNTupleBrowser::Browse() const
{
   const auto &descriptor = _inspector->GetDescriptor();
   Treemap(descriptor.GetFieldZero());
   _canvas->Show();
}
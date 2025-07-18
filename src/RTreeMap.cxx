#include "RTreeMap.hxx"
#include <ROOT/RBox.hxx>
#include <ROOT/RText.hxx>

void RTreeMap::DrawTreeMap(const ROOT::RFieldDescriptor &fieldDesc, const std::pair<float, float> &begin,
                           const std::pair<float, float> &end, int depth) const
{
   const bool sliceVertical = depth % 2 == 0;
   const bool isRoot = fieldDesc.GetParentId() == ROOT::kInvalidDescriptorId;

   std::uint64_t size = 0;
   const auto &descriptor = fInspector->GetDescriptor();
   const auto childrenIds = fieldDesc.GetLinkIds();

   if (isRoot) {
      for (const auto &childFldId : childrenIds) {
         const auto &childFldDesc = descriptor.GetFieldDescriptor(childFldId);
         size += fInspector->GetFieldTreeInspector(childFldId).GetCompressedSize();
      }
   } else {
      size = fInspector->GetFieldTreeInspector(fieldDesc.GetId()).GetCompressedSize();
   }

   auto toPad = [&](float u) { return 0.25f + u * 0.5f; };
   const float textDepthOffset = 0.005f, textSizeFactor = 0.06f;

   const std::pair<float, float> drawBegin = {toPad(begin.first), toPad(begin.second)};
   const std::pair<float, float> drawEnd = {toPad(end.first), toPad(end.second)};

   fCanvas->Draw<RBox>(RPadPos(drawBegin.first, drawBegin.second), RPadPos(drawEnd.first, drawEnd.second));

   auto text =
      fCanvas->Add<RText>(RPadPos((drawBegin.first + drawEnd.first) / 2.0f, (drawBegin.second + drawEnd.second) / 2.0f),
                          fieldDesc.GetFieldName() + " (" + std::to_string(size) + ")");
   text->text.align = RAttrText::kCenter;
   text->text.size = textSizeFactor / (depth + 1);
   text->text.angle = (sliceVertical) ? 0 : 90;
   std::pair<float, float> current = begin;
   for (const auto &childFldId : childrenIds) {
      const auto &childFldDesc = descriptor.GetFieldDescriptor(childFldId);
      const std::uint64_t childSize = fInspector->GetFieldTreeInspector(childFldId).GetCompressedSize();
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
         DrawTreeMap(childFldDesc, nextBegin, nextEnd, depth + 1);
      }
   }
}
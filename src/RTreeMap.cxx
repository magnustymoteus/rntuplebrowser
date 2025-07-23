#include "RTreeMap.hxx"
#include <ROOT/RBox.hxx>
#include <ROOT/RText.hxx>

void RTreeMap::DrawTreeMap(const RTreeMappable &elem, const std::pair<float, float> &begin,
                           const std::pair<float, float> &end, int depth) const
{
   const int sliceVertical = depth % 2 == 0;
   auto toPad = [](float u) { return 0.25f + u * 0.5f; };
   const float textSizeFactor = 0.06f;

   const std::array<float, 2> drawBegin = {toPad(begin.first), toPad(begin.second)};
   const std::array<float, 2> drawEnd = {toPad(end.first), toPad(end.second)};

   auto box = fCanvas->Add<RBox>(RPadPos(drawBegin[0], drawBegin[1]), RPadPos(drawEnd[0], drawEnd[1]));
   box->fill.color = elem.GetColor();
   box->fill.style = RAttrFill::kSolid;
   const std::uint64_t size = elem.GetSize();
   auto text = fCanvas->Add<RText>(RPadPos((drawBegin[0] + drawEnd[0]) / 2.0f, (drawBegin[1] + drawEnd[1]) / 2.0f),
                                   elem.GetName() + " (" + std::to_string(size) + ")");
   text->text.align = RAttrText::kCenter;
   text->text.size = textSizeFactor / (depth + 1);
   text->text.angle = (sliceVertical) ? 0 : 90;

   float currentPos = (sliceVertical) ? begin.first : begin.second;
   for (std::uint64_t i = 0; i < elem.GetNChildren(); ++i) {
      const RTreeMappable &child = fNodes[elem.GetChildrenIdx() + i];
      const std::uint64_t childSize = child.GetSize();
      if (childSize > 0) {
         float frac = static_cast<float>(childSize) / static_cast<float>(size);
         std::pair<float, float> nextBegin = begin;
         std::pair<float, float> nextEnd = end;
         if (sliceVertical) {
            nextBegin.first = currentPos;
            nextEnd.first = currentPos + frac * (end.first - begin.first);
            currentPos = nextEnd.first;
         } else {
            nextBegin.second = currentPos;
            nextEnd.second = currentPos + frac * (end.second - begin.second);
            currentPos = nextEnd.second;
         }
         DrawTreeMap(child, nextBegin, nextEnd, depth + 1);
      }
   }
}
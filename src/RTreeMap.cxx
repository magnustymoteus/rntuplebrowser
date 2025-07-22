#include "RTreeMap.hxx"
#include <ROOT/RBox.hxx>
#include <ROOT/RText.hxx>

void RTreeMap::DrawTreeMap(const std::shared_ptr<const RTreeMappable> &currentElem,
                           const std::pair<float, float> &begin, const std::pair<float, float> &end, int depth) const
{
   const bool sliceVertical = depth % 2 == 0;

   auto toPad = [&](float u) { return 0.25f + u * 0.5f; };
   const float textDepthOffset = 0.005f, textSizeFactor = 0.06f;

   const std::pair<float, float> drawBegin = {toPad(begin.first), toPad(begin.second)};
   const std::pair<float, float> drawEnd = {toPad(end.first), toPad(end.second)};

   fCanvas->Draw<RBox>(RPadPos(drawBegin.first, drawBegin.second), RPadPos(drawEnd.first, drawEnd.second));
   const std::uint64_t size = currentElem->GetSize();
   auto text =
      fCanvas->Add<RText>(RPadPos((drawBegin.first + drawEnd.first) / 2.0f, (drawBegin.second + drawEnd.second) / 2.0f),
                          currentElem->GetName() + " (" + std::to_string(size) + ")");
   text->text.align = RAttrText::kCenter;
   text->text.size = textSizeFactor / (depth + 1);
   text->text.angle = (sliceVertical) ? 0 : 90;
   std::pair<float, float> current = begin;
   for (const auto &child : currentElem->GetChildren()) {
      const std::uint64_t childSize = child->GetSize();
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
         DrawTreeMap(child, nextBegin, nextEnd, depth + 1);
      }
   }
}
#include "RTreeMap.hxx"
#include <ROOT/RBox.hxx>
#include <ROOT/RText.hxx>

#define TREEMAP_TEXTCOLOR RColor(255, 255, 255)
constexpr float HEADER_HEIGHT = 0.05f;
constexpr float PAD_TEXT_OFFSET = 0.01f;
constexpr float TEXT_SIZE_FACTOR = 0.02f;

// Helper to add text with consistent styling
inline void AddStyledText(RCanvas *canvas, RPadPos pos, const std::string &content, RAttrText::EAlign align)
{
   auto text = canvas->Add<RText>(pos, content);
   text->text.align = align;
   text->text.size = TEXT_SIZE_FACTOR;
   text->text.color = TREEMAP_TEXTCOLOR;
}

// Draws a tree map visualization for the given element recursively
void RTreeMap::DrawTreeMap(const RTreeMappable &elem, std::pair<float, float> begin, std::pair<float, float> end,
                           int depth) const
{
   const bool sliceVertical = depth % 2 == 0;
   if (elem.GetNChildren() > 0)
      end.second -= HEADER_HEIGHT;

   auto toPad = [](float u) { return 0.25f + u * 0.5f; };
   std::array<float, 2> drawBegin = {toPad(begin.first), toPad(begin.second)};
   std::array<float, 2> drawEnd = {toPad(end.first), toPad(end.second)};
   const std::uint64_t size = elem.GetSize();
   const std::string label = elem.GetName() + " (" + std::to_string(size) + "B)";

   if (elem.GetNChildren() > 0) {
      auto windowBox =
         fCanvas->Add<RBox>(RPadPos(drawBegin[0], drawEnd[1]), RPadPos(drawEnd[0], drawEnd[1] + HEADER_HEIGHT * 0.5f));
      windowBox->fill.color = RColor(100, 100, 100);
      windowBox->fill.style = RAttrFill::kSolid;

      AddStyledText(fCanvas.get(), RPadPos(drawBegin[0] + PAD_TEXT_OFFSET, drawEnd[1] + HEADER_HEIGHT * 0.25f), label,
                    RAttrText::kLeftCenter);
   } else {
      auto box = fCanvas->Add<RBox>(RPadPos(drawBegin[0], drawBegin[1]), RPadPos(drawEnd[0], drawEnd[1]));
      box->fill.color = elem.GetColor();
      box->fill.style = RAttrFill::kSolid;

      AddStyledText(fCanvas.get(), RPadPos((drawBegin[0] + drawEnd[0]) / 2.0f, (drawBegin[1] + drawEnd[1]) / 2.0f),
                    label, RAttrText::kCenter);
   }

   float currentPos = sliceVertical ? begin.first : begin.second;
   for (std::uint64_t i = 0; i < elem.GetNChildren(); ++i) {
      const RTreeMappable &child = fNodes[elem.GetChildrenIdx() + i];
      const std::uint64_t childSize = child.GetSize();
      if (childSize == 0)
         continue;

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
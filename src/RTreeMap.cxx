#include "RTreeMap.hxx"
#include <ROOT/RBox.hxx>
#include <ROOT/RText.hxx>
#include <iomanip>

#define TREEMAP_TEXTCOLOR RColor(255, 255, 255)
constexpr float INDENTATION_OFFSET = 0.0125f;
constexpr float PAD_TEXT_OFFSET = 0.005f;
constexpr float TEXT_SIZE_FACTOR = 0.01f;

inline void AddStyledText(RCanvas *canvas, RPadPos pos, const std::string &content, RAttrText::EAlign align)
{
   auto text = canvas->Add<RText>(pos, content);
   text->text.align = align;
   text->text.size = TEXT_SIZE_FACTOR;
   text->text.color = TREEMAP_TEXTCOLOR;
}
static std::string GetDataStr(const uint64_t &bytes)
{
   const std::vector<std::string> &units{"B", "KB", "MB", "GB", "TB", "PB", "EB"};
   const uint64_t &order = std::log10(bytes) / 3.0f;
   const std::string &unit = units[order];
   const float &finalSize = static_cast<float>(bytes) / std::pow(1000, order);
   std::stringstream stream;
   stream << std::fixed << std::setprecision(2) << finalSize;
   return stream.str() + unit;
}

void RTreeMap::DrawLegend() const
{
   uint8_t i = 0;
   for (const auto &entry : fColumnLegend) {
      const auto &offset = 0.9f, factor = 0.05f;
      const auto &posY = offset - i * factor;
      auto box = fCanvas->Add<RBox>(RPadPos(offset, posY), RPadPos(offset + factor, posY - factor));
      box->fill.color = entry.second;
      box->fill.style = RAttrFill::kSolid;

      auto text = fCanvas->Add<RText>(RPadPos(offset + factor, posY - 0.025f), entry.first);
      text->text.align = RAttrText::kLeftCenter;
      text->text.size = TEXT_SIZE_FACTOR;

      i++;
   }
}

// Draws a tree map visualization for the given element recursively
void RTreeMap::DrawTreeMap(const RTreeMappable &elem, std::pair<float, float> begin, std::pair<float, float> end,
                           int depth) const
{
   const bool sliceVertical = depth % 2 == 0;
   if (elem.GetNChildren() > 0) {
      end.first -= INDENTATION_OFFSET;
      end.second -= INDENTATION_OFFSET;
      begin.first += INDENTATION_OFFSET;
      begin.second += INDENTATION_OFFSET;
   }

   auto toPad = [](float u) { return 0.125f + u * 0.75f; };
   std::array<float, 2> drawBegin = {toPad(begin.first), toPad(begin.second)};
   std::array<float, 2> drawEnd = {toPad(end.first), toPad(end.second)};
   const std::uint64_t size = elem.GetSize();
   const std::string label = elem.GetName() + " (" + GetDataStr(size) + ")";

   if (elem.GetNChildren() > 0) {
      auto windowBox = fCanvas->Add<RBox>(RPadPos(drawBegin[0], drawBegin[1]), RPadPos(drawEnd[0], drawEnd[1]));
      windowBox->fill.color = RColor(100, 100, 100);
      windowBox->fill.style = RAttrFill::kSolid;
      windowBox->border.color = RColor::kWhite;

      AddStyledText(fCanvas.get(), RPadPos(drawBegin[0] + PAD_TEXT_OFFSET, drawEnd[1] + PAD_TEXT_OFFSET), label,
                    RAttrText::kLeftCenter);
   } else {
      auto box = fCanvas->Add<RBox>(RPadPos(drawBegin[0], drawBegin[1]), RPadPos(drawEnd[0], drawEnd[1]));
      box->fill.color = elem.GetColor();
      box->fill.style = RAttrFill::kSolid;
      box->border.color = RColor::kWhite;

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
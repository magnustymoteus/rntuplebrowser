#include "RTreeMap.hxx"
#include <ROOT/RBox.hxx>
#include <ROOT/RText.hxx>
#include <iomanip>

using Point = std::array<float, 2>;
using Rect = std::array<Point, 2>;

#define TREEMAP_TEXTCOLOR RColor(255, 255, 255)
constexpr float INDENTATION_OFFSET = 0.015f;
constexpr float PAD_TEXT_OFFSET = 0.005f;
constexpr float TEXT_SIZE_FACTOR = 0.01f;

static std::string GetDataStr(const uint64_t &bytes)
{
   const std::vector<std::string> units{"B", "KB", "MB", "GB", "TB", "PB", "EB"};
   const uint64_t order = std::log10(bytes) / 3.0f;
   const std::string unit = units[order];
   const float finalSize = static_cast<float>(bytes) / std::pow(1000, order);
   std::stringstream stream;
   stream << std::fixed << std::setprecision(2) << finalSize;
   return stream.str() + unit;
}

void RTreeMap::DrawLegend() const
{
   uint8_t index = 0;
   for (const auto &entry : fColumnLegend) {
      const auto offset = 0.9f, factor = 0.05f;
      const auto posY = offset - index * factor;
      auto box = fBoxPad->Add<RBox>(RPadPos(offset, posY), RPadPos(offset + factor, posY - factor));
      box->fill.color = entry.second;
      box->fill.style = RAttrFill::kSolid;

      auto text = fTextPad->Add<RText>(RPadPos(offset + factor, posY - 0.025f), entry.first);
      text->text.align = RAttrText::kLeftCenter;
      text->text.size = TEXT_SIZE_FACTOR;

      index++;
   }
}

/* algorithm: https://vanwijk.win.tue.nl/stm.pdf */
static float ComputeWorstRatio(const std::vector<RTreeMappable> &row, float width, float height, uint64_t totalSize,
                               bool horizontalRows)
{
   if (row.empty())
      return 0.0f;
   uint64_t sumRow = 0;
   for (const auto &child : row)
      sumRow += child.GetSize();
   if (sumRow == 0)
      return 0.0f;
   float worstRatio = 0.0f;
   for (const auto &child : row) {
      float ratio = horizontalRows
                       ? static_cast<float>(child.GetSize() * width * totalSize) / (sumRow * sumRow * height)
                       : static_cast<float>(child.GetSize() * height * totalSize) / (sumRow * sumRow * width);
      float aspectRatio = std::max(ratio, 1.0f / ratio);
      if (aspectRatio > worstRatio)
         worstRatio = aspectRatio;
   }
   return worstRatio;
}

static std::vector<std::pair<RTreeMappable, Rect>> SquarifyChildren(const std::vector<RTreeMappable> &children,
                                                                    Point begin, Point end, bool horizontalRows,
                                                                    uint64_t totalSize)
{
   float width = end[0] - begin[0];
   float height = end[1] - begin[1];
   std::vector<RTreeMappable> remainingChildren = children;
   std::sort(remainingChildren.begin(), remainingChildren.end(),
             [](const RTreeMappable &a, const RTreeMappable &b) { return a.GetSize() > b.GetSize(); });
   std::vector<std::pair<RTreeMappable, Rect>> result;
   Point remainingBegin = begin;
   while (!remainingChildren.empty()) {
      std::vector<RTreeMappable> row;
      float currentWorstRatio = std::numeric_limits<float>::max();
      float remainingWidth = end[0] - remainingBegin[0];
      float remainingHeight = end[1] - remainingBegin[1];
      if (remainingWidth <= 0 || remainingHeight <= 0)
         break;
      while (!remainingChildren.empty()) {
         row.push_back(remainingChildren.front());
         remainingChildren.erase(remainingChildren.begin());
         float newWorstRatio = ComputeWorstRatio(row, remainingWidth, remainingHeight, totalSize, horizontalRows);
         if (newWorstRatio > currentWorstRatio) {
            remainingChildren.insert(remainingChildren.begin(), row.back());
            row.pop_back();
            break;
         }
         currentWorstRatio = newWorstRatio;
      }
      uint64_t sumRow = 0;
      for (const auto &child : row)
         sumRow += child.GetSize();
      if (sumRow == 0)
         continue;
      float dimension = horizontalRows ? (static_cast<float>(sumRow) / totalSize * height)
                                       : (static_cast<float>(sumRow) / totalSize * width);
      float position = 0.0f;
      for (const auto &child : row) {
         float childDimension = static_cast<float>(child.GetSize()) / sumRow * (horizontalRows ? width : height);
         Point childBegin = horizontalRows ? Point{remainingBegin[0] + position, remainingBegin[1]}
                                           : Point{remainingBegin[0], remainingBegin[1] + position};
         Point childEnd = horizontalRows
                             ? Point{remainingBegin[0] + position + childDimension, remainingBegin[1] + dimension}
                             : Point{remainingBegin[0] + dimension, remainingBegin[1] + position + childDimension};
         result.push_back({child, {childBegin, childEnd}});
         position += childDimension;
      }
      if (horizontalRows)
         remainingBegin[1] += dimension;
      else
         remainingBegin[0] += dimension;
   }
   return result;
}
/* */

void RTreeMap::DrawTreeMap(const RTreeMappable &element, Point begin, Point end, int depth) const
{
   auto toPad = [](float u) { return 0.125f + u * 0.75f; };
   Point drawBegin = {toPad(begin[0]), toPad(begin[1])};
   Point drawEnd = {toPad(end[0]), toPad(end[1])};
   bool isLeaf = (element.GetNChildren() == 0);

   RColor boxColor = isLeaf ? element.GetColor() : RColor(100, 100, 100);
   auto box = fBoxPad->Add<RBox>(RPadPos(drawBegin[0], drawBegin[1]), RPadPos(drawEnd[0], drawEnd[1]));
   box->fill.color = boxColor;
   box->fill.style = RAttrFill::kSolid;
   box->border.color = RColor::kWhite;

   const std::string label = element.GetName() + " (" + GetDataStr(element.GetSize()) + ")";
   RPadPos labelPos = isLeaf ? RPadPos((drawBegin[0] + drawEnd[0]) / 2.0f, (drawBegin[1] + drawEnd[1]) / 2.0f)
                             : RPadPos(drawBegin[0] + PAD_TEXT_OFFSET, drawEnd[1] + PAD_TEXT_OFFSET);
   RAttrText::EAlign align = isLeaf ? RAttrText::kCenter : RAttrText::kLeftCenter;

   auto text = fTextPad->Add<RText>(labelPos, label);
   text->text.align = align;
   float rectWidth = end[0] - begin[0];
   float rectHeight = end[1] - begin[1];
   text->text.size = std::min(std::min(rectWidth, rectHeight) * 0.1f, TEXT_SIZE_FACTOR);
   text->text.color = TREEMAP_TEXTCOLOR;

   if (!isLeaf) {
      float indent = INDENTATION_OFFSET;
      Point innerBegin = {begin[0] + indent, begin[1] + indent};
      Point innerEnd = {end[0] - indent, end[1] - indent};
      std::vector<RTreeMappable> children;
      for (std::uint64_t i = 0; i < element.GetNChildren(); ++i)
         children.push_back(fNodes[element.GetChildrenIdx() + i]);
      uint64_t totalSize = 0;
      for (const auto &child : children)
         totalSize += child.GetSize();
      if (totalSize == 0)
         return;
      float width = innerEnd[0] - innerBegin[0];
      float height = innerEnd[1] - innerBegin[1];
      bool horizontalRows = width > height;
      auto childRects = SquarifyChildren(children, innerBegin, innerEnd, horizontalRows, totalSize);
      for (const auto &[child, rect] : childRects)
         DrawTreeMap(child, rect[0], rect[1], depth + 1);
   }
}
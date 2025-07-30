#include "RTreeMap.hxx"
#include <ROOT/RBox.hxx>
#include <ROOT/RText.hxx>
#include <iomanip>

using namespace ROOT::Experimental;

static constexpr float kIndentationOffset = 0.02f;
static constexpr float kPadTextOffset = 0.004f;
static constexpr float kTextSizeFactor = 0.009f;
static constexpr const char *kUnits[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB"};

static uint64_t ComputeFnv(const std::string &str)
{
   uint64_t h = 14695981039346656037ULL;
   for (char c : str)
      h = (h ^ static_cast<uint8_t>(c)) * 1099511628211ULL;
   return h;
}

static RColor ComputeColor(const std::string &str)
{
   const uint64_t hash = ComputeFnv(str);
   return RColor((hash >> 16) & 0xFF, (hash >> 8) & 0xFF, hash & 0xFF);
}

static std::string GetDataStr(const uint64_t &bytes)
{
   const uint64_t order = std::log10(bytes) / 3.0f;
   const std::string unit = kUnits[order];
   const float finalSize = static_cast<float>(bytes) / std::pow(1000, order);
   std::stringstream stream;
   stream << std::fixed << std::setprecision(2) << finalSize << unit;
   return stream.str();
}

void RTreeMap::DrawLegend(const std::set<std::string> &legend) const
{
   uint8_t index = 0;
   for (const auto &entry : legend) {
      const auto offset = 0.9f, factor = 0.05f;
      const auto posY = offset - index * factor;
      auto box = fBoxPad->Add<RBox>(RPadPos(offset, posY), RPadPos(offset + factor, posY - factor));
      box->fill.color = ComputeColor(entry);
      box->fill.style = RAttrFill::kSolid;

      auto text = fTextPad->Add<RText>(RPadPos(offset + factor, posY - 0.025f), entry);
      text->text.align = RAttrText::kLeftCenter;
      text->text.size = kTextSizeFactor;

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

static std::vector<std::pair<RTreeMappable, RTreeMap::RRect>>
SquarifyChildren(const std::vector<RTreeMappable> &children, RTreeMap::RVec2 begin, RTreeMap::RVec2 end,
                 bool horizontalRows, uint64_t totalSize)
{
   float width = end.x - begin.x;
   float height = end.y - begin.y;
   std::vector<RTreeMappable> remainingChildren = children;
   std::sort(remainingChildren.begin(), remainingChildren.end(),
             [](const RTreeMappable &a, const RTreeMappable &b) { return a.GetSize() > b.GetSize(); });
   std::vector<std::pair<RTreeMappable, RTreeMap::RRect>> result;
   RTreeMap::RVec2 remainingBegin = begin;
   while (!remainingChildren.empty()) {
      std::vector<RTreeMappable> row;
      float currentWorstRatio = std::numeric_limits<float>::max();
      float remainingWidth = end.x - remainingBegin.x;
      float remainingHeight = end.y - remainingBegin.y;
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
         RTreeMap::RVec2 childBegin = horizontalRows ? RTreeMap::RVec2{remainingBegin.x + position, remainingBegin.y}
                                                     : RTreeMap::RVec2{remainingBegin.x, remainingBegin.y + position};
         RTreeMap::RVec2 childEnd =
            horizontalRows
               ? RTreeMap::RVec2{remainingBegin.x + position + childDimension, remainingBegin.y + dimension}
               : RTreeMap::RVec2{remainingBegin.x + dimension, remainingBegin.y + position + childDimension};
         result.push_back({child, {childBegin, childEnd}});
         position += childDimension;
      }
      if (horizontalRows)
         remainingBegin.y += dimension;
      else
         remainingBegin.x += dimension;
   }
   return result;
}
/* */

void RTreeMap::DrawTreeMap(const RTreeMappable &element, RTreeMap::RVec2 begin, RTreeMap::RVec2 end, int depth) const
{
   auto toPad = [](float u) { return 0.125f + u * 0.75f; };
   RTreeMap::RVec2 drawBegin = {toPad(begin.x), toPad(begin.y)};
   RTreeMap::RVec2 drawEnd = {toPad(end.x), toPad(end.y)};
   bool isLeaf = (element.GetNChildren() == 0);

   RColor boxColor = isLeaf ? ComputeColor(element.GetType()) : RColor(100, 100, 100);
   auto box = fBoxPad->Add<RBox>(RPadPos(drawBegin.x, drawBegin.y), RPadPos(drawEnd.x, drawEnd.y));
   box->fill.color = boxColor;
   box->fill.style = RAttrFill::kSolid;
   box->border.color = RColor(0, 0, 0);
   box->border.width = 0.15f;

   const std::string label = element.GetName() + " (" + GetDataStr(element.GetSize()) + ")";
   RPadPos labelPos = isLeaf ? RPadPos((drawBegin.x + drawEnd.x) / 2.0f, (drawBegin.y + drawEnd.y) / 2.0f)
                             : RPadPos(drawBegin.x + kPadTextOffset, drawEnd.y - kPadTextOffset);
   RAttrText::EAlign align = isLeaf ? RAttrText::kCenter : RAttrText::kLeftTop;

   auto text = fTextPad->Add<RText>(labelPos, label);
   text->text.align = align;
   float rectWidth = end.x - begin.x;
   float rectHeight = end.y - begin.y;
   text->text.size = std::min(std::min(rectWidth, rectHeight) * 0.1f, kTextSizeFactor);
   text->text.color = RColor::kWhite;

   if (!isLeaf) {
      float indent = kIndentationOffset;
      RTreeMap::RVec2 innerBegin = {begin.x + indent, begin.y + indent};
      RTreeMap::RVec2 innerEnd = {end.x - indent, end.y - indent};
      std::vector<RTreeMappable> children;
      for (std::uint64_t i = 0; i < element.GetNChildren(); ++i)
         children.push_back(fNodes[element.GetChildrenIdx() + i]);
      uint64_t totalSize = 0;
      for (const auto &child : children)
         totalSize += child.GetSize();
      if (totalSize == 0)
         return;
      float width = innerEnd.x - innerBegin.x;
      float height = innerEnd.y - innerBegin.y;
      bool horizontalRows = width > height;
      auto childRects = SquarifyChildren(children, innerBegin, innerEnd, horizontalRows, totalSize);
      for (const auto &[child, rect] : childRects)
         DrawTreeMap(child, rect.p1, rect.p2, depth + 1);
   }
}
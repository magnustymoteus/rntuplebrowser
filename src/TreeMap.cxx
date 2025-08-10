//
// Created by patryk on 08.08.25.
//
#include "TreeMap.hxx"

#include <cmath>
#include <queue>
#include <iomanip>
#include <iostream>

#include <ROOT/RColumnElementBase.hxx>


static constexpr float kIndentationOffset = 0.005f;
static constexpr float kPadTextOffset = 0.004f;
static constexpr float kTextSizeFactor = 0.009f;
static constexpr const char *kUnits[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB"};
static constexpr uint8_t kColumnTypes = static_cast<uint8_t>(ROOT::ENTupleColumnType::kMax) + 1;

static uint64_t ComputeFnv(uint64_t a)

{
   const uint64_t FNV_offset = 14695981039346656037ULL;
   const uint64_t FNV_prime = 1099511628211ULL;
   uint64_t h = FNV_offset;
   for (int i = 0; i < 8; ++i) {
      uint8_t octet = static_cast<uint8_t>(a >> (i * 8));
      h ^= octet;
      h *= FNV_prime;
   }
   return h;
}

static TreeMap::RGBColor ComputeColor(uint8_t n)
{
   const uint64_t hash = ComputeFnv(n);
   return TreeMap::RGBColor((hash >> 16) & 0xFF, (hash >> 8) & 0xFF, hash & 0xFF);
}

static std::string GetFloatStr(const float &n, const uint8_t &precision)
{
   std::stringstream stream;
   stream << std::fixed << std::setprecision(precision) << n;
   return stream.str();
}

static std::string GetDataStr(uint64_t bytes)
{
   const uint64_t order = std::log10(bytes) / 3.0f;
   const std::string unit = kUnits[order];
   const float finalSize = static_cast<float>(bytes) / std::pow(1000, order);
   return GetFloatStr(finalSize, 2) + unit;
}

struct DiskOccupation {
   uint64_t fSize;
   std::string fName;
   DiskOccupation(const std::string &name, uint64_t size) : fSize(size), fName(name) {}
   bool operator<(const DiskOccupation &other) const {
      return fSize < other.fSize;
   }
   bool operator==(const DiskOccupation &other) const {
      return fName == other.fName;
   }
};
static std::array<uint64_t, kColumnTypes> GetDiskOccupation(const std::vector<TreeMapNode> &nodes)
{
   std::array<uint64_t, kColumnTypes> arr {};
   for (const auto &node : nodes) {
      if (node.fNChildren > 0)
         continue;
      arr[node.fType] += node.fSize;
   }
   return arr;
}
/* algorithm: https://vanwijk.win.tue.nl/stm.pdf */
static float ComputeWorstRatio(const std::vector<TreeMapNode> &row, float width, float height, uint64_t totalSize,
                               bool horizontalRows)
{
   if (row.empty())
      return 0.0f;
   uint64_t sumRow = 0;
   for (const auto &child : row)
      sumRow += child.fSize;
   if (sumRow == 0)
      return 0.0f;
   float worstRatio = 0.0f;
   for (const auto &child : row) {
      float ratio = horizontalRows
                       ? static_cast<float>(child.fSize * width * totalSize) / (sumRow * sumRow * height)
                       : static_cast<float>(child.fSize * height * totalSize) / (sumRow * sumRow * width);
      float aspectRatio = std::max(ratio, 1.0f / ratio);
      if (aspectRatio > worstRatio)
         worstRatio = aspectRatio;
   }
   return worstRatio;
}
static std::vector<std::pair<TreeMapNode, TreeMap::Rect>> SquarifyChildren(const std::vector<TreeMapNode> &children,
                                                                              TreeMap::Rect rect, bool horizontalRows,
                                                                              uint64_t totalSize)
{
   float width = rect.fTopRight.x - rect.fBottomLeft.x;
   float height = rect.fTopRight.y - rect.fBottomLeft.y;
   std::vector<TreeMapNode> remainingChildren = children;
   std::sort(remainingChildren.begin(), remainingChildren.end(),
             [](const TreeMapNode &a, const TreeMapNode &b) { return a.fSize > b.fSize; });
   std::vector<std::pair<TreeMapNode, TreeMap::Rect>> result;
   TreeMap::Vec2 remainingBegin = rect.fBottomLeft;
   while (!remainingChildren.empty()) {
      std::vector<TreeMapNode> row;
      float currentWorstRatio = std::numeric_limits<float>::max();
      float remainingWidth = rect.fTopRight.x - remainingBegin.x;
      float remainingHeight = rect.fTopRight.y - remainingBegin.y;
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
         sumRow += child.fSize;
      if (sumRow == 0)
         continue;
      float dimension = horizontalRows ? (static_cast<float>(sumRow) / totalSize * height)
                                       : (static_cast<float>(sumRow) / totalSize * width);
      float position = 0.0f;
      for (const auto &child : row) {
         float childDimension = static_cast<float>(child.fSize) / sumRow * (horizontalRows ? width : height);
         TreeMap::Vec2 childBegin = horizontalRows ? TreeMap::Vec2{remainingBegin.x + position, remainingBegin.y}
                                                     : TreeMap::Vec2{remainingBegin.x, remainingBegin.y + position};
         TreeMap::Vec2 childEnd =
            horizontalRows
               ? TreeMap::Vec2{remainingBegin.x + position + childDimension, remainingBegin.y + dimension}
               : TreeMap::Vec2{remainingBegin.x + dimension, remainingBegin.y + position + childDimension};
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
void TreeMap::DrawLegend() const
{
   const std::array<uint64_t, kColumnTypes> &diskOccupation = GetDiskOccupation(fNodes);
   //std::sort(diskOccupation.begin(), diskOccupation.end(), std::greater<>())

   for (uint8_t index=0, counter=0;index<diskOccupation.size();index++) {
      const auto &entry = diskOccupation[index];
      if(entry == 0) continue;
      const auto offset = 0.835f, factor = 0.05f;
      const auto posY = offset - counter * factor;
      const std::string &typeName = ROOT::Internal::RColumnElementBase::GetColumnTypeName(static_cast<ROOT::ENTupleColumnType>(index));
      AddBox(Rect(Vec2(offset, posY), Vec2(offset+factor, posY-factor)), ComputeColor(index));

      const float percent = (entry / static_cast<float>(fNodes[0].fSize)) * 100.0f;
      const auto &content = "(" + GetDataStr(entry) + " / " + GetDataStr(fNodes[0].fSize) + ")";

      float currOffset = 0.0125f;
      for (const auto &currContent : {typeName, content, GetFloatStr(percent, 2) + "%"}) {
         AddText(Vec2(offset+factor, posY-currOffset), currContent, kTextSizeFactor);
         currOffset += 0.01f;
      }
      counter++;
   }
}
void TreeMap::DrawTreeMap(const TreeMapNode &element, TreeMap::Rect rect, int depth) const
{
   TreeMap::Rect drawRect = TreeMap::Rect(TreeMap::Vec2(rect.fBottomLeft.x, rect.fBottomLeft.y),
                                              TreeMap::Vec2(rect.fTopRight.x, rect.fTopRight.y));
   bool isLeaf = (element.fNChildren == 0);
   TreeMap::RGBColor boxColor = isLeaf ? ComputeColor(element.fType) : TreeMap::RGBColor(100, 100, 100);
   AddBox(drawRect, boxColor, 0.15f);

   const std::string label = element.fName + " (" + GetDataStr(element.fSize) + ")";
   const Vec2 &labelPos = isLeaf ? Vec2((drawRect.fBottomLeft.x + drawRect.fTopRight.x) / 2.0f,
                                       (drawRect.fBottomLeft.y + drawRect.fTopRight.y) / 2.0f)
                             : Vec2(drawRect.fBottomLeft.x + kPadTextOffset, drawRect.fTopRight.y - kPadTextOffset);
   bool align = isLeaf ? true : false;

   float rectWidth = rect.fTopRight.x - rect.fBottomLeft.x;
   float rectHeight = rect.fTopRight.y - rect.fBottomLeft.y;
   float textSize = std::min(std::min(rectWidth, rectHeight) * 0.1f, kTextSizeFactor);
   AddText(labelPos, label, textSize, TreeMap::RGBColor(255,255,255), isLeaf);

   if (!isLeaf) {
      float indent = kIndentationOffset;
      TreeMap::Rect innerRect =
         TreeMap::Rect(TreeMap::Vec2(rect.fBottomLeft.x + indent, rect.fBottomLeft.y + indent),
                         TreeMap::Vec2(rect.fTopRight.x - indent, rect.fTopRight.y - indent * 4.0f));
      std::vector<TreeMapNode> children;
      for (std::uint64_t i = 0; i < element.fNChildren; ++i)
         children.push_back(fNodes[element.fChildrenIdx + i]);
      uint64_t totalSize = 0;
      for (const auto &child : children)
         totalSize += child.fSize;
      if (totalSize == 0)
         return;
      float width = innerRect.fTopRight.x - innerRect.fBottomLeft.x;
      float height = innerRect.fTopRight.y - innerRect.fBottomLeft.y;
      bool horizontalRows = width > height;
      auto childRects = SquarifyChildren(children, innerRect, horizontalRows, totalSize);
      for (const auto &[child, rect] : childRects)
         DrawTreeMap(child, rect, depth + 1);
   }
}
static TreeMapNode CreateTreeMappable(const ROOT::Experimental::RNTupleInspector &insp, const ROOT::RFieldDescriptor &fldDesc,
                                       std::uint64_t childrenIdx, std::uint64_t nChildren, ROOT::DescriptorId_t rootId,
                                       size_t rootSize)
{
   uint64_t size =
      (rootId != fldDesc.GetId()) ? insp.GetFieldTreeInspector(fldDesc.GetId()).GetCompressedSize() : rootSize;
   auto type = fldDesc.GetTypeChecksum();
   return TreeMapNode(fldDesc.GetFieldName(), type ? type.value() : 0, size, childrenIdx, nChildren);
}
static TreeMapNode CreateTreeMappable(const ROOT::Experimental::RNTupleInspector::RColumnInspector &colInsp, std::uint64_t childrenIdx)
{
   return TreeMapNode("", static_cast<uint8_t>(colInsp.GetType()),
                       colInsp.GetCompressedSize(), childrenIdx, 0);
}
TreeMap::TreeMap(const ROOT::Experimental::RNTupleInspector &insp)
{
   const auto &descriptor = insp.GetDescriptor();
   const auto rootId = descriptor.GetFieldZero().GetId();
   size_t rootSize = 0;
   for (const auto &childId : descriptor.GetFieldDescriptor(rootId).GetLinkIds()) {
      rootSize += insp.GetFieldTreeInspector(childId).GetCompressedSize();
   }

   std::queue<std::pair<uint64_t, bool>> queue; // (columnid/fieldid, isfield)
   queue.push({rootId, true});
   while (!queue.empty()) {
      size_t levelSize = queue.size();
      size_t levelChildrenStart = fNodes.size() + levelSize;
      for (size_t i = 0; i < levelSize; ++i) {
         const auto &current = queue.front();
         queue.pop();

         std::vector<uint64_t> children;
         size_t nChildren = 0;
         if (current.second) {
            const auto &fldDesc = descriptor.GetFieldDescriptor(current.first);
            children = fldDesc.GetLinkIds();
            for (const auto childId : children) {
               queue.push({childId, 1});
            }
            for (const auto &columnDesc : descriptor.GetColumnIterable(fldDesc.GetId())) {
               const auto &columnId = columnDesc.GetPhysicalId();
               children.push_back(columnId);
               queue.push({columnId, 0});
            }
            nChildren = children.size();
            const auto &node = CreateTreeMappable(insp, fldDesc, levelChildrenStart, nChildren, rootId, rootSize);
            fNodes.push_back(node);
         } else {
            const auto &colInsp = insp.GetColumnInspector(current.first);
            const auto &node = CreateTreeMappable(colInsp, levelChildrenStart);
            fNodes.push_back(node);
         }

         levelChildrenStart += nChildren;
      }
   }
}
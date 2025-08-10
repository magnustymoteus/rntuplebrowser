#include "RTreeMap.hxx"
#include <ROOT/RBox.hxx>
#include <ROOT/RText.hxx>

/* */
using namespace ROOT::Experimental;
void RTreeMap::AddText(const Vec2 &pos, const std::string &content, float size, const RGBColor &color, bool alignCenter) const {
   auto text = fTextPad->Add<RText>(RPadPos(pos.x, pos.y), content);
   text->text.align = alignCenter ? RAttrText::kCenter : RAttrText::kLeftTop;
   text->text.size = size;
   text->text.color = RColor(color.r, color.g, color.b, color.a);
}
void RTreeMap::AddBox(const Rect &rect, const RGBColor &color, float borderWidth) const {
   auto box = fBoxPad->Add<RBox>(RPadPos(rect.fBottomLeft.x, rect.fBottomLeft.y),
                                RPadPos(rect.fTopRight.x, rect.fTopRight.y));
   box->fill.color = RColor(color.r, color.g, color.b, color.a);
   box->fill.style = RAttrFill::kSolid;
   box->border.color = RColor(0, 0, 0);
   box->border.width = borderWidth;
}

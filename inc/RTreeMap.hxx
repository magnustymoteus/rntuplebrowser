#ifndef TREEMAP_HXX
#define TREEMAP_HXX

#include "TreeMap.hxx"

#include <ROOT/RDrawable.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RNTuple.hxx>
#include <ROOT/RPad.hxx>
#include <set>

namespace ROOT::Experimental {

class RTreeMap final : public RDrawable, public TreeMap {
public:
      RTreeMap(std::shared_ptr<RCanvas> canvas)
    : TreeMap(), RDrawable("treemap"),
      fBoxPad(canvas->AddPad(RPadPos(0, 0), RPadExtent(1, 1))),
      fTextPad(canvas->AddPad(RPadPos(0, 0), RPadExtent(1, 1)))
   {
      DrawTreeMap(fNodes[0], Rect(Vec2(0.025, 0.05), Vec2(0.825, 0.9)), 0);
      DrawLegend();
   }
   RTreeMap(std::shared_ptr<RCanvas> canvas, const ROOT::Experimental::RNTupleInspector &insp)
      : TreeMap(insp), RDrawable("treemap"),
        fBoxPad(canvas->AddPad(RPadPos(0, 0), RPadExtent(1, 1))),
        fTextPad(canvas->AddPad(RPadPos(0, 0), RPadExtent(1, 1)))
   {
      DrawTreeMap(fNodes[0], Rect(Vec2(0.025, 0.05), Vec2(0.825, 0.9)), 0);
      DrawLegend();
   }
private:
   std::shared_ptr<RPad> fBoxPad;
   std::shared_ptr<RPad> fTextPad;
   void AddBox(const Rect &rect, const RGBColor &color, float borderWidth) const final;
   void AddText(const Vec2 &pos, const std::string &content, float size, const RGBColor &color=RGBColor(0,0,0), bool alignCenter=false) const final;
};
} // namespace ROOT::Experimental

#endif
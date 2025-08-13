//
// Created by patryk on 04.08.25.
//
#ifndef TTREEMAP_HXX
#define TTREEMAP_HXX

#include "RTreeMapBase.hxx"

#include "TROOT.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TPad.h"

#include <vector>

class TTreeMap final : public ROOT::Experimental::RTreeMapBase, public TObject {
public:
   struct Node final : public ROOT::Experimental::RTreeMapBase::Node, public TObject {
   public:
      ClassDef(Node, 1)
   };
   TTreeMap() = default;
   void Paint(Option_t *opt) override;

   ClassDefOverride(TTreeMap, 1) private
      : void AddBox(const Rect &rect, const RGBColor &color, float borderWidth) const final;
   void AddText(const Vec2 &pos, const std::string &content, float size, const RGBColor &color = RGBColor(0, 0, 0),
                bool alignCenter = false) const final;
};
#endif

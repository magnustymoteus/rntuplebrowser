//
// Created by patryk on 04.08.25.
//
#include "RTreeMapPainter.hxx"

#include <TCanvas.h>
#include <TPad.h>
#include <TBox.h>
#include <TLatex.h>
#include <TColor.h>

#include <cmath>

void ROOT::Experimental::RTreeMapPainter::Paint(Option_t *)
{
   if (!gPad)
      return;
   gPad->Clear();
   gPad->Range(0, 0, 1, 1);
   gPad->cd();
   DrawTreeMap(fNodes[0], Rect(Vec2(0.025, 0.05), Vec2(0.825, 0.9)), 0);
   DrawLegend();
}
void ROOT::Experimental::RTreeMapPainter::AddBox(const Rect &rect, const RGBColor &color, float borderWidth) const
{
   auto box = new TBox(rect.fBottomLeft.x, rect.fBottomLeft.y, rect.fTopRight.x, rect.fTopRight.y);
   box->SetFillColor(TColor::GetColor(color.r, color.g, color.b, color.a));
   box->SetLineColor(kGray);
   box->SetLineWidth(std::ceil(borderWidth));
   box->Draw("l");
}
void ROOT::Experimental::RTreeMapPainter::AddText(const Vec2 &pos, const std::string &content, float size,
                                                  const RGBColor &color, bool alignCenter) const
{
   TLatex t;
   t.SetTextFont(42);
   t.SetTextSize(size);
   t.SetTextAlign((alignCenter) ? 22 : 13);
   t.SetTextColor(TColor::GetColor(color.r, color.g, color.b, color.a));
   t.DrawLatex(pos.x, pos.y, content.c_str());
}

//
// Created by patryk on 16.07.25.
//
#include <RNTupleBrowser.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RBox.hxx>
#include <ROOT/RText.hxx>
#include <iostream>

void RNTupleBrowser::Browse() const {
    auto box1 = _canvas->Draw<RBox>(RPadPos(0.25_normal, 0.25_normal), RPadPos(0.75_normal, 0.75_normal));
    auto text = _canvas->Add<RText>(RPadPos(0.25_normal, 0.74_normal), "SomeField");
    text->text.align = RAttrText::kLeftBottom;
    _canvas->Show();
}
//
// Created by patryk on 16.07.25.
//
#include <RNTupleBrowser.hxx>
#include <ROOT/RNTupleInspector.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RBox.hxx>
#include <ROOT/RText.hxx>
#include <iostream>

RNTupleBrowser::RNTupleBrowser(const std::string_view tupleName, const std::string_view storage) {
    _canvas = RCanvas::Create("RNTupleBrowser");
    auto box1 = _canvas->Draw<RBox>(RPadPos(0.25_normal, 0.25_normal), RPadPos(0.75_normal, 0.75_normal));
    auto text = _canvas->Add<RText>(RPadPos(0.25_normal, 0.74_normal), "SomeField");
    text->text.align = RAttrText::kLeftBottom;

    _reader = ROOT::RNTupleReader::Open(tupleName, storage);
    _canvas->Show();
}
void RNTupleBrowser::Browse() const {

}
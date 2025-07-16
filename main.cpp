#include <ROOT/RCanvas.hxx>
#include <ROOT/RBrowser.hxx>
#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleReader.hxx>
#include "ROOT/RBox.hxx"
#include "ROOT/RText.hxx"
#include "RNTupleBrowser.hxx"
#include <TFile.h>
#include <TApplication.h>

using namespace ROOT::Experimental;

void DisplayCanvas() {
    auto canvas = RCanvas::Create("Hello world!");
    auto box1 = canvas->Draw<RBox>(RPadPos(0.25_normal, 0.25_normal), RPadPos(0.75_normal, 0.75_normal));
    auto text = canvas->Add<RText>(RPadPos(0.25_normal, 0.74_normal), "SomeField");
    text->text.align = RAttrText::kLeftBottom;
    auto browser = RNTupleBrowser();
    browser.Test();
    canvas->Show();
}
void DisplayBrowser() {
    auto browser = new ROOT::RBrowser("Hello there!");
}
void ReadModel(const std::string &fileName, const std::string &tupleName) {
    auto reader = ROOT::RNTupleReader::Open(tupleName.c_str(), fileName.c_str());
}
int main(int argc, char** argv) {
    TApplication app("ROOT Application", &argc, argv);
    DisplayCanvas();
    //DisplayBrowser();
    app.Run();
    return 0;
}

#include <ROOT/RCanvas.hxx>
#include <ROOT/RBrowser.hxx>
#include <ROOT/RNTuple.hxx>
#include <ROOT/RNTupleReader.hxx>
#include <TFile.h>
#include <TApplication.h>

using namespace ROOT::Experimental;

void DisplayCanvas() {
    auto canvas = RCanvas::Create("Hello world!");
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

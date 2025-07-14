#include <ROOT/RCanvas.hxx>
#include <ROOT/RText.hxx>
#include <ROOT/RLine.hxx>
#include <TApplication.h>

using namespace ROOT::Experimental;

void DisplayCanvas() {
    auto canvas = RCanvas::Create("Hello world!");
    canvas->Show();
}

int main(int argc, char** argv) {
    TApplication app("ROOT Application", &argc, argv);
    DisplayCanvas();
    app.Run();
    return 0;
}

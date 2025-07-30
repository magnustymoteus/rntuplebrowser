#include <ROOT/RCanvas.hxx>
#include <ROOT/RBrowser.hxx>
#include <ROOT/RNTuple.hxx>
#include "RNTupleBrowser.hxx"
#include <TFile.h>
#include <TApplication.h>

void DisplayCanvas()
{
   auto browser = ROOT::Experimental::RNTupleBrowser("mini", "../gg_data.ntuple");
   browser.Browse();
}
int main(int argc, char **argv)
{
   TApplication app("ROOT Application", &argc, argv);
   DisplayCanvas();
   app.Run();
   return 0;
}

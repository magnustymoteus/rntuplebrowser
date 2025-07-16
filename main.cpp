#include <ROOT/RCanvas.hxx>
#include <ROOT/RBrowser.hxx>
#include <ROOT/RNTuple.hxx>
#include "RNTupleBrowser.hxx"
#include <TFile.h>
#include <TApplication.h>

using namespace ROOT::Experimental;

void DisplayCanvas()
{
   auto browser = RNTupleBrowser("new_tuple", "/home/magnustymoteus/Desktop/root_macros/rntuple/new_file.root");
   browser.Browse();
}
void DisplayBrowser()
{
   auto browser = new ROOT::RBrowser("Hello there!");
}
int main(int argc, char **argv)
{
   TApplication app("ROOT Application", &argc, argv);
   DisplayCanvas();
   // DisplayBrowser();
   app.Run();
   return 0;
}

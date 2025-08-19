#include "RTreeMapPainter.hxx"

#include <ROOT/RCanvas.hxx>
#include <ROOT/RBrowser.hxx>
#include <ROOT/RNTuple.hxx>
#include "ROOT/RWebWindowsManager.hxx"

#include <TFile.h>
#include <TApplication.h>
#include "TWebCanvas.h"
#include "TROOT.h"

static const std::string kTupleName = "poster_tuple";
static const std::string kFileName = "../example_file.root";
void DisplayCanvas(bool disableJs=false)
{
  if(!disableJs) {
  ROOT::RWebWindowsManager::AddServerLocation("webcanv", "../webcanv");
  TWebCanvas::AddCustomClass("ROOT::Experimental::RTreeMapPainter");
  TWebCanvas::SetCustomScripts("modules:webcanv/RTreeMapPainter.mjs");
  }
  auto tm = ROOT::Experimental::RTreeMapPainter::Import(kFileName, kTupleName);
  auto c = new TCanvas("c_tm","TreeMap");
  c->Add(tm.release());
}
int main(int argc, char **argv)
{
   TApplication app("ROOT Application", &argc, argv);
   DisplayCanvas(true);
   app.Run();
   return 0;
}

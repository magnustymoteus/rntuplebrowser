#include "TTreeMap.hxx"
#include "RTreeMap.hxx"

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
void DisplayRCanvas()
{
   const auto insp = ROOT::Experimental::RNTupleInspector::Create(kTupleName, kFileName);
   auto canvas = ROOT::Experimental::RCanvas::Create("RNTupleBrowser");
   canvas->Draw<ROOT::Experimental::RTreeMap>(canvas, std::cref(*insp));
   canvas->Show();
}
void DisplayTCanvas(bool disableJs=false)
{
  if(!disableJs) {
  ROOT::RWebWindowsManager::AddServerLocation("webcanv", "../webcanv");
  TWebCanvas::AddCustomClass("TTreeMap");
  TWebCanvas::SetCustomScripts("modules:webcanv/treemap.mjs");
  }
  const auto insp = ROOT::Experimental::RNTupleInspector::Create(kTupleName, kFileName);
  auto tm = new TTreeMap(*insp);
  auto c = new TCanvas("c_tm","TreeMap");
  c->Add(tm);
}
int main(int argc, char **argv)
{
   TApplication app("ROOT Application", &argc, argv);
   //DisplayRCanvas();
   DisplayTCanvas();
   app.Run();
   return 0;
}

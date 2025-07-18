//
// Created by patryk on 16.07.25.
//
#ifndef RNTUPLEBROWSER_HXX
#define RNTUPLEBROWSER_HXX

#include <ROOT/RNTuple.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RNTupleInspector.hxx>
using namespace ROOT::Experimental;
class RNTupleBrowser {
private:
   const std::shared_ptr<RCanvas> _canvas;
   const std::string _tupleName, _storage;

public:
   RNTupleBrowser(const std::string_view tupleName, const std::string_view storage)
      : _canvas(RCanvas::Create("RNTupleBrowser")), _tupleName(tupleName), _storage(storage)
   {
   }
   void Browse() const;
};

#endif // RNTUPLEBROWSER_HXX

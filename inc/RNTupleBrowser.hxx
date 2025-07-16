//
// Created by patryk on 16.07.25.
//
#ifndef RNTUPLEBROWSER_HXX
#define RNTUPLEBROWSER_HXX

#include <ROOT/RNTuple.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RNTupleInspector.hxx>
#include <ROOT/RNTupleDescriptor.hxx>
using namespace ROOT::Experimental;
class RNTupleBrowser {
private:
   const std::shared_ptr<RCanvas> _canvas;
   const std::unique_ptr<RNTupleInspector> _inspector;
   void Treemap(const ROOT::RFieldDescriptor &fieldDesc, const float xBegin = 0.0f, const float xEnd = 1.0,
                const int &depth = 0) const;

public:
   RNTupleBrowser(const std::string_view tupleName, const std::string_view storage)
      : _canvas(RCanvas::Create("RNTupleBrowser")), _inspector(RNTupleInspector::Create(tupleName, storage))
   {
   }
   void Browse() const;
};

#endif // RNTUPLEBROWSER_HXX

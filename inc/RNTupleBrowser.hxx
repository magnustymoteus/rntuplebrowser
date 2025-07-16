//
// Created by patryk on 16.07.25.
//
#ifndef RNTUPLEBROWSER_HXX
#define RNTUPLEBROWSER_HXX

#include <ROOT/RNTuple.hxx>
#include <ROOT/RCanvas.hxx>
#include <ROOT/RNTupleReader.hxx>
#include <ROOT/RNTupleDescriptor.hxx>
using namespace ROOT::Experimental;
class RNTupleBrowser {
    private:
        std::shared_ptr<RCanvas> _canvas;
        std::unique_ptr<ROOT::RNTupleReader> _reader;
    public:
        RNTupleBrowser(const std::string_view tupleName, const std::string_view storage);
        void Browse() const;
};

#endif //RNTUPLEBROWSER_HXX

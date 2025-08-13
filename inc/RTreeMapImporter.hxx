//
// Created by patryk on 13.08.25.
//

#ifndef RNTUPLEBROWSER_RTREEMAPIMPORTER_HXX
#define RNTUPLEBROWSER_RTREEMAPIMPORTER_HXX

#include "TTreeMap.hxx"

namespace ROOT::Experimental {
class RTreeMapImporter {
private:
   std::string fTupleName, fSourceFileName;

public:
   static std::unique_ptr<RTreeMapImporter> Create(std::string_view sourceFileName, std::string_view tupleName);
   TTreeMap *Import() const;
};
} // namespace ROOT::Experimental

#endif // RNTUPLEBROWSER_RTREEMAPIMPORTER_HXX
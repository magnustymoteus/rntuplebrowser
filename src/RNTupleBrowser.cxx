//
// Created by patryk on 16.07.25.
//
#include <RNTupleBrowser.hxx>
#include <ROOT/RCanvas.hxx>
#include "RTreeMap.hxx"
#include <iostream>

void RNTupleBrowser::Browse() const
{
   _canvas->Draw<RTreeMap>(_canvas, _tupleName, _storage);
   _canvas->Show();
}
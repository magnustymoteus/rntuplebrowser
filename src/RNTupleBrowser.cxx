//
// Created by patryk on 16.07.25.
//
#include <RNTupleBrowser.hxx>
#include <ROOT/RCanvas.hxx>
#include "RTreeMap.hxx"
#include <iostream>

void RNTupleBrowser::Browse() const
{
   fCanvas->Draw<RTreeMap>(fCanvas, fTupleName, fStorage);
   fCanvas->Show();
}
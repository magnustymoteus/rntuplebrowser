// Main.cxx
#include "RBrowser.h"         // <-- this is where ROOT::RBrowser is declared
int main() {
    auto browser = new ROOT::RBrowser();
    browser->ls();           // for example, call a method to avoid “unused” warnings
    return 0;
}


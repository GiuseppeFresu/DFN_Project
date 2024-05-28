#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <limits> // Includi la libreria per numeric_limits
#include "Fractures.hpp"

using namespace std;

namespace FractureLibrary
{
    bool ImportDFN(const string& filePath,
                    Fractures& fractures);

    bool ImportFractures(const string& filename,
                        Fractures& fractures);

    void verificaIntersezioni(const Fractures& fractures,
                              map<int, vector<int>>& intersezioni);

    // Definisci un alias per una tupla di tre interi
    using FracturePair = tuple<int, int, int>;

    // Definisci il set per memorizzare le coppie di fratture già verificate
    extern set<FracturePair> coppieVerificate; // Dichiarazione esterna

    // Dichiarazione della variabile epsilon
    extern const double epsilon; //constexpr

    }

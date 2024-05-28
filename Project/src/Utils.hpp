#pragma once

#include <iostream>
#include "Fractures.hpp"

using namespace std;

namespace FractureLibrary
{
    bool ImportDFN(const string& filePath,
                    Fractures& fractures);

    bool ImportFractures(const string& filename,
                        Fractures& fractures);

    vector<unsigned int> FindIntersectingFractures(const Fractures& fractures);

}

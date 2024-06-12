#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <list>
#include <set>
#include "Fractures.hpp"

namespace FractureLibrary
{
    bool ImportFractures(const string& filename, Fractures& fractures);

    void verificaIntersezioni(Fractures& fractures, map<int, vector<int>>& intersezioni);

    Trace calcolaTraccia(const Matrix3Xd& P, const Matrix3Xd& Q, int id1, int id2, int& traceId);

    void sortTracesByLength(Fractures& fractures);

    void writeResults(const Fractures& fractures, const string& filename);

    using FracturePair = tuple<int, int, int>;

    extern set<FracturePair> coppieVerificate;

    extern const double epsilon;


}



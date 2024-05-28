#pragma once

#include <vector>
#include "Eigen/Eigen"

using namespace std;
using namespace Eigen;

namespace FractureLibrary
{
    struct Point
    {
        double x, y, z;
    };

    struct Fractures
    {
        size_t NumberFractures;
        vector<unsigned int> FracturesId;
        vector<Matrix3Xd> FracturesVertices;

        Fractures() : NumberFractures(0){}
    };
}

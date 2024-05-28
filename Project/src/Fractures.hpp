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
        int NumberFractures = 0;
        vector<unsigned int> FracturesId = {};
        vector<Matrix3Xd> FracturesVertices = {};
    };
}

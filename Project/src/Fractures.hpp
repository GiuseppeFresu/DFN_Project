
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

    struct Traccia {
        int id;
        int fractureId1;
        int fractureId2;
        std::array<double, 3> point1;
        std::array<double, 3> point2;
    };
}


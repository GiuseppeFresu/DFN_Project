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

    struct Trace
    {
        int traceId;
        int fractureId1;
        int fractureId2;
        Point p1;
        Point p2;
        bool Tips; // true if non-passing, false if passing
        double length;

        Trace(int id, int f1, int f2, Point pt1, Point pt2, bool nonPassing)
            : traceId(id), fractureId1(f1), fractureId2(f2), p1(pt1), p2(pt2), Tips(nonPassing)
        {
            length = sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2) + pow(p2.z - p1.z, 2));
        }
    };

    struct Fractures
    {
        size_t NumberFractures;
        vector<unsigned int> FracturesId;
        vector<Matrix3Xd> FracturesVertices;
        vector<Trace> Traces;

        Fractures() : NumberFractures(0) {}

        void clear()
        {
            NumberFractures = 0;
            FracturesId.clear();
            FracturesVertices.clear();
            Traces.clear();
        }
    };
}

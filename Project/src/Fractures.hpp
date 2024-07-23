#pragma once

#include <vector>
#include "Eigen/Eigen"

using namespace std;
using namespace Eigen;

namespace FractureLibrary
{
    struct Point
    {
        double x;
        double y;
        double z;

        Point() : x(0), y(0), z(0) {}
        Point(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
    };

    struct Trace
    {
        int traceId;
        int fractureId1;
        int fractureId2;
        Point p1;
        Point p2;
        double length;

        Trace(int id, int f1, int f2, Point pt1, Point pt2)
            : traceId(id), fractureId1(f1), fractureId2(f2), p1(pt1), p2(pt2)
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

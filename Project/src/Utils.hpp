#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <list>
#include <set>
#include <tuple>
#include "Fractures.hpp"

namespace FractureLibrary
{

   extern const double epsilon;

   using FracturePair = tuple<int, int, int>;

   extern set<FracturePair> pairFound;

   bool ImportFractures(const string& filename,
                        Fractures& fractures);

   bool intersection2D(const vector<pair<double, double>>& P,
                       const vector<pair<double, double>>& Q);

   vector<pair<double, double>> projectsOnPlane(const Matrix3Xd& vertices,
                                                const string& plane);

   bool checkSeparation(const Matrix3Xd& P, const Matrix3Xd& Q);

   bool isPointOnEdge(const Point& pt, const Matrix3Xd& vertices);

   Trace calculateTrace(const Matrix3Xd& P, const Matrix3Xd& Q,
                        int id1, int id2, int& traceId);

   void checkIntersections(Fractures& fractures, map<int,
                           vector<int>>& intersections);

   void sortTracesByLength(vector<Trace>& traces);

   void writeTraces(const Fractures& fractures,
                    const string& filename);

   void writeResults(const Fractures& fractures,
                                 const string& filename);

}

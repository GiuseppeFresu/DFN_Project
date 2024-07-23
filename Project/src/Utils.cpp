#include "Utils.hpp"
#include <ostream>
#include <list>
#include <cmath>
#include <algorithm>

namespace FractureLibrary
{

    const double epsilon = 1e-12;

// ***************************************************************************

    bool ImportFractures(const string& filename,
                     Fractures& fractures)
    {
        ifstream file;
        file.open(filename);

        if(file.fail())
        {
            cerr << "File open failed: " <<filename << endl;
            return false;
        }

        list<string> listLines;
        string line;
        while (getline(file, line))
        {
            if (!line.empty() && line[0] == '#')
            {
                continue;
            }

            listLines.push_back(line);
        }

        file.close();

        fractures.NumberFractures = stoi(listLines.front());

        listLines.pop_front();

        fractures.FracturesId.reserve(fractures.NumberFractures);

        if (fractures.NumberFractures == 0)
        {
            cerr << "There is no fractures" << endl;
            return false;
        }

        while (!listLines.empty())
        {
            string line = listLines.front();
            istringstream converter(line);
            unsigned int id;
            int numVertices;
            char delimiter;

            converter >> id >> delimiter >> numVertices;

            listLines.pop_front();

            if (converter.fail() || delimiter != ';')
            {
                cerr << "Error reading fracture header." << endl;
                return false;
            }

            fractures.FracturesId.push_back(id);

            Matrix3Xd vertices(3, numVertices);
            for (int i = 0; i < 3; i++)
            {
                if (listLines.empty())
                {
                    cerr << "Unexpected end of file while reading vertices." << endl;
                    return false;
                }

                string vertexLine = listLines.front();
                listLines.pop_front();
                istringstream vertexStream(vertexLine);
                string value;

                for (int j = 0; j < numVertices; j++)
                {
                    if (!getline(vertexStream, value, ';'))
                    {
                        cerr << "Error reading vertex line." << endl;
                        return false;
                    }

                    vertices(i, j) = stod(value);
                }

                if (vertexStream.fail())
                {
                    cerr << "Error reading vertex coordinates." << endl;
                    return false;
                }

            }

            fractures.FracturesVertices.push_back(vertices);
        }

        return true;
    }

// ***************************************************************************

    bool intersection2D(const vector<pair<double, double>>& P,
                            const vector<pair<double, double>>& Q)
        {
            auto getEdges = [](const vector<pair<double, double>>& poly)
            {
                vector<pair<double, double>> edges(poly.size());
                for (size_t i = 0; i < poly.size(); i++)
                {
                    size_t j = (i + 1) % poly.size();
                    edges[i] = {poly[j].first - poly[i].first,
                                poly[j].second - poly[i].second};
                }
                return edges;
            };

            auto getNormals = [](const vector<pair<double, double>>& edges)
            {
                vector<pair<double, double>> normals(edges.size());
                for (size_t i = 0; i < edges.size(); i++)
                {
                    normals[i] = {-edges[i].second, edges[i].first};
                }
                return normals;
            };

            auto project = [](const vector<pair<double, double>>& poly,
                              const pair<double, double>& axis)
            {
                double min = poly[0].first * axis.first + poly[0].second * axis.second;
                double max = min;
                for (const auto& point : poly)
                {
                    double projection = point.first * axis.first + point.second * axis.second;
                    if (projection < min) min = projection;
                    if (projection > max) max = projection;
                }
                return make_pair(min, max);
            };

            auto overlap = [](const pair<double, double>& p1,
                              const pair<double, double>& p2)
            {
                return !(p1.second + epsilon < p2.first || p2.second + epsilon < p1.first);
            };

            vector<pair<double, double>> edgesP = getEdges(P);
            vector<pair<double, double>> edgesQ = getEdges(Q);
            vector<pair<double, double>> normalsP = getNormals(edgesP);
            vector<pair<double, double>> normalsQ = getNormals(edgesQ);

            for (const auto& axis : normalsP)
            {
                if (!overlap(project(P, axis), project(Q, axis))) return false;
            }

            for (const auto& axis : normalsQ)
            {
                if (!overlap(project(P, axis), project(Q, axis))) return false;
            }

            return true;
        }

// ***************************************************************************

    vector<pair<double, double>> projectsOnPlane(const Matrix3Xd& vertices,
                                                 const string& plane)
    {
        vector<pair<double, double>> projection(vertices.cols());
        if (plane == "XY")
        {
            for (int i = 0; i < vertices.cols(); i++)
            {
                projection[i] = {vertices(0, i), vertices(1, i)};
            }
        }
        else if (plane == "YZ")
        {
            for (int i = 0; i < vertices.cols(); i++)
            {
                projection[i] = {vertices(1, i), vertices(2, i)};
            }
        }
        else if (plane == "ZX")
        {
            for (int i = 0; i < vertices.cols(); i++)
            {
                projection[i] = {vertices(2, i), vertices(0, i)};
            }
        }
        return projection;
    }

// ***************************************************************************

    bool checkSeparation(const Matrix3Xd& P, const Matrix3Xd& Q)
    {
           auto calculateNormal = [](const Vector3d& p1, const Vector3d& p2)
           {
               Vector3d normal = p1.cross(p2);
               return normal.normalized();
           };

           auto isSeparated = [&](const Matrix3Xd& A, const Matrix3Xd& B)
           {
               for (int i = 0; i < A.cols(); i++)
               {
                   Vector3d normal = calculateNormal(A.col(i), A.col((i + 1) % A.cols()));


                   double minA = normal.dot(A.col(0));
                   double maxA = minA;
                   for (int j = 1; j < A.cols(); j++)
                   {
                       double projection = normal.dot(A.col(j));
                       if (projection < minA) minA = projection;
                       if (projection > maxA) maxA = projection;
                   }

                   double minB = normal.dot(B.col(0));
                   double maxB = minB;
                   for (int j = 1; j < B.cols(); j++)
                   {
                       double projection = normal.dot(B.col(j));
                       if (projection < minB) minB = projection;
                       if (projection > maxB) maxB = projection;
                   }

                   if (maxA < minB - epsilon || maxB < minA - epsilon)
                   {
                       return true;
                   }
               }

               return false;
           };

        if (isSeparated(P, Q) || isSeparated(Q, P))
        {
            return true;
        }

        return false;
    }

// ***************************************************************************

    bool intersectPlanes(const Matrix3Xd& vertices1, const Matrix3Xd& vertices2,
                         Vector3d& pt1, Vector3d& pt2)
    {
        Hyperplane<double, 3> plane1 = Hyperplane<double, 3>::Through(vertices1.col(0),
                                                                      vertices1.col(1),
                                                                      vertices1.col(2));
        Hyperplane<double, 3> plane2 = Hyperplane<double, 3>::Through(vertices2.col(0),
                                                                      vertices2.col(1),
                                                                      vertices2.col(2));

        Vector3d normal1 = plane1.normal();
        Vector3d normal2 = plane2.normal();

        if (normal1.cross(normal2).norm() < epsilon)
        {
            return false;
        }

        Vector3d direction = normal1.cross(normal2);
        double denom = direction.dot(direction);

        Vector3d pointOnPlane1 = plane1.normal() * (-plane1.offset());
        Vector3d pointOnPlane2 = plane2.normal() * (-plane2.offset());

        double t1 = (pointOnPlane2 - pointOnPlane1).dot(normal2.cross(direction)) / denom;
        pt1 = pointOnPlane1 + t1 * normal1;

        double t2 = (pointOnPlane1 - pointOnPlane2).dot(normal1.cross(direction)) / denom;
        pt2 = pointOnPlane2 + t2 * normal2;

        return true;
    }

// ***************************************************************************

    bool isPointOnEdges(const Matrix3Xd& points, const Vector3d& pt, double epsilon)
    {
        int numVertices = points.cols();
        for (int i = 0; i < numVertices; ++i)
        {
            int next = (i + 1) % numVertices;

            Vector3d edge = points.col(next) - points.col(i);
            Vector3d toPt = pt - points.col(i);

            double t = toPt.dot(edge) / edge.squaredNorm();

            if (t >= 0 && t <= 1)
            {
                Vector3d projection = points.col(i) + t * edge;
                if ((projection - pt).norm() < epsilon)
                {
                    return true;
                }
            }
        }
        return false;
    }

// ***************************************************************************

    Trace calculateTrace(const Matrix3Xd& P, const Matrix3Xd& Q, int id1, int id2, int& traceId)
    {
        Vector3d pt1, pt2;

        if (intersectPlanes(P, Q, pt1, pt2))
        {
            return Trace(traceId++, id1, id2, {pt1.x(), pt1.y(), pt1.z()}, {pt2.x(), pt2.y(), pt2.z()});
        }
        throw runtime_error("Intersection computation failed between fractures");
    }

// ***************************************************************************

    void checkIntersections(Fractures& fractures, map<int, vector<int>>& intersections)
    {
        const vector<unsigned int>& ids = fractures.FracturesId;
        const vector<Matrix3Xd>& vertices = fractures.FracturesVertices;
        set<FracturePair> pairFound;
        int traceId = 0;

        for (size_t i = 0; i < ids.size(); i++)
        {
            for (size_t j = i + 1; j < ids.size(); j++)
            {
                int id1 = ids[i];
                int id2 = ids[j];

                if (id1 == id2)
                {
                    continue;
                }

                FracturePair pair(id1, id2, fractures.NumberFractures);

                if (pairFound.find(pair) != pairFound.end())
                {
                    continue;
                }

                const Matrix3Xd& P = vertices[i];
                const Matrix3Xd& Q = vertices[j];

                bool intersectionXY = intersection2D(projectsOnPlane(P, "XY"),
                                                     projectsOnPlane(Q, "XY"));
                bool intersectionYZ = intersection2D(projectsOnPlane(P, "YZ"),
                                                     projectsOnPlane(Q, "YZ"));
                bool intersectionZX = intersection2D(projectsOnPlane(P, "ZX"),
                                                     projectsOnPlane(Q, "ZX"));

                if (intersectionXY && intersectionYZ && intersectionZX)
                {
                    if (!checkSeparation(P, Q))
                    {
                        {
                            intersections[id1].push_back(id2);
                            intersections[id2].push_back(id1);
                            pairFound.insert(pair);
                        }

                        try
                        {
                            Trace trace = calculateTrace(P, Q, id1, id2, traceId);

                            fractures.Traces.push_back(trace);
                        }

                        catch (const exception& e)
                        {
                            {
                                cerr <<"Error calculating trace between fractures "
                                     << id1 << " and " << id2 << ": " << e.what() << endl;
                            }
                        }
                    }
                }
            }
        }
    }

// ***************************************************************************

    void sortTracesByLength(vector<Trace>& traces)
    {
        sort(traces.begin(), traces.end(), [](const Trace& a, const Trace& b)
             {
                 return a.length > b.length;
             });
    }

// ***************************************************************************

    void writeTraces(const Fractures& fractures, const string& filename)
    {
        ofstream outFile(filename);
        if (!outFile)
        {
            cerr << "Failed to open file for writing: " << filename << endl;
            return;
        }

        outFile << "# Number of Traces" << endl;
        outFile << fractures.Traces.size() << endl;
        outFile << "# TraceId; FractureId1; FractureId2; X1; Y1; Z1; X2; Y2; Z2" << endl;
        for (const auto& trace : fractures.Traces)
        {
            outFile << trace.traceId << "; "
                    << trace.fractureId1 << "; "
                    << trace.fractureId2 << "; "
                    << trace.p1.x << "; "
                    << trace.p1.y << "; "
                    << trace.p1.z << "; "
                    << trace.p2.x << "; "
                    << trace.p2.y << "; "
                    << trace.p2.z << endl;
        }

        outFile.close();
    }

// ***************************************************************************


    void writeResults(const Fractures& fractures, const string& filename)
        {
            map<int, vector<Trace>> non_passing;
            map<int, vector<Trace>> passing;

            for (const auto& trace : fractures.Traces)
            {
                bool Tips1 = true;
                const auto& vertices1 = fractures.FracturesVertices[trace.fractureId1];
                if (isPointOnEdges(vertices1, Vector3d(trace.p1.x, trace.p1.y, trace.p1.z), epsilon) &&
                    isPointOnEdges(vertices1, Vector3d(trace.p2.x, trace.p2.y, trace.p2.z), epsilon))
                {
                    Tips1 = false;
                }

                if (Tips1)
                {
                    non_passing[trace.fractureId1].push_back(trace);
                }
                else
                {
                    passing[trace.fractureId1].push_back(trace);
                }

                bool Tips2 = true;
                const auto& vertices2 = fractures.FracturesVertices[trace.fractureId2];
                if (isPointOnEdges(vertices2, Vector3d(trace.p1.x, trace.p1.y, trace.p1.z), epsilon) &&
                    isPointOnEdges(vertices2, Vector3d(trace.p2.x, trace.p2.y, trace.p2.z), epsilon))
                {
                    Tips2 = false;
                }

                if (Tips2)
                {
                    non_passing[trace.fractureId2].push_back(trace);
                }
                else
                {
                    passing[trace.fractureId2].push_back(trace);
                }
            }
            for (auto& entry : passing)
            {
                sortTracesByLength(entry.second);
            }
            for (auto& entry : non_passing)
            {
                sortTracesByLength(entry.second);
            }

            ofstream outFile(filename);
            if (!outFile)
            {
                cerr << "Failed to open file for writing: " << filename << endl;
                return;
            }

            outFile << "# FractureId; NumTraces" << endl;
            for (const auto& fractureId : fractures.FracturesId)
            {
                int numTraces = passing[fractureId].size() + non_passing[fractureId].size();

                if (numTraces != 0)
                {
                    outFile << fractureId << "; " << numTraces << endl;
                    outFile << "# TraceId; Tips; Length" << endl;
                    for (const auto& trace : passing[fractureId])
                    {
                        outFile << trace.traceId << "; " << "false" << "; " << trace.length << endl;
                    }
                    for (const auto& trace : non_passing[fractureId])
                    {
                        outFile << trace.traceId << "; " << "true" << "; " << trace.length << endl;
                    }
                }
            }

            outFile.close();
    }

}

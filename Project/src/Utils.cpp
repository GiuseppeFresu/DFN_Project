#include "Utils.hpp"
#include <ostream>
#include <list>
#include <cmath>
#include <algorithm>

namespace FractureLibrary
{

    const double epsilon = numeric_limits<double>::epsilon();

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
            vector<pair<double, double>> edges;
            for (size_t i = 0; i < poly.size(); i++)
            {
                size_t j = (i + 1) % poly.size();
                edges.push_back({poly[j].first - poly[i].first,
                                 poly[j].second - poly[i].second});
            }
            return edges;
        };

        auto getNormals = [](const vector<pair<double, double>>& edges)
        {
            vector<pair<double, double>> normals;
            for (const auto& edge : edges)
            {
                normals.push_back({-edge.second, edge.first});
            }
            return normals;
        };

        auto project = [](const vector<pair<double, double>>& poly, const pair<double, double>& axis)
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

        auto overlap = [](const pair<double, double>& p1, const pair<double, double>& p2)
        {
            return !(p1.second + epsilon < p2.first || p2.second + epsilon < p1.first);
        };

        vector<pair<double, double>> edgesP = getEdges(P);
        vector<pair<double, double>> edgesQ = getEdges(Q);
        vector<pair<double, double>> normalsP = getNormals(edgesP);
        vector<pair<double, double>> normalsQ = getNormals(edgesQ);

        for (const auto& axis : normalsP)
        {
            auto projP = project(P, axis);
            auto projQ = project(Q, axis);
            if (!overlap(projP, projQ)) return false;
        }

        for (const auto& axis : normalsQ)
        {
            auto projP = project(P, axis);
            auto projQ = project(Q, axis);
            if (!overlap(projP, projQ)) return false;
        }

        return true;
    }

// ***************************************************************************

    vector<pair<double, double>> projectsOnPlane(const Matrix3Xd& vertices,
                                             const string& plane)
    {
        vector<pair<double, double>> projection;
        for (int i = 0; i < vertices.cols(); i++)
        {
            if (plane == "XY")
            {
                projection.push_back({vertices(0, i), vertices(1, i)});
            }
            else if (plane == "YZ")
            {
                projection.push_back({vertices(1, i), vertices(2, i)});
            }
            else if (plane == "ZX")
            {
                projection.push_back({vertices(2, i), vertices(0, i)});
            }
        }

        return projection;
    }

// ***************************************************************************

    bool  checkSeparation(const Matrix3Xd& P, const Matrix3Xd& Q)
    {
        for (int i = 0; i < P.cols(); i++)
        {
            Vector3d normalP = P.col(i).cross(P.col((i + 1) % P.cols()));
            bool separateP = true;

            for (int j = 0; j < Q.cols(); j++)
            {
                if (normalP.dot(Q.col(j) - P.col(i)) <= epsilon)
                {
                    separateP = false;
                    break;
                }
            }

            if (separateP) return true;
        }

        for (int i = 0; i < Q.cols(); i++)
        {
            Vector3d normalQ = Q.col(i).cross(Q.col((i + 1) % Q.cols()));
            bool separateQ = true;

            for (int j = 0; j < P.cols(); j++)
            {
                if (normalQ.dot(P.col(j) - Q.col(i)) <= epsilon)
                {
                    separateQ = false;
                    break;
                }
            }

            if (separateQ) return true;
        }

        return false;
    }

// ***************************************************************************

    bool isPointOnEdge(const Point& pt, const Matrix3Xd& vertices)
    {
        for (int i = 0; i < vertices.cols(); ++i)
        {
            Vector3d p1 = vertices.col(i);
            Vector3d p2 = vertices.col((i + 1) % vertices.cols());

            Vector3d v1(pt.x - p1.x(), pt.y - p1.y(), pt.z - p1.z());
            Vector3d v2(p2.x() - p1.x(), p2.y() - p1.y(), p2.z() - p1.z());
            Vector3d crossProduct = v1.cross(v2);
            if (crossProduct.norm() < epsilon)
            {
                double dotProduct = v1.dot(v2);
                if (dotProduct >= 0 && dotProduct <= v2.squaredNorm())
                {
                    return true;
                }
            }
        }

        return false;
    }

// ***************************************************************************

    Trace calculateTrace(const Matrix3Xd& P, const Matrix3Xd& Q,
                     int id1, int id2, int& traceId)
    {
        vector<Point> intersectionPoints;

        for (int i = 0; i < P.cols(); i++)
        {
            Vector3d p1 = P.col(i);
            Vector3d p2 = P.col((i + 1) % P.cols());

            for (int j = 0; j < Q.cols(); j++)
            {
                Vector3d q1 = Q.col(j);
                Vector3d q2 = Q.col((j + 1) % Q.cols());

                Vector3d dir1 = p2 - p1;
                Vector3d dir2 = q2 - q1;
                Vector3d cross_dir = dir1.cross(dir2);
                double det = cross_dir.norm();

                if (det > epsilon)
                {
                    Vector3d pq = q1 - p1;
                    double t1 = pq.cross(dir2).norm() / det;
                    double t2 = pq.cross(dir1).norm() / det;

                    if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1)
                    {
                        Vector3d intersection = p1 + t1 * dir1;
                        intersectionPoints.push_back({intersection.x(),
                                                      intersection.y(),
                                                      intersection.z()});
                    }
                }
            }
        }

        if (intersectionPoints.size() < 2)
        {
            throw runtime_error("Non sufficient intersection points to form a trace.");
        }

        Point pt1 = intersectionPoints[0];
        Point pt2 = intersectionPoints[1];

        bool isPt1OnP = isPointOnEdge(pt1, P);
        bool isPt2OnP = isPointOnEdge(pt2, P);

        bool Tips = !(isPt1OnP && isPt2OnP);

        return Trace(traceId++, id1, id2, pt1, pt2, Tips);
    }

// ***************************************************************************

    void checkIntersections(Fractures& fractures,
                          map<int, vector<int>>& intersections)
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
                        intersections[id1].push_back(id2);
                        intersections[id2].push_back(id1);

                        try
                        {
                            Trace trace = calculateTrace(P, Q, id1, id2, traceId);
                            fractures.Traces.push_back(trace);
                        }
                        catch (const exception& e)
                        {
                            cerr <<"Error calculating trace between fractures "
                                 << id1 << " and " << id2 << ": " << e.what() << endl;
                        }

                    }

                    pairFound.insert(pair);

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
        map<int, vector<Trace>> passing;
        map<int, vector<Trace>> non_passing;

        for (const auto& trace : fractures.Traces)
            {
                if (trace.Tips)
                {
                    non_passing[trace.fractureId1].push_back(trace);
                    non_passing[trace.fractureId2].push_back(trace);
                }
                else
                {
                    passing[trace.fractureId1].push_back(trace);
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

        for (const auto& fractureId : fractures.FracturesId)
        {
            int numTraces = passing[fractureId].size() + non_passing[fractureId].size();

            if(numTraces != 0)
            {
                outFile << "# FractureId; NumTraces" << endl;
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

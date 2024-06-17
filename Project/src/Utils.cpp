#include "Utils.hpp"
#include <ostream>
#include <list>
#include <cmath>
#include <algorithm>

namespace FractureLibrary
{
const double epsilon = numeric_limits<double>::epsilon();

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


pair<double, double> proietta(const Matrix3Xd& P, int idx, const string& piano)
{
    if (piano == "XY")
        return {P(0, idx), P(1, idx)};
    else if (piano == "YZ")
        return {P(1, idx), P(2, idx)};
    else if (piano == "ZX")
        return {P(2, idx), P(0, idx)};
    else
        throw invalid_argument("Piano non valido");
}

bool verificaIntersezione2D(const vector<pair<double, double>>& P,
                            const vector<pair<double, double>>& Q)
{
    auto getEdges = [](const vector<pair<double, double>>& poly)
    {
        vector<pair<double, double>> edges;
        for (size_t i = 0; i < poly.size(); i++)
        {
            size_t j = (i + 1) % poly.size();
            edges.push_back({poly[j].first - poly[i].first, poly[j].second - poly[i].second});
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

vector<pair<double, double>> proiettaSuPiano(const Matrix3Xd& vertici, const string& piano)
{
    vector<pair<double, double>> proiezione;
    for (int i = 0; i < vertici.cols(); i++)
    {
        if (piano == "XY")
        {
            proiezione.push_back({vertici(0, i), vertici(1, i)});
        }
        else if (piano == "YZ")
        {
            proiezione.push_back({vertici(1, i), vertici(2, i)});
        }
        else if (piano == "ZX")
        {
            proiezione.push_back({vertici(2, i), vertici(0, i)});
        }
    }
    return proiezione;
}

bool verificaSeparazione(const Matrix3Xd& P, const Matrix3Xd& Q)
{
    for (int i = 0; i < P.cols(); i++)
    {
        Vector3d normaleP = P.col(i).cross(P.col((i + 1) % P.cols()));
        bool separato = true;

        for (int j = 0; j < Q.cols(); j++)
        {
            if (normaleP.dot(Q.col(j) - P.col(i)) <= epsilon)
            {
                separato = false;
                break;
            }
        }

        if (separato) return true;
    }

    for (int i = 0; i < Q.cols(); i++)
    {
        Vector3d normaleQ = Q.col(i).cross(Q.col((i + 1) % Q.cols()));
        bool separato = true;

        for (int j = 0; j < P.cols(); j++)
        {
            if (normaleQ.dot(P.col(j) - Q.col(i)) <= epsilon)
            {
                separato = false;
                break;
            }
        }

        if (separato) return true;
    }

    return false;
}

//funzione per vedere se punto è al bordo
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

Trace calcolaTraccia(const Matrix3Xd& P, const Matrix3Xd& Q, int id1, int id2, int& traceId)
{
    vector<Point> intersectionPoints;

    // Itera sui bordi di P
    for (int i = 0; i < P.cols(); i++)
    {
        Vector3d p1 = P.col(i);
        Vector3d p2 = P.col((i + 1) % P.cols());

        // Itera sui bordi di Q
        for (int j = 0; j < Q.cols(); j++)
        {
            Vector3d q1 = Q.col(j);
            Vector3d q2 = Q.col((j + 1) % Q.cols());

            // Calcola l'intersezione tra i segmenti [p1, p2] e [q1, q2]
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
                    intersectionPoints.push_back({intersection.x(), intersection.y(), intersection.z()});
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

    // Verifica se entrambi i punti sono sui bordi delle fratture
    bool isPt1OnP = isPointOnEdge(pt1, P);
    bool isPt1OnQ = isPointOnEdge(pt1, Q);
    bool isPt2OnP = isPointOnEdge(pt2, P);
    bool isPt2OnQ = isPointOnEdge(pt2, Q);

    bool isPassing = (isPt1OnP && isPt2OnP) || (isPt1OnQ && isPt2OnQ);

    return Trace(traceId++, id1, id2, pt1, pt2, !isPassing);
}

void verificaIntersezioni(Fractures& fractures,
                          map<int, vector<int>>& intersezioni)
{
    const vector<unsigned int>& ids = fractures.FracturesId;
    const vector<Matrix3Xd>& vertici = fractures.FracturesVertices;
    set<FracturePair> coppieVerificate;
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

            FracturePair coppia(id1, id2, fractures.NumberFractures);

            if (coppieVerificate.find(coppia) != coppieVerificate.end())
            {
                continue;
            }

            const Matrix3Xd& P = vertici[i];
            const Matrix3Xd& Q = vertici[j];

            bool intersezioneXY = verificaIntersezione2D(proiettaSuPiano(P, "XY"), proiettaSuPiano(Q, "XY"));
            bool intersezioneYZ = verificaIntersezione2D(proiettaSuPiano(P, "YZ"), proiettaSuPiano(Q, "YZ"));
            bool intersezioneZX = verificaIntersezione2D(proiettaSuPiano(P, "ZX"), proiettaSuPiano(Q, "ZX"));

            if (intersezioneXY && intersezioneYZ && intersezioneZX)
            {
                if (!verificaSeparazione(P, Q))
                {
                    intersezioni[id1].push_back(id2);
                    intersezioni[id2].push_back(id1);

                    try
                    {
                        Trace trace = calcolaTraccia(P, Q, id1, id2, traceId);
                        fractures.Traces.push_back(trace);
                    }
                    catch (const exception& e)
                    {
                        cerr << "Error calculating trace between fractures " << id1 << " and " << id2 << ": " << e.what() << endl;
                    }

                }

                coppieVerificate.insert(coppia);

            }
        }
    }
}

void sortTracesByLength(Fractures& fractures)
{
    sort(fractures.Traces.begin(), fractures.Traces.end(), [](const Trace& a, const Trace& b)
         {
             return a.length > b.length;
         });
}

void writeResults(const Fractures& fractures, const string& filename)
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
        outFile << trace.traceId << ";"
                << trace.fractureId1 << ";"
                << trace.fractureId2 << ";"
                << trace.p1.x << ";"
                << trace.p1.y << ";"
                << trace.p1.z << ";"
                << trace.p2.x << ";"
                << trace.p2.y << ";"
                << trace.p2.z << endl;
    }

    outFile.close();
}

void writeResultsForFractures(const Fractures& fractures, const string& filename)
{
    map<int, vector<Trace>> passanti;
    map<int, vector<Trace>> non_passanti;

    for (const auto& trace : fractures.Traces)
    {
        if (trace.Tips)
        {
            non_passanti[trace.fractureId1].push_back(trace);
            non_passanti[trace.fractureId2].push_back(trace);
        }
        else
        {
            passanti[trace.fractureId1].push_back(trace);
            passanti[trace.fractureId2].push_back(trace);
        }
    }

    ofstream outFile(filename);
    if (!outFile)
    {
        cerr << "Failed to open file for writing: " << filename << endl;
        return;
    }

    for (const auto& fractureId : fractures.FracturesId)
    {
        outFile << "# FractureId; NumTraces" << endl;
        int numTraces = passanti[fractureId].size() + non_passanti[fractureId].size();
        outFile << fractureId << "; " << numTraces << endl;

        // Ordina e stampa le tracce passanti
        sort(passanti[fractureId].begin(), passanti[fractureId].end(), [](const Trace& a, const Trace& b)
             { return a.length > b.length; });
        outFile << "# TraceId; Tips; Length" << endl;
        for (const auto& trace : passanti[fractureId])
        {
            outFile << trace.traceId << "; " << "false" << "; " << trace.length << endl;
        }

        // Ordina e stampa le tracce non-passanti
        sort(non_passanti[fractureId].begin(), non_passanti[fractureId].end(), [](const Trace& a, const Trace& b)
             { return a.length > b.length; });
        for (const auto& trace : non_passanti[fractureId])
        {
            outFile << trace.traceId << "; " << "true" << "; " << trace.length << endl;
        }
    }

    outFile.close();
}
}

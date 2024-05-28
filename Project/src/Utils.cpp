#include "Utils.hpp"
#include <ostream>
#include <list>
#include <cmath>

namespace FractureLibrary
{
    bool ImportDFN(const string& filePath,
                   Fractures& fractures)
    {
        if(!ImportFractures(filePath + "FR3_data.txt",
                           fractures))
        {
            return false;
        }

        if(!ImportFractures(filePath + "FR10_data.txt",
                           fractures))
        {
            return false;
        }

        if(!ImportFractures(filePath + "FR50_data.txt",
                           fractures))
        {
            return false;
        }

        if(!ImportFractures(filePath + "FR82_data.txt",
                           fractures))
        {
            return false;
        }

        if(!ImportFractures(filePath + "FR200_data.txt",
                           fractures))
        {
            return false;
        }

        if(!ImportFractures(filePath + "FR362_data.txt",
                           fractures))
        {
            return false;
        }

        return true;
    }

    bool ImportFractures(const string& filename,
                         Fractures& fractures)
    {
        ifstream file;
        file.open(filename);

        if(file.fail())
        {
            cerr << "File open failed." << endl;
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

        file.close();

        return true;
    }

    // Dichiarazione della tolleranza epsilon
    const double epsilon = numeric_limits<double>::epsilon();

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

    set<FracturePair> coppieVerificate;

    void verificaIntersezioni(const Fractures& fractures,
                              map<int, vector<int>>& intersezioni)
    {
        const vector<unsigned int>& ids = fractures.FracturesId;
        const vector<Matrix3Xd>& vertici = fractures.FracturesVertices;

        for (size_t i = 0; i < ids.size(); i++)
        {
            for (size_t j = i + 1; j < ids.size(); j++)
            {
                int id1 = ids[i];
                int id2 = ids[j];

                // Verifica se la coppia di fratture è la stessa
                if (id1 == id2)
                {
                    continue; // Passa alla prossima iterazione
                }

                // Costruisci una tupla di tre interi che rappresenta la coppia di fratture
                FracturePair coppia(id1, id2, fractures.NumberFractures);

                // Verifica se la coppia di fratture è già stata verificata
                if (coppieVerificate.find(coppia) != coppieVerificate.end())
                {
                    continue; // Passa alla prossima iterazione
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
                    }
                }

                // Aggiungi la coppia di fratture verificata al set
                coppieVerificate.insert(coppia);
            }

        }

    }
}

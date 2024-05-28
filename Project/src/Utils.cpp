#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <string>
#include <list>
#include <cmath>
#include <algorithm>

namespace FractureLibrary
{
    void printVector(const vector<unsigned int>& vec)
    {
        for (unsigned int id : vec)
        {
            cout << id << " ";
        }
        cout << endl;
    }

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
        fractures.FracturesVertices.reserve(fractures.NumberFractures);

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


}

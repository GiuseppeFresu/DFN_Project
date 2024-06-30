#include <iostream>
#include "Fractures.hpp"
#include "Utils.hpp"

using namespace FractureLibrary;
using namespace std;

int main()
{
    string filepath = "DFN/";
    vector<string> filenames = {"FR3_data.txt", "FR10_data.txt", "FR50_data.txt",
                                "FR82_data.txt", "FR200_data.txt", "FR362_data.txt"};

    for (const auto& filename : filenames)
    {
        Fractures fractures;

        if (!ImportFractures(filepath + filename, fractures))
        {
            cerr << "Error importing fractures from " << filename << endl;
            return 1;
        }

        map<int, vector<int>> file_intersections;
        checkIntersections(fractures, file_intersections);

        string outputTraces = filename + "_traces.txt";
        writeTraces(fractures, outputTraces);

        string outputResults = filename + "_results.txt";
        writeResults(fractures, outputResults);

        fractures.clear();
    }

    return 0;

}

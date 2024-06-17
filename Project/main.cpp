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
            cerr << "Errore durante l'importazione delle fratture da " << filename << endl;
            return 1;
        }

        map<int, vector<int>> intersezioni_file;
        verificaIntersezioni(fractures, intersezioni_file);

        // Ordina le tracce per lunghezza
        sortTracesByLength(fractures);

        // Scrivi i risultati nel file
        string outputFilename = filename + "_results.txt";
        writeResults(fractures, outputFilename);

        // Scrivo i risultati nel file per tracce passanti e non passanti
        string outputFilenamePart3 = filename + "_results_fractures.txt";
        writeResultsForFractures(fractures, outputFilenamePart3);

        fractures.clear();
    }

    return 0;

}



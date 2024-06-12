#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include "Fractures.hpp"
#include "Utils.hpp"

using namespace std;
using namespace FractureLibrary;

void scriviTracce(const std::vector<Traccia>& tracce, const std::string& filename) {
    std::ofstream outfile(filename);
    if (!outfile) {
        std::cerr << "Errore nell'apertura del file di output" << std::endl;
        return;
    }

    outfile << "# Number of Traces\n" << tracce.size() << "\n";
    outfile << "# TraceId; FractureId1; FractureId2; X1; Y1; Z1; X2; Y2; Z2\n";
    for (const auto& traccia : tracce) {
        outfile << traccia.id << "; "
                << traccia.fractureId1 << "; "
                << traccia.fractureId2 << "; "
                << traccia.point1[0] << "; " << traccia.point1[1] << "; " << traccia.point1[2] << "; "
                << traccia.point2[0] << "; " << traccia.point2[1] << "; " << traccia.point2[2] << "\n";
    }
}


std::vector<Traccia> calcolaIntersezioni(const Fractures& fractures) {
    std::vector<Traccia> tracce;
    // Logica per calcolare le intersezioni e popolare il vettore `tracce`
    // ...
    return tracce;
}

int main()
{
    std::string outputFilename = "output_traces.txt";
    map<int, vector<int>> intersezioni_file;
    std::vector<Traccia> tracce;

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





        cout << "Intersezioni per il file " << filename << ":" << endl;
        for (const auto& entry : intersezioni_file)
        {
            int id = entry.first;
            const vector<int>& intersezioni = entry.second;

            cout << "Frattura " << id << " interseca con: ";
            for (int intersected_id : intersezioni)
            {
                cout << intersected_id << " ";
            }
            cout << endl;
        }
    }
        scriviTracce(tracce, outputFilename);
    return 0;
}


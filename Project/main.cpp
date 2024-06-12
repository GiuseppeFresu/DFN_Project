#include <iostream> //Ciao questo è il mio branch
#include <string>
#include "Fractures.hpp"
#include "Utils.hpp"

using namespace std;
using namespace FractureLibrary;

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

    return 0;
}

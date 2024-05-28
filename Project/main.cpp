#include <iostream>
#include "Fractures.hpp"
#include "Utils.hpp"

using namespace std;
using namespace Eigen;
using namespace FractureLibrary;

int main()
{
    Fractures fractures;

    string filepath = "DFN/";
    if(!ImportDFN(filepath,
                    fractures))
    {
        return 1;
    }

    return 0;
}

#include "cs_lib.h"
#include <iostream>

/*
 * Example of using clscript in C/C++ program
 */ 

using namespace std;
using namespace clscript;

int main(int argc, char **argv)
{
    CSRuntime runtime(CS_DEV_GPU);

    if(argc != 2){
        cout << "usage: " << argv[0] << " PATH_TO_test.cl" << endl;
        return -1;
    }
    CSLib lib(&runtime, (const char*)(argv[1]));
    CSBuffer buf(&runtime, 4096);
    CSWorkSize gWS(4096);
    lib.exec("test", gWS, NullWorkSize, 0, &buf);

    return 0;
}

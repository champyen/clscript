#include "cs_lib.h"
#include <iostream>

#include <stdio.h>
/*
 * Example of using clscript in C/C++ program
 */ 

using namespace std;
using namespace clscript;

int main(int argc, char **argv)
{
    if(argc != 2){
        cout << "usage: " << argv[0] << " PATH_TO_test.cl" << endl;
        return -1;
    }

    try{
        CSRuntime runtime(CS_DEV_GPU);

        CSLib lib(&runtime, (const char*)(argv[1]));
        CSBuffer buf(&runtime, 4096*sizeof(cl_int));
        CSWorkSize gWS(4096);
        printf("%s buf %p\n", __func__, &buf);
        lib.exec("test", gWS, NullWorkSize, 0, &buf);
    }catch(cl_int err){
        cout << argv[0] << " error: " << CSGetErrorMsg(err) << endl;
    }

    return 0;
}

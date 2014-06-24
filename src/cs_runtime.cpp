#include "cs_runtime.h"
#include <iostream>

using namespace std;

namespace clscript{

CSRuntime::CSRuntime(CSDeviceType devType) :
    mPlat(NULL),
    mDev(NULL),
    mCtx(NULL),
    mQueue(NULL),
    mUseSyncFlag(false)
{
    cl_int status;
    cl_platform_id *plats = NULL;
    cl_uint numPlats;
    clGetPlatformIDs(0, NULL, &numPlats);

    if(numPlats){
        plats = new cl_platform_id[numPlats];
        clGetPlatformIDs(numPlats, plats, NULL);

        cout << __func__ << ": " << numPlats << " platforms found\n";
        for(int i = 0; i < numPlats; i++){
            cl_uint numDevs;
            status = clGetDeviceIDs(plats[i], devType, 0, NULL, &numDevs);
            if(numDevs){
                cout << __func__ << ": get " << numDevs << " device with specific deviceype\n" ;
                mPlat = plats[i];
                status = clGetDeviceIDs(mPlat, devType, 1, &mDev, NULL);
                mCtx = clCreateContext(NULL, 1, &mDev, NULL, NULL, &status);
                mQueue = clCreateCommandQueue(mCtx, mDev, 0, &status);
                break;
            }
        }

        delete plats;
    }
}

/* TODO - setup runtime by INI file
CSRuntime::CSRuntime(char *iniFile)
{
}
*/

CSRuntime::~CSRuntime()
{
    if(mDev){
        clReleaseCommandQueue(mQueue);
        clReleaseContext(mCtx);
        clReleaseDevice(mDev);
    }
}

} //namespace clscript

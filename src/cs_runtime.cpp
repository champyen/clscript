#include "cs_runtime.h"
#include <iostream>

using namespace std;

namespace clscript{

static const char* CSErrorMsg[] =
{
    // Error Codes
      "CL_SUCCESS"                                      //   0
    , "CL_DEVICE_NOT_FOUND"                             //  -1
    , "CL_DEVICE_NOT_AVAILABLE"                         //  -2
    , "CL_COMPILER_NOT_AVAILABLE"                       //  -3
    , "CL_MEM_OBJECT_ALLOCATION_FAILURE"                //  -4
    , "CL_OUT_OF_RESOURCES"                             //  -5
    , "CL_OUT_OF_HOST_MEMORY"                           //  -6
    , "CL_PROFILING_INFO_NOT_AVAILABLE"                 //  -7
    , "CL_MEM_COPY_OVERLAP"                             //  -8
    , "CL_IMAGE_FORMAT_MISMATCH"                        //  -9
    , "CL_IMAGE_FORMAT_NOT_SUPPORTED"                   //  -10
    , "CL_BUILD_PROGRAM_FAILURE"                        //  -11
    , "CL_MAP_FAILURE"                                  //  -12

    , "CL_MISALIGNED_SUB_BUFFER_OFFSET"                 //  -13
    , "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST"    //  -14
    , "CL_COMPILE_PROGRAM_FAILURE"                      //  -15
    , "CL_LINKER_NOT_AVAILABLE"                         //  -16
    , "CL_LINK_PROGRAM_FAILURE"                         //  -17
    , "CL_DEVICE_PARTITION_FAILED"                      //  -18
    , "CL_KERNEL_ARG_INFO_NOT_AVAILABLE"                //  -19

    , ""        //  -20
    , ""        //  -21
    , ""        //  -22
    , ""        //  -23
    , ""        //  -24
    , ""        //  -25
    , ""        //  -26
    , ""        //  -27
    , ""        //  -28
    , ""        //  -29

    , "CL_INVALID_VALUE"                                //  -30
    , "CL_INVALID_DEVICE_TYPE"                          //  -31
    , "CL_INVALID_PLATFORM"                             //  -32
    , "CL_INVALID_DEVICE"                               //  -33
    , "CL_INVALID_CONTEXT"                              //  -34
    , "CL_INVALID_QUEUE_PROPERTIES"                     //  -35
    , "CL_INVALID_COMMAND_QUEUE"                        //  -36
    , "CL_INVALID_HOST_PTR"                             //  -37
    , "CL_INVALID_MEM_OBJECT"                           //  -38
    , "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR"              //  -39
    , "CL_INVALID_IMAGE_SIZE"                           //  -40
    , "CL_INVALID_SAMPLER"                              //  -41
    , "CL_INVALID_BINARY"                               //  -42
    , "CL_INVALID_BUILD_OPTIONS"                        //  -43
    , "CL_INVALID_PROGRAM"                              //  -44
    , "CL_INVALID_PROGRAM_EXECUTABLE"                   //  -45
    , "CL_INVALID_KERNEL_NAME"                          //  -46
    , "CL_INVALID_KERNEL_DEFINITION"                    //  -47
    , "CL_INVALID_KERNEL"                               //  -48
    , "CL_INVALID_ARG_INDEX"                            //  -49
    , "CL_INVALID_ARG_VALUE"                            //  -50
    , "CL_INVALID_ARG_SIZE"                             //  -51
    , "CL_INVALID_KERNEL_ARGS"                          //  -52
    , "CL_INVALID_WORK_DIMENSION"                       //  -53
    , "CL_INVALID_WORK_GROUP_SIZE"                      //  -54
    , "CL_INVALID_WORK_ITEM_SIZE"                       //  -55
    , "CL_INVALID_GLOBAL_OFFSET"                        //  -56
    , "CL_INVALID_EVENT_WAIT_LIST"                      //  -57
    , "CL_INVALID_EVENT"                                //  -58
    , "CL_INVALID_OPERATION"                            //  -59
    , "CL_INVALID_GL_OBJECT"                            //  -60
    , "CL_INVALID_BUFFER_SIZE"                          //  -61
    , "CL_INVALID_MIP_LEVEL"                            //  -62
    , "CL_INVALID_GLOBAL_WORK_SIZE"                     //  -63
    , "CL_INVALID_PROPERTY"                             //  -64
    , "CL_INVALID_IMAGE_DESCRIPTOR"                     //  -65
    , "CL_INVALID_COMPILER_OPTIONS"                     //  -66
    , "CL_INVALID_LINKER_OPTIONS"                       //  -67
    , "CL_INVALID_DEVICE_PARTITION_COUNT"               //  -68
    , "CL_UNKNOWN_ERROR_CODE"
};

string CSGetErrorMsg(int err)
{
    int idx = -err;
    if(idx > 68 || idx < 0)
        return string(CSErrorMsg[69]);
    else
        return string(CSErrorMsg[idx]);
}

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
    CL_CALL(status = clGetPlatformIDs(0, NULL, &numPlats));

    if(numPlats){
        plats = new cl_platform_id[numPlats];
        CL_CALL(status = clGetPlatformIDs(numPlats, plats, NULL));

        cout << __func__ << ": " << numPlats << " platforms found\n";
        for(int i = 0; i < numPlats; i++){
            cl_uint numDevs;
            CL_CALL(status = clGetDeviceIDs(plats[i], devType, 0, NULL, &numDevs));
            if(numDevs){
                cout << __func__ << ": get " << numDevs << " device with specific deviceype\n" ;
                mPlat = plats[i];
                CL_CALL(status = clGetDeviceIDs(mPlat, devType, 1, &mDev, NULL));
                CL_CALL(mCtx = clCreateContext(NULL, 1, &mDev, NULL, NULL, &status));
                CL_CALL(mQueue = clCreateCommandQueue(mCtx, mDev, 0, &status));
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
        cl_int status;
        CL_CALL(status = clReleaseCommandQueue(mQueue));
        CL_CALL(status = clReleaseContext(mCtx));
        CL_CALL(status = clReleaseDevice(mDev));
    }
}

} //namespace clscript

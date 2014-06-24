#ifndef _CS_RUNTIME_H_
#define _CS_RUNTIME_H_

#include <CL/cl.h>
#include <string>
#include <list>

#define CL_CALL(x) { \
        x; \
        if(status != CL_SUCCESS) \
            throw status; \
    }

namespace clscript{

using namespace std;

string CSGetErrorMsg(int err);

typedef enum{
    CS_DEV_CPU = CL_DEVICE_TYPE_CPU,
    CS_DEV_GPU = CL_DEVICE_TYPE_GPU
}CSDeviceType;

class CSBuffer;
class CSLib;

class CSRuntime{
    public:
        CSRuntime(CSDeviceType);
        //CSRuntime(char*);
        ~CSRuntime();
        friend class CSBuffer;
        friend class CSLib;

    private:
        cl_platform_id 	        mPlat;
        cl_device_id            mDev;
        cl_context              mCtx;
        cl_command_queue        mQueue;
        bool                    mUseSyncFlag;
};

} //namespace clscript

#endif

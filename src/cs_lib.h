#ifndef _CS_LIB_H_
#define _CS_LIB_H_

#include "cs_runtime.h"
#include "cs_buffer.h"
#include <string>
#include <map>

using namespace std;

#define CS_MAX_ARGS    32

namespace clscript{
typedef enum{
    CS_CHAR = 0x00,
    CS_CHAR2 = 0x02,
    CS_CHAR3 = 0x03,
    CS_CHAR4 = 0x04,
    CS_CHAR8 = 0x08,
    CS_CHAR16 = 0x10,
    CS_SHORT = 0x20,
    CS_SHORT2 = 0x22,
    CS_SHORT3 = 0x23,
    CS_SHORT4 = 0x24,
    CS_SHORT8 = 0x28,
    CS_SHORT16 = 0x30,
    CS_INT = 0x40,
    CS_INT2 = 0x42, 
    CS_INT3 = 0x43,
    CS_INT4 = 0x44,
    CS_INT8 = 0x48, 
    CS_INT16 = 0x50,
    CS_LONG = 0x60,
    CS_LONG2 = 0x62,
    CS_LONG3 = 0x63,
    CS_LONG4 = 0x64,
    CS_LONG8 = 0x68,
    CS_LONG16 = 0x70,
    CS_FLOAT = 0x80,
    CS_FLOAT2 = 0x82,
    CS_FLOAT3 = 0x83,
    CS_FLOAT4 = 0x84,
    CS_FLOAT8 = 0x85,
    CS_FLOAT16 = 0x90,
    CS_DOUBLE = 0xA0,
    CS_DOUBLE2 = 0xA2,
    CS_DOUBLE3 = 0xA3,
    CS_DOUBLE4 = 0xA4,
    CS_DOUBLE8 = 0xA8,
    CS_DOUBLE16 = 0xB0,
    CS_HALF = 0xC0,
    CS_BUF = 0xC1
}CSArgType;

typedef struct{
    cl_kernel       kernel;
    cl_uint         numArgs;
    cl_uint         *argType;
}CSKernel;

typedef struct{
    void            *argVal;
    size_t          argSize;
}CSArg;

class CSLib;
class CSWorkSize{
    public:
        CSWorkSize() : mDim(0) {}
        CSWorkSize(size_t x) : mDim(1)
        {
            mWorkSize[0] = x;
        }
        CSWorkSize(size_t x, size_t y) : mDim(2)
        {
            mWorkSize[0] = x; mWorkSize[1] = y; 
        }
        CSWorkSize(size_t x, size_t y, size_t z) : mDim(3)
        {
            mWorkSize[0] = x; mWorkSize[1] = x; mWorkSize[2] = z; 
        }
        
        void addDim(size_t v){
            if(mDim < 3)
                mWorkSize[mDim++] = v;
        }
        
        size_t getWorkSize()
        {
            size_t sz = 1;
            for(int i = 0; i < mDim; i++)
                sz *= mWorkSize[i];
            return sz;
        }
        
        friend class CSLib;

    private:
        cl_uint mDim;
        size_t mWorkSize[3] = {0, 0, 0};
};

static CSWorkSize NullWorkSize;

class CSLib
{
    public:
        CSLib(CSRuntime *runtime, const char *fileName, const char *buildOpt = NULL);
        CSLib(CSRuntime *runtime, int files, const char **fileName, const char *buildOpt = NULL);
        ~CSLib();
        int exec(const char *, CSWorkSize&, CSWorkSize&, ...);
        int exec(const char *, CSWorkSize&, CSWorkSize&, CSArg *);

    private:
        CSRuntime                   *mRuntime;
        cl_program                  mProg;
        unsigned int                mNumKernels;
        CSKernel                   *mKernels;
        map<string, unsigned int>   mIdxMap;
};

} // namespace clscript

#endif

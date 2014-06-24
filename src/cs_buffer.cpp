#include "cs_buffer.h"

namespace clscript{

CSBuffer::CSBuffer(CSRuntime *runtime, size_t size, void *hostptr, bool autoSync) :
    mRuntime(runtime),
    mSize(size),
    mHostBuf(hostptr)
{
    cl_int status;
    cl_mem_flags flags = CL_MEM_READ_WRITE;
    if(hostptr){
        if(mAutoSync && (mRuntime->mUseSyncFlag))           //autosync
            flags |= CL_MEM_USE_HOST_PTR;
        else                                                //hostptr is used to initialized
            flags |= CL_MEM_COPY_HOST_PTR;  
    }
    mBuf = clCreateBuffer(runtime->mCtx, flags, size, hostptr, &status);
}

CSBuffer::~CSBuffer()
{
    clReleaseMemObject(mBuf);
}

void* CSBuffer::getHostBuf()
{
    return mHostBuf;
}

size_t CSBuffer::getSize()
{
    return mSize;
}

void CSBuffer::sync()
{
    if(mHostBuf && mAutoSync && !(mRuntime->mUseSyncFlag)){
        //copyback
        clEnqueueReadBuffer(mRuntime->mQueue, mBuf, CL_TRUE, 0, mSize, mHostBuf, 0, NULL, NULL);
    }
}

void CSBuffer::read(void *buf)
{
    clEnqueueReadBuffer(mRuntime->mQueue, mBuf, CL_TRUE, 0, mSize, buf, 0, NULL, NULL);
}

}

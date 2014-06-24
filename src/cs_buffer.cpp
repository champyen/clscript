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
    CL_CALL(mBuf = clCreateBuffer(runtime->mCtx, flags, size, hostptr, &status));
}

CSBuffer::~CSBuffer()
{
    CL_CALL(cl_int status = clReleaseMemObject(mBuf));
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
    cl_int status;
    if(mHostBuf && mAutoSync && !(mRuntime->mUseSyncFlag)){
        //copyback
        CL_CALL(status = clEnqueueReadBuffer(mRuntime->mQueue, mBuf, CL_TRUE, 0, mSize, mHostBuf, 0, NULL, NULL));
    }
}

void CSBuffer::read(void *buf)
{
    cl_int status; 
    CL_CALL(status = clEnqueueReadBuffer(mRuntime->mQueue, mBuf, CL_TRUE, 0, mSize, buf, 0, NULL, NULL));
}

} //namespace clscript

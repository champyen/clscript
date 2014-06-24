#ifndef _CS_BUFFER_H_
#define _CS_BUFFER_H_

#include "cs_runtime.h"

namespace clscript{

class CSLib;
class CSBuffer{
    public:
        CSBuffer(CSRuntime *runtime, size_t size, void *hostptr = NULL, bool autoSync = false);
        ~CSBuffer();
        friend class CSLib;
        void *getHostBuf();
        size_t getSize();
        void sync();
        void read(void *buf);

    private:
        CSRuntime       *mRuntime;
        cl_mem          mBuf;
        size_t          mSize;
        void            *mHostBuf;
        bool            mAutoSync;
};

} //namespace clscript
#endif

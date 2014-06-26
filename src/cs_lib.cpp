
#include "cs_lib.h"
#include "cs_buffer.h"
#include <iostream>
#include <map>
#include <list>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

namespace clscript {

CSLib::CSLib(CSRuntime *runtime, const char *fileName, const char *buildOpt)
    : CSLib(runtime, 1, &fileName, buildOpt)
{
}

CSLib::CSLib(CSRuntime *runtime, int files, const char **fileName, const char *buildOpt) :
    mRuntime(runtime),
    mNumKernels(0),
    mKernels(NULL)
{
    cl_int status;
    char **src = new char*[files];
    size_t *flen = new size_t[files];
    mIdxMap.clear();

    for(int i = 0; i < files; i++){
        cout << "read file: " << fileName[i] << endl;

        FILE *fp = fopen(fileName[i], "r");
        if(fp){
            fseek(fp, 0L, SEEK_END);
            flen[i] = ftell(fp);
            cout << fileName[i] << " size: " << flen[i] << endl;

            fseek(fp, 0L, SEEK_SET);
            src[i] = new char[flen[i]];
            fread(src[i], flen[i], 1, fp); 

            fclose(fp);
        }else{
            throw std::invalid_argument(fileName[i]);
        }
    }

    CL_CALL(mProg = clCreateProgramWithSource(runtime->mCtx, files, (const char**)src, flen, &status));
    CL_CALL(status = clBuildProgram(mProg, 1, &(runtime->mDev), NULL, NULL, NULL));

    CL_CALL(status = clCreateKernelsInProgram(mProg, 0, NULL, &mNumKernels));
    cout << __func__ << ": " << mNumKernels << endl;
    if(mNumKernels){
        mKernels = new CSKernel[mNumKernels];
        cl_kernel *kernels = new cl_kernel[mNumKernels];
        CL_CALL(status = clCreateKernelsInProgram(mProg, mNumKernels, kernels, NULL));
        for(int i = 0; i < mNumKernels; i++){
            char strbuf[1024];
            size_t retlen;
            mKernels[i].kernel = kernels[i];
            CL_CALL(status = clGetKernelInfo(kernels[i], CL_KERNEL_FUNCTION_NAME, 1024, strbuf, &retlen));
            mIdxMap[strbuf] = i;
            printf("%s kernel[%s] - idx[%d, %d] \n", __func__, strbuf, i, mIdxMap[string(strbuf)]);

            CL_CALL(status = clGetKernelInfo(kernels[i], CL_KERNEL_NUM_ARGS, sizeof(cl_uint), &(mKernels[i].numArgs), &retlen));
            mKernels[i].argType = new cl_uint[mKernels[i].numArgs];
            cout << __func__ << ": create kernel - " << strbuf << ", # of args - " << mKernels[i].numArgs << endl; 

            for(int j = 0; j < mKernels[i].numArgs; j++){
                CL_CALL(status = clGetKernelArgInfo(kernels[i], j, CL_KERNEL_ARG_TYPE_NAME, 1024, strbuf, &retlen));
                cout << "arg[" << j << "] - " << strbuf << endl ;
                
                char *bptr = NULL;
                
                if((bptr = strchr(strbuf, '*')) != NULL){
                    mKernels[i].argType[j] = CS_BUF;
                }else if((bptr = strstr(strbuf, "int")) != NULL){
                    bptr += 3;
                    mKernels[i].argType[j] = CS_INT;
                }else if((bptr = strstr(strbuf, "long")) != NULL){
                    bptr += 4;
                    mKernels[i].argType[j] = CS_LONG;
                }else if((bptr = strstr(strbuf, "float")) != NULL){
                    bptr += 5;
                    mKernels[i].argType[j] = CS_FLOAT;
                }else if((bptr = strstr(strbuf, "double")) != NULL){
                    bptr += 6;
                    mKernels[i].argType[j] = CS_DOUBLE;
                }else if((bptr = strstr(strbuf, "half")) != NULL){
                    bptr += 4;
                    mKernels[i].argType[j] = CS_HALF;
                }
                
                if(*bptr == '2'){
                    mKernels[i].argType[j] += 2;
                }else if(*bptr == '4'){
                    mKernels[i].argType[j] += 4;
                }else if(*bptr == '8'){
                    mKernels[i].argType[j] += 8;
                }else if(*bptr == '1' && *(bptr+1) == '6'){
                    mKernels[i].argType[j] += 16;
                }
            }
            
        }
        delete kernels;
    }
    printf("%s idx[%d] \n", __func__, mIdxMap[string("test")]);

    for(int i = 0; i < files; i++)
        delete src[i];
    delete src;
}

CSLib::~CSLib()
{
    cl_int status;
    for(int i = 0; i < mNumKernels; i++){
        delete mKernels[i].argType;
        CL_CALL(status = clReleaseKernel(mKernels[i].kernel));
    }
    CL_CALL(status = clReleaseProgram(mProg));
    if(mKernels)
        delete mKernels;
}

#define CS_ARG_CASE2( argType, clType, vaType) \
                case argType: { \
                    clType v = va_arg(ap, vaType); \
                    CL_CALL(status = clSetKernelArg(kernel, i, sizeof(clType), &v)); \
                } break;

#define CS_ARG_CASE( argType, clType) \
                case argType: { \
                    clType v = va_arg(ap, clType); \
                    CL_CALL(status = clSetKernelArg(kernel, i, sizeof(clType), &v)); \
                } break;

#define CS_ARG_VEC( argType, clType ) \
                CS_ARG_CASE(argType##2, clType##2) \
                CS_ARG_CASE(argType##3, clType##3) \
                CS_ARG_CASE(argType##4, clType##4) \
                CS_ARG_CASE(argType##8, clType##8) \
                CS_ARG_CASE(argType##16, clType##16) 

int CSLib::exec(const char *kName, CSWorkSize &gWS, CSWorkSize &lWS, ...)
{
    if(mIdxMap.find(kName) != mIdxMap.end()){
        cl_int status;
        unsigned int idx = mIdxMap[string(kName)];
        printf("%s - [%s, %d]\n", __func__, kName, idx);
        cl_kernel kernel = mKernels[idx].kernel;
        list<CSBuffer*> bufList;
        va_list ap;

        va_start(ap, lWS);

        for(int i = 0; i < mKernels[idx].numArgs; i++){
            switch(mKernels[idx].argType[i]){
                CS_ARG_CASE(CS_INT, cl_int);
                CS_ARG_VEC(CS_INT, cl_int);

                CS_ARG_CASE(CS_LONG, cl_long);
                CS_ARG_VEC(CS_LONG, cl_long);

                CS_ARG_CASE2(CS_FLOAT, cl_float, cl_double);
                CS_ARG_VEC(CS_FLOAT, cl_float);

                CS_ARG_CASE(CS_DOUBLE, cl_double);
                CS_ARG_VEC(CS_DOUBLE, cl_double);

                case CS_BUF: {
                    CSBuffer *v = va_arg(ap, CSBuffer*);
                    CL_CALL(status = clSetKernelArg(kernel, i, sizeof(cl_mem), &(v->mBuf)));
                    bufList.push_back(v);
                } break;
            }
        }
        va_end(ap);
        
        CL_CALL(status = clEnqueueNDRangeKernel(mRuntime->mQueue, kernel, gWS.mDim, NULL, gWS.mWorkSize, (lWS.mDim ? lWS.mWorkSize : NULL), 0, NULL, NULL));

        for (list<CSBuffer*>::iterator i = bufList.begin(); i != bufList.end (); i++)
            (*i)->sync();
    }
    
    return 0;
}

int CSLib::exec(const char *kName, CSWorkSize &gWS, CSWorkSize &lWS, CSArg *args)
{
    cl_int status;
    if(mIdxMap.find(kName) != mIdxMap.end()){
        unsigned int idx = mIdxMap[string(kName)];
        list<CSBuffer*> bufList;
        printf("%s - [%s, %d][args:%d]\n", __func__, kName, idx, mKernels[idx].numArgs);
        cl_kernel kernel = mKernels[idx].kernel;
        for(int i = 0; i < mKernels[idx].numArgs; i++){
            if(mKernels[idx].argType[i] == CS_BUF){
                CSBuffer *buf = (CSBuffer*)(args[i].argVal);
                CL_CALL(status = clSetKernelArg(kernel, i, sizeof(cl_mem), &(buf->mBuf)));
                bufList.push_back(buf);
            }else{
                cl_int val = *((cl_int*)args[i].argVal);
                CL_CALL(status = clSetKernelArg(kernel, i, args[i].argSize, args[i].argVal));
            }
        }

        CL_CALL(status = clEnqueueNDRangeKernel(mRuntime->mQueue, kernel, gWS.mDim, NULL, gWS.mWorkSize, (lWS.mDim ? lWS.mWorkSize : NULL), 0, NULL, NULL));

        for (list<CSBuffer*>::iterator i = bufList.begin(); i != bufList.end (); i++)
            (*i)->sync();
    }
    return 0;
}

} //namespace clscript

#include "cs_lib.h"
#include <iostream>
#include <list>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;
using namespace clscript;

#define RELEASE_ARG(argType, argList) \
    if(argList.size()){ \
        for (list<argType*>::iterator i = argList.begin(); i != argList.end (); i++) \
            delete (*i); \
    } 

int main(int argc, char **argv)
{
    try{
        CSRuntime runtime(CS_DEV_GPU);
        CSArg args[argc-4];
        list<CSBuffer*> bufList;
        map<CSBuffer*, string> ofileMap;
        list<int*> intList;
        list<float*> floatList;
        list<double*> doubleList;
        CSWorkSize gWS, lWS;

        if(argc < 4){
            cout << "usage - " << argv[0] << " CL_FILE KERNEL_NAME WORK_SIZES ARGS ..." << endl;
            cout << "worksizes global:local - X(,Y(,Z))(:X(,Y(,Z)) , eg. 1024,512:16,16 " << endl;
            cout << "float   argument - f:val, eg. f:16.0 " << endl;
            cout << "double  argument - f:val, eg. d:16.0 " << endl;
            cout << "integer argument - i:val, eg. d:16 " << endl;
            cout << "buffer  argument - b:((sz=BUF_SIZE,)(if=IN_FILE,)(of=OUT_FILE,)), eg. b:sz=1024,if=in.yuv,of=out.yuv" << endl;
            cout << "                 - buffer default size is global worksize." << endl;
            exit(0);
        }

        //load CL file
        const char **fileNames;
        int fcnt = 1;
        {
            char *fileStr = argv[1];
            while(fileStr){
                fileStr = strchr(fileStr, ',');
                if(fileStr){
                    fileStr++;
                    fcnt++;
                }
            }
            fileNames = new const char*[fcnt];
            
            fileStr = argv[1];
            for(int i = 0; i < fcnt; i++){
                char* fName = fileStr;
                fileStr = strchr(fileStr, ',');
                if(fileStr){
                    *fileStr = 0;
                    fileStr++;
                }
                fileNames[i] = fName;
            }
        }
        CSLib lib(&runtime, fcnt, fileNames);

        //parse worksize info
        char *wsStr = argv[3];
        {
            char *lwsStr = strchr(wsStr, ':');
            if(lwsStr)
                *(lwsStr++) = '\0';
            cout << "global worksize:";
            for(int i = 0; i < 3; i++, wsStr++){
                size_t sz = atoi(wsStr);
                cout << sz << ",";
                gWS.addDim(sz);
                wsStr = strchr(wsStr, ',');
                if(!wsStr)
                    break;
            }
            cout << endl;
            if(lwsStr){
                cout << "local worksize:";
                for(int i = 0; i < 3; i++, lwsStr++){
                    size_t sz = atoi(lwsStr);
                    cout << sz << ",";
                    lWS.addDim(sz);
                    lwsStr = strchr(lwsStr, ',');
                    if(!lwsStr)
                        break;
                }
                cout << endl;
            }
        }

        for(int i = 0; i < argc-4; i++){
            switch(argv[i+4][0]){
                case 'b': {
                        //buffer
                        CSBuffer *v;
                        char *hostbuf = NULL;
                        size_t size = gWS.getWorkSize();

                        cout << "param[" << i << "]:buf:";
                        {
                            char *szStr = strstr(argv[i+4], "sz=");
                            char *ifStr = strstr(argv[i+4], "if=");
                            char *ofStr = strstr(argv[i+4], "of=");

                            for( char *str = strchr(argv[i+4], ','); str != NULL; str = strchr(str, ','))
                                *(str++) = '\0';
                            
                            //size
                            if(szStr){
                                size = atoi(szStr+3);
                            }
                            cout << "sz=" << size;
                            //input file
                            if(ifStr){
                                ifStr += 3;
                                hostbuf = new char[size];
                                FILE *fp = fopen(ifStr, "rb");
                                fread(hostbuf, size, 1, fp);
                                fclose(fp);
                                cout << ",if=" << ifStr;
                            }

                            v = new CSBuffer(&runtime, size, (void*)hostbuf);
                            if(ofStr){
                                ofStr += 3;
                                ofileMap[v] = string(ofStr);
                                cout << ",of=" << ofStr;
                            }
                        }
                        cout << endl;
                        args[i].argVal = v;
                        bufList.push_back(v);
                    } break;
                case 'f': {
                        //float
                        cl_float *v = new cl_float[1];
                        *v = (cl_float)atof(argv[i+4] + 2);
                        args[i].argVal = v;
                        args[i].argSize = sizeof(cl_float);
                        cout << "param[" << i << "]:float:" << *v << endl;
                        floatList.push_back(v);
                    } break;
                case 'd': {
                        //double
                        cl_double *v = new cl_double[1];
                        *v = atof(argv[i+4] + 2);
                        args[i].argVal = v;
                        args[i].argSize = sizeof(cl_double);
                        cout << "param[" << i << "]:double:" << *v << endl;
                        doubleList.push_back(v);
                    } break;
                case 'i': {
                        //integer
                        int *v = new cl_int[1];
                        *v = atoi(argv[i+4] + 2);
                        args[i].argVal = v;
                        args[i].argSize = sizeof(cl_int);
                        cout << "param[" << i << "]:int:" << *v << endl;
                        intList.push_back(v);
                    } break;
                default:
                    //error
                    break;
            }
        }

        cout << "executing kernel[" << argv[2] << "] ....." << endl;
        lib.exec(argv[2], gWS, lWS, args);

        if(bufList.size()){ 
            for (list<CSBuffer*>::iterator i = bufList.begin(); i != bufList.end (); i++){
                if(ofileMap.find(*i) != ofileMap.end()){
                    cout << "outfile:" << ofileMap[(*i)] << endl;
                    size_t size = (*i)->getSize();
                    char *wb = new char[size];
                    //read-back buffer data and write
                    (*i)->read(wb);
                    FILE *fp = fopen(ofileMap[(*i)].c_str(), "wb");
                    fwrite(wb, size, 1, fp);
                    fclose(fp);

                    delete wb;
                } 
                char *hostbuf = (char*)((*i)->getHostBuf());
                if(hostbuf)
                    delete hostbuf;
                delete (*i); 
            }
        } 
        RELEASE_ARG(int, intList);
        RELEASE_ARG(float, floatList);
        RELEASE_ARG(double, doubleList);
    } catch (cl_int err) {
        cout << "error:" << CSGetErrorMsg(err) << endl;
    }

    return 0;
}

clscript
========

clscript - a utility/framework for testing &amp; executing OpenCL kernels

Writing OpenCL host program is a boring and burden for OpenCL programmer. 
CLScript provides a CLI utility and a simple framework to simplify the process
of writing OpenCL applications. The major spirit of CLScript is viewing .cl files
as library in C. OpenCL kernels are used as APIs.

Curretly CLScript is under development, any feedback is appreciated.

    In test/test.cpp, an example shows how to use CLScript framework:

    try{
        CSRuntime runtime(CS_DEV_GPU);

        CSLib lib(&runtime, (const char*)(argv[1]));
        CSBuffer buf(&runtime, 4096*sizeof(cl_int));
        CSWorkSize gWS(4096);
        lib.exec("test", gWS, NullWorkSize, 2, &buf);
    }catch(cl_int err){
        cout << argv[0] << " error: " << CSGetErrorMsg(err) << endl;
    }

Programmer can make use of OpenCL simply by using CSRuntime, CSLib and CSBuffer
classes in CLScript to use functions in .cl files. exec() in CSLib takes and 
passes various number of arguments. Programmers do not have to setup specific API
and interface to use kernel function.


CLScript also provides another CLI utility - csutil which run OpenCL kernels 
directly. Programmers can focus on writing OpenCL kernel code and test without 
writing corresponding host program.

csutil only can be used with parameters in order.

    csutil CL_FILES KERNEL_NAME WORKSIZES ARGUMENTS
    1. CL_FILES    : the path for .cl source files
                     each file must be seperated by ','.
                     eg. ../test2/test2_1.cl,../test2/test2_1.cl
    2. KERNEL_NAME : the name of called kernel function
    3. WORKSIZES   : worksizes are used to specified global & local worksize in
                     OpenCL. The sizes of global and local is seperated by ':'. 
                     And each dimension is seperated by ','.
                     eg. 
                        *) 2D 512x512 global worksize -
                           512,512 
                        *) 2D 512x512 global worksize & 16x16 local worksize
                           512,512:16,16 
    4. ARGUMENTS   :
                     *) int    - i:INT         eg. i:16
                     *) float  - f:FLOAT       eg. f:16.0
                     *) double - d:DOUBLE      eg. d.16.0
                     *) buffer - b:[sz=SIZE,][if=INPUT,][of=OUTPUT]
                                 each buffer can be created with three optional 
                                 parameters: sz, of and if.
                                 sz: SIZE of the buffer in bytes.
                                 if: binary file for initialize the buffer
                                 of: the name of outfile for dumpping the 
                                     buffer.

For example, you can try csutil with test.cl by the following command:

    csutil test.cl test 4096 i:2 b:sz=16384,of=out.bin              



#ifndef thread_context_hpp_
#define thread_context_hpp_

#define MAXTHREADS  256
#define NUM_INTS    32

// Includes POSIX thread headers for Linux thread support
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <vector>
#include <string>
#include <sstream>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>

#include <cuda.h>
#include <cuda_runtime_api.h>
#include <helper_cuda_drvapi.h>

//============================================================================//

typedef struct _CUDAContext_st
{
    CUcontext   hcuContext;
    CUmodule    hcuModule;
    CUfunction  hcuFunction;
    CUdeviceptr dptr;
    int         deviceID;
    int         threadNum;

} CUDAContext;

//============================================================================//

typedef struct _CUDAContext_pair
{
    int device_count;
    int num_threads;
    CUDAContext* gpu;
    CUDAContext* cpu;
} CUDAContextData;

//============================================================================//

typedef struct _AppArgs_t
{
    int argc;
    char** argv;
} AppArgs;

//============================================================================//

//define input ptx file
#define PTX_FILE "thread_context_kernel.ptx"
#define CUBIN_FILE "thread_context_kernel.cubin"

#define CLEANUP_ON_ERROR(dptr, hcuModule, hcuContext, status) \
    if ( dptr ) cuMemFree( dptr ); \
    if ( hcuModule ) cuModuleUnload( hcuModule ); \
    if ( hcuContext ) cuCtxDestroy( hcuContext ); \
    return status;

#define THREAD_QUIT \
    printf("Error\n"); \
    return 0;

//============================================================================//

inline AppArgs& get_args()
{
    static AppArgs _instance;
    return _instance;
}

//============================================================================//

bool inline
find_module_path(const char *module_file,
               std::string& module_path,
               char** argv,
               std::string& ptx_source)
{
    char* actual_path = sdkFindFilePath(module_file, argv[0]);

    if (actual_path)
    {
        module_path = actual_path;
    }
    else
    {
        printf("> find_module_path file not found: <%s> \n", module_file);
        return false;
    }

    if (module_path.empty())
    {
        printf("> find_module_path could not find file: <%s> \n", module_file);
        return false;
    }
    else
    {
        printf("> find_module_path found file at <%s>\n", module_path.c_str());

        if (module_path.rfind(".ptx") != std::string::npos)
        {
            FILE *fp;
            FOPEN(fp, module_path.c_str(), "rb");
            fseek(fp, 0, SEEK_END);
            int file_size = ftell(fp);
            char *buf = new char[file_size+1];
            fseek(fp, 0, SEEK_SET);
            int ret = fread(buf, sizeof(char), file_size, fp);
            if (ret == 0)
                printf("Error reading module path file: %s", module_path.c_str());
            fclose(fp);
            buf[file_size] = '\0';
            ptx_source = buf;
            delete[] buf;
        }

        return true;
    }
}

//============================================================================//

// declaration, forward
extern bool run_test();

//============================================================================//

extern CUresult init_cuda_context(CUDAContext *pContext,
                                  CUdevice hcuDevice,
                                  int deviceID,
                                  char **argv,
                                  std::string kernel_func = "kernelFunction");

//============================================================================//

extern std::string get_context_info(CUDAContext*);

//============================================================================//

extern int thread_proc(CUDAContext*);

//============================================================================//

extern bool final_cuda_check(CUDAContextData*);

//============================================================================//

extern CUDAContextData get_cuda_context(int device_count,
                                        int num_threads,
                                        std::string kernel_func);

//============================================================================//

#endif

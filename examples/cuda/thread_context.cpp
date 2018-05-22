/*
 * Copyright 1993-2015 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */

/******************************************************************************
*
*   Module: threadMigration.cpp
*
*   Description:
*     Simple sample demonstrating multi-GPU/multithread functionality using
*     the CUDA Context Management API.  This API allows the a CUDA context to be
*     associated with a CPU process. A host thread may have only one device context
*     current at a time.
*
*    Refer to the CUDA programming guide 4.5.3.3 on Context Management
*
******************************************************************************/

#include "thread_context.hpp"

using namespace std;

//============================================================================//

std::mutex& get_mutex()
{
    static std::mutex _instance;
    return _instance;
}

//============================================================================//

std::atomic_int& thread_launch_count()
{
    static std::atomic_int _instance;
    return _instance;
}

//============================================================================//

// This sample uses the Driver API interface.  The CUDA context needs
// to be setup and the CUDA module (CUBIN) is built by NVCC
CUresult
init_cuda_context(CUDAContext* pContext,
                  CUdevice hcuDevice,
                  int deviceID,
                  char** argv,
                  string kernel_func)
{
    CUcontext  hcuContext  = 0;
    CUmodule   hcuModule   = 0;
    CUfunction hcuFunction = 0;
    CUdeviceptr dptr       = 0;
    CUdevprop devProps;

    // cuCtxCreate: Function works on floating contexts and current context
    CUresult status = cuCtxCreate(&hcuContext, 0, hcuDevice);

    if (CUDA_SUCCESS != status)
    {
        fprintf(stderr, "cuCtxCreate for <deviceID=%d> failed %d\n",
                deviceID, status);
        CLEANUP_ON_ERROR(dptr, hcuModule, hcuContext, status);
    }

    status = CUDA_ERROR_INVALID_IMAGE;

    if (CUDA_SUCCESS != cuDeviceGetProperties(&devProps, hcuDevice))
    {
        printf("cuDeviceGetProperties FAILED\n");
        CLEANUP_ON_ERROR(dptr, hcuModule, hcuContext, status);
    }

    string module_path, ptx_source;

    if (!find_module_path(PTX_FILE, module_path, argv, ptx_source))
    {
        if (!find_module_path(CUBIN_FILE, module_path, argv, ptx_source))
        {
            fprintf(stderr,
                    "> find_module_path could not find <threadMigration> ptx or cubin\n");
            CLEANUP_ON_ERROR(dptr, hcuModule, hcuContext, status);
        }
    }

    status = cuModuleLoad(&hcuModule, module_path.c_str());

    if (CUDA_SUCCESS != status)
    {
        fprintf(stderr, "cuModuleLoad failed %d\n", status);
        CLEANUP_ON_ERROR(dptr, hcuModule, hcuContext, status);
    }

    status = cuModuleGetFunction(&hcuFunction, hcuModule, kernel_func.c_str());

    if (CUDA_SUCCESS != status)
    {
        fprintf(stderr, "cuModuleGetFunction failed %d\n", status);
        CLEANUP_ON_ERROR(dptr, hcuModule, hcuContext, status);
    }

    // Here we must release the CUDA context from the thread context
    status = cuCtxPopCurrent(NULL);

    if (CUDA_SUCCESS != status)
    {
        fprintf(stderr, "cuCtxPopCurrent failed %d\n", status);
        CLEANUP_ON_ERROR(dptr, hcuModule, hcuContext, status);
    }

    pContext->hcuContext  = hcuContext;
    pContext->hcuModule   = hcuModule;
    pContext->hcuFunction = hcuFunction;
    pContext->deviceID    = deviceID;

    return CUDA_SUCCESS;
}

//============================================================================//

std::string get_context_info(CUDAContext* pParams)
{
    std::stringstream ss;
    ss << "<CUDA Device=" << pParams->deviceID
       << ", Context=" << pParams->hcuContext
       << ", Thread=" << pParams->threadNum
       << ">";
    return ss.str();
}

//============================================================================//

// thread_proc launches the CUDA kernel on a CUDA context.
// We have more than one thread that talks to a CUDA context
int thread_proc(CUDAContext *pParams)
{
    printf("%s - thread_proc() Launched...\n",
           get_context_info(pParams).c_str());

    //std::unique_lock<std::mutex>(get_mutex());
    int wrong = 0;
    int* pInt = 0;

#ifdef DEBUG
    printf("%s - thread_proc() pushing context...\n",
           get_context_info(pParams).c_str());
#endif
    // cuCtxPushCurrent: Attach the caller CUDA context to the thread context.
    CUresult status = cuCtxPushCurrent(pParams->hcuContext);

#ifdef DEBUG
    printf("%s - thread_proc() context pushed...\n",
           get_context_info(pParams).c_str());
#endif

    if (CUDA_SUCCESS != status)
    {
        printf("Error! status = %i\n", status);
        THREAD_QUIT;
    }

#ifdef DEBUG
    printf("%s - thread_proc() Checking CUDA errors...\n",
           get_context_info(pParams).c_str());
#endif

    checkCudaErrors(cuMemAlloc(&pParams->dptr, NUM_INTS*sizeof(int)));

    // There are two ways to launch CUDA kernels via the Driver API.
    // In this CUDA Sample, we illustrate both ways to pass parameters
    // and specify parameters.  By default we use the simpler method.

    // This is the new CUDA 4.0 API for Kernel Parameter passing and Kernel Launching (simpler method)
    void *args[5] = { &pParams->dptr };

#ifdef DEBUG
    printf("%s - thread_proc() launching kernel...\n",
          get_context_info(pParams).c_str());
#endif

    // new CUDA 4.0 Driver API Kernel launch call
    status = cuLaunchKernel(pParams->hcuFunction, 1, 1, 1,
                            32, 1, 1,
                            0,
                            NULL, args, NULL);

    if (CUDA_SUCCESS != status)
    {
        fprintf(stderr, "cuLaunch failed %d\n", status);
        THREAD_QUIT;
    }

#ifdef DEBUG
    printf("%s - thread_proc() getting answer...\n",
           get_context_info(pParams).c_str());
#endif

    pInt = (int *) malloc(NUM_INTS*sizeof(int));

    if (!pInt)
        return 0;

    int pIntTotal = 0;

    if (CUDA_SUCCESS == cuMemcpyDtoH(pInt, pParams->dptr, NUM_INTS*sizeof(int)))
    {
        for (int i = 0; i < NUM_INTS; i++)
        {
            if (pInt[i] != 30-i)
            {
                printf("%s error [%d]=%d!\n",
                       get_context_info(pParams).c_str(), i, pInt[i]);
                wrong++;
            }
            else
            {
                pIntTotal += pInt[i];
            }
        }

        if (!wrong)
            thread_launch_count()++;

    }

    free(pInt);
    fflush(stdout);
    //checkCudaErrors(cuMemFree(pParams->dptr));

    // cuCtxPopCurrent: Detach the current CUDA context from the calling thread.
    checkCudaErrors(cuCtxPopCurrent(NULL));

    printf("%s - thread_proc() Finished!\n",
           get_context_info(pParams).c_str());

    return pIntTotal;
}

//============================================================================//

bool final_error_check(CUDAContext *pContext, int NumThreads, int device_count)
{
    if (thread_launch_count().load() != NumThreads*device_count)
    {
        printf("<Expected=%d, Actual=%d> ThreadLaunchCounts(s)\n",
               NumThreads*device_count, thread_launch_count().load());
        return false;
    }
    else
    {
        for (int iDevice = 0; iDevice < device_count; iDevice++)
        {
            // cuCtxDestroy called on current context or a floating context
            if (CUDA_SUCCESS != cuCtxDestroy(pContext[iDevice].hcuContext))
                return false;
        }

        return true;
    }
}

//============================================================================//

CUDAContextData get_cuda_context(int device_count, int num_threads,
                                 std::string kernel_func)
{
    CUresult status;
    //int argc = get_args().argc;
    char** argv = get_args().argv;

    CUDAContextData _null = { 0, 0, nullptr, nullptr };
    printf("[ %s ] API test...\n", argv[0]);

    int hcuDevice = 0;

    printf("> %d CUDA device(s), %d Thread(s)/device to launched\n\n",
           device_count, num_threads);

    if (device_count == 0)
        return _null;

    int ihThread = 0;
    int ThreadIndex = 0;

    CUDAContext* _gpu_context = new CUDAContext[device_count];
    CUDAContext* _cpu_context = new CUDAContext[device_count * num_threads];

    for (int iDevice = 0; iDevice < device_count; iDevice++)
    {
        char szName[256];
        status = cuDeviceGet(&hcuDevice, iDevice);

        if (CUDA_SUCCESS != status)
            return _null;

        status = cuDeviceGetName(szName, 256, hcuDevice);

        if (CUDA_SUCCESS != status)
            return _null;

        CUdevprop devProps;

        if (CUDA_SUCCESS == cuDeviceGetProperties(&devProps, hcuDevice))
        {
            int major = 0, minor = 0;
            checkCudaErrors(cuDeviceComputeCapability(&major, &minor, hcuDevice));
            printf("Device %d: \"%s\" (Compute %d.%d)\n", iDevice, szName, major, minor);
            printf("\tsharedMemPerBlock: %d\n", devProps.sharedMemPerBlock);
            printf("\tconstantMemory   : %d\n", devProps.totalConstantMemory);
            printf("\tregsPerBlock     : %d\n", devProps.regsPerBlock);
            printf("\tclockRate        : %d\n", devProps.clockRate);
            printf("\n");
        }

        if (CUDA_SUCCESS != init_cuda_context(&_gpu_context[iDevice],
                                              hcuDevice, iDevice, argv,
                                              kernel_func))
        {
            bool ret = final_error_check(_gpu_context, num_threads, device_count);
            if(!ret)
                printf("Error initializing CUDA contexts\n");
            return _null;
        }
        else
        {
            for (int iThread = 0; iThread < num_threads; iThread++, ihThread++)
            {
                _cpu_context[ThreadIndex].hcuContext =
                        _gpu_context[iDevice].hcuContext;
                _cpu_context[ThreadIndex].hcuModule =
                        _gpu_context[iDevice].hcuModule;
                _cpu_context[ThreadIndex].hcuFunction =
                        _gpu_context[iDevice].hcuFunction;
                _cpu_context[ThreadIndex].deviceID =
                        _gpu_context[iDevice].deviceID;
                _cpu_context[ThreadIndex].threadNum = iThread;
                ThreadIndex += 1;
            }
        }
    }

    CUDAContextData _data;
    _data.device_count = device_count;
    _data.num_threads = num_threads;
    _data.gpu = _gpu_context;
    _data.cpu = _cpu_context;
    return _data;
}

//============================================================================//

bool final_cuda_check(CUDAContextData* data)
{
    bool ret_status = final_error_check(data->gpu,
                                        data->num_threads,
                                        data->device_count);
    free(data->gpu);
    data->gpu = nullptr;
    return ret_status;
}

//============================================================================//

bool run_test()
{
    int argc = get_args().argc;
    char** argv = get_args().argv;

    printf("[ threadMigration ] API test...\n");
    // By default, we will launch 2 CUDA threads for each device
    int num_threads = 2;

    if (argc > 1)
    {
        if (checkCmdLineFlag(argc, (const char **)argv, "numthreads"))
        {
            num_threads = getCmdLineArgumentInt(argc, (const char **) argv, "numthreads");

            if (num_threads < 1 || num_threads > 15)
            {
                printf("Usage: \"%s -n=<threads>\", <threads> ranges 1-15\n",
                       argv[0]);
                return 1;
            }
        }
    }

    int device_count;
    int hcuDevice = 0;
    CUresult status;
    status = cuInit(0);

    if (CUDA_SUCCESS != status)
        return false;

    status = cuDeviceGetCount(&device_count);

    if (CUDA_SUCCESS != status)
        return false;

    if (device_count == 0)
    {
        return false;
    }

    int ihThread = 0;
    int ThreadIndex = 0;

    CUDAContext *pContext = (CUDAContext*) malloc(sizeof(CUDAContext)*device_count);

    std::vector<std::thread*>   _threads(num_threads, nullptr);
    std::vector<CUDAContext>    _params(num_threads);

    for (int iDevice = 0; iDevice < device_count; iDevice++)
    {
        char szName[256];
        status = cuDeviceGet(&hcuDevice, iDevice);

        if (CUDA_SUCCESS != status)
            return false;

        status = cuDeviceGetName(szName, 256, hcuDevice);

        if (CUDA_SUCCESS != status)
            return false;

        CUdevprop devProps;

        if (CUDA_SUCCESS == cuDeviceGetProperties(&devProps, hcuDevice))
        {
            int major = 0, minor = 0;
            checkCudaErrors(cuDeviceComputeCapability(&major, &minor, hcuDevice));
            printf("Device %d: \"%s\" (Compute %d.%d)\n", iDevice, szName, major, minor);
            printf("\tsharedMemPerBlock: %d\n", devProps.sharedMemPerBlock);
            printf("\tconstantMemory   : %d\n", devProps.totalConstantMemory);
            printf("\tregsPerBlock     : %d\n", devProps.regsPerBlock);
            printf("\tclockRate        : %d\n", devProps.clockRate);
            printf("\n");
        }

        if (CUDA_SUCCESS != init_cuda_context(&pContext[iDevice], hcuDevice, iDevice, argv))
        {
            return final_error_check(pContext, num_threads, device_count);
        }
        else
        {
            for (int iThread = 0; iThread < num_threads; iThread++, ihThread++)
            {
                _params[ThreadIndex].hcuContext = pContext[iDevice].hcuContext;
                _params[ThreadIndex].hcuModule = pContext[iDevice].hcuModule;
                _params[ThreadIndex].hcuFunction = pContext[iDevice].hcuFunction;
                _params[ThreadIndex].deviceID = pContext[iDevice].deviceID;
                _params[ThreadIndex].threadNum = iThread;
                // Launch (num_threads) for each CUDA context
                _threads[ThreadIndex] = new thread(thread_proc, &(_params[ThreadIndex]));
                ThreadIndex += 1;
            }
        }
    }

    for(auto& itr : _threads)
        itr->join();

    bool ret_status = final_error_check(pContext, num_threads, device_count);
    free(pContext);
    return ret_status;
}

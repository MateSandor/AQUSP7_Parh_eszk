#include <stdio.h>
#define CL_TARGET_OPENCL_VERSION 220
#include <stdlib.h>
#include <CL/cl.h>
#include <time.h>
#include "kernel_loader.h"
#pragma OPENCL EXTENSION cl_khr_fp64 : enable



typedef struct cubic_data{
	float a;
	float b;
	float c;
	float d;
	float x1;
	float x2;
	float x3;
}Cubic_data;


int main() {
    // Input array
    int n = 10;
    srand(time(0));
    printf("START\n");
    cl_platform_id platform;
    cl_device_id device;
    cl_uint num_platforms, num_devices;
    cl_int err;
    cl_kernel kernel;
    err = clGetPlatformIDs(1, & platform, & num_platforms);
    if (err != CL_SUCCESS) {
        printf("Error getting platform ID: %d\n", err);
        return -1;
    }
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, & device, & num_devices);
    if (err != CL_SUCCESS) {
        printf("Error getting device ID: %d\n", err);
        return -1;
    }

    // Create the OpenCL context and command queue
    cl_context context = clCreateContext(NULL, 1, & device, NULL, NULL, & err);
	
    if (err != CL_SUCCESS) {
        printf("Error creating context: %d\n", err);
        return -1;
    }
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, & err);
	
    if (err != CL_SUCCESS) {
        printf("Error creating command queue: %d\n", err);
        return -1;
    }
    // Create the kernel program and build it ///////// edit
    const char * kernel_code = load_kernel_source("kernels/harmadfoku.cl", & err);
    if (err != 0) {
        printf("Source code loading error!\n");
        return 0;
    }
    cl_program program = clCreateProgramWithSource(context, 1, & kernel_code, NULL, & err);
    if (err != CL_SUCCESS) {
        printf("Error creating program: %d\n", err);
        return -1;
    }
    err = clBuildProgram(program, 1, & device, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
		size_t real_size;
		err = clGetProgramBuildInfo(
		program,
		device,
		CL_PROGRAM_BUILD_LOG,
		0,
		NULL,
		&real_size
		);
		char* build_log = (char*)malloc(sizeof(char)*(real_size + 1));
		err = clGetProgramBuildInfo(
		program,
		device,
		CL_PROGRAM_BUILD_LOG,
		real_size+1,
		build_log,
		&real_size
		);
		printf("\n\nRealsize: %d\n", real_size);
		printf("\n\Build log: %s\n", build_log);
        printf("Error building program: %d\n", err);
        return -1;
    }
	//Using malloc, instead of arrays.
    while (n != 100000000) {
        clock_t start, end, temp;
        start = clock();
		Cubic_data * datas = (Cubic_data*)malloc(n*sizeof(Cubic_data));

	// Pre filling the randomizied data.
	
        for (int i = 0; i < n; i++) {
            datas[i].a = (float)(rand() % 2000 - 1000) / 100;
            datas[i].b = (float)(rand() % 2000 - 1000) / 100;
            datas[i].c = (float)(rand() % 2000 - 1000) / 100;
            datas[i].d = (float)(rand() % 2000 - 1000) / 100;
        }

        // Create the buffer for the input array
        cl_mem input_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(Cubic_data), NULL, & err);
        if (err != CL_SUCCESS) {
            printf("Error creating buffer: %d\n", err);
            return -1;
        }
		//output
        if (err != CL_SUCCESS) {
            printf("Error creating buffer: %d\n", err);
            return -1;
        }

        // Create the kernel and set the arguments
        kernel = clCreateKernel(program, "cubic_equation", & err);
        if (err != CL_SUCCESS) {
            printf("Error creating kernel: %d\n", err);
            return -1;
        }
		//Giving pointers to the kernel
        err = clSetKernelArg(kernel, 0, sizeof(cl_mem), & input_buffer);
        if (err != CL_SUCCESS) {
            printf("Error setting kernel arguments: %d\n", err);
            return -1;
        }

		//Writing into the buffer
        err = clEnqueueWriteBuffer(queue, input_buffer, CL_TRUE, 0, n * sizeof(Cubic_data), datas, 0, NULL, NULL);
        if (err != CL_SUCCESS) {
            printf("Error writing to buffer: %d\n", err);
            return -1;
        }
        size_t global_size = n;
		//Run the kernel code
        err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, & global_size, NULL, 0, NULL, NULL);
        if (err != CL_SUCCESS) {
            printf("Error enqueuing kernel: %d\n", err);
            return -1;
        }

        err = clEnqueueReadBuffer(queue, input_buffer, CL_TRUE, 0, n * sizeof(Cubic_data), datas, 0, NULL, NULL);
        if (err != CL_SUCCESS) {
            printf("Error reading from buffer: %d\n", err);
            return -1;
        }

        clReleaseMemObject(input_buffer);

        /*for (int i = 0; i < n; i++) {
        	printf("%f %f %f %f\n%f %f %f \n", datas[i].a,datas[i].b,datas[i].c,datas[i].d,datas[i].x1, datas[i].x2, datas[i].x3);
        }*/
        end = clock();
        printf("\n\n%d:  Overall runtime:\t%lf\n", n, ((float)(end - start)) / CLOCKS_PER_SEC);
        n *= 10;
    }
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    return 0;
}
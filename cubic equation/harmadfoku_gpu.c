#include <stdio.h>
#define CL_TARGET_OPENCL_VERSION 220
#include <stdlib.h>
#include <CL/cl.h>
#include <time.h>
#include "kernel_loader.h"
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

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
        double * a = (double * ) malloc(n * sizeof(double));
        double * b = (double * ) malloc(n * sizeof(double));
        double * c = (double * ) malloc(n * sizeof(double));
        double * d = (double * ) malloc(n * sizeof(double));
        double * x1 = (double * ) malloc(n * sizeof(double));
        double * x2 = (double * ) malloc(n * sizeof(double));
        double * x3 = (double * ) malloc(n * sizeof(double));

	// Pre filling the randomizied data.
	
        for (int i = 0; i < n; i++) {
            a[i] = (float)(rand() % 2000 - 1000) / 100;
            b[i] = (float)(rand() % 2000 - 1000) / 100;
            c[i] = (float)(rand() % 2000 - 1000) / 100;
            d[i] = (float)(rand() % 2000 - 1000) / 100;
        }

        // Create the buffer for the input array
        cl_mem a_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(double), NULL, & err);
        cl_mem b_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(double), NULL, & err);
        cl_mem c_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(double), NULL, & err);
        cl_mem d_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(double), NULL, & err);
        if (err != CL_SUCCESS) {
            printf("Error creating buffer: %d\n", err);
            return -1;
        }
		//output
        cl_mem x1_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(double), NULL, & err);
        cl_mem x2_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(double), NULL, & err);
        cl_mem x3_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, n * sizeof(double), NULL, & err);
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
        err = clSetKernelArg(kernel, 0, sizeof(cl_mem), & a_buffer);
        err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), & b_buffer);
        err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), & c_buffer);
        err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), & d_buffer);
        err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), & x1_buffer);
        err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), & x2_buffer);
        err |= clSetKernelArg(kernel, 6, sizeof(cl_mem), & x3_buffer);
        if (err != CL_SUCCESS) {
            printf("Error setting kernel arguments: %d\n", err);
            return -1;
        }

		//Writing into the buffer
        err = clEnqueueWriteBuffer(queue, a_buffer, CL_TRUE, 0, n * sizeof(double), a, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(queue, b_buffer, CL_TRUE, 0, n * sizeof(double), b, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(queue, c_buffer, CL_TRUE, 0, n * sizeof(double), c, 0, NULL, NULL);
        err |= clEnqueueWriteBuffer(queue, d_buffer, CL_TRUE, 0, n * sizeof(double), d, 0, NULL, NULL);
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

        err = clEnqueueReadBuffer(queue, x1_buffer, CL_TRUE, 0, n * sizeof(double), x1, 0, NULL, NULL);
        err |= clEnqueueReadBuffer(queue, x2_buffer, CL_TRUE, 0, n * sizeof(double), x2, 0, NULL, NULL);
        err |= clEnqueueReadBuffer(queue, x3_buffer, CL_TRUE, 0, n * sizeof(double), x3, 0, NULL, NULL);
        if (err != CL_SUCCESS) {
            printf("Error reading from buffer: %d\n", err);
            return -1;
        }

        clReleaseMemObject(a_buffer);
        clReleaseMemObject(b_buffer);
        clReleaseMemObject(c_buffer);
        clReleaseMemObject(d_buffer);
        clReleaseMemObject(x1_buffer);
        clReleaseMemObject(x2_buffer);
        clReleaseMemObject(x3_buffer);

        /*for (int i = 0; i < n; i++) {
        	printf("%lf %lf %lf \n", x1[i], x2[i], x3[i]);
        }*/
        end = clock();
        printf("\n\n%d:  Overall runtime:\t%lf\n", n, ((double)(end - start)) / CLOCKS_PER_SEC);
        n *= 10;
    }
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    return 0;
}
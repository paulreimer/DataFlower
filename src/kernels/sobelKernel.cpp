#include "sobelKernel.h"

#include "settings.h"

// integer ceiling helper function
// Helper to get next up value for integer division
//*****************************************************************************
/*
size_t DivUp(size_t dividend, size_t divisor) 
{
	return (dividend % divisor == 0) ? (dividend / divisor) : (dividend / divisor + 1);
}
*/

#define DIVUP(dividend, divisor) ((dividend % divisor == 0) ? (dividend / divisor) : (dividend / divisor + 1))

#define BLOCKDIMX 16
#define BLOCKDIMY 16

//--------------------------------------------------------------
SobelKernel::SobelKernel() : ofxClKernel("ckSobel", 2)
{
	threshold = 35.0;
	
	width = VIDEO_WIDTH;
	height = VIDEO_HEIGHT;
	
	input = NULL;
	output = NULL;

	localWorkSize[0] = BLOCKDIMX;
	localWorkSize[1] = BLOCKDIMY;
	globalWorkSize[0]= DIVUP(width, BLOCKDIMX) * BLOCKDIMX;
	globalWorkSize[1]= DIVUP(height, BLOCKDIMY) * BLOCKDIMY;
	
//		setWorkSize(lWorkSz, gWorkSz);
}

//--------------------------------------------------------------
SobelKernel::~SobelKernel() {
	if(inBuffer)	clReleaseMemObject(inBuffer);
	if(outBuffer)	clReleaseMemObject(outBuffer);
}

//--------------------------------------------------------------
void SobelKernel::update() {
	if (input == NULL || output == NULL) return;

	// Non-blocking/asynchronous copy of input image data from host to device
	clErr = clEnqueueWriteBuffer(commandQ, inBuffer, CL_FALSE, 0, bufferSizeElems*4, input, 0, NULL, NULL);
//		if (!checkOpenClError(clErr, "clEnqueueWriteBuffer")) return;
	
	enqueue();
	
	// Blocking/synchronous copy of processed image back to host
	clErr = clEnqueueReadBuffer(commandQ, outBuffer, CL_TRUE, 0, bufferSizeElems, output, 0, NULL, NULL);
//		if (!checkOpenClError(clErr, "clEnqueueReadBuffer")) return;
}

//--------------------------------------------------------------
void SobelKernel::initKernelAssets() {
	bufferSizeElems = width * height * sizeof (unsigned char);
	inBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, bufferSizeElems*4, NULL, &clErr);
	outBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bufferSizeElems, NULL, &clErr);
	checkOpenClError(clErr, "clCreateBuffer(s)");
}

//--------------------------------------------------------------
void SobelKernel::initKernelArgs() {
	// Set the Argument values for the Sobel kernel
	clErr = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&inBuffer);
	clErr |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&outBuffer);
	clErr |= clSetKernelArg(kernel, 2, ((BLOCKDIMX + 2) * (BLOCKDIMY + 2) * sizeof(cl_uchar4)), NULL);
	clErr |= clSetKernelArg(kernel, 3, sizeof(cl_uint), (void*)&width);
	clErr |= clSetKernelArg(kernel, 4, sizeof(cl_uint), (void*)&height);
	
	// Set the threshold (arg 5) in update()
}

//--------------------------------------------------------------
void SobelKernel::setKernelArgs() {
//		printf("Set threshold to %d\n", threshold);
	clErr = clSetKernelArg(kernel, 5, sizeof(cl_float), (void*)&threshold);
}

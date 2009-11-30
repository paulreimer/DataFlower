#pragma once

#include "ofxClKernel.h"

class SobelKernel : public ofxClKernel 
{
public:
	SobelKernel();
	~SobelKernel();
	
	void update();

	size_t width, height;

	char* input;
	char* output;
	float threshold;

	cl_mem inBuffer;	// OpenCL device memory buffer object:  input
	cl_mem outBuffer;	// OpenCL device memory buffer object:  output
	size_t bufferSizeElems;

protected:	
	void initKernelAssets();	
	void initKernelArgs();
	void setKernelArgs();
};

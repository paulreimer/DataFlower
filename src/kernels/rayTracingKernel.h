#pragma once

#include "ofxClKernel.h"

class RayTracingKernel : public ofxClKernel 
{
public:
	RayTracingKernel();
	~RayTracingKernel();

	void update();
	bool updateVolume(cl_context context, unsigned char* h_volume, int w, int h, int d);	

	int width, height;
	
	float density;
	float brightness;
	float transferOffset;
	float transferScale;
	
	float viewTranslation[3];

	GLuint pbo;	// OpenGL pixel buffer object

	float invViewMatrix[12];
	
	float transferFunc[];

	cl_mem pbo_cl;
	cl_mem d_volumeArray;
	cl_mem d_transferFuncArray;
	cl_mem d_invViewMatrix;
	cl_sampler d_volumeSampler;
	cl_sampler d_transferFuncSampler;

protected:
	void initKernelAssets();
	void initKernelArgs();
	void setKernelArgs();
	
	bool setup(cl_context _context, cl_command_queue _commandQ);
};

#include "settings.h"
#include "ofxClKernel.h"

#define VOLUME_WIDTH	VIDEO_WIDTH
#define VOLUME_HEIGHT	VIDEO_HEIGHT
#define VOLUME_DEPTH	90
#define VOLUME_SIZE		VOLUME_WIDTH*VOLUME_HEIGHT*VOLUME_DEPTH

class RayTracingKernel : public ofxClKernel 
{
public:
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
	
	//--------------------------------------------------------------
	RayTracingKernel() : ofxClKernel("d_render", 2)
	{
		density = 0.15f;
		brightness = 1.5f;
		transferOffset = 0.5f;
		transferScale = 0.65f;
		
		pbo = 0;

		width = 512;
		height = 512;
		
		globalWorkSize[0] = width;
		globalWorkSize[1] = height;
		
		d_volumeArray = NULL;
		
		viewTranslation[0] = 0.0;
		viewTranslation[1] = 0.0;
		viewTranslation[2] = 0.0;
	}
	
	//--------------------------------------------------------------
	~RayTracingKernel() {
		if(d_volumeArray)		clReleaseMemObject(d_volumeArray);
		if(d_transferFuncArray)	clReleaseMemObject(d_transferFuncArray);
		//
		//		if(pbo_cl)				clReleaseMemObject(pbo_cl);
		if(d_invViewMatrix)		clReleaseMemObject(d_invViewMatrix);
		glDeleteBuffersARB		(1, &pbo);
	}
	
	//--------------------------------------------------------------
	void update() {
		// Transfer ownership of buffer from GL to CL
#ifdef GL_INTEROP
		// Acquire PBO for OpenCL writing
		clErr = clEnqueueAcquireGLObjects(commandQ, 1, &pbo_cl, 0, 0, 0);
		if (!checkOpenClError(clErr, "clEnqueueAcquireGLObjects")) return;		
#endif
		clErr = clEnqueueWriteBuffer(commandQ, d_invViewMatrix,CL_FALSE, 0,12*sizeof(float), invViewMatrix, 0, 0, 0);
//		if (!checkOpenClError(clErr, "clEnqueueWriteBuffer")) return;		
		
		enqueue();
		
#ifdef GL_INTEROP
		// Transfer ownership of buffer back from CL to GL
		clErr = clEnqueueReleaseGLObjects(commandQ, 1, &pbo_cl, 0, 0, 0);
		if (!checkOpenClError(clErr, "clEnqueueReleaseGLObjects")) return;		
#else
		// Explicit Copy
		// map the PBO to copy data from the CL buffer via host
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
		
		// map the buffer object into client's memory
		GLubyte* ptr = (GLubyte*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB,
												GL_WRITE_ONLY_ARB);
		clErr = clEnqueueReadBuffer(commandQ, pbo_cl, CL_TRUE, 0,
							sizeof(unsigned int) * width * height,
							ptr, 0, NULL, NULL);
//		if (!checkOpenClError(clErr, "clEnqueueReadBuffer")) return;		
		glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);
#endif		
	}

	//--------------------------------------------------------------
	bool updateVolume(cl_context context, unsigned char* h_volume, int w, int h, int d)
	{
		// create 3D array and copy data to device
		cl_image_format volume_format;
		volume_format.image_channel_order = CL_R;
		volume_format.image_channel_data_type = CL_UNORM_INT8;
		
		printf("Load image of size: (%d, %d, %d)\n", w, h, d);

		if(d_volumeArray != NULL) clReleaseMemObject(d_volumeArray);

		d_volumeArray = clCreateImage3D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &volume_format,
										w,h,d,
										w, w*h,
										h_volume, &clErr);
		if (!checkOpenClError(clErr, "clCreateImage3D")) return false;
		
		clErr = clSetKernelArg(kernel, 8, sizeof(cl_mem), (void *) &d_volumeArray);		
		
		return checkOpenClError(clErr, "clSetKernelArg 8");
	}
	

protected:
	//--------------------------------------------------------------
	void initKernelAssets() {
		// create transfer function texture
		float transferFunc[] = {
			0.0, 0.0, 0.0, 0.0,
			1.0, 0.0, 0.0, 1.0,
			1.0, 0.5, 0.0, 1.0,
			1.0, 1.0, 0.0, 1.0,
			0.0, 1.0, 0.0, 1.0,
			0.0, 1.0, 1.0, 1.0,
			0.0, 0.0, 1.0, 1.0,
			1.0, 0.0, 1.0, 1.0,
			0.0, 0.0, 0.0, 0.0,
		};
		
		cl_image_format transferFunc_format;
		transferFunc_format.image_channel_order = CL_RGBA;
		transferFunc_format.image_channel_data_type = CL_FLOAT;
		
		d_transferFuncArray = clCreateImage2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
											  &transferFunc_format,
											  9,1, sizeof(float) * 9 * 4,
											  transferFunc,
											  &clErr);
//		if (!checkOpenClError(clErr, "clCreateImage2D")) return;
		
		clErr |= clSetKernelArg(kernel, 9, sizeof(cl_mem), (void *) &d_transferFuncArray);
		if (!checkOpenClError(clErr, "clCreateImage2D (arg 9)")) return;
		
		// init invViewMatrix
		d_invViewMatrix = clCreateBuffer(context, CL_MEM_READ_ONLY,
										 12 * sizeof(float), 0,
										 &clErr);
		
		clErr |= clSetKernelArg(kernel, 7, sizeof(cl_mem), (void *) &d_invViewMatrix);
		if (!checkOpenClError(clErr, "clCreateBuffer (arg 7)")) return;
	}
	
	void initKernelArgs() {
		clErr = clSetKernelArg(kernel, 1, sizeof(unsigned int), &width);
		clErr |= clSetKernelArg(kernel, 2, sizeof(unsigned int), &height);
		if (!checkOpenClError(clErr, "initKernelArgs 1, 2")) return;
	}	
	
	void setKernelArgs() {
		// execute OpenCL kernel, writing results to PBO
		clErr = clSetKernelArg(kernel, 3, sizeof(float), &density);
		clErr |= clSetKernelArg(kernel, 4, sizeof(float), &brightness);
		clErr |= clSetKernelArg(kernel, 5, sizeof(float), &transferOffset);
		clErr |= clSetKernelArg(kernel, 6, sizeof(float), &transferScale);
//		if (!checkOpenClError(clErr, "setKernelArgs 3, 4, 5, 6")) return;		
	}
	
	//--------------------------------------------------------------
	bool setup(cl_context _context, cl_command_queue _commandQ)
	{
		if (!ofxClKernel::setup(_context, _commandQ)) return false;
		
		if (pbo)
	{
			// delete old buffer
			clReleaseMemObject(pbo_cl);
			glDeleteBuffersARB(1, &pbo);
		}
		
		// create pixel buffer object for display
		glGenBuffersARB(1, &pbo);
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
		glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, width * height * sizeof(GLubyte) * 4, 0, GL_STREAM_DRAW_ARB);
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
		
	#ifdef GL_INTEROP
		// create OpenCL buffer from GL PBO
		pbo_cl = clCreateFromGLBuffer(context, CL_MEM_WRITE_ONLY, pbo, &clErr);
		if (!checkOpenClError(clErr, "clCreateFromGLBuffer")) return false;
	#else
		pbo_cl = clCreateBuffer(context, CL_MEM_WRITE_ONLY, width * height * sizeof(GLubyte) * 4, NULL, &clErr);
		if (!checkOpenClError(clErr, "clCreateBuffer")) return false;
	#endif		

		clErr = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &pbo_cl);
		if (!checkOpenClError(clErr, "clSetKernelArg 0")) return false;		
	}

};
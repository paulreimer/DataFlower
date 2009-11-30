#pragma once

#include "VideoFilter.h"

#ifdef USE_OPENCL
#include "../kernels/rayTracingKernel.h"
// Unfortunately, we schedule our own invocation
#include "ofxClScheduler.h"
#endif

class VoxelBufferFilter : public GrayscaleFilter
{
public:
#ifdef USE_OPENCL
	VoxelBufferFilter(ofxClScheduler& _clScheduler, RayTracingKernel& _rayTracer);
	VoxelBufferFilter();
	virtual ~VoxelBufferFilter();

	void setup();
	void update();

	void destroy();
#endif

	void keyPressed(int key);

	class VoxelBufferSettings {
		friend class VoxelBufferFilter;
	protected:
		bool	do_capture;

		VoxelBufferSettings() {
		}
	} settings;

protected:
	int frameOffsetBytes;
	int numCaptureFrames;
	int capturedFrames;
	ofxSimpleGuiSliderInt* captureProgressSlider;

	unsigned char* h_volume;

	int volumeSize[3];
	void resizeVolume(int width, int height, int depth);

#ifdef USE_OPENCL
	ofxClScheduler* clScheduler;
	RayTracingKernel* rayTracer;
#endif
};

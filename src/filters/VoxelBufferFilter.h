#pragma once

#include "VideoFilter.h"

#include "../kernels/rayTracingKernel.cpp"
// Unfortunately, we schedule our own invocation
#include "ofxClScheduler.h"

class VoxelBufferFilter : public GrayscaleFilter  {
public:
	VoxelBufferFilter(ofxClScheduler& _clScheduler, RayTracingKernel& _rayTracer);
	VoxelBufferFilter();
	virtual ~VoxelBufferFilter();

	void setup();
	void update();
	void keyPressed(int key);
	
	void destroy();

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

	ofxClScheduler* clScheduler;
	RayTracingKernel* rayTracer;
};

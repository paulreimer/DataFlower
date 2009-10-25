#pragma once

#include "VoxelBufferFilter.h"

#ifdef USE_OPENCL

VoxelBufferFilter::VoxelBufferFilter(ofxClScheduler& _clScheduler, RayTracingKernel& _rayTracer)
{
	if (verbose) printf("VoxelBufferFilter::VoxelBufferFilter(_clScheduler, _rayTracer)\n");
	clScheduler = &_clScheduler;
	rayTracer = &_rayTracer;
}

VoxelBufferFilter::VoxelBufferFilter()
{
	if (verbose) printf("VoxelBufferFilter::VoxelBufferFilter()\n");
}

VoxelBufferFilter::~VoxelBufferFilter()
{
	if (verbose) printf("VoxelBufferFilter::~VoxelBufferFilter()\n");
	destroy();
}

void VoxelBufferFilter::setup()
{
	GrayscaleFilter::setup();

	numCaptureFrames = 90;
	frameOffsetBytes = 0;
	capturedFrames = 0;

	volumeSize[0] = videoSize.x;
	volumeSize[1] = videoSize.y;
	volumeSize[2] = numCaptureFrames;

	addToggle("Capture", settings.do_capture);
	addSlider("Num. Capture Frames", numCaptureFrames, 1, 512);

	captureProgressSlider = &addSlider("Capture Progress", capturedFrames, 0, numCaptureFrames);

	h_volume = (unsigned char*)malloc(videoSize.x*videoSize.y*numCaptureFrames);

	enableKeyEvents();
}

void VoxelBufferFilter::update()
{
	if (numCaptureFrames != volumeSize[2] && !isMouseDown()) {
		resizeVolume(videoSize.x, videoSize.y, numCaptureFrames);
		captureProgressSlider->max = numCaptureFrames;

		capturedFrames = 0;
		frameOffsetBytes = 0;
	}

	if (settings.do_capture)
	{
		if (capturedFrames < numCaptureFrames)
	{
			char* latestFrame = input.getCvImage()->imageData;

			frameOffsetBytes = volumeSize[0]*volumeSize[1]*capturedFrames;

			// We add our frame to the accumulator
			memcpy(h_volume+(frameOffsetBytes), latestFrame, videoSize.x*videoSize.y);
			// and increment the pointer offset by 1 frame size
//			frameOffsetBytes += videoSize.x*videoSize.y;
			capturedFrames++;
		}
		if (capturedFrames >= numCaptureFrames)
	{
			settings.do_capture = false;
			// Update the GPU's copy
			rayTracer->updateVolume(clScheduler->context, h_volume, volumeSize[0], volumeSize[1], volumeSize[2]);

			// Reset the pointer offset
			frameOffsetBytes = 0;
		}
	}
}

void VoxelBufferFilter::destroy()
{
	free(h_volume);

	printf("VoxelBufferFilter::destroy()\n");
}

#endif

//--------------------------------------------------------------
void VoxelBufferFilter::resizeVolume(int width, int height, int depth)
{
	// Case 1: same size
	if (width == volumeSize[0] && height == volumeSize[1] && depth == volumeSize[2]) {
		//		break;

		// Case 2: requires less memory than allocated, so free the tail
	} else if (width == volumeSize[1] && height == volumeSize[1] && depth < volumeSize[2]) {
		free(h_volume + (volumeSize[0]*volumeSize[1]*depth));

		// Case 3: free and reallocate entire volume
	}
	else {
		free(h_volume);
		h_volume = (unsigned char*)malloc(width*height*depth);
	}

	volumeSize[0] = width;
	volumeSize[1] = height;
	volumeSize[2] = depth;

	// Extremely unlikely Case 4: same memory requirements (ie width+, height-)
}

void VoxelBufferFilter::keyPressed(int key)
{
    switch(key)
	{
		case ' ':
			settings.do_capture = !settings.do_capture;
			break;
	}
}

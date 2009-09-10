#pragma once
/*
MORPHING_METHOD_NONE = 0,
MORPHING_METHOD_ERODE = 1,
MORPHING_METHOD_ERODE_ERODE	= 2,
MORPHING_METHOD_ERODE_DILATE = 3
*/

#ifdef USE_OPENCV_TRUNK
#include "AdaptiveSkinFilter.h"

AdaptiveSkinFilter::AdaptiveSkinFilter() : filter(1, CvAdaptiveSkinDetector::MORPHING_METHOD_ERODE_DILATE){
	if (verbose) printf("AdaptiveSkinFilter::AdaptiveSkinFilter()\n");
}

AdaptiveSkinFilter::~AdaptiveSkinFilter() {
	if (verbose) printf("AdaptiveSkinFilter::~AdaptiveSkinFilter()\n");
	destroy();
}

void AdaptiveSkinFilter::setup() {
	ColorFilter::setup();

	bgrInput.allocate(videoSize.x, videoSize.y);
	grayOutput.allocate(videoSize.x, videoSize.y);

	addContent("Output", &output);
	addContent("BGR Input", &bgrInput);
	addSlider("Morphing Method",
			  &settings.morphing_mode, 0, 3);
}

void AdaptiveSkinFilter::update() {
	cvCvtColor(input.getCvImage(), bgrInput.getCvImage(), CV_RGB2BGR);
	bgrInput.flagImageChanged();

	filter.process(bgrInput.getCvImage(), grayOutput.getCvImage());
	output = grayOutput;
}

void AdaptiveSkinFilter::destroy() {
	bgrInput.clear();
	grayOutput.clear();

	if (verbose) printf("AdaptiveSkinFilter::destroy()\n");
}

#endif
#pragma once
/*
MORPHING_METHOD_NONE = 0,
MORPHING_METHOD_ERODE = 1,
MORPHING_METHOD_ERODE_ERODE	= 2,
MORPHING_METHOD_ERODE_DILATE = 3
*/

#include "AdaptiveSkinFilter.h"

AdaptiveSkinFilter::AdaptiveSkinFilter() : filter(1, CvAdaptiveSkinDetector::MORPHING_METHOD_ERODE_DILATE){
	printf("AdaptiveSkinFilter::AdaptiveSkinFilter()\n");
	settings.sampling_scale = 1;
	settings.morphing_mode = CvAdaptiveSkinDetector::MORPHING_METHOD_ERODE;
}

AdaptiveSkinFilter::~AdaptiveSkinFilter() {
	printf("AdaptiveSkinFilter::~AdaptiveSkinFilter()\n");
	destroy();
}

void AdaptiveSkinFilter::setup() {
	VideoFilter::setup();

	bgrInput.allocate(VIDEO_SIZE);
	grayOutput.allocate(VIDEO_SIZE);

	addContent("Output", &output);
	addContent("BGR Input", &bgrInput);
	addSlider("Morphing Method",
			  &settings.morphing_mode, 0, 3);
}

void AdaptiveSkinFilter::update() {
//	VideoFilter::update();
	cvCvtColor(input.getCvImage(), bgrInput.getCvImage(), CV_RGB2BGR);
	bgrInput.flagImageChanged();

	filter.process(bgrInput.getCvImage(), grayOutput.getCvImage());
	output = grayOutput;
}

void AdaptiveSkinFilter::destroy() {
	printf("AdaptiveSkinFilter::destroy()\n");
}

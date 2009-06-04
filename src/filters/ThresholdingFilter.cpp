#pragma once

#include "ThresholdingFilter.h"

ThresholdingFilter::ThresholdingFilter() {
	printf("ThresholdingFilter::ThresholdingFilter()\n");
	settings.threshold = 120;
	settings.max_value = 255;
	settings.mode = CV_THRESH_BINARY;
	settings.use_otsu = false;
}

ThresholdingFilter::~ThresholdingFilter() {
	printf("ThresholdingFilter::~ThresholdingFilter()\n");
	destroy();
}

void ThresholdingFilter::setup() {
	VideoFilter::setup();

	grayInput.allocate(VIDEO_SIZE);
	grayOutput.allocate(VIDEO_SIZE);

	addContent("Output", &output);
	addSlider("Threshold",
			  &settings.threshold, 0, 255);
	addSlider("Max Value",
			  &settings.max_value, 0, 255);
	addSlider("Mode",
			  &settings.mode, 0, 4);
	addToggle("Use Otsu",
			  &settings.use_otsu);
}

void ThresholdingFilter::update() {
//	VideoFilter::update();
	grayInput = input;
	
	int mode = settings.mode;
	if (settings.use_otsu)
		mode |= CV_THRESH_OTSU;
	
	cvThreshold(grayInput.getCvImage(), grayOutput.getCvImage(),
				settings.threshold, settings.max_value, mode);

    grayOutput.flagImageChanged();
	
//	grayOutput.threshold(settings.threshold);
	output = grayOutput;
}

void ThresholdingFilter::destroy() {
	printf("ThresholdingFilter::destroy()\n");
}

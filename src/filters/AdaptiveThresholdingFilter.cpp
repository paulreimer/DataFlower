#pragma once

#include "AdaptiveThresholdingFilter.h"

AdaptiveThresholdingFilter::AdaptiveThresholdingFilter() {
	printf("AdaptiveThresholdingFilter::AdaptiveThresholdingFilter()\n");
	settings.blocksize = 9;
	settings.offset = 0;
	settings.invert = false;
	settings.use_gauss = false;
}

AdaptiveThresholdingFilter::~AdaptiveThresholdingFilter() {
	printf("AdaptiveThresholdingFilter::~AdaptiveThresholdingFilter()\n");
	destroy();
}

void AdaptiveThresholdingFilter::setup() {
	VideoFilter::setup();

	grayOutput.allocate(VIDEO_SIZE);

	addContent("Output", &output);
	addSlider("Block Size",
			  &settings.blocksize, 1, 49);
	addSlider("Offset",
			  &settings.offset, 0, 255);
	addToggle("Invert",
			  &settings.invert);
	addToggle("Use Gauss",
			  &settings.use_gauss);
}

void AdaptiveThresholdingFilter::update() {
//	VideoFilter::update();
	grayOutput = input;
	
	grayOutput.adaptiveThreshold(settings.blocksize, settings.offset,
								 settings.invert, settings.use_gauss);
	output = grayOutput;
}

void AdaptiveThresholdingFilter::destroy() {
	printf("AdaptiveThresholdingFilter::destroy()\n");
}

#pragma once

#include "ThresholdingFilter.h"

ThresholdingFilter::ThresholdingFilter(string name) : VideoFilter(name) {
	printf("ThresholdingFilter::ThresholdingFilter()\n");
	settings.threshold = 65;
}

ThresholdingFilter::~ThresholdingFilter() {
	printf("ThresholdingFilter::~ThresholdingFilter()\n");
	destroy();
}

void ThresholdingFilter::setup() {
	VideoFilter::setup();

	grayOutput.allocate(VIDEO_SIZE);

	addContent("Output", &output);
	addSlider("Threshold",
				   &settings.threshold, 0, 255);
}

void ThresholdingFilter::update() {
//	VideoFilter::update();
	grayOutput = input;
	grayOutput.threshold(settings.threshold);
	output = grayOutput;
}

void ThresholdingFilter::destroy() {
	printf("ThresholdingFilter::destroy()\n");
}

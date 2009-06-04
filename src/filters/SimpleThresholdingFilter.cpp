#pragma once

#include "SimpleThresholdingFilter.h"

SimpleThresholdingFilter::SimpleThresholdingFilter() {
	printf("SimpleThresholdingFilter::SimpleThresholdingFilter()\n");
	settings.threshold = 120;
	settings.invert = false;
}

SimpleThresholdingFilter::~SimpleThresholdingFilter() {
	printf("SimpleThresholdingFilter::~SimpleThresholdingFilter()\n");
	destroy();
}

void SimpleThresholdingFilter::setup() {
	VideoFilter::setup();

	grayOutput.allocate(VIDEO_SIZE);

	addContent("Output", &output);
	addSlider("Threshold",
			  &settings.threshold, 0, 255);
	addToggle("Invert",
			  &settings.invert);
}

void SimpleThresholdingFilter::update() {
//	VideoFilter::update();
	grayOutput = input;
	
	grayOutput.threshold(settings.threshold, settings.invert);
	output = grayOutput;
}

void SimpleThresholdingFilter::destroy() {
	printf("SimpleThresholdingFilter::destroy()\n");
}

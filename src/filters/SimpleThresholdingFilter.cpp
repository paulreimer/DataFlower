#pragma once

#include "SimpleThresholdingFilter.h"

SimpleThresholdingFilter::SimpleThresholdingFilter() {
	if (verbose) printf("SimpleThresholdingFilter::SimpleThresholdingFilter()\n");
}

SimpleThresholdingFilter::~SimpleThresholdingFilter() {
	if (verbose) printf("SimpleThresholdingFilter::~SimpleThresholdingFilter()\n");
	destroy();
}

void SimpleThresholdingFilter::setup() {
	GrayscaleFilter::setup();

	addContent("Output", output);
	addSlider("Threshold", settings.threshold, 0, 255);
	addToggle("Invert", settings.invert);
}

void SimpleThresholdingFilter::update() {
	output = input;
	output.threshold(settings.threshold, settings.invert);
}

void SimpleThresholdingFilter::destroy() {
	if (verbose) printf("SimpleThresholdingFilter::destroy()\n");
}

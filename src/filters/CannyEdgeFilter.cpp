#pragma once

#include "CannyEdgeFilter.h"

CannyEdgeFilter::CannyEdgeFilter() {
	settings.low_threshold = 200;
	settings.high_threshold = 400;
	settings.aperture_size = 5;
	printf("CannyEdgeFilter::CannyEdgeFilter()\n");
}

CannyEdgeFilter::~CannyEdgeFilter() {
	printf("CannyEdgeFilter::~CannyEdgeFilter()\n");
	destroy();
}

void CannyEdgeFilter::setup() {
	VideoFilter::setup();

	grayInput.allocate(VIDEO_SIZE);
	grayOutput.allocate(VIDEO_SIZE);

	addContent("Output", &output);
	addSlider("Low Threshold",
			  &settings.low_threshold, 0, 5000);
	addSlider("High Threshold",
			  &settings.high_threshold, 0, 10000);
	addSlider("Aperture Size",
			  &settings.aperture_size, 0, 10);
}

void CannyEdgeFilter::update() {
	grayInput = input;
	cvCanny(grayInput.getCvImage(), grayOutput.getCvImage(),
			settings.low_threshold, settings.high_threshold,
			settings.aperture_size);
	output = grayOutput;
}

void CannyEdgeFilter::destroy() {
	printf("CannyEdgeFilter::destroy()\n");
}

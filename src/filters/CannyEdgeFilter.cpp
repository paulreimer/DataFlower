#pragma once

#include "CannyEdgeFilter.h"

CannyEdgeFilter::CannyEdgeFilter() {
	if (verbose) printf("CannyEdgeFilter::CannyEdgeFilter()\n");
}

CannyEdgeFilter::~CannyEdgeFilter() {
	if (verbose) printf("CannyEdgeFilter::~CannyEdgeFilter()\n");
	destroy();
}

void CannyEdgeFilter::setup() {
	GrayscaleFilter::setup();

	addContent("Output", output);
	addSlider("Low Threshold", settings.low_threshold, 0, 5000);
	addSlider("High Threshold", settings.high_threshold, 0, 10000);
	addSlider("Aperture Size", settings.aperture_size, 0, 10);
}

void CannyEdgeFilter::update() {
	cvCanny(input.getCvImage(), input.getCvImage(),
			settings.low_threshold, settings.high_threshold,
			settings.aperture_size);
}

void CannyEdgeFilter::destroy() {
	if (verbose) printf("CannyEdgeFilter::destroy()\n");
}

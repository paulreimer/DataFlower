#pragma once

#include "ContourFindingFilter.h"

ContourFindingFilter::ContourFindingFilter(string name) : VideoFilter(name) {
	printf("ContourFindingFilter::ContourFindingFilter()\n");
}

ContourFindingFilter::~ContourFindingFilter() {
	printf("ContourFindingFilter::~ContourFindingFilter()\n");
	destroy();
}

void ContourFindingFilter::setup() {	
	VideoFilter::setup();
}

void ContourFindingFilter::update() {
	VideoFilter::setup();

	contourFinder.findContours(grayInput, 20, (VIDEO_WIDTH*VIDEO_HEIGHT)/3, 10, false);
}

void ContourFindingFilter::destroy() {
	printf("ContourFindingFilter::destroy()\n");
}

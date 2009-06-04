#pragma once

#include "ContourFindingFilter.h"

ContourFindingFilter::ContourFindingFilter() {
	printf("ContourFindingFilter::ContourFindingFilter()\n");
	settings.num_blobs = 10;
	settings.find_holes = false;
}

ContourFindingFilter::~ContourFindingFilter() {
	printf("ContourFindingFilter::~ContourFindingFilter()\n");
	destroy();
}

void ContourFindingFilter::setup() {
	VideoFilter::setup();
	grayInput.allocate(VIDEO_SIZE);

	addContent("Output", &output);
	addSlider("Blobs",
			  &settings.num_blobs, 1, 10);
	addToggle("Find Holes", &settings.find_holes);
}

void ContourFindingFilter::update() {
	grayInput = input;
	contourFinder.findContours(grayInput, 20, (VIDEO_WIDTH*VIDEO_HEIGHT)/3,
							   settings.num_blobs, settings.find_holes);
}

void ContourFindingFilter::draw() {
	VideoFilter::draw();
	contourFinder.draw(x, y, width, width*VIDEO_INVWIDTH*VIDEO_HEIGHT);
}

void ContourFindingFilter::destroy() {
	printf("ContourFindingFilter::destroy()\n");
}

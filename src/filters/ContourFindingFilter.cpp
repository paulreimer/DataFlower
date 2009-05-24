#pragma once

#include "ContourFindingFilter.h"

ContourFindingFilter::ContourFindingFilter(string name) : VideoFilter(name) {
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

	addContent("Output", &output, VIDEO_WIDTH);
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
	for (int i = 0; i < contourFinder.nBlobs; i++){
		contourFinder.blobs[i].draw(x, y);
	}
}

void ContourFindingFilter::destroy() {
	printf("ContourFindingFilter::destroy()\n");
}

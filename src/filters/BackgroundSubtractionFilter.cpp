#pragma once

#include "BackgroundSubtractionFilter.h"


BackgroundSubtractionFilter::BackgroundSubtractionFilter() {
	if (verbose) printf("BackgroundSubtractionFilter::BackgroundSubtractionFilter()\n");
}

BackgroundSubtractionFilter::~BackgroundSubtractionFilter() {
	if (verbose) printf("BackgroundSubtractionFilter::~BackgroundSubtractionFilter()\n");
	destroy();
}

void BackgroundSubtractionFilter::setup() {
	ColorFilter::setup();

    inputCapture.allocate(videoSize.x, videoSize.y);
	grayCapture.allocate(videoSize.x, videoSize.y);

	fgMask.allocate(videoSize.x, videoSize.y);

	addContent("Output", output);
	addContent("Background", grayCapture);
	addButton("Capture", settings.capture);
/*
	addSlider("Block Size",
			  &settings.blocksize, 1, 49);
	addSlider("Offset",
			  &settings.offset, 0, 255);
	addToggle("Use Gauss",
			  &settings.use_gauss);	
*/
	addToggle("Invert", settings.invert);
	
	addSlider("Threshold", settings.threshold, 0, 255);
}

void BackgroundSubtractionFilter::update() {
	fgMask = input;

	fgMask.absDiff(fgMask, grayCapture);
//	fgMask.adaptiveThreshold(settings.blocksize, settings.offset,
//							 settings.invert, settings.use_gauss);

	fgMask.threshold(settings.threshold, settings.invert);

	output.set(0);
	cvCopy(input.getCvImage(), output.getCvImage(), fgMask.getCvImage());
	
	if (settings.capture == true){
		settings.capture = false;
		inputCapture = input;
		grayCapture = inputCapture;
	}
}

void BackgroundSubtractionFilter::destroy() {
	inputCapture.clear();
	grayCapture.clear();
	fgMask.clear();
	
	if (verbose) printf("BackgroundSubtractionFilter::destroy()\n");
}

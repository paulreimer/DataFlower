#pragma once

#include "BackgroundSubtractionFilter.h"


BackgroundSubtractionFilter::BackgroundSubtractionFilter() {
	printf("BackgroundSubtractionFilter::BackgroundSubtractionFilter()\n");
	settings.capture = true;

	settings.blocksize = 9;
	settings.offset = 0;
	settings.invert = false;
	settings.use_gauss = false;

	//settings.threshold = 120;
}

BackgroundSubtractionFilter::~BackgroundSubtractionFilter() {
	printf("BackgroundSubtractionFilter::~BackgroundSubtractionFilter()\n");
	destroy();
}

void BackgroundSubtractionFilter::setup() {
	VideoFilter::setup();

    inputCapture.allocate(VIDEO_SIZE);
	grayCapture.allocate(VIDEO_SIZE);

	fgMask.allocate(VIDEO_SIZE);

	addContent("Output", &output);
	addContent("Background", &grayCapture);
	addButton("Capture", &settings.capture);

	addSlider("Block Size",
			  &settings.blocksize, 1, 49);
	addSlider("Offset",
			  &settings.offset, 0, 255);
	addToggle("Invert",
			  &settings.invert);
	addToggle("Use Gauss",
			  &settings.use_gauss);	
	
//	addSlider("Threshold", &settings.threshold, 0, 255);
}

void BackgroundSubtractionFilter::update() {
	fgMask = input;

	fgMask.absDiff(fgMask, grayCapture);
	fgMask.adaptiveThreshold(settings.blocksize, settings.offset,
							 settings.invert, settings.use_gauss);

//	fgMask.threshold(settings.threshold, settings.invert);

	output.set(0);
	cvCopy(input.getCvImage(), output.getCvImage(), fgMask.getCvImage());
	
	if (settings.capture == true){
		settings.capture = false;
		inputCapture = input;
		grayCapture = inputCapture;
	}
}

void BackgroundSubtractionFilter::destroy() {
	printf("BackgroundSubtractionFilter::destroy()\n");
}

#pragma once

#include "DifferencingFilter.h"


DifferencingFilter::DifferencingFilter(string name) : VideoFilter(name) {
	printf("DifferencingFilter::DifferencingFilter()\n");
	settings.capture = true;
	settings.once = true;
}

DifferencingFilter::~DifferencingFilter() {
	printf("DifferencingFilter::~DifferencingFilter()\n");
	destroy();
}

void DifferencingFilter::setup() {
	VideoFilter::setup();

	grayInput.allocate(VIDEO_SIZE);
	grayOutput.allocate(VIDEO_SIZE);

    inputCapture.allocate(VIDEO_SIZE);
	grayCapture.allocate(VIDEO_SIZE);

	addContent("Output", &output);
	addContent("Background", &grayCapture);
	addButton("Capture", &settings.capture);
}

void DifferencingFilter::update() {
//	VideoFilter::update();
	grayInput = input;
	
	grayInput.absDiff(grayInput, grayCapture);
	output = grayInput;

	if (settings.capture == true){
		settings.capture = !settings.once;
		inputCapture = input;
		grayCapture = inputCapture;
	}
}

void DifferencingFilter::destroy() {
	printf("DifferencingFilter::destroy()\n");
}

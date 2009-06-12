#pragma once

#include "CaptureFilter.h"


CaptureFilter::CaptureFilter() {
	printf("CaptureFilter::CaptureFilter()\n");
	settings.capture = false;
}

CaptureFilter::~CaptureFilter() {
	printf("CaptureFilter::~CaptureFilter()\n");
	destroy();
}

void CaptureFilter::setup() {
	VideoFilter::setup();

	addContent("Output", &output);
	addButton("Capture", &settings.capture);
}

void CaptureFilter::update() {
	if (settings.capture == true){
		settings.capture = false;

		ofxCvColorImage* capture = new ofxCvColorImage();
		capture->allocate(VIDEO_SIZE);
		*capture = input;

		captures.push_back(capture);
//		addContent("Capture #"+captures.size(), capture);
	}
}

void CaptureFilter::destroy() {
	printf("CaptureFilter::destroy()\n");
//	delete captures;
}

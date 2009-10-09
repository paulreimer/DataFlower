#pragma once

#include "CaptureFilter.h"


CaptureFilter::CaptureFilter()
{
	if (verbose) printf("CaptureFilter::CaptureFilter()\n");
}

CaptureFilter::~CaptureFilter()
{
	if (verbose) printf("CaptureFilter::~CaptureFilter()\n");
	destroy();
}

void CaptureFilter::setup() 
{
	ColorFilter::setup();

	addContent("Output", output);
	addButton("Capture", settings.capture);
}

void CaptureFilter::update() 
{
	if (settings.capture == true)
	{
		settings.capture = false;

		ofxCvColorImage* capture = new ofxCvColorImage();
		capture->allocate(videoSize.x, videoSize.y);
		*capture = input;

		captures.push_back(capture);
//		addContent("Capture #"+captures.size(), capture);
	}
}

void CaptureFilter::destroy() 
{
	if (verbose) printf("CaptureFilter::destroy()\n");
	for (int i=0; i < captures.size(); i++)
		delete captures[i];
}

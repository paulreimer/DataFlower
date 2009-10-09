#pragma once

#include "MotionHistoryImageFilter.h"

MotionHistoryImageFilter::MotionHistoryImageFilter()
{
	if (verbose) printf("GrayscaleFilter::GrayscaleFilter()\n");
}

MotionHistoryImageFilter::~MotionHistoryImageFilter()
{
	if (verbose) printf("GrayscaleFilter::~GrayscaleFilter()\n");
	destroy();
}

void MotionHistoryImageFilter::setup() 
{
	GrayscaleFilter::setup();
	addContent("Output", output);
	addSlider("Duration", settings.duration, 0.0, 20.0);
}

void MotionHistoryImageFilter::update() 
{
	cvUpdateMotionHistory(input.getCvImage(), output.getCvImage(),
						  ofGetElapsedTimeMillis(), settings.duration/1000);
	
	output.flagImageChanged();
}

void MotionHistoryImageFilter::destroy() 
{
	if (verbose) printf("GrayscaleFilter::destroy()\n");
}

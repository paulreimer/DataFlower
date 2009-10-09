#pragma once

#include "CamShiftTrackingFilter.h"

CamShiftTrackingFilter::CamShiftTrackingFilter()
{
	if (verbose) printf("CamShiftTrackingFilter::CamShiftTrackingFilter()\n");
}

CamShiftTrackingFilter::~CamShiftTrackingFilter()
{
	if (verbose) printf("CamShiftTrackingFilter::~CamShiftTrackingFilter()\n");
	destroy();
}

void CamShiftTrackingFilter::setup() 
{
	GrayscaleFilter::setup();

	addContent("Output", output);
	addSlider("Threshold", settings.threshold, 0, 255);
	addToggle("Invert", settings.invert);
}

void CamShiftTrackingFilter::update() 
{
	output.threshold(settings.threshold, settings.invert);
}

void CamShiftTrackingFilter::destroy() 
{
	if (verbose) printf("CamShiftTrackingFilter::destroy()\n");
}

#pragma once

#include "ColorspaceConversionFilter.h"

ColorspaceConversionFilter::ColorspaceConversionFilter()
{
	if (verbose) printf("ColorspaceConversionFilter::ColorspaceConversionFilter()\n");
}

ColorspaceConversionFilter::~ColorspaceConversionFilter()
{
	if (verbose) printf("ColorspaceConversionFilter::~ColorspaceConversionFilter()\n");
	destroy();
}

void ColorspaceConversionFilter::setup() 
{
	ColorspaceConversionFilter::setup();
	addContent("Output", output);
	addSlider("Mode", settings.code, CV_RGB2HSV, CV_RGB2HSV);
}

void ColorspaceConversionFilter::update() 
{
	cvCvtColor(input.getCvImage(), output.getCvImage(), settings.code);

	output.flagImageChanged();
}

void ColorspaceConversionFilter::destroy() 
{
	if (verbose) printf("ColorspaceConversionFilter::destroy()\n");
}

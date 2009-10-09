#pragma once

#include "ContourFindingFilter.h"

ContourFindingFilter::ContourFindingFilter()
{
	if (verbose) printf("ContourFindingFilter::ContourFindingFilter()\n");
}

ContourFindingFilter::~ContourFindingFilter()
{
	if (verbose) printf("ContourFindingFilter::~ContourFindingFilter()\n");
	destroy();
}

void ContourFindingFilter::setup() 
{
	GrayscaleFilter::setup();

	addContent("Output", output);
	addSlider("Blobs", settings.num_blobs, 1, 10);
	addToggle("Find Holes", settings.find_holes);
}

void ContourFindingFilter::update() 
{
	contourFinder.findContours(input, 20, (videoSize.x*videoSize.y)/3,
							   settings.num_blobs, settings.find_holes);
}

void ContourFindingFilter::draw() 
{
	VideoFilter::draw();
	contourFinder.draw(x, y, width, width*videoSize.y/videoSize.x);
}

void ContourFindingFilter::destroy() 
{
	if (verbose) printf("ContourFindingFilter::destroy()\n");
}

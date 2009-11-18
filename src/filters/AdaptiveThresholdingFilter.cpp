#include "AdaptiveThresholdingFilter.h"

AdaptiveThresholdingFilter::AdaptiveThresholdingFilter()
{
	if (verbose) printf("AdaptiveThresholdingFilter::AdaptiveThresholdingFilter()\n");
}

AdaptiveThresholdingFilter::~AdaptiveThresholdingFilter()
{
	if (verbose) printf("AdaptiveThresholdingFilter::~AdaptiveThresholdingFilter()\n");
	destroy();
}

void AdaptiveThresholdingFilter::setup() 
{
	GrayscaleFilter::setup();

	addContent("Output", output);
	addSlider("Block Size", settings.blocksize, 1, 49);
	addSlider("Offset", settings.offset, 0, 255);
	addToggle("Invert", settings.invert);
	addToggle("Use Gauss", settings.use_gauss);
}

void AdaptiveThresholdingFilter::update() 
{
	output = input;
	output.adaptiveThreshold(settings.blocksize, settings.offset,
							 settings.invert, settings.use_gauss);
}

void AdaptiveThresholdingFilter::destroy() 
{
	if (verbose) printf("AdaptiveThresholdingFilter::destroy()\n");
	GrayscaleFilter::destroy();
}

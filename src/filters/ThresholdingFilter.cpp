#include "ThresholdingFilter.h"

ThresholdingFilter::ThresholdingFilter()
{
	if (verbose) printf("ThresholdingFilter::ThresholdingFilter()\n");
}

ThresholdingFilter::~ThresholdingFilter()
{
	if (verbose) printf("ThresholdingFilter::~ThresholdingFilter()\n");
	destroy();
}

void ThresholdingFilter::setup() 
{
	GrayscaleFilter::setup();

	addContent("Output", output);
	addSlider("Threshold", settings.threshold, 0, 255);
	addSlider("Max Value", settings.max_value, 0, 255);
	addSlider("Mode", settings.mode, 0, 4);
	addToggle("Use Otsu", settings.use_otsu);
}

void ThresholdingFilter::update() 
{
	int mode = settings.mode;
	if (settings.use_otsu)
		mode |= CV_THRESH_OTSU;
	
	cvThreshold(input.getCvImage(), output.getCvImage(),
				settings.threshold, settings.max_value, mode);

    output.flagImageChanged();
}

void ThresholdingFilter::destroy() 
{
	if (verbose) printf("ThresholdingFilter::destroy()\n");
	GrayscaleFilter::destroy();	
}

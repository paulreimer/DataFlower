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
	ColorFilter::setup();
	fgMask.allocate(videoSize.x, videoSize.y);

	addContent("Output", output);
	addContent("Mask", fgMask);
	addSlider("Duration", settings.duration, 0.0, 20.0, 0.0);
}

void MotionHistoryImageFilter::update() 
{
	cvUpdateMotionHistory(input.getCvImage(), fgMask.getCvImage(),
						  ofGetElapsedTimeMillis(), settings.duration/1000);
	
	fgMask.flagImageChanged();
	
//	cvSetZero(output.getCvImage());
	cvCopy(input.getCvImage(), output.getCvImage(), fgMask.getCvImage());
	output.flagImageChanged();
}

void MotionHistoryImageFilter::destroy() 
{
	if (verbose) printf("GrayscaleFilter::destroy()\n");
	ColorFilter::destroy();	
}

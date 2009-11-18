#include "DifferencingFilter.h"


DifferencingFilter::DifferencingFilter()
{
	if (verbose) printf("DifferencingFilter::DifferencingFilter()\n");
}

DifferencingFilter::~DifferencingFilter()
{
	if (verbose) printf("DifferencingFilter::~DifferencingFilter()\n");
	destroy();
}

void DifferencingFilter::setup() 
{
	GrayscaleFilter::setup();

	capture.allocate(videoSize.x, videoSize.y);

	addContent("Output", output);
	addContent("Background", capture);
	addButton("Capture", settings.capture);
	addToggle("Hold", settings.once);
}

void DifferencingFilter::update() 
{
	output.absDiff(input, capture);

	if (settings.capture == true)
	{
		settings.capture = !settings.once;
		capture = input;
	}
}

void DifferencingFilter::destroy() 
{	
	if (verbose) printf("DifferencingFilter::destroy()\n");
	GrayscaleFilter::destroy();

	capture.clear();
}

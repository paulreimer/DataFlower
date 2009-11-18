#include "DistanceTransformFilter.h"

DistanceTransformFilter::DistanceTransformFilter()
{
	if (verbose) printf("DistanceTransformFilter::DistanceTransformFilter()\n");
}

DistanceTransformFilter::~DistanceTransformFilter()
{
	if (verbose) printf("DistanceTransformFilter::DistanceTransformFilter()\n");
	destroy();
}

void DistanceTransformFilter::setup() 
{
	GrayscaleFilter::setup();
	addContent("Output", output);
	addSlider("Distance Type", settings.distance_type, CV_DIST_L1, CV_DIST_C);
	addSlider("Mask Size", settings.mask_size, 3, 5);
}

void DistanceTransformFilter::update() 
{
	if (settings.mask_size != 3 || settings.mask_size != 5)
		settings.mask_size = 3;
	
	cvDistTransform(input.getCvImage(), output.getCvImage(),
					settings.distance_type=CV_DIST_L2,
					settings.mask_size);

	output.flagImageChanged();
}

void DistanceTransformFilter::destroy() 
{
	if (verbose) printf("DistanceTransformFilter::destroy()\n");
	GrayscaleFilter::destroy();	
}

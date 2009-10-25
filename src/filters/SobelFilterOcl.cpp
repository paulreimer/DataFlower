#pragma once

#include "SobelFilterOcl.h"

#ifdef USE_OPENCL

SobelFilterOcl::SobelFilterOcl(ofxClScheduler& _clScheduler)
{
	if (verbose) printf("SobelFilterOcl::SobelFilterOcl(_clScheduler)\n");
	clScheduler = &_clScheduler;
}

SobelFilterOcl::SobelFilterOcl()
{
	if (verbose) printf("SobelFilterOcl::SobelFilterOcl()\n");
}

SobelFilterOcl::~SobelFilterOcl()
{
	if (verbose) printf("SobelFilterOcl::~SobelFilterOcl()\n");
	destroy();
}

void SobelFilterOcl::setup()
{
	ColorFilter::setup();

	rgbaInput.allocate(videoSize.x, videoSize.y);
	grayOutput.allocate(videoSize.x, videoSize.y);

	oclSobelFilter.input = rgbaInput.getCvImage()->imageData;
	oclSobelFilter.output = grayOutput.getCvImage()->imageData;

	addContent("RGBA", rgbaInput);
	addContent("Output", output);
	addSlider("Threshold", oclSobelFilter.threshold, 0.0, 255.0);

	oclSobelFilter.loadFromFile("sobelFilter.cl");
	clScheduler->initKernel(oclSobelFilter);
}

void SobelFilterOcl::update()
{
	rgbaInput = input;

	clScheduler->enqueueKernel(oclSobelFilter);

	grayOutput.flagImageChanged();
	output = grayOutput;

	rgbaInput.setAlpha(grayOutput);
	rgbaInput.flagImageChanged();

	//rgbaInput.setAlpha(grayOutput);
}

void SobelFilterOcl::destroy()
{
	rgbaInput.clear();
	grayOutput.clear();

	if (verbose) printf("SobelFilterOcl::destroy()\n");
}

#endif

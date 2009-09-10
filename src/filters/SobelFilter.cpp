#pragma once

#include "SobelFilter.h"

SobelFilter::SobelFilter() {
	if (verbose) printf("SobelFilter::SobelFilter()\n");
}

SobelFilter::~SobelFilter() {
	if (verbose) printf("SobelFilter::~SobelFilter()\n");
	destroy();
}

void SobelFilter::setup() {
	ColorFilter::setup();

	grayInput.allocate(videoSize.x, videoSize.y);

	outputX.allocate(videoSize.x, videoSize.y);
	outputY.allocate(videoSize.x, videoSize.y);

	blankImg.allocate(videoSize.x, videoSize.y);

	tmpOutputX = cvCreateImage(cvGetSize(input.getCvImage()),IPL_DEPTH_16S,1);
	tmpOutputY = cvCreateImage(cvGetSize(input.getCvImage()),IPL_DEPTH_16S,1);
	
	addContent("Output (X'->R, Y'->B)", output);
	addSlider("X Deriv. Order", settings.x_order, 0, 2);
	addSlider("Y Deriv. Order", settings.y_order, 0, 2);
	addSlider("Aperture Size", settings.aperture_size, 1, 7);
}

void SobelFilter::update() {
	settings.aperture_size |= 1;
	
	if (settings.x_order) {
		cvSobel(grayInput.getCvImage(), tmpOutputX,
				settings.x_order, 0,
				settings.aperture_size);
		cvConvertScaleAbs(tmpOutputX, outputX.getCvImage(), 1, 0);
		outputX.flagImageChanged();
	}

	if (settings.y_order) {
		cvSobel(grayInput.getCvImage(), tmpOutputY,
				0, settings.y_order,
				settings.aperture_size);
		cvConvertScaleAbs(tmpOutputY, outputY.getCvImage(), 1, 0);
		outputY.flagImageChanged();
	}
	
	if (settings.x_order && settings.y_order)
		output.setFromGrayscalePlanarImages(outputX, blankImg, outputY);
	else if (settings.x_order)
		output = outputX;
	else if (settings.y_order)
		output = outputY;

}

void SobelFilter::destroy() {
	if (verbose) printf("SobelFilter::destroy()\n");
	
	cvReleaseImage(&tmpOutputX);
	cvReleaseImage(&tmpOutputY);
	
	grayInput.clear();
	
	outputX.clear();
	outputY.clear();
	
	blankImg.clear();
}

#pragma once

#include "GrayscaleFilter.h"

GrayscaleFilter::GrayscaleFilter() {
	printf("GrayscaleFilter::GrayscaleFilter()\n");
}

GrayscaleFilter::~GrayscaleFilter() {
	printf("GrayscaleFilter::~GrayscaleFilter()\n");
	destroy();
}

void GrayscaleFilter::setup() {
	VideoFilter::setup();
	addContent("Output", &output);
	grayOutput.allocate(VIDEO_SIZE);

}

void GrayscaleFilter::update() {
	grayOutput = input;
	output = grayOutput;
}

void GrayscaleFilter::destroy() {
	printf("GrayscaleFilter::destroy()\n");
}

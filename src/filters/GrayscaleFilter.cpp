#pragma once

#include "GrayscaleFilter.h"

GrayscaleFilter::GrayscaleFilter(string name) : VideoFilter(name) {
	printf("GrayscaleFilter::GrayscaleFilter()\n");
}

GrayscaleFilter::~GrayscaleFilter() {
	printf("GrayscaleFilter::~GrayscaleFilter()\n");
	destroy();
}

void GrayscaleFilter::setup() {
	VideoFilter::setup();
	grayOutput.allocate(VIDEO_SIZE);
	
}

void GrayscaleFilter::update() {
	grayOutput = input;
	output = grayOutput;
}

void GrayscaleFilter::destroy() {
	printf("GrayscaleFilter::destroy()\n");
}

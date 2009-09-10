#pragma once

#include "GrayscalePassthroughFilter.h"

GrayscalePassthroughFilter::GrayscalePassthroughFilter() {
	if (verbose) printf("GrayscaleFilter::GrayscaleFilter()\n");
}

GrayscalePassthroughFilter::~GrayscalePassthroughFilter() {
	if (verbose) printf("GrayscaleFilter::~GrayscaleFilter()\n");
	destroy();
}

void GrayscalePassthroughFilter::setup() {
	GrayscaleFilter::setup();
	addContent("Output", output);
}

void GrayscalePassthroughFilter::update() {
	output = input;
}

void GrayscalePassthroughFilter::destroy() {
	if (verbose) printf("GrayscaleFilter::destroy()\n");
}

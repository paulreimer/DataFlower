#pragma once

#include "PassthroughFilter.h"

PassthroughFilter::PassthroughFilter() {
	if (verbose) printf("PassthroughFilter::PassthroughFilter()\n");
}

PassthroughFilter::~PassthroughFilter() {
	if (verbose) printf("PassthroughFilter::~PassthroughFilter()\n");
	destroy();
}

void PassthroughFilter::setup() {
	ColorFilter::setup();
	addContent("Output", output);
}

void PassthroughFilter::update() {
	output = input;
}

void PassthroughFilter::destroy() {
	if (verbose) printf("PassthroughFilter::destroy()\n");
}

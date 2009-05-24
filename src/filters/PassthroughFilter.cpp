#pragma once

#include "PassthroughFilter.h"

PassthroughFilter::PassthroughFilter(string name) : VideoFilter(name) {
	printf("PassthroughFilter::PassthroughFilter()\n");
}

PassthroughFilter::~PassthroughFilter() {
	printf("PassthroughFilter::~PassthroughFilter()\n");
	destroy();
}

void PassthroughFilter::setup() {
	VideoFilter::setup();
	addContent("Output", &output);
}

void PassthroughFilter::update() {
	output = input;
}

void PassthroughFilter::destroy() {
	printf("PassthroughFilter::destroy()\n");
}

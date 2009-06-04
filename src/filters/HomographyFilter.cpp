#pragma once

#include "HomographyFilter.h"

HomographyFilter::HomographyFilter() {
	settings.mirror_h = true;
	settings.mirror_v = true;
	printf("HomographyFilter::HomographyFilter()\n");
}

HomographyFilter::~HomographyFilter() {
	printf("HomographyFilter::~HomographyFilter()\n");
	destroy();
}

void HomographyFilter::setup() {
	VideoFilter::setup();
	addContent("Output", &output);
	addToggle("Flip Horizontally", &settings.mirror_h);
	addToggle("Flip Vertically", &settings.mirror_v);	
}

void HomographyFilter::update() {
	output = input;
	output.mirror(settings.mirror_v, settings.mirror_h);
}

void HomographyFilter::destroy() {
	printf("HomographyFilter::destroy()\n");
}

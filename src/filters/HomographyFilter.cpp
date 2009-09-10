#pragma once

#include "HomographyFilter.h"

HomographyFilter::HomographyFilter() {
	if (verbose) printf("HomographyFilter::HomographyFilter()\n");
}

HomographyFilter::~HomographyFilter() {
	if (verbose) printf("HomographyFilter::~HomographyFilter()\n");
	destroy();
}

void HomographyFilter::setup() {
	ColorFilter::setup();
	addContent("Output", output);
	addToggle("Flip Horizontally", settings.mirror_h);
	addToggle("Flip Vertically", settings.mirror_v);	
}

void HomographyFilter::update() {
	int flipMode = 0;
	
	if		( settings.mirror_v && !settings.mirror_h ) flipMode = 0;
	else if	(!settings.mirror_v &&	settings.mirror_h ) flipMode = 1;
	else if	( settings.mirror_v &&	settings.mirror_h ) flipMode = -1;
	else {
		output = input;
		return;
	}
	
	cvFlip(input.getCvImage(), output.getCvImage(), flipMode );
    output.flagImageChanged();
}

void HomographyFilter::destroy() {
	if (verbose) printf("HomographyFilter::destroy()\n");
}

#pragma once

#include "VideoPipeline.h"
#include "testApp.h"

extern testApp* myApp;

VideoPipeline::VideoPipeline() : ofxMSAInteractiveObject() {
//	guiFocus		= 0;
	disableAllEvents();

	verbose			= true;

	printf("VideoPipeline::VideoPipeline()\n");
}

VideoPipeline::~VideoPipeline() {
	printf("VideoPipeline::~VideoPipeline()\n");
	destroy();
}

void VideoPipeline::setConfig(ofxSimpleGuiConfig *config) {
	this->config = config;
}

void VideoPipeline::setup() {
	input.allocate(VIDEO_SIZE);
	output.allocate(VIDEO_SIZE);

	int i = 0;
	for (list<VideoFilter*>::iterator filter = filters.begin(); filter != filters.end(); filter++) {
		(*filter)->setup();
		(*filter)->setPos(this->x + (i++)*280, this->y);
	}

	setDraw(true);
}

void VideoPipeline::update() {
	list<VideoFilter*>::iterator lhs = filters.begin();
	list<VideoFilter*>::iterator rhs = filters.begin();
	(*lhs)->input = input;
	(*lhs)->update();

	rhs++;
	for (; rhs != filters.end(); rhs++) {
		(*rhs)->input = (*lhs)->output;
		(*rhs)->update();
		lhs++;
	}
	
	VideoFilter* debug = *lhs;

	// lhs remains at filters.begin if size() == 1
	output = (*lhs)->output;
}

void VideoPipeline::setDraw(bool b) {
	doDraw = b;
}

void VideoPipeline::toggleDraw() {
	setDraw(!doDraw);
}

VideoFilter *VideoPipeline::addFilter(VideoFilter* filter) {
	filters.push_back(filter);
	return filter;
}

void VideoPipeline::draw() {
	if(!doDraw) return;
	glDisableClientState(GL_COLOR_ARRAY);

	ofLine(256, 210,
		   filters.front()->x, filters.front()->y+filters.front()->height/2);
	list<VideoFilter*>::iterator lhs = filters.begin();
	list<VideoFilter*>::iterator rhs = filters.begin();
	(*lhs)->draw();
	rhs++;
	for (; rhs != filters.end(); rhs++) {
		ofNoFill();
        ofSetColor(0xFFFFFF);
		ofSetLineWidth(5.0);

		ofSetLineWidth(1.0);
		ofLine((*lhs)->x+(*lhs)->width, (*lhs)->y+(*lhs)->height/2,
			   (*rhs)->x, (*rhs)->y+(*rhs)->height/2);
		(*rhs)->draw();
		lhs++;
	}
}

void VideoPipeline::destroy() {
	printf("VideoPipeline::destroy()\n");
}

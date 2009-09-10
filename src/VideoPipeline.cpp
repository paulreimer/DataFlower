#pragma once

#include "VideoPipeline.h"
#include "testApp.h"

extern testApp* myApp;

VideoPipeline::VideoPipeline() {
	ref_type = FIDUCIAL_PIPELINE_TYPE;
	disableAllEvents();

	verbose = SYSTEM_VERBOSE;
	
	filters.clear();
	
	if (verbose) printf("VideoPipeline::VideoPipeline()\n");
}

VideoPipeline::~VideoPipeline() {
	if (verbose) printf("VideoPipeline::~VideoPipeline()\n");
	destroy();
}

void VideoPipeline::setConfig(ofxSimpleGuiConfig *config) {
	this->config = config;
}

void VideoPipeline::setHitPoint(ofxPoint2f hitPoint) {
	this->hitPoint = hitPoint;
}

void VideoPipeline::setVideoSize(ofPoint videoSize) {
	if (this->videoSize != videoSize) {
		this->videoSize = videoSize;
		setup();
	}
}

void VideoPipeline::setup() {
	if (input.width != videoSize.x || input.height != videoSize.y) {
		input.clear();
		input.allocate(videoSize.x, videoSize.y);
	}
	if (output.width != videoSize.x || output.height != videoSize.y) {
		output.clear();
		output.allocate(videoSize.x, videoSize.y);
	}
	setDraw(true);
	enableAppEvents();
}

void VideoPipeline::update() {
	if (filters.empty()) return;
	if (!filters.front()->isAllocated()) return;
	
//	printf("update %d filters\n", filters.size());
	list<VideoFilter*>::iterator lhs = filters.begin();
	list<VideoFilter*>::iterator rhs = filters.begin();

	//	(*lhs)->input = input;
	(*lhs)->input_ref() = input;
	(*lhs)->update();

	for (++rhs; rhs != filters.end() && (*rhs)->enabled; lhs++, rhs++) {
		//		(*rhs)->input = (*lhs)->output;
		if ((*lhs)->output_ref().getCvImage()->nChannels == 3)
			(*rhs)->input_ref() = dynamic_cast<ColorFilter*>((*lhs))->output_ref();
		else
			(*rhs)->input_ref() = dynamic_cast<GrayscaleFilter*>((*lhs))->output_ref();

		(*rhs)->update();
	}

	// lhs remains at filters.begin if size() == 1
//	output = (*lhs)->output;
}

void VideoPipeline::setDraw(bool b) {
	doDraw = b;
}

void VideoPipeline::toggleDraw() {
	setDraw(!doDraw);
}

VideoFilter *VideoPipeline::addFilter(VideoFilter* filter) {
	ofPoint sze = videoSize;
	if (!filters.empty())
		sze.set(filters.back()->output_ref().width,
				filters.back()->output_ref().height);
	
	if (sze.x != 0 && sze.y != 0) {
		if (filter->videoSize.x != sze.x || filter->videoSize.y != sze.y) {
			if (filter->isAllocated())
				filter->destroy();
			
			filter->videoSize = sze;
		}

		if (!filter->isAllocated())
			filter->setup();
	}
	
	if (filter->fiducial == NULL) {
		filter->setPos(this->x + this->width + config->padding.x, this->y);
		this->width += filter->width + config->padding.x;		
	}

	filters.push_back(filter);
	return filter;
}

void VideoPipeline::draw() {
	if(!doDraw || filters.empty()) return;
	glDisableClientState(GL_COLOR_ARRAY);

	ofxPoint2f startpoint, endpoint, ref;
	
	startpoint.set(254, 215);
/*
	endpoint.set(filters.front()->x,
				 filters.front()->y + filters.front()->height/2);

	if (filters.front()->fiducial != NULL) {
		ref.set(filters.front()->x + filters.front()->width/2,
				filters.front()->y + filters.front()->height/2);
		endpoint.rotateRad(-filters.front()->fiducial->current.angle, ref);
	}
*/
	endpoint = filters.front()->input_point();
	
	ofSetColor(0xFFFFFF);
	ofLine(startpoint.x,startpoint.y, endpoint.x,endpoint.y);
	
	map<VideoFilter*, ofxPoint2f>::iterator hit_p;
	list<VideoFilter*>::iterator lhs = filters.begin();
	list<VideoFilter*>::iterator rhs = filters.begin();

	ofNoFill();
	ofSetColor(0xFFFFFF);
	ofSetLineWidth(1.0);

	if (!(*lhs)->isAllocated()) return;

	for (++rhs; rhs != filters.end(); lhs++, rhs++) {
		startpoint = (*lhs)->output_point();
		endpoint = (*rhs)->input_point();

		ofLine(startpoint.x,startpoint.y, endpoint.x,endpoint.y);
		
		if ((*rhs)->wasHit())
			ofCircle(endpoint.x, endpoint.y, 5);
	}

	if ((*lhs)->fiducial != NULL) {
		startpoint = (*lhs)->output_point();
		ofLine(startpoint.x,startpoint.y, hitPoint.x,hitPoint.y);
		ofCircle(hitPoint.x, hitPoint.y, 5);
	}
}

void VideoPipeline::truncate(int newlen) {
	if (filters.empty() || filters.size() <= newlen) return;

	if (newlen == 0)
		filters.clear();
	else
		filters.resize(newlen);
}

void VideoPipeline::destroy() {
	if (verbose) printf("VideoPipeline::destroy()\n");
}

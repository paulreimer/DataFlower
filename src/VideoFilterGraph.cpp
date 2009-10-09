#pragma once

#include "VideoFilterGraph.h"
#include "testApp.h"

using namespace GuiElements::renderer;

extern testApp* myApp;

VideoFilterGraph::VideoFilterGraph()
{
//	ref_type = FIDUCIAL_FILTER_GRAPH_TYPE;
	disableAllEvents();
	
	verbose = SYSTEM_VERBOSE;
	
	filters.clear();
	flows.clear();
	
	if (verbose) printf("VideoFilterGraph::VideoFilterGraph()\n");
}

VideoFilterGraph::~VideoFilterGraph()
{
	if (verbose) printf("VideoFilterGraph::~VideoFilterGraph()\n");
	destroy();
}

void VideoFilterGraph::setConfig(ofxSimpleGuiConfig *config)
{
	this->config = config;
}

void VideoFilterGraph::setVideoSize(ofPoint videoSize) 
{
	if (this->videoSize != videoSize)
	{
		this->videoSize = videoSize;
		setup();
	}
}

void VideoFilterGraph::setup() 
{
	if (input.width != videoSize.x || input.height != videoSize.y)
	{
		input.clear();
		input.allocate(videoSize.x, videoSize.y);
	}
	if (output.width != videoSize.x || output.height != videoSize.y)
	{
		output.clear();
		output.allocate(videoSize.x, videoSize.y);
	}
	setDraw(true);
	//	enableAppEvents();
}

void VideoFilterGraph::update() 
{
}

void VideoFilterGraph::setDraw(bool b) 
{
	doDraw = b;
}

void VideoFilterGraph::toggleDraw() 
{
	setDraw(!doDraw);
}
/*
VideoFilter *VideoFilterGraph::addFilter(VideoFilter* filter) 
{
	ofPoint sze = videoSize;
	if (!filters.empty())
		sze.set(filters.back()->output_ref().width,
				filters.back()->output_ref().height);
	
	if (sze.x != 0 && sze.y != 0)
	{
		if (filter->videoSize.x != sze.x || filter->videoSize.y != sze.y)
	{
			if (filter->isAllocated())
				filter->destroy();
			
			filter->videoSize = sze;
		}
		
		if (!filter->isAllocated())
			filter->setup();
	}
	
	if (filter->fiducial == NULL)
	{
		filter->setPos(this->x + this->width + config->padding.x, this->y);
		this->width += filter->width + config->padding.x;		
	}
	
	filters.push_back(filter);
	return filter;
}
*/

void VideoFilterGraph::draw() 
{
	if(!doDraw || filters.empty()) return;
	glDisableClientState(GL_COLOR_ARRAY);

	ofxPoint2f startpoint, endpoint, ref;

	map<VideoFilterPtr, ofxPoint2f>::iterator hit_p;
	map<fiducialIndex,VideoFilter*>::iterator lhs = filters.begin();
	map<fiducialIndex,VideoFilter*>::iterator rhs = filters.begin();

	VideoFilter *from=lhs->second, *to;

	startpoint.set	(254, 215);
	
	endpoint.set	(from->x,
					 from->y + (from->height/2));

	endpoint.rotateRad(angle, from->angle);
	
	if (!from->isAllocated()) return;
	
#ifndef TARGET_OPENGLES
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
#endif
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	
	drawArrow(startpoint, endpoint, GuiElements::types::ACTOR_TYPE_RGB);

#ifndef TARGET_OPENGLES
	glPopAttrib();
#endif
}

void VideoFilterGraph::destroy() 
{
	if (verbose) printf("VideoFilterGraph::destroy()\n");
}

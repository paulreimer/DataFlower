#pragma once

#include "VideoPipeline.h"
#include "testApp.h"

#include "GuiElements.h"

using namespace GuiElements::renderer;

extern testApp* myApp;

VideoPipeline::VideoPipeline()
{
//	ref_type = FIDUCIAL_PIPELINE_TYPE;
	disableAllEvents();

	verbose = SYSTEM_VERBOSE;
	
	filters.clear();
	
	if (verbose) printf("VideoPipeline::VideoPipeline()\n");
}

VideoPipeline::~VideoPipeline()
{
	if (verbose) printf("VideoPipeline::~VideoPipeline()\n");
	destroy();
}

void VideoPipeline::setConfig(ofxSimpleGuiConfig *config) 
{
	this->config = config;
}

void VideoPipeline::setEdgeHitPoint(ofxPoint2f edgeHitPoint) 
{
	this->edgeHitPoint = edgeHitPoint;
}

void VideoPipeline::setVideoSize(ofPoint videoSize) 
{
	if (this->videoSize != videoSize)
	{
		this->videoSize = videoSize;
		setup();
	}
}

void VideoPipeline::setup() 
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

void VideoPipeline::update() 
{
	if (filters.empty()) return;
	if (!filters.front()->isAllocated()) return;
	
//	printf("update %d filters\n", filters.size());
	list<VideoFilterPtr>::iterator lhs = filters.begin();
	list<VideoFilterPtr>::iterator rhs = filters.begin();

	VideoFilter *from=*lhs, *to;

	if (!from->enabled) return;
	//	(*lhs)->input = input;
	from->input_ref() = input;
	from->update();

	for (++rhs; rhs != filters.end() && (*rhs)->enabled; lhs++, rhs++) {
		from = *lhs;
		to = *rhs;
		//		(*rhs)->input = (*lhs)->output;
		if (from->output_ref().getCvImage()->nChannels == 3)
#ifdef USE_SMART_POINTERS
			to->input_ref() = from.cast<ColorFilter>()->output_ref();
#else
			to->input_ref() = dynamic_cast<ColorFilter*>(from)->output_ref();
#endif
		else
#ifdef USE_SMART_POINTERS
			to->input_ref() = from.cast<GrayscaleFilter>()->output_ref();
#else
			to->input_ref() = dynamic_cast<GrayscaleFilter*>(from)->output_ref();
#endif

		to->update();
	}

	// lhs remains at filters.begin if size() == 1
//	output = (*lhs)->output;
}

void VideoPipeline::setDraw(bool b) 
{
	doDraw = b;
}

void VideoPipeline::toggleDraw() 
{
	setDraw(!doDraw);
}

VideoFilter *VideoPipeline::addFilter(VideoFilter* filter) 
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

bool VideoPipeline::dropFilter(VideoFilter* filter) 
{
	list<VideoFilterPtr>::iterator chk = find(filters.begin(), filters.end(), filter);

	if (chk != filters.end()) {
		filters.erase(chk);
		return true;
	}
	
	return false;
}

void VideoPipeline::draw() 
{
	if(!doDraw || filters.empty()) return;
	glDisableClientState(GL_COLOR_ARRAY);

/*
	endpoint.set(filters.front()->x,
				 filters.front()->y + filters.front()->height/2);

	if (filters.front()->fiducial != NULL)
	{
		ref.set(filters.front()->x + filters.front()->width/2,
				filters.front()->y + filters.front()->height/2);
		endpoint.rotateRad(-filters.front()->fiducial->current.angle, ref);
	}
*/
	
	map<VideoFilterPtr, ofxPoint2f>::iterator hit_p;
	list<VideoFilterPtr>::iterator lhs = filters.begin();
	list<VideoFilterPtr>::iterator rhs = filters.begin();

	VideoFilter *from=*lhs, *to;

	ofxPoint2f startpoint, endpoint, ref;
	
	startpoint.set	(254, 215);
	endpoint.set	(from->x,
					 from->y + (from->height/2));
	
	ofNoFill();
	ofSetColor(0xFFFFFF);
	ofSetLineWidth(1.0);

	if (!from->isAllocated()) return;

#ifndef TARGET_OPENGLES
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
#endif
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
		
	GuiElements::types::actor_type_t actor_type = GuiElements::types::ACTOR_TYPE_RGB;
	drawArrow(startpoint, endpoint, actor_type);
//	ofLine(startpoint.x,startpoint.y, endpoint.x,endpoint.y);
	
	for (++rhs; rhs != filters.end(); lhs++, rhs++) {
		from = *lhs;
		to = *rhs;

		startpoint.set	(from->x + from->width,
						 from->y + (from->height/2));
		
		endpoint.set	(to->x,
						 to->y + (to->height/2));
		
		if (from->isAllocated() &&
			from->output_ref().getCvImage()->nChannels == 3)
			actor_type = GuiElements::types::ACTOR_TYPE_RGB;
		else
			actor_type = GuiElements::types::ACTOR_TYPE_GRAY;
		
		drawArrow(startpoint, endpoint, actor_type);
	}
	
	if (lhs != filters.end() && from->fiducial != NULL)
	{
		startpoint.set(from->x + from->width, from->y + (from->height/2));

		if (from->isAllocated() &&
			from->output_ref().getCvImage()->nChannels == 3)
			actor_type = GuiElements::types::ACTOR_TYPE_RGB;
		else
			actor_type = GuiElements::types::ACTOR_TYPE_GRAY;

		drawArrow(startpoint, edgeHitPoint, actor_type);
	}
	
#ifndef TARGET_OPENGLES
	glPopAttrib();
#endif
}

void VideoPipeline::truncate(int newlen) 
{
	if (filters.empty() || filters.size() <= newlen) return;

	if (newlen == 0)
		filters.clear();
	else
		filters.resize(newlen);
}

void VideoPipeline::destroy() 
{
	if (verbose) printf("VideoPipeline::destroy()\n");
}

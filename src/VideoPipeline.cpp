#include "VideoPipeline.h"
#include "testApp.h"
#include "gui_types.h"

using namespace ofxFiducialBacked::renderer::primitives;

VideoPipeline::VideoPipeline()
{
	disableAllEvents();

	verbose = SYSTEM_VERBOSE;
	config = &defaultSimpleGuiConfig;

	if (verbose) printf("VideoPipeline::VideoPipeline()\n");
}

VideoPipeline::~VideoPipeline()
{
	if (verbose) printf("VideoPipeline::~VideoPipeline()\n");
}

void VideoPipeline::setConfig(ofxSimpleGuiConfig* const config) 
{
	this->config = config;
}

void VideoPipeline::setVideoSize(const ofPoint& videoSize) 
{
	if (this->videoSize != videoSize)
	{
		this->videoSize = videoSize;
		setup();
	}
}

void VideoPipeline::setup() 
{
	ofxFiducialBacked::gui::GuiSet::setup();
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
}

void VideoPipeline::update() 
{
	if (empty() || !input.bAllocated) return;
	
//	printf("update %d filters\n", size());
	iterator lhs = begin();
	iterator rhs = begin();

	VideoFilter *from, *to;
#ifdef USE_SMART_POINTERS
	from = lhs->second.cast<VideoFilterPtr>();
#else
	from = dynamic_cast<VideoFilterPtr>(lhs->second);
#endif

	if (!from->enabled) return;
	//	(*lhs)->input = input;
	from->input_ref() = input;
	from->update();

	for (++rhs; rhs != end() && rhs->second->enabled; lhs++, rhs++) {
#ifdef USE_SMART_POINTERS
		from = lhs->second.cast<VideoFilterPtr>();
		to = lhs->second.cast<VideoFilterPtr>();
#else
		from = dynamic_cast<VideoFilterPtr>(lhs->second);
		to = dynamic_cast<VideoFilterPtr>(rhs->second);
#endif

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

	// lhs remains at begin if size() == 1
//	output = (*lhs)->output;
}

VideoFilter *VideoPipeline::addFilter(VideoFilter* /*const*/ filter) 
{
	if (filter==NULL)
		return NULL;

	VideoFilterPtr prev;
#ifdef USE_SMART_POINTERS
	prev = back()->second.cast<VideoFilterPtr>();
#else
	prev = dynamic_cast<VideoFilterPtr>(back().second);
#endif
	
	ofPoint sze;
	if (empty())
		sze = videoSize;
	else
		sze.set(prev->output_ref().width,
				prev->output_ref().height);
		
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

	if (empty())
		filter->setPos(x,y);
	else
		filter->setPos(prev->x + prev->width + config->padding.x, prev->y);

	push_back(make_pair(INVALID_FIDUCIAL_ID,filter));
	return filter;
}

void VideoPipeline::dropFilter(VideoFilter* filter) 
{
	using ofxFiducialBacked::types::gui::fiducial_pair_by_value;

	remove_if(fiducial_pair_by_value(filter));
}

void VideoPipeline::draw() /* const */
{
	using ofxFiducialBacked::types::actors::ACTOR_TYPE_RGB;
	using ofxFiducialBacked::types::actors::ACTOR_TYPE_GRAY;

	if(empty()) return;
	glDisableClientState(GL_COLOR_ARRAY);

	map<VideoFilterPtr, ofxPoint2f>::iterator hit_p;
	const_iterator lhs = begin();
	const_iterator rhs = begin();

	VideoFilter *from, *to;
#ifdef USE_SMART_POINTERS
	from = lhs->second.cast<VideoFilterPtr>();
#else
	from = dynamic_cast<VideoFilterPtr>(lhs->second);
#endif

	ofxPoint2f startpoint, endpoint, ref;
	
	startpoint.set	(254, 215);
	endpoint.set	(from->x,
					 from->y + (from->height/2));
	
	ofNoFill();
	ofSetColor(0xFFFFFF);
	ofSetLineWidth(1.0);

#ifndef TARGET_OPENGLES
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
#endif
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);

	render::arrow(startpoint, endpoint, ACTOR_TYPE_RGB);

	for (++rhs; rhs != end(); lhs++) {
		if (!rhs->second->enabled)
			continue;
#ifdef USE_SMART_POINTERS
		from = lhs->second.cast<VideoFilterPtr>();
		to = lhs->second.cast<VideoFilterPtr>();
#else
		from = dynamic_cast<VideoFilterPtr>(lhs->second);
		to = dynamic_cast<VideoFilterPtr>(rhs->second);
#endif
		
		startpoint.set	(from->x + from->width, from->y + (from->height/2));
		endpoint.set	(to->x,					to->y + (to->height/2));

		if (from->output_ref().getCvImage()->nChannels == 3)
			render::arrow(startpoint, endpoint, ACTOR_TYPE_RGB);
		else
			render::arrow(startpoint, endpoint, ACTOR_TYPE_GRAY);

		rhs++;
	}
	


#ifndef TARGET_OPENGLES
	glPopAttrib();
#endif
}

void VideoPipeline::truncate(int newlen) 
{
	if (empty() || size() <= newlen) return;

	if (newlen == 0)
		clear();
	else
		resize(newlen);
}

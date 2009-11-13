#pragma once

#include "VideoSystem.h"
#include "testApp.h"

#include "GuiElements.h"

extern testApp* myApp;

VideoSystem::VideoSystem()
{
	verbose = SYSTEM_VERBOSE;
	if (verbose) printf("VideoSystem::VideoSystem()\n");

#ifdef USE_GUI
	gui_config.sliderHeight		= 10;
	gui_config.sliderTextHeight	= 12;
	
	gui_config.fontSize			= 8;
	gui_config.fontOffset.set	(0, gui_config.fontSize-1);
	
	gui_config.setup();

	// Static function, gotta call it somewhere.
	// (it won't be used 
	GuiElements::style::setup();
#endif
}
VideoSystem::~VideoSystem()
{
	if (verbose) printf("VideoSystem::~VideoSystem()\n");
	destroy();
}

void VideoSystem::setup() 
{
	grabImgs.resize(grabSizes.size();
	vidImgs.resize(vidSizes.size());
	grabImgsNew.resize(grabImgs.size());

	vidGrabbers.resize(grabSizes.size());
	vidPlayers.resize(vidSizes.size());
	vidImgsNew.resize(vidImgs.size());

	for (int i=0; i < grabSizes.size(); i++)
	{
		vidGrabbers[i].setVerbose(true);
		vidGrabbers[i].initGrabber(grabSizes[i].x, grabSizes[i].y);
		
		grabImgs[i].allocate(grabSizes[i].x, grabSizes[i].y);
#ifdef USE_GUI
		gui->addContent("Capture "+ofToString(i), grabImgs[i]);
#endif
	}

	for (int i=0; i < vidSizes.size(); i++)
	{
		vidPlayers[i].loadMovie("fingers.mov");
		vidPlayers[i].play();

		vidSizes[i].set(vidPlayers[i].width, vidPlayers[i].height);

		vidImgs[i].allocate(grabSizes[i].x, grabSizes[i].y);
#ifdef USE_GUI
		gui->addContent("Video "+ofToString(i), (vidImgs[]));
#endif
	}
/*	
	// TODO:this is an awful hack, to retry
	//		after a failed race condition with setup event
	//		for other Systems that add a pipeline
	pipeline_iter it;
	VideoPipelinePtr pipeline;
	ofxCvColorImage* src;
	for(it = pipelines.begin(); it != pipelines.end(); it++) {
		pipeline = it->first;
		src = it->second;
		
		// Try to find a suitable input, *any* input for this pipeline
		if (src == NULL) addPipeline(pipeline);
	}
*/
}

VideoPipeline *VideoSystem::addPipeline(VideoPipeline* const pipeline, ofxCvColorImage* src) 
{
	pipeline->setConfig(&gui_config);
	
	if (src == NULL)
		if (!grabImgs.empty())
			src = &grabImgs[0];//.back());
		else if (!vidImgs.empty())
			src = &vidImgs[0];//.back());

	if (src != NULL && src->bAllocated)
	{
		pipeline->videoSize.set(src->width, src->height);
		pipeline->setup();
	}

	pipelines.push_back(make_pair(pipeline, src));
	return pipeline;
}

void VideoSystem::dropPipeline(VideoPipeline* const pipeline) 
{
	pipeline_iter pipeline_it;

	for (pipeline_it = pipelines.begin(); pipeline_it != pipelines.end(); pipeline_it++)
		if (pipeline_it->first == pipeline)
			pipelines.erase(pipeline_it);
}

VideoFilterGraph *VideoSystem::addGraph(VideoFilterGraph* const graph) 
{
	graph->enableAppEvents();
	
	graphs.push_back(graph);
	return graph;
}

void VideoSystem::dropGraph(VideoFilterGraph* const graph) 
{
//	graphs.remove(graph);
	filter_graph_iter graph_it;
	
	for (graph_it = graphs.begin(); graph_it != graphs.end(); graph_it++)
		if (graph_it->first == graph)
			graphs.erase(graph_it);
}

void VideoSystem::update() 
{
	// Reset new frame flags
	for (int i=0; i < imgs_newFrame.size(); i++)
		vidImgsNew[i] = false;

	for (int i=0; i < vidGrabbers.size(); i++)
	{
		ofVideoGrabber& vidGrabber = vidGrabbers[i];
		ofxCvColorImage& grabImg = grabImgs[i];
		
		vidGrabber.grabFrame();

		if (vidGrabber.isFrameNew()) {
			vidImgsNew[i] = true;
			grabImg.setFromPixels(vidGrabber.getPixels(), grabSizes[i].x, grabSizes[i].y);
		}
	}

	for (int i=0; i < vidPlayers.size(); i++)
	{
		ofVideoPlayer& vidPlayer = vidPlayers[i];
		ofxCvColorImage& vidImg = vidImgs[];

		vidPlayer.idleMovie();

		if (vidPlayer.isFrameNew()) {
			grabImgsNew[i] = true;
			vidImg.setFromPixels(vidPlayer.getPixels(), vidSizes[i].x, vidSizes[i].y);
		}
	}

	int src_idx;

	pipeline_iter pipeline_it;
	VideoPipelinePtr pipeline;
	for(pipeline_it = pipelines.begin(); pipeline_it != pipelines.end(); pipeline_it++)
	{
		pipeline = pipeline_it->first;
		src_idx = pipeline_it->second;
		pipeline->bNewFrame = false;

		if (src_idx < imgs.size()
			&& imgs[src_idx].bAllocated
			&& imgs_newFrame[src_idx])
		{
			pipeline->bNewFrame = true;
			pipeline->input = imgs[src_idx];

			if (pipeline->enabled)
				pipeline->update();
		}
//		pipeline->input = (ofxCvColorImage&)(*src);
	}
}

void VideoSystem::toggleDraw() 
{
	pipeline_iter pipeline_it;
	for(pipeline_it = pipelines.begin(); pipeline_it != pipelines.end(); pipeline_it++)
		pipeline_it->first->toggleDraw();
}

void VideoSystem::draw()/* const */
{
	pipeline_iter_const pipeline_it;
	VideoPipelinePtr pipeline;
	for(pipeline_it = pipelines.begin(); pipeline_it != pipelines.end(); pipeline_it++)
	{
		pipeline = pipeline_it->first;
		
		if (pipeline->enabled && pipeline->doDraw)
			pipeline->draw();
	}
}

void VideoSystem::destroy() 
{
	if (verbose) printf("VideoSystem::destroy()\n");
}


void VideoSystem::fiducialFound(fiducialEvtArgs &args) 
{
	ofxFiducial		&fiducial	= *args.fiducial;
	fiducialIndex	fiducialId	= fiducial.getId();

	cout << "Fiducial " << fiducialId << " found (" << fiducial.life << ")" << endl;

	filter_iter filter_it;
	filter_it = filters.find(fiducialId);
	
	VideoFilterPtr filter;
	
	if (filter_it == filters.end())
	{
		filter = myApp->tuiSystem.createFiducialFilter(&fiducial);
		myApp->tuiSystem.fiducialsCornersMap[fiducial.getId()] = fiducial.cornerPoints;
		
		filter->setConfig(&gui_config);
		
		// TODO: make this user-adjustable, or set automatically
		filter->videoSize.set(VIDEO_SIZE);
		filter->setup();
		
		filters.insert(make_pair(fiducialId, filter));
	}
	else {
		filter = filter_it->second;
	}
	
	if (fiducialId == NULL_FIDUCIAL_ID)
	{
		graphs.push_back(new VideoFilterGraph());
	}
}

void VideoSystem::fiducialLost(fiducialEvtArgs &args) 
{
	ofxFiducial&	fiducial	= *args.fiducial;
	fiducialIndex	fiducialId	= fiducial.getId();

	cout << "Fiducial " << fiducialId << " lost (" << fiducial.life << ")" << endl;

	filter_iter filter_it = filters.find(fiducialId);
	
	VideoFilterPtr filter;
	
	if (filter_it != filters.end())
	{
		filter = filter_it->second;
		filter->fiducial = NULL;
		filter->enabled = false;
		filter->disableAllEvents();
		
		filters.erase(filter_it);
	}
	
	if (fiducialId == NULL_FIDUCIAL_ID)
	{
//		graphs.erase(&fiducial);
	}
}

void VideoSystem::fiducialUpdated(fiducialEvtArgs &args)
{
	ofxFiducial&	fiducial	= *args.fiducial;
	fiducialIndex	fiducialId	= fiducial.getId();

	cout << "Fiducial " << fiducialId << " updated (" << fiducial.life << ")" << endl;
	
	filter_iter filter_it = filters.find(fiducialId);
	
	VideoFilterPtr filter;
	
	if (filter_it != filters.end())
	{
		filter = filter_it->second;
		
		ofxPoint2f fidPos(fiducial.current.xpos*myApp->tuiSystem.x_scale,
						  fiducial.current.ypos*myApp->tuiSystem.y_scale);
		
		float angle = fiducial.getAngle();
		float aspect_ratio = filter->videoSize.y/filter->videoSize.x;
		
		ofxPoint2f filterPos(fidPos.x - (filter->width/2),
							 fidPos.y + (filter->width*aspect_ratio) - 20);
		
		filterPos.rotateRad(angle, fidPos);
		
		filter->setPos(filterPos.x, filterPos.y, angle);
	}
}



void VideoSystem::fiducialRayIntersectionFound(fiducialRayIntersectionEvtArgs &args)
{	
	VideoFilterGraph& graph = *graphs.front();

	cout << "Fiducial " << args.from->getId() << " hits " << args.to->getId() << endl;

	VideoFilterPtr filter;
	edge_t edge;
	
	GuiElements::types::lineSegment ray;
	ray.to.set(args.pt);	

	filter_iter filter_it;
	fiducialIndex	from_fiducialId	= args.from->getId(),
					to_fiducialId	= args.to->getId();

	filter_it = filters.find(args.from->getId());
	if (filter_it != filters.end())
	{
		filter = filter_it->second;
		ray.from.set(filter->x+filter->width,
					 filter->y+(filter->height/2));

		switch (filter->type)
		{
			case COLOR_FILTER:
				ray.from_type = GuiElements::types::ACTOR_TYPE_RGB;
				break;
			case GRAYSCALE_FILTER:
				ray.from_type = GuiElements::types::ACTOR_TYPE_GRAY;
				break;
		}
		
		edge.from = &filter;
	}

	filter_it = filters.find(args.to->getId());
	if (filter_it != filters.end())
	{
		filter = filter_it->second;

		switch (filter->type)
		{
			case COLOR_FILTER:
				ray.to_type = GuiElements::types::ACTOR_TYPE_RGB;
				break;
			case GRAYSCALE_FILTER:
				ray.to_type = GuiElements::types::ACTOR_TYPE_GRAY;
				break;
		}

		edge.to = &filter;
	}
	
	edge.ray = ray;

	graph.edges.push_back(edge);
}

class edge_is_from {
public:
	VideoFilterPtr from;
	edge_is_from(VideoFilterPtr _from) : from(_from) {}
	bool operator()(const edge_t& edge) {
		return *(edge.from)==from;
	}
};

class edge_is_to {
public:
	VideoFilterPtr to;
	edge_is_to(VideoFilterPtr _to) : to(_to) {}
	bool operator()(const edge_t& edge) {
		return *(edge.to)==to;
	}
};

void VideoSystem::fiducialRayIntersectionLost(fiducialRayIntersectionEvtArgs &args)
{
	cout << "Fiducial " << args.from->getId() << " misses " << args.to->getId() << endl;	

	ofxFiducial& from = *args.from;
	ofxFiducial& to = *args.to;
	ofxPoint2f pt = args.pt;

	VideoFilterGraph& graph = *graphs.front();
	list<edge_t>::iterator edge_it;
	edge_it = find_if(graph.edges.begin(),
					  graph.edges.end(),
					  edge_is_from(filters[args.from->getId()]));

	if (edge_it != graph.edges.end())
		graph.edges.erase(edge_it);
}

void VideoSystem::fiducialRayIntersectionUpdated(fiducialRayIntersectionEvtArgs &args)
{
/*
	ofxFiducial& from = *args.from;
	ofxFiducial& to = *args.to;
	ofxPoint2f pt = args.pt;

	VideoFilterGraph& graph = graphs[x];
	graph.edges.push_back
	(make_pair
	 (filters[from].output_ref(),
	  filters[to].input_ref()));
*/
}

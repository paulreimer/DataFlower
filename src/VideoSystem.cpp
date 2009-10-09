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
	
	grabImgs.resize(grabSizes.size());
	vidImgs.resize(vidSizes.size());

	vidGrabbers.resize(grabSizes.size());
	vidPlayers.resize(vidSizes.size());
	
	for (int g=0; g < grabSizes.size(); g++) {
		vidGrabbers[g].setVerbose(true);
		vidGrabbers[g].initGrabber(grabSizes[g].x, grabSizes[g].y);
		
		grabImgs[g].allocate(grabSizes[g].x, grabSizes[g].y);
#ifdef USE_GUI
		gui->addContent("Capture "+ofToString(g), grabImgs[g]);
#endif
	}

	vidPlayers.resize(vidSizes.size());
	for (int v=0; v < vidSizes.size(); v++) {
		vidPlayers[v].loadMovie("fingers.mov");
		vidPlayers[v].play();

		vidSizes[v].set(vidPlayers[v].width, vidPlayers[v].height);

		vidImgs[v].allocate(vidSizes[v].x, vidSizes[v].y);
#ifdef USE_GUI
		gui->addContent("Video "+ofToString(v), (vidImgs[v]));
#endif
	}
	
	// TODO:this is an awful hack, to retry
	//		after a failed race condition with setup event
	//		for other Systems that add a pipeline
	map<VideoPipelinePtr, ofxCvColorImage*>::iterator it;
	VideoPipelinePtr pipeline;
	ofxCvColorImage* src;
	for(it = pipelines.begin(); it != pipelines.end(); it++) {
		pipeline = it->first;
		src = it->second;
		
		// Try to find a suitable input, *any* input for this pipeline
		if (src == NULL) addPipeline(pipeline);
	}		
}

VideoPipeline *VideoSystem::addPipeline(VideoPipeline* pipeline, ofxCvColorImage* src) 
{
	pipeline->setConfig(&gui_config);
	
	if (src == NULL)
		if (!grabImgs.empty())
			src = &grabImgs[0];//.back());
		else if (!vidImgs.empty())
			src = &vidImgs[0];//.back());

	pipelines[pipeline] = src;

	if (src != NULL && src->bAllocated)
	{
		pipeline->videoSize.set(src->width, src->height);
		pipeline->setup();
	}

	return pipeline;
}

void VideoSystem::dropPipeline(VideoPipeline* pipeline) 
{
	pipelines.erase(pipeline);
}

VideoFilterGraph *VideoSystem::addGraph(VideoFilterGraph* graph, ofxCvColorImage* src) 
{
	graph->setConfig(&gui_config);
	
	if (src == NULL)
		if (!grabImgs.empty())
			src = &grabImgs[0];//.back());
		else if (!vidImgs.empty())
			src = &vidImgs[0];//.back());
	
	graphs[graph] = src;
	
	if (src != NULL && src->bAllocated)
	{
		graph->videoSize.set(src->width, src->height);
		graph->setup();
	}
	
	return graph;
}

void VideoSystem::dropGraph(VideoFilterGraph* graph) 
{
	graphs.erase(graph);
}

void VideoSystem::update() 
{
	for (int g=0; g < vidGrabbers.size(); g++) {
		ofVideoGrabber& vidGrabber = vidGrabbers[g];
		ofxCvColorImage& grabImg = grabImgs[g];
		
		vidGrabber.grabFrame();

		if (vidGrabber.isFrameNew()) {
			bGotFrame = true;
			grabImg.setFromPixels(vidGrabber.getPixels(), grabSizes[g].x, grabSizes[g].y);
		}
	}

	for (int v=0; v < vidPlayers.size(); v++) {
		ofVideoPlayer& vidPlayer = vidPlayers[v];
		ofxCvColorImage& vidImg = vidImgs[v];

		vidPlayer.idleMovie();

		if (vidPlayer.isFrameNew()) {
			bGotFrame = true;
			vidImg.setFromPixels(vidPlayer.getPixels(), vidSizes[v].x, vidSizes[v].y);
		}
	}

	ofxCvColorImage* src;

	map<VideoPipelinePtr, ofxCvColorImage*>::iterator pipe_it;
	VideoPipelinePtr pipeline;
	for(pipe_it = pipelines.begin(); pipe_it != pipelines.end(); pipe_it++) {
		pipeline = pipe_it->first;
		src = pipe_it->second;

		if (src != NULL && src->bAllocated)
		{
			pipeline->input = (*src);

			if (pipeline->enabled)
				pipeline->update();
		}
			//pipeline->input = (ofxCvColorImage&)(*src);
	}

	map<VideoFilterGraphPtr, ofxCvColorImage*>::iterator graph_it;
	VideoFilterGraphPtr graph;
	for(graph_it = graphs.begin(); graph_it != graphs.end(); graph_it++) {
		graph = graph_it->first;
		src = graph_it->second;
		
		if (src != NULL && src->bAllocated)
		{
			graph->input = (*src);
			
			if (graph->enabled)
				graph->update();
		}
		//graph->input = (ofxCvColorImage&)(*src);
	}
	
}

void VideoSystem::toggleDraw() 
{
	map<VideoPipelinePtr, ofxCvColorImage*>::iterator pipe_it;
	for(pipe_it = pipelines.begin(); pipe_it != pipelines.end(); pipe_it++)
		pipe_it->first->toggleDraw();

	map<VideoFilterGraphPtr, ofxCvColorImage*>::iterator graph_it;
	for(graph_it = graphs.begin(); graph_it != graphs.end(); graph_it++)
		graph_it->first->toggleDraw();
}

void VideoSystem::draw() 
{
	ofxCvColorImage* src;

	map<VideoPipelinePtr, ofxCvColorImage*>::iterator pipe_it;
	VideoPipelinePtr pipeline;
	for(pipe_it = pipelines.begin(); pipe_it != pipelines.end(); pipe_it++)
	{
		pipeline = pipe_it->first;
		
		if (pipeline->enabled && pipeline->doDraw)
			pipeline->draw();
	}

	map<VideoFilterGraphPtr, ofxCvColorImage*>::iterator graph_it;
	VideoFilterGraphPtr graph;
	for(graph_it = graphs.begin(); graph_it != graphs.end(); graph_it++)
	{
		graph = graph_it->first;
		
		if (graph->enabled && graph->doDraw)
			graph->draw();
	}
}

void VideoSystem::destroy() 
{
	if (verbose) printf("VideoSystem::destroy()\n");
}


void VideoSystem::fiducialFound(fiducialEvtArgs &args) 
{
	ofxFiducial& fiducial = *args.fiducial;
	fiducialIndex fiducialId = args.fiducial;
	map<fiducialIndex,VideoFilter*>::iterator filter_it = filters.find(fiducialId);
	
	VideoFilterPtr filter;
	
	if (filter_it == filters.end())
	{
		filter = myApp->tuiSystem.createFiducialFilter(&fiducial);
		myApp->tuiSystem.fiducialCornersMap[&fiducial] = fiducial.cornerPoints;
		
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
		//		flows.insert()
	}
	
	fiducial.life += MAX_FIDUCIAL_LIFE;	
}

void VideoSystem::fiducialLost(fiducialEvtArgs &args) 
{
	ofxFiducial& fiducial = *args.fiducial;
	fiducialIndex fiducialId = &fiducial;
	map<fiducialIndex,VideoFilter*>::iterator filter_it = filters.find(fiducialId);
	
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
		//		flows.erase()
	}
}

void VideoSystem::fiducialUpdated(fiducialEvtArgs &args)
{
	ofxFiducial& fiducial = *args.fiducial;
	fiducialIndex fiducialId = args.fiducial;
	map<fiducialIndex,VideoFilter*>::iterator filter_it = filters.find(fiducialId);
	
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
	ofxFiducial& from = *args.from;
	ofxFiducial& to = *args.to;
	ofxPoint2f pt = args.pt;
}

void VideoSystem::fiducialRayIntersectionLost(fiducialRayIntersectionEvtArgs &args)
{
	ofxFiducial& from = *args.from;
	ofxFiducial& to = *args.to;
	ofxPoint2f pt = args.pt;
	
}

void VideoSystem::fiducialRayIntersectionUpdated(fiducialRayIntersectionEvtArgs &args)
{
	ofxFiducial& from = *args.from;
	ofxFiducial& to = *args.to;
	ofxPoint2f pt = args.pt;
	
}

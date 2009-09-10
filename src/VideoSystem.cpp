#pragma once

#include "VideoSystem.h"
#include "testApp.h"

extern testApp* myApp;

VideoSystem::VideoSystem() {
	verbose = SYSTEM_VERBOSE;
	if (verbose) printf("VideoSystem::VideoSystem()\n");
}
VideoSystem::~VideoSystem() {
	if (verbose) printf("VideoSystem::~VideoSystem()\n");
	destroy();
}

void VideoSystem::setup() {
	
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
	map<VideoPipeline*, ofxCvColorImage*>::iterator it;
	VideoPipeline* pipeline;
	ofxCvColorImage* src;
	for(it = pipelines.begin(); it != pipelines.end(); it++) {
		pipeline = it->first;
		src = it->second;
		
		// Try to find a suitable input, *any* input for this pipeline
		if (src == NULL) addPipeline(pipeline);
	}
		
}

VideoPipeline *VideoSystem::addPipeline(VideoPipeline* pipeline, ofxCvColorImage* src) {
	pipeline->setConfig(&gui_config);
	
	if (src == NULL)
		if (!grabImgs.empty())
			src = &grabImgs[0];//.back());
		else if (!vidImgs.empty())
			src = &vidImgs[0];//.back());

	pipelines[pipeline] = src;

	if (src != NULL && src->bAllocated) {
		pipeline->videoSize.set(src->width, src->height);
		pipeline->setup();
	}

	return pipeline;
}

void *VideoSystem::dropPipeline(VideoPipeline* pipeline) {
	pipelines.erase(pipeline);
}

void VideoSystem::update() {

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

	map<VideoPipeline*, ofxCvColorImage*>::iterator it;
	VideoPipeline* pipeline;
	ofxCvColorImage* src;
	for(it = pipelines.begin(); it != pipelines.end(); it++) {
		pipeline = it->first;
		src = it->second;

		if (src != NULL && src->bAllocated) {
			pipeline->input = (*src);
		}
			//pipeline->input = (ofxCvColorImage&)(*src);
	}
	
}

void VideoSystem::toggleDraw() {
	map<VideoPipeline*, ofxCvColorImage*>::iterator it;
	VideoPipeline* pipeline;
	for(it = pipelines.begin(); it != pipelines.end(); it++) {
		pipeline = it->first;
		pipeline->toggleDraw();
	}
}

void VideoSystem::draw() {
}

void VideoSystem::destroy() {
	if (verbose) printf("VideoSystem::destroy()\n");
}

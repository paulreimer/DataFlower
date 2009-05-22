#pragma once

#include "VideoSystem.h"
#include "testApp.h"

extern testApp* myApp;

VideoSystem::VideoSystem() {
	printf("VideoSystem::VideoSystem()\n");
}
VideoSystem::~VideoSystem() {
	printf("VideoSystem::~VideoSystem()\n");
	destroy();
}

VideoSystem& VideoSystem::setup() {
#ifdef _USE_LIVE_VIDEO
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(VIDEO_SIZE);
#else
	vidPlayer.loadMovie("fingers.mov");
	vidPlayer.play();
#endif
	
    colorImg.allocate(VIDEO_SIZE);

//	myApp->guiSystem.gui.page(0)->setName("Input");
	myApp->guiSystem.gui.addContent("Webcam", &colorImg);
	VideoPipeline *pipe = new VideoPipeline();
	pipe->pages.push_back(new DifferencingFilter("First Differ"));
	pipe->pages.push_back(new ThresholdingFilter("First Thresholder"));
	pipelines.push_back(pipe);
	
	vector<VideoPipeline*>::iterator it;
	VideoPipeline* pipeline;
	for(it = pipelines.begin(); it != pipelines.end(); it++) {
		pipeline = *it;
		pipeline->setup();
	}

}

void VideoSystem::update() {
	bool bNewFrame = false;
	
#ifdef _USE_LIVE_VIDEO
	vidGrabber.grabFrame();
	bNewFrame = vidGrabber.isFrameNew();
#else
	vidPlayer.idleMovie();
	bNewFrame = vidPlayer.isFrameNew();
#endif
	
	if (bNewFrame) {
#ifdef _USE_LIVE_VIDEO
		colorImg.setFromPixels(vidGrabber.getPixels(), VIDEO_SIZE);
#else
		colorImg.setFromPixels(vidPlayer.getPixels(), VIDEO_SIZE);
#endif

		vector<VideoPipeline*>::iterator it;
		VideoPipeline* pipeline;
		for(it = pipelines.begin(); it != pipelines.end(); it++) {
			pipeline = *it;
			pipeline->input = colorImg;
			pipeline->update();
		}
	}
}

void VideoSystem::draw() {
	vector<VideoPipeline*>::iterator it;
	VideoPipeline* pipeline;
	for(it = pipelines.begin(); it != pipelines.end(); it++) {
		pipeline = *it;
		pipeline->draw();
	}
}

void VideoSystem::destroy() {
	printf("VideoSystem::destroy()\n");
}

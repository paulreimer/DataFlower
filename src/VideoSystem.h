#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "VideoPipeline.h"

#include "settings.h"

class VideoSystem : public ofBaseUpdates {
public:	
	VideoSystem();
	virtual ~VideoSystem();
	
	VideoSystem& setup();
	void update();
	void draw();
	
	void destroy();
	
#ifdef _USE_LIVE_VIDEO
	ofVideoGrabber			vidGrabber;
#else
	ofVideoPlayer			vidPlayer;
#endif
	
	ofxCvColorImage			colorImg;
	
	vector <VideoPipeline*>	pipelines;
};

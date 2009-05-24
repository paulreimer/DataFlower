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

	ofxCvColorImage			colorImg;

	VideoPipeline			*pipeline(int i);
	
	VideoPipeline			*addPipeline(VideoPipeline *pipeline);

#ifdef _USE_LIVE_VIDEO
	ofVideoGrabber			vidGrabber;
#else
	ofVideoPlayer			vidPlayer;
#endif
	
protected:
	vector <VideoPipeline*>	pipelines;
};

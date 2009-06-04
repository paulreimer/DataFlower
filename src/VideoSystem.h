#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"

#include "ofxOpenCv.h"
#include "VideoPipeline.h"

#include "settings.h"

class VideoSystem : public ofxMSAInteractiveObject {
public:
	VideoSystem();
	virtual ~VideoSystem();

	void setup();
	void update();
	void draw();
	void toggleDraw();

	void destroy();

	ofxCvColorImage			colorImg;

	VideoPipeline			*pipeline(int i);

	VideoPipeline			*addPipeline(VideoPipeline *pipeline);

#ifdef _USE_LIVE_VIDEO
	ofVideoGrabber			vidGrabber;
#else
	ofVideoPlayer			vidPlayer;
#endif

	bool bGotFrame;
protected:
	ofxSimpleGuiConfig gui_config;
	vector <VideoPipeline*>	pipelines;
};

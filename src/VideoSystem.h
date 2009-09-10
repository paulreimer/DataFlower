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

	std::vector<ofxCvColorImage> grabImgs;
	std::vector<ofxCvColorImage> vidImgs;

//	VideoPipeline			*pipeline(int i);

	VideoPipeline			*addPipeline(VideoPipeline* pipeline, ofxCvColorImage* src = NULL);
	void					*dropPipeline(VideoPipeline* pipeline);

	std::vector<ofVideoGrabber>	vidGrabbers;
	std::vector<ofPoint>		grabSizes;

	std::vector<ofVideoPlayer>	vidPlayers;
	std::vector<ofPoint>		vidSizes;

	bool bGotFrame;

	bool verbose;

	map <VideoPipeline*, ofxCvColorImage*> pipelines;
	
#ifdef USE_GUI
	ofxSimpleGuiPage* gui;
	ofxSimpleGuiConfig gui_config;
#endif
protected:

};

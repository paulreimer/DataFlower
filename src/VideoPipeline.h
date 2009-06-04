#pragma once

#include "ofMain.h"
#include "filters.h"

#include "ofxSimpleGuiConfig.h"
#include "ofxOpenCv.h"

class VideoPipeline : public ofxMSAInteractiveObject {
public:
	VideoPipeline();
	virtual ~VideoPipeline();

	void setup();

	void update();
	void draw();

	void destroy();

	void setDraw(bool b);
	void toggleDraw();
	bool					doDraw;

	VideoFilter				*filter(int i);				// 1 based index of page
	VideoFilter				*filter(string name);

	VideoFilter				*addFilter(VideoFilter *filter);

	ofxCvColorImage			input;

	ofxCvColorImage			output;

	void setConfig(ofxSimpleGuiConfig *config);


protected:
	list <VideoFilter*>	filters;

	ofxSimpleGuiConfig*		config;
};

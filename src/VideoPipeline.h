#pragma once

#include "ofMain.h"
#include "filters.h"

#include "FiducialBackedObject.h"
#include "ofxSimpleGuiConfig.h"
#include "ofxOpenCv.h"
#include "ofxPoint2f.h"

class VideoPipeline : public FiducialBackedObject {
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
	ofPoint					videoSize;

	VideoFilter*			filter(int i);				// 1 based index of page
	VideoFilter*			filter(string name);

	VideoFilter*			addFilter(VideoFilter *filter);

	ofxCvColorImage			input;

	ofxCvColorImage			output;

	void setConfig(ofxSimpleGuiConfig *config);
	void setHitPoint(ofxPoint2f hitPoint);
	void setVideoSize(ofPoint sze);

	int size() { return filters.size(); }
	void truncate(int newlen=0);
	
	ofxPoint2f hitPoint;
	
protected:
	list <VideoFilter*>	filters;

	ofxSimpleGuiConfig*		config;
};

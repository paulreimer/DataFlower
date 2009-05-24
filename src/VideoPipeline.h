#pragma once

#include "ofMain.h"
#include "VideoFilter.h"
#include "filters.h"

#include "ofxFidMain.h"
#include "ofxSimpleGuiToo.h"
#include "ofxOpenCv.h"

class VideoPipeline : public ofxSimpleGuiToo {
public:
	VideoPipeline();
	virtual ~VideoPipeline();

	void setup();

	void updateListeners();

	void update();
	void draw();
	
	void destroy();

	VideoFilter				*filter(int i);				// 1 based index of page
	VideoFilter				*filter(string name);
	
	VideoFilter				*addFilter(VideoFilter *filter);

	ofxCvColorImage			input;

	ofxCvColorImage			output;

protected:
	vector <VideoFilter*>	pages;

	void mouseMoved(ofMouseEventArgs &e);
	void mousePressed(ofMouseEventArgs &e);	
	void mouseDragged(ofMouseEventArgs &e);	
	void mouseReleased(ofMouseEventArgs &e);
	
	void keyPressed(ofKeyEventArgs &e);
	void keyReleased(ofKeyEventArgs &e);
};

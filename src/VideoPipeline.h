#pragma once

#include "ofMain.h"
#include "filters/ThresholdingFilter.h"
#include "filters/DifferencingFilter.h"
#include "filters/GrayscaleFilter.h"

#include "ofxDraggable.h"
#include "ofxSimpleGuiToo.h"
#include "ofxOpenCv.h"

class VideoPipeline : public ofxSimpleGuiToo {
public:
	VideoPipeline();
	virtual ~VideoPipeline();

	void setup();

	void addListeners();
	void removeListeners();

	void update();
	void draw();
	
	void destroy();

	ofxCvColorImage			input;

	ofxCvColorImage			output;
	
	ofxSimpleGuiPage		*inputPage;
	ofxSimpleGuiPage		*outputPage;

	vector <VideoFilter*>	pages;

//	VideoFilter				*page(int i);				// 1 based index of page
//	VideoFilter				*page(string name);
	
//	VideoFilter				*addPage(string name = "");

	void mouseMoved(ofMouseEventArgs &e);
	void mousePressed(ofMouseEventArgs &e);	
	void mouseDragged(ofMouseEventArgs &e);	
	void mouseReleased(ofMouseEventArgs &e);
	
	void keyPressed(ofKeyEventArgs &e);
	void keyReleased(ofKeyEventArgs &e);
	
	void setDraw(bool b);
};

#pragma once

#include "ofMain.h"
#include "ofxSimpleGuiPage.h"
#include "ofxDraggable.h"
#include "ofxOpenCv.h"
#include "settings.h"

class VideoFilter : public ofxSimpleGuiPage {
public:
	VideoFilter(string name);

	virtual ~VideoFilter();

	void setup();
	void update();
	void draw();
	
	void destroy();

	ofxCvColorImage 		input;

	ofxCvColorImage			output;

	ofxSimpleGuiControl		*addControl(ofxSimpleGuiControl* control);
	ofxSimpleGuiButton		*addButton(string name, bool *value);
	ofxSimpleGuiContent		*addContent(string name, ofBaseDraws *content, float fixwidth = -1);
	ofxSimpleGuiFPSCounter	*addFPSCounter();
	ofxSimpleGuiSliderInt	*addSlider(string name, int *value, int min, int max);
	ofxSimpleGuiSliderFloat	*addSlider(string name, float *value, float min, float max, float smoothing = 0);
	ofxSimpleGuiSlider2d	*addSlider2d(string name, ofPoint* value, float xmin, float xmax, float ymin, float ymax);
	ofxSimpleGuiTitle		*addTitle(string name, bool *value = NULL);
	ofxSimpleGuiToggle		*addToggle(string name, bool *value);
	
	void onPress(int mx, int my, int button);
	void onDragOver(int mx, int my, int button);

protected:
	int saveX, saveY;
	
};
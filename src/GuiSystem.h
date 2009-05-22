#pragma once

#include "ofMain.h"
#include "ofxSimpleGuiToo.h"

class GuiSystem : public ofBaseUpdates {
public:	
	GuiSystem();
	virtual ~GuiSystem();
	
	GuiSystem& setup();
	void update();
	void draw();
	
	void destroy();
	ofxSimpleGuiToo gui;
};

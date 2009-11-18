#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"

#include "ofxSimpleGuiToo.h"

class GuiSystem : public ofxMSAInteractiveObject 
{
public:
	GuiSystem();
	virtual ~GuiSystem();

	void setup();
	void draw();
	
	void destroy();
	ofxSimpleGuiToo gui;
	bool verbose;
	int startPage;
};

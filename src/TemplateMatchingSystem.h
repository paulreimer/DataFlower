#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"

#include "CaptureFilter.h"
//#include "ofxCvMain.h"

class TemplateMatchingSystem : public ofxMSAInteractiveObject {
public:
	TemplateMatchingSystem();
	virtual ~TemplateMatchingSystem();

	void setup();
	void update();
	void draw();

	void destroy();

protected:	
	CaptureFilter templCaptureFilter;
};

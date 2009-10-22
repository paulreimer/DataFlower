#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"
#include "VideoPipeline.h"

#include "CaptureFilter.h"
//#include "ofxCvMain.h"

class TemplateMatchingSystem : public ofxMSAInteractiveObject 
{
public:
	TemplateMatchingSystem();
	virtual ~TemplateMatchingSystem();

	void setup();
	void update();
	void draw() const;

	void destroy();

	bool verbose;

protected:	
	CaptureFilter templCaptureFilter;
	
	VideoPipeline pipe;
};

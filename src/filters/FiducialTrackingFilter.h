#pragma once

#include "VideoFilter.h"
#include "ofxFidMain.h"

class FiducialTrackingFilter : public GrayscaleFilter  
{
public:
	FiducialTrackingFilter();
	virtual ~FiducialTrackingFilter();

	void setup();
	void update();
	void draw();

	void destroy();

	ofxFiducialTracker		fidFinder;
};

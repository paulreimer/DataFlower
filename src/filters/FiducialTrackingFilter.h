#pragma once

#include "VideoFilter.h"
#include "ofxFidMain.h"

class FiducialTrackingFilter : public VideoFilter  {
public:
	FiducialTrackingFilter(string name = "Fiducial Tracker");
	virtual ~FiducialTrackingFilter();

	void setup();
	void update();
	void draw();

	void destroy();

	ofxFiducialTracker		fidFinder;

protected:
	ofxCvGrayscaleImage		grayInput;
	ofxCvGrayscaleImage 	grayOutput;
	
};

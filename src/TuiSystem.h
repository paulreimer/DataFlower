#pragma once

#include "ofMain.h"
#include "ofxCvMain.h"
#include "ofxFidMain.h"

class TuiSystem : public ofBaseUpdates {
public:	
	TuiSystem();
	virtual ~TuiSystem();

	void setup();
	void update();
	void draw();
	
	void destroy();

	ofxCvGrayscaleImage grayImage;
	ofxCvGrayscaleImage grayBg;
	ofxCvGrayscaleImage	grayDiff;
	
	ofxFiducialTracker	fidfinder;
	
	int 				threshold;
	bool				bLearnBakground;
	bool				backgroundSubOn;
	
};

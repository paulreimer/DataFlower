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

	void updateFromFiducial(ofxFiducial* &fiducial );
	
	
	//list to store fiducials
	std::list <ofxFiducial> *fiducialsList;
	
	//list to store fingers
	std::list <ofxFinger> *fingersList;

	ofEvent<ofxFiducial*> fiducialDetected;

};

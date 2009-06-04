#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"

#include "ofxCvMain.h"
#include "ofxFidMain.h"
#include "FiducialTrackingFilter.h"

class TuiSystem : public ofxMSAInteractiveObject {
public:
	TuiSystem();
	virtual ~TuiSystem();

	void setup();
	void update();
	void draw();
	void toggleDraw();

	void destroy();

	void updateFromFiducial(ofxFiducial* &fiducial);

//	bool leftToRightSort(ofxFiducial lhs, ofxFiducial rhs);

	//list to store fiducials
//	std::list <ofxFiducial> *fiducialsList;

	//list to store fingers
//	std::list <ofxFinger> *fingersList;

protected:
	bool doDraw;
	
	FiducialTrackingFilter fidtracker;

};

#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"
#include "VideoPipeline.h"

#include "ofxCvMain.h"
#include "ofxFidMain.h"
#include "FiducialTrackingFilter.h"
#include "MarkerFindingFilter.h"

#define FIDUCIAL_PIPELINE_TYPE 1
#define FIDUCIAL_FILTER_TYPE 2

typedef std::map<byte, std::map<ofxFiducial*, FiducialBackedObject*> > fid_objs_table;
typedef std::map<byte, std::map<ofxFiducial*, FiducialBackedObject*> >::iterator fid_objs_lookup;

typedef std::map<ofxFiducial*, FiducialBackedObject*>fid_obj_table;
typedef std::map<ofxFiducial*, FiducialBackedObject*>::iterator fid_obj_lookup;

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
	fid_objs_table fid_objs;
	
	std::list <ofxFiducial>* fiducialsList() {
		return &fidtracker.fidFinder.fiducialsList;
	};

	//list to store fingers
	std::list <ofxFinger> *fingersList;
	VideoFilter* createFiducialFilter(ofxFiducial* fiducial);

	ofEvent<ofxFiducial> fiducialFound;
	ofEvent<ofxFiducial> fiducialLost;
	ofEvent<ofxFiducial> fiducialUpdated;

	ofPoint getVideoSize() { return fidtracker.videoSize; }
	
	bool verbose;	
protected:
	bool doDraw;

	VideoPipeline pipe;
	FiducialTrackingFilter fidtracker;
//	MarkerFindingFilter markerfinder;
};

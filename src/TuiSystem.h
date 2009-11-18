#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"
#include "VideoPipeline.h"

#include "ofxCvMain.h"
#include "ofxFidMain.h"
#include "FiducialTrackingFilter.h"
#include "MarkerFindingFilter.h"
#include "PhysicsEngine.h"

//typedef map<int, int>				fiducial_edges_t;
//typedef fiducial_edges_t::iterator	fiducial_edge_iter;

typedef map<int, vector<ofPoint> >	fiducials_corners_t;
typedef fiducials_corners_t::iterator		fiducial_corners_iter;

class TuiSystem : public ofxMSAInteractiveObject
{
public:
	TuiSystem();
	virtual ~TuiSystem();

	void setup();
	void update();
	void draw() const;

	void destroy();

	fiducials_corners_t	fiducialsCornersMap;

	VideoFilter* createFiducialFilter(ofxFiducial* const fiducial);

	bool verbose;

	ofPoint videoSize;
	
	ofxFiducialBacked::physics::Engine fiducial_physics;
protected:
	VideoPipeline pipe;
	FiducialTrackingFilter *fidtracker;
//	MarkerFindingFilter* markerfinder;
};

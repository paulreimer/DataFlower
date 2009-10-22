#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"
#include "VideoPipeline.h"

#include "ofxCvMain.h"
#include "ofxFidMain.h"
#include "FiducialTrackingFilter.h"
#include "MarkerFindingFilter.h"

typedef int fiducialIndex;
//typedef ofxFiducial* fiducialIndex;

typedef map<int, int>				fiducial_edges_t;
typedef fiducial_edges_t::iterator	fiducial_edge_iter;

typedef list<ofxFiducial>			fiducials_t;
typedef fiducials_t::iterator		fiducial_iter;

typedef map<int, int>				my_fiducials_t;
typedef my_fiducials_t::iterator	my_fiducial_iter;

typedef map<int, vector<ofPoint> >	fiducials_corners_t;
typedef fiducials_corners_t::iterator		fiducial_corners_iter;

typedef list<ofxFinger>				fingers_t;
typedef fingers_t::iterator			finger_iter;

typedef map<int, int>				my_fingers_t;
typedef my_fingers_t::iterator		my_finger_iter;

struct fiducialRayIntersectionEvtArgs {
	ofxFiducial* from;
	ofxFiducial* to;
	ofxPoint2f pt;
	fiducialRayIntersectionEvtArgs() {
		from = NULL;
		to = NULL;
		pt = ofxPoint2f(0,0);
	}
};

struct fiducialEvtArgs {
	ofxFiducial* fiducial;
	vector<ofPoint>* cornerPts;

	fiducialEvtArgs() {
		fiducial = NULL;
		cornerPts = NULL;
	}
};

class TuiSystem : public ofxMSAInteractiveObject
{
public:
	TuiSystem();
	virtual ~TuiSystem();

	void setup();
	void update();
	void draw() const;
	void toggleDraw();

	void destroy();

//	fiducial_edges_t fiducialEdges;

	fiducials_corners_t	fiducialsCornersMap;
	fiducial_edges_t	fiducialEdges;

	fiducials_t			&fiducialsList;
	fingers_t			&fingersList;
	
	my_fiducials_t		fiducialsMap;
	my_fingers_t		fingersMap;

	VideoFilter* createFiducialFilter(ofxFiducial* const fiducial);

	ofEvent<fiducialEvtArgs> fiducialFoundEvt;
	ofEvent<fiducialEvtArgs> fiducialLostEvt;
	ofEvent<fiducialEvtArgs> fiducialUpdatedEvt;

	ofEvent<fiducialRayIntersectionEvtArgs> fiducialRayIntersectionFoundEvt;
	ofEvent<fiducialRayIntersectionEvtArgs> fiducialRayIntersectionLostEvt;
	ofEvent<fiducialRayIntersectionEvtArgs> fiducialRayIntersectionUpdatedEvt;

	void fiducialRayIntersectionFound		(fiducialRayIntersectionEvtArgs &args);
	void fiducialRayIntersectionLost		(fiducialRayIntersectionEvtArgs &args);
	void fiducialRayIntersectionUpdated		(fiducialRayIntersectionEvtArgs &args);

	ofPoint getVideoSize() const { return fidtracker.videoSize; }

	bool verbose;

	float x_scale, y_scale;
	ofPoint videoSize;
	vector<ofPoint> window_corners;
protected:
	bool doDraw;

	ofxPoint2f intersects_window_edge(const ofxPoint2f& origin, const ofxPoint2f& endpoint) const;

	VideoPipeline pipe;
	FiducialTrackingFilter fidtracker;
//	MarkerFindingFilter markerfinder;
};

ofxPoint2f intersects(const ofxPoint2f& origin, const ofxPoint2f& endpoint,
					  const ofxPoint2f& box_origin, double box_angle,
					  int box_w, int box_h);

ofxPoint2f intersects(const ofxPoint2f& origin, const ofxPoint2f& endpoint,
					  const vector<ofPoint>& corners);


//ofxPoint2f intersects_window_edge(ofxPoint2f origin, double angle);

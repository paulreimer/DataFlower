#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"
#include "VideoPipeline.h"

#include "ofxCvMain.h"
#include "ofxFidMain.h"
#include "FiducialTrackingFilter.h"
#include "MarkerFindingFilter.h"

//typedef int fiducialIndex;

typedef ofxFiducial* fiducialIndex;
/*
typedef std::map<byte, std::map<fiducialIndex, FiducialBackedObject*> > fid_objs_table;
typedef std::map<byte, std::map<fiducialIndex, FiducialBackedObject*> >::iterator fid_objs_lookup;

typedef std::map<fiducialIndex, FiducialBackedObject*>fid_obj_table;
typedef std::map<fiducialIndex, FiducialBackedObject*>::iterator fid_obj_lookup;
*/
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
	fiducialEvtArgs() {
		fiducial = NULL;
	}
};

class TuiSystem : public ofxMSAInteractiveObject 
{
public:
	TuiSystem();
	virtual ~TuiSystem();

	void setup();
	void update();
	void draw();
	void toggleDraw();

	void destroy();

	//list to store fiducials
	std::map<fiducialIndex, vector<ofPoint> > fiducialCornersMap;
	std::map<fiducialIndex, fiducialIndex> fiducialEdges;
	
	std::list<ofxFiducial> &fiducialsList;

	//list to store fingers
	std::list <ofxFinger> &fingersList;
	VideoFilter* createFiducialFilter(ofxFiducial* fiducial);

	ofEvent<fiducialEvtArgs> fiducialFoundEvt;
	ofEvent<fiducialEvtArgs> fiducialLostEvt;
	ofEvent<fiducialEvtArgs> fiducialUpdatedEvt;

	ofEvent<fiducialRayIntersectionEvtArgs> fiducialRayIntersectionFoundEvt;
	ofEvent<fiducialRayIntersectionEvtArgs> fiducialRayIntersectionLostEvt;
	ofEvent<fiducialRayIntersectionEvtArgs> fiducialRayIntersectionUpdatedEvt;

	void fiducialRayIntersectionFound		(fiducialRayIntersectionEvtArgs &args);
	void fiducialRayIntersectionLost		(fiducialRayIntersectionEvtArgs &args);
	void fiducialRayIntersectionUpdated		(fiducialRayIntersectionEvtArgs &args);
	
	ofPoint getVideoSize() { return fidtracker.videoSize; }
	
	bool verbose;	

	float x_scale, y_scale;
	ofPoint videoSize;
	vector<ofPoint> window_corners;
protected:
	bool doDraw;

	ofxPoint2f intersects_window_edge(ofxPoint2f origin, ofxPoint2f endpoint);
	
	VideoPipeline pipe;
	FiducialTrackingFilter fidtracker;
//	MarkerFindingFilter markerfinder;
};

ofxPoint2f intersects(ofxPoint2f origin, ofxPoint2f endpoint,
					  ofxPoint2f box_origin, double box_angle, int box_w, int box_h);

ofxPoint2f intersects(ofxPoint2f origin, ofxPoint2f endpoint,
					  vector<ofPoint>& corners);


//ofxPoint2f intersects_window_edge(ofxPoint2f origin, double angle);

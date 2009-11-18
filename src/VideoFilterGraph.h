#pragma once

#include "ofMain.h"
#include "filters.h"

#include "TuiSystem.h"

#include "GuiGraph.h"
#include "Renderer.h"

#include "ofxSimpleGuiConfig.h"
#include "ofxOpenCv.h"

typedef VideoFilter* vertex_t;

struct edge_t {
	vertex_t* from;
	vertex_t* to;
	ofxFiducialBacked::types::actors::lineSegment ray;

#ifdef USE_FIDUCIALS
	int fiducial;
#endif
};

class VideoFilterGraph : public ofxFiducialBacked::gui::GuiGraph
{
public:
	VideoFilterGraph();
	virtual ~VideoFilterGraph();

//	void setup();

	void update();
	void draw();

	void destroy();

/*
	ofxCvColorImage&		input_ref() {
		return flows.front().first;
	};
	ofxCvColorImage&		output_ref() {
		return flows.front().output;
	};
*/
	list	<vertex_t>	vertices;
	list	<edge_t>	edges;

	map<ofxCvImage*, ofxCvImage*> image_edges;
	
	void fiducialRayTermination			(ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs &args);
	void fiducialRayIntersectionFound	(ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs &args);
	void fiducialRayIntersectionLost	(ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs &args);
	void fiducialRayIntersectionUpdated	(ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs &args);	
protected:
#ifdef USE_GUI
	ofxSimpleGuiConfig fid_gui_conf;
#endif
};

#ifdef USE_SMART_POINTERS
typedef Poco::SharedPtr<VideoFilterGraph> VideoFilterGraphPtr;
#else
typedef VideoFilterGraph* VideoFilterGraphPtr;
#endif

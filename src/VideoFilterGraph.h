#pragma once

#include "ofMain.h"
#include "filters.h"

#include "TuiSystem.h"
#include "FiducialBackedObject.h"
#include "ofxSimpleGuiConfig.h"
#include "ofxOpenCv.h"
#include "GuiElements.h"

typedef VideoFilter* vertex_t;

struct edge_t {
	vertex_t* from;
	vertex_t* to;
	GuiElements::types::lineSegment ray;

#ifdef USE_FIDUCIALS
	fiducialIndex fiducial;
#endif
};

class VideoFilterGraph : public FiducialBackedObject {
public:
	VideoFilterGraph();
	virtual ~VideoFilterGraph();

//	void setup();

	void update();
	void draw();

	void destroy();

	void setDraw(bool b);
	void toggleDraw();
	bool					doDraw;
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

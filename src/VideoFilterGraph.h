#pragma once

#include "ofMain.h"
#include "filters.h"

#include "TuiSystem.h"
#include "FiducialBackedObject.h"
#include "ofxSimpleGuiConfig.h"
#include "ofxOpenCv.h"
#include "GuiElements.h"

typedef unsigned char filter_type_t;
enum filter_types {
	NULL_FILTER,
	COLOR_FILTER,
	GRAYSCALE_FILTER,
	MULTI_FILTER,
	SOURCE_FILTER,
};

class VideoFilterGraph : public FiducialBackedObject {
public:
	VideoFilterGraph();
	virtual ~VideoFilterGraph();
	
	void setup();
	
	void update();
	void draw();
	
	void destroy();
	
	void setDraw(bool b);
	void toggleDraw();
	bool					doDraw;
	ofPoint					videoSize;
	
	ofxCvColorImage			input;	
	ofxCvColorImage			output;
	
	void setConfig(ofxSimpleGuiConfig *config);
	void setVideoSize(ofPoint sze);
	
//	int size() { return filters.size(); }
	
	map<fiducialIndex,VideoFilter*> filters;
	list<map<	pair<filter_type_t,ofxCvImage*>,
				pair<filter_type_t,ofxCvImage*> > > flows;

	list<GuiElements::types::lineSegment> rays;

protected:
#ifdef USE_GUI
	ofxSimpleGuiConfig*		config;
	ofxSimpleGuiConfig fid_gui_conf;
#endif
};

#ifdef USE_SMART_POINTERS
typedef Poco::SharedPtr<VideoFilterGraph> VideoFilterGraphPtr;
#else
typedef VideoFilterGraph* VideoFilterGraphPtr;
#endif

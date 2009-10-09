#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"

#include "ofxOpenCv.h"
#include "VideoPipeline.h"
#include "VideoFilterGraph.h"

#include "settings.h"

class VideoSystem : public ofxMSAInteractiveObject 
{
public:
	VideoSystem();
	virtual ~VideoSystem();

	void setup();
	void update();
	void draw();
	void toggleDraw();

	void destroy();

	std::vector<ofxCvColorImage> grabImgs;
	std::vector<ofxCvColorImage> vidImgs;

//	VideoPipeline			*pipeline(int i);

	VideoPipeline			*addPipeline(VideoPipeline* pipeline, ofxCvColorImage* src = NULL);
	void					dropPipeline(VideoPipeline* pipeline);

	VideoFilterGraph		*addGraph(VideoFilterGraph* graph, ofxCvColorImage* src = NULL);
	void					dropGraph(VideoFilterGraph* graph);

	std::vector<ofVideoGrabber>	vidGrabbers;
	std::vector<ofPoint>		grabSizes;

	std::vector<ofVideoPlayer>	vidPlayers;
	std::vector<ofPoint>		vidSizes;

	bool bGotFrame;

	bool verbose;

	map <VideoPipelinePtr, ofxCvColorImage*> pipelines;
	map <VideoFilterGraphPtr, ofxCvColorImage*> graphs;
	
	map<fiducialIndex,VideoFilter*> filters;

#ifdef USE_GUI
	ofxSimpleGuiPage* gui;
	ofxSimpleGuiConfig gui_config;
#endif

#ifdef USE_TUI
	void fiducialFound	(fiducialEvtArgs &args);
	void fiducialLost	(fiducialEvtArgs &args);
	void fiducialUpdated(fiducialEvtArgs &args);

	void fiducialRayIntersectionFound(fiducialRayIntersectionEvtArgs &args);
	void fiducialRayIntersectionLost(fiducialRayIntersectionEvtArgs &args);
	void fiducialRayIntersectionUpdated(fiducialRayIntersectionEvtArgs &args);
#endif	
	
protected:

};

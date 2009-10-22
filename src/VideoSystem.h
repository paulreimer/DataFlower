#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"

#include "ofxOpenCv.h"
#include "VideoPipeline.h"
#include "VideoFilterGraph.h"

#include "settings.h"

typedef pair<	VideoPipelinePtr,
				ofxCvColorImage*>	pipeline_t;
typedef list<pipeline_t>			pipelines_t;
typedef pipelines_t::iterator		pipeline_iter;
typedef pipelines_t::const_iterator	pipeline_iter_const;

typedef VideoFilterGraphPtr			filter_graph_t;
typedef	list<filter_graph_t>		filter_graphs_t;
typedef filter_graphs_t::iterator	filter_graph_iter;
typedef filter_graphs_t::const_iterator	filter_graph_iter_const;

typedef map<fiducialIndex,
			VideoFilterPtr>			filters_t;
typedef filters_t::iterator			filter_iter;
typedef filters_t::const_iterator	filter_iter_const;

class VideoSystem : public ofxMSAInteractiveObject 
{
public:
	VideoSystem();
	virtual ~VideoSystem();

	void setup();
	void update();
	void draw();// const;
	void toggleDraw();

	void destroy();

	std::vector<ofxCvColorImage> grabImgs;
	std::vector<ofxCvColorImage> vidImgs;

//	VideoPipeline			*pipeline(int i);

	VideoPipeline			*addPipeline(VideoPipeline* const pipeline, ofxCvColorImage* src = NULL);
	void					dropPipeline(VideoPipeline* const pipeline);

	VideoFilterGraph		*addGraph(VideoFilterGraph* const graph);
	void					dropGraph(VideoFilterGraph* const graph);

	std::vector<ofVideoGrabber>	vidGrabbers;
	std::vector<ofPoint>		grabSizes;

	std::vector<ofVideoPlayer>	vidPlayers;
	std::vector<ofPoint>		vidSizes;

	bool bGotFrame;

	bool verbose;

	pipelines_t		pipelines;
	filter_graphs_t	graphs;
	
	map<fiducialIndex,VideoFilterPtr> filters;

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

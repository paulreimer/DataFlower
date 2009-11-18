#pragma once

#include "ofMain.h"
#include "ofxMSAInteractiveObject.h"

#include "ofxOpenCv.h"
#include "VideoPipeline.h"
#include "VideoFilterGraph.h"

#include "settings.h"
#include "event_types.h"

typedef pair<VideoPipelinePtr, int>	pipeline_t;
typedef list<pipeline_t>			pipelines_t;
typedef pipelines_t::iterator		pipeline_iter;
typedef pipelines_t::const_iterator	pipeline_iter_const;

typedef VideoFilterGraphPtr			filter_graph_t;
typedef	list<filter_graph_t>		filter_graphs_t;
typedef filter_graphs_t::iterator	filter_graph_iter;
typedef filter_graphs_t::const_iterator	filter_graph_iter_const;

typedef map<int, VideoFilterPtr>	filters_t;
typedef filters_t::iterator			filter_iter;
typedef filters_t::const_iterator	filter_iter_const;

struct VideoSource {
	ofxCvColorImage image;
	bool			new_frame;
};

typedef map<int, VideoSource>		video_sources_t;
typedef video_sources_t::iterator	video_source_iter;

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

	VideoPipeline			*addPipeline(VideoPipeline* const pipeline, int src_idx = 0);
	void					dropPipeline(VideoPipeline* const pipeline);

	VideoFilterGraph		*addGraph(VideoFilterGraph* const graph);
	void					dropGraph(VideoFilterGraph* const graph);

	std::vector<ofVideoGrabber>	vidGrabbers;
	std::vector<ofPoint>		grabSizes;

	std::vector<ofVideoPlayer>	vidPlayers;
	std::vector<ofPoint>		vidSizes;

//	bool bGotFrame;

	bool verbose;

	pipelines_t		pipelines;
	filter_graphs_t	graphs;
	video_sources_t	sources;

	filters_t filters;

	float scaleFac;

#ifdef USE_GUI
	ofxSimpleGuiPage* gui;
#endif

#ifdef USE_TUI
	void fiducialFound	(ofxFiducialBacked::types::events::fiducialEvtArgs &args);
	void fiducialLost	(ofxFiducialBacked::types::events::fiducialEvtArgs &args);
	void fiducialUpdated(ofxFiducialBacked::types::events::fiducialEvtArgs &args);

	void fiducialRayIntersectionFound	(ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs &args);
	void fiducialRayIntersectionLost	(ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs &args);
	void fiducialRayIntersectionUpdated	(ofxFiducialBacked::types::events::fiducialRayIntersectionEvtArgs &args);
#endif	
	
protected:

};

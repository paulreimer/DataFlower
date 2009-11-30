#include "VideoSystem.h"
#include "testApp.h"

using namespace ofxFiducialBacked::types::events;

extern testApp* myApp;

/*
class edge_is_from {
public:
	VideoFilterPtr from;
	edge_is_from(VideoFilterPtr _from) : from(_from) {}
	bool operator()(const edge_t& edge) {
		return *(edge.from)==from;
	}
};

class edge_is_to {
public:
	VideoFilterPtr to;
	edge_is_to(VideoFilterPtr _to) : to(_to) {}
	bool operator()(const edge_t& edge) {
		return *(edge.to)==to;
	}
};
*/

VideoSystem::VideoSystem()
{
	verbose = SYSTEM_VERBOSE;
	if (verbose) printf("VideoSystem::VideoSystem()\n");
	
	scaleFac = DEFAULT_FIDUCIAL_SCALE_FACTOR;
}

VideoSystem::~VideoSystem()
{
	if (verbose) printf("VideoSystem::~VideoSystem()\n");
	destroy();
}

void VideoSystem::setup() 
{
//	sources.resize(grabSizes.size()+vidPlayers.size());

//	grabImgs.resize(grabSizes.size());
//	vidImgs.resize(vidSizes.size());

//	grabImgsNew.resize(grabImgs.size());
//	vidImgsNew.resize(vidImgs.size());

	vidGrabbers.resize(grabSizes.size());
	vidPlayers.resize(vidSizes.size());

	int offset = 0;
	for (int i=0; i < grabSizes.size(); i++)
	{
		vidGrabbers[i].setVerbose(true);
		vidGrabbers[i].initGrabber(grabSizes[i].x, grabSizes[i].y);
		
		sources[offset+i].image.allocate(grabSizes[i].x, grabSizes[i].y);
#ifdef USE_GUI
		gui->addContent("Capture "+ofToString(i), sources[i].image);
#endif
	}
	offset+=grabSizes.size();

	for (int i=0; i < vidSizes.size(); i++)
	{
		vidPlayers[i].loadMovie("fingers.mov");
		vidPlayers[i].play();

		vidSizes[i].set(vidPlayers[i].width, vidPlayers[i].height);

		sources[offset+i].image.allocate(grabSizes[i].x, grabSizes[i].y);
#ifdef USE_GUI
		gui->addContent("Video "+ofToString(i), sources[offset+i].image);
#endif
	}
}

VideoPipeline *VideoSystem::addPipeline(VideoPipeline* const pipeline, int src_idx) 
{
	ofxCvColorImage* src = NULL;
	video_source_iter source_it = sources.find(src_idx);
	if (source_it != sources.end())
		src = &source_it->second.image;

	if (src != NULL && src->bAllocated)
	{
		pipeline->videoSize.set(src->width, src->height);
		pipeline->setup();
		pipeline->enableDrawEvents();
	}

	pipelines.push_back(make_pair(pipeline, src_idx));
	return pipeline;
}

void VideoSystem::dropPipeline(VideoPipeline* const pipeline) 
{
	pipeline_iter pipeline_it;

	for (pipeline_it = pipelines.begin(); pipeline_it != pipelines.end(); pipeline_it++)
		if (pipeline_it->first == pipeline)
			pipelines.erase(pipeline_it);
}

VideoFilterGraph *VideoSystem::addGraph(VideoFilterGraph* const graph) 
{
	graph->enableAppEvents();

	ofAddListener(myApp->tuiSystem.fiducial_physics.fiducialRayTerminationEvt,
				  graph, &VideoFilterGraph::fiducialRayTermination);
/*
	ofAddListener(myApp->tuiSystem.fiducial_physics.fiducialRayIntersectionFoundEvt,
				  graph, &VideoFilterGraph::fiducialRayIntersectionFound);
	ofAddListener(myApp->tuiSystem.fiducial_physics.fiducialRayIntersectionLostEvt,	
				  graph, &VideoFilterGraph::fiducialRayIntersectionLost);
	ofAddListener(myApp->tuiSystem.fiducial_physics.fiducialRayIntersectionUpdatedEvt,
				  graph, &VideoFilterGraph::fiducialRayIntersectionUpdated);
*/	
	
	graphs.push_back(graph);
	return graph;
}

void VideoSystem::dropGraph(VideoFilterGraph* const graph) 
{
//	graphs.remove(graph);
	filter_graph_iter graph_it;
	
	for (graph_it = graphs.begin(); graph_it != graphs.end(); graph_it++)
		if (*graph_it == graph)
			graphs.erase(graph_it);
}

void VideoSystem::update() 
{
	// Reset new frame flags
	for (int i=0; i < sources.size(); i++)
		sources[i].new_frame = false;

	int offset = 0;
	for (int i=0; i < vidGrabbers.size(); i++)
	{
		ofVideoGrabber& vidGrabber = vidGrabbers[i];
		ofxCvColorImage& grabImg = sources[offset+i].image;
		
		vidGrabber.grabFrame();

		if (vidGrabber.isFrameNew()) {
			sources[offset+i].new_frame = true;
			grabImg.setFromPixels(vidGrabber.getPixels(), grabSizes[i].x, grabSizes[i].y);
		}
	}
	offset+=vidGrabbers.size();

	for (int i=0; i < vidPlayers.size(); i++)
	{
		ofVideoPlayer& vidPlayer = vidPlayers[i];
		ofxCvColorImage& vidImg = sources[offset+i].image;

		vidPlayer.idleMovie();

		if (vidPlayer.isFrameNew()) {
			sources[offset+i].new_frame = true;
			vidImg.setFromPixels(vidPlayer.getPixels(), vidSizes[i].x, vidSizes[i].y);
		}
	}

	pipeline_iter pipeline_it;
	video_source_iter source_it;
	VideoPipelinePtr pipeline;
	for(pipeline_it = pipelines.begin(); pipeline_it != pipelines.end(); pipeline_it++)
	{
		pipeline = pipeline_it->first;
		if (pipeline->enabled)
		{
			source_it = sources.find(pipeline_it->second);
			if (source_it != sources.end() && source_it->second.new_frame)
			{
				pipeline->input = source_it->second.image;
				pipeline->update();
			}
		}
	}
}

void VideoSystem::toggleDraw() 
{
	pipeline_iter pipeline_it;
	for(pipeline_it = pipelines.begin(); pipeline_it != pipelines.end(); pipeline_it++)
		pipeline_it->first->enabled = !pipeline_it->first->enabled;
}

void VideoSystem::draw()/* const */
{}

void VideoSystem::destroy() 
{
	if (verbose) printf("VideoSystem::destroy()\n");
}


void VideoSystem::fiducialFound(fiducialEvtArgs &args) 
{
	ofxFiducial	&fiducial	= *args.fiducial;
	int			fiducialId	= fiducial.getId();

	if (verbose)
		cout << "Fiducial " << fiducialId << " found (" << fiducial.life << ")" << endl;

	filter_iter filter_it;
	filter_it = filters.find(fiducialId);
	
	VideoFilterPtr filter;
	float h, w;

	if (filter_it == filters.end())
	{
		filter = myApp->tuiSystem.createFiducialFilter(&fiducial);
//		myApp->tuiSystem.fiducialsCornersMap[fiducial.getId()] = fiducial.cornerPoints;
	
		vector<ofPoint>& corners = myApp->tuiSystem.fiducialsCornersMap[fiducial.getId()];
		
		w = filter->width;
		h = filter->height;

		corners.resize(4);
		corners[0].set(0, 0);
		corners[1].set(w, 0);
		corners[2].set(w, h);
		corners[3].set(0, h);
		
		filter->setScale(0.5);
		
		// TODO: make this user-adjustable, or set automatically
		filter->videoSize.set(VIDEO_SIZE);
		filter->setup();
		
		filters.insert(make_pair(fiducialId, filter));
	}
	else {
		filter = filter_it->second;
	}
	
	if (fiducialId < FIRST_FILTER_FIDUCIAL_ID)
	{
//		addGraph(new VideoFilterGraph());
		video_source_iter source_it = sources.find(fiducialId);
		if (source_it != sources.end())
		{
			VideoPipeline* pipe = new VideoPipeline();
			addPipeline(pipe, fiducialId);
			pipe->addFilter(filter);			
		}
	}
}

void VideoSystem::fiducialLost(fiducialEvtArgs &args) 
{
	ofxFiducial&	fiducial	= *args.fiducial;
	int			fiducialId	= fiducial.getId();

//	if (verbose)
		cout << "Fiducial " << fiducialId << " lost (" << fiducial.life << ")" << endl;

	filter_iter filter_it = filters.find(fiducialId);
	
	VideoFilterPtr filter;
	
	if (filter_it != filters.end())
	{
		filter = filter_it->second;
		filter->enabled = false;
		filter->disableAllEvents();
		
		filters.erase(filter_it);
	}
	
	if (fiducialId < FIRST_FILTER_FIDUCIAL_ID)
	{
		//		graphs.erase(&fiducial);
		pipeline_iter pipeline_it;
		for (pipeline_it = pipelines.begin(); pipeline_it != pipelines.end(); pipeline_it++)
			if (pipeline_it->second == fiducialId)
				pipelines.erase(pipeline_it);
	}
}

void VideoSystem::fiducialUpdated(fiducialEvtArgs &args)
{
	ofxFiducial&	fiducial	= *args.fiducial;
	int				fiducialId	= fiducial.getId();
	ofPoint			to_window	= args.to_window;

	if (verbose)
		cout << "Fiducial " << fiducialId << " updated (" << fiducial.life << ")" << endl;
	
	filter_iter filter_it = filters.find(fiducialId);	
	VideoFilterPtr filter;

	if (filter_it != filters.end())
	{
		filter = filter_it->second;

		float r_size = fiducial.getRootSize();
		ofPoint offset(-r_size/2 - 16*to_window.x,
					   r_size/2 + 16*to_window.y);
		
		filter->setPos(fiducial.getX()*to_window.x, fiducial.getY()*to_window.y);
		filter->setScale(r_size*scaleFac);
		filter->setAngle(fiducial.getAngle()+FIDUCIAL_ANGLE_CORRECTION_DEGREES);
		filter->setOffset(offset);
	}
}

//--------------------------------------------------------------
void
VideoSystem::fiducialRayIntersectionFound(fiducialRayIntersectionEvtArgs &args)
{
	//	 ofxFiducial& from = *args.from;
	//	 ofxFiducial& to = *args.to;
	//	 ofxPoint2f from_pt = args.from_pt;
	//	 ofxPoint2f to_pt = args.to_pt;
	
	cout << "Found intersection" << endl;
}

//--------------------------------------------------------------
void
VideoSystem::fiducialRayIntersectionLost(fiducialRayIntersectionEvtArgs &args)
{
	//	 ofxFiducial& from = *args.from;
	//	 ofxFiducial& to = *args.to;
	//	 ofxPoint2f from_pt = args.from_pt;
	//	 ofxPoint2f to_pt = args.to_pt;
	
	cout << "Lost intersection" << endl;
}

//--------------------------------------------------------------
void
VideoSystem::fiducialRayIntersectionUpdated(fiducialRayIntersectionEvtArgs &args)
{
	//	 ofxFiducial& from = *args.from;
	//	 ofxFiducial& to = *args.to;
	//	 ofxPoint2f from_pt = args.from_pt;
	//	 ofxPoint2f to_pt = args.to_pt;
	
	cout << "Updated intersection" << endl;
}

#include "VideoFilterGraph.h"
#include "testApp.h"

using namespace ofxFiducialBacked::renderer::primitives;
using namespace ofxFiducialBacked::types::events;

VideoFilterGraph::VideoFilterGraph()
{
//	ref_type = FIDUCIAL_FILTER_GRAPH_TYPE;
	disableAllEvents();

	verbose = SYSTEM_VERBOSE;

	vertices.clear();
	edges.clear();

	if (verbose) printf("VideoFilterGraph::VideoFilterGraph()\n");
}

VideoFilterGraph::~VideoFilterGraph()
{
	if (verbose) printf("VideoFilterGraph::~VideoFilterGraph()\n");
	destroy();
}

void VideoFilterGraph::update()
{
	//process edges
}

void VideoFilterGraph::draw()
{
	GuiGraph::draw();
/*
	if(!enabled) return;
	glDisableClientState(GL_COLOR_ARRAY);

	list<edge_t>::iterator edges_it;

//	ofxPoint2f startpoint, endpoint, ref;
//	startpoint.set	(254, 215);

//	endpoint.set	(ray_it->from.x,
//					 ray_it->from.y + (from->height/2));

//	endpoint.rotateRad(angle, from->angle);

#ifndef TARGET_OPENGLES
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);

//	drawArrow(startpoint, endpoint, GuiElements::types::ACTOR_TYPE_RGB);

	for (edges_it = edges.begin(); edges_it != edges.end(); edges_it++)
		render::arrow(edges_it->ray);

#ifndef TARGET_OPENGLES
	glPopAttrib();
#endif
*/
}

void VideoFilterGraph::destroy()
{
	if (verbose) printf("VideoFilterGraph::destroy()\n");
}

//--------------------------------------------------------------
void
VideoFilterGraph::fiducialRayTermination(fiducialRayIntersectionEvtArgs &args)
{
	if (args.from == NULL)
		return;

	float dist = args.from_pt.distance(args.to_pt);
	float trimPct = (float)TRIM_RAY_PIXELS/dist;	
	
	ofxPoint2f	draw_from = args.from_pt.getInterpolated(args.to_pt, trimPct);
	ofxPoint2f	draw_to;
	
	ofPoint		to_window = args.to_window;

	if (args.to != NULL)
		draw_to	= args.from_pt.getInterpolated(args.to_pt, 1-trimPct);
	else
		draw_to = args.to_pt;

	segments[args.from->getId()] = make_pair(draw_from*to_window, draw_to*to_window);
}
/*
//--------------------------------------------------------------
void
VideoFilterGraph::fiducialRayIntersectionFound(fiducialRayIntersectionEvtArgs &args)
{
//	 ofxFiducial& from = *args.from;
//	 ofxFiducial& to = *args.to;
//	 ofxPoint2f from_pt = args.from_pt;
//	 ofxPoint2f to_pt = args.to_pt;

	cout << "Found intersection" << endl;
}

//--------------------------------------------------------------
void
VideoFilterGraph::fiducialRayIntersectionLost(fiducialRayIntersectionEvtArgs &args)
{
	//	 ofxFiducial& from = *args.from;
	//	 ofxFiducial& to = *args.to;
	//	 ofxPoint2f from_pt = args.from_pt;
	//	 ofxPoint2f to_pt = args.to_pt;

	cout << "Lost intersection" << endl;
}

//--------------------------------------------------------------
void
VideoFilterGraph::fiducialRayIntersectionUpdated(fiducialRayIntersectionEvtArgs &args)
{
	//	 ofxFiducial& from = *args.from;
	//	 ofxFiducial& to = *args.to;
	//	 ofxPoint2f from_pt = args.from_pt;
	//	 ofxPoint2f to_pt = args.to_pt;

	cout << "Updated intersection" << endl;
}
*/
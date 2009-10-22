#pragma once

#include "VideoFilterGraph.h"
#include "testApp.h"

using namespace GuiElements::renderer;

extern testApp* myApp;

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

void VideoFilterGraph::setDraw(bool b)
{
	doDraw = b;
}

void VideoFilterGraph::toggleDraw()
{
	setDraw(!doDraw);
}

void VideoFilterGraph::draw()
{
	if(!doDraw) return;
	glDisableClientState(GL_COLOR_ARRAY);

	list<edge_t>::iterator edges_it;
/*
	ofxPoint2f startpoint, endpoint, ref;
	startpoint.set	(254, 215);

	endpoint.set	(ray_it->from.x,
					 ray_it->from.y + (from->height/2));

	endpoint.rotateRad(angle, from->angle);
*/
#ifndef TARGET_OPENGLES
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);

//	drawArrow(startpoint, endpoint, GuiElements::types::ACTOR_TYPE_RGB);

	for (edges_it = edges.begin(); edges_it != edges.end(); edges_it++)
		drawArrow(edges_it->ray);

#ifndef TARGET_OPENGLES
	glPopAttrib();
#endif
}

void VideoFilterGraph::destroy()
{
	if (verbose) printf("VideoFilterGraph::destroy()\n");
}

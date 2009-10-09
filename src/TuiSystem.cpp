#pragma once

#include "TuiSystem.h"
#include "VideoSystem.h"
#include "testApp.h"

extern testApp* myApp;

#define SCREEN_TO_VIDEO();
#define VIDEO_TO_SCREEN();

TuiSystem::TuiSystem() :
fiducialsList(fidtracker.fidFinder.fiducialsList),
fingersList(fidtracker.fidFinder.fingersList)
{
	verbose = SYSTEM_VERBOSE;
	if (verbose) printf("TuiSystem::TuiSystem()\n");
	doDraw = true;	
	x_scale=y_scale=1.0;
}

TuiSystem::~TuiSystem()
{
	if (verbose) printf("TuiSystem::~TuiSystem()\n");
	destroy();
}

void TuiSystem::setup()
{
	pipe.setPos(280, 72);
	myApp->videoSystem.addPipeline(&pipe);	

	pipe.addFilter(new HomographyFilter());
	pipe.addFilter(new SimpleThresholdingFilter());
	pipe.addFilter(&fidtracker);

	videoSize.set(fidtracker.width, fidtracker.height);
	x_scale=myApp->window.width/videoSize.x;
	y_scale=myApp->window.height/videoSize.y;

	int w = myApp->window.width,
		h = myApp->window.height;

	window_corners.resize(4);
	window_corners[0].set(0, 0);
	window_corners[1].set(w, 0);
	window_corners[2].set(w, h);
	window_corners[3].set(0, h);
	
//	markerfinder.markerFinder.storeFilename = ofToDataPath(MARKERS_FILE);
//	pipe.addFilter(&markerfinder);
	
//	fingersList = &fidtracker.fidFinder.fingersList;
//	fiducialsList = &fidtracker.fidFinder.fiducialsList;	

	//fiducialsList = &fidfinder.fiducials;	
}

//--------------------------------------------------------------
void TuiSystem::update()
{
	list<ofxFiducial>::iterator fiducial_iter;
	ofxFiducial *fiducial;
	
	fiducialEvtArgs fid_args;
	// First pass, fiducial updates
	for (fiducial_iter = fiducialsList.begin(); fiducial_iter != fiducialsList.end(); fiducial_iter++) {
		fiducial = &(*fiducial_iter);

		fid_args.fiducial = fiducial;
		// delay creating a filter until somewhat stable
		if (fiducial->life == 2)
			if (fiducial->state != FIDUCIAL_LOST)
			{
				ofNotifyEvent(this->fiducialFoundEvt, fid_args);
//				fiducial->life += MAX_FIDUCIAL_LIFE;
			} else
				ofNotifyEvent(this->fiducialLostEvt, fid_args);
		else if (fiducial->life < 2)
			continue;

		ofNotifyEvent(this->fiducialUpdatedEvt, fid_args);
	}
	
	list<ofxFiducial>::iterator chk_fiducial_iter;
	ofxFiducial *chk_fiducial, *hit_fiducial;

	map<fiducialIndex, fiducialIndex>::iterator fiducialEdge, fiducialEdgePrev;
//	ofxFiducial *chk_fiducial, *hit_fiducial;
	
	ofxPoint2f first_hit_point, edge_hit_point, hit_point;
	ofxPoint2f origin, endpoint;
	ofxPoint2f midPoint;

	fiducialIndex fiducialId, chk_fiducialId;
	float angle, box_angle;
	float dist, min_dist;

	fiducialRayIntersectionEvtArgs fid_ray_args;
	for (fiducial_iter = fiducialsList.begin(); fiducial_iter != fiducialsList.end(); fiducial_iter++)
	{
//		fiducialId = fiducial->getId();
		fiducial = &(*fiducial_iter);
		fiducialId = fiducial;
		
		angle = fiducial->getAngle();
		vector<ofPoint>& fiducialCorners = fiducialCornersMap[fiducialId];
		
		origin = ofxPoint2f(fiducialCorners[1].x, fiducialCorners[1].y)
					.midPoint(ofxPoint2f(fiducialCorners[2].x, fiducialCorners[2].y));
//		origin.set	(fiducialCorners[1],
//					 fiducialAABB.y+(fiducialAABB.height)/2);

		
		endpoint.set(origin.x + myApp->window.width + myApp->window.height, origin.y);
		endpoint.rotateRad(angle, origin);
		
		first_hit_point = edge_hit_point = intersects_window_edge(origin, endpoint);
		
		min_dist = origin.distance(first_hit_point);
		
		hit_fiducial = NULL;
		
		// Check if this candidate fiducial intersects any others, and choose the one
		// with minimum distance intersection
		for (chk_fiducial_iter = fiducialsList.begin(); chk_fiducial_iter != fiducialsList.end(); chk_fiducial_iter++) {
//			chk_fiducialId = fiducial->getId();
			chk_fiducial = &(*chk_fiducial_iter);
			chk_fiducialId = fiducial;

			vector<ofPoint>& chk_fiducialCorners = fiducialCornersMap[chk_fiducialId];
			
			if (chk_fiducial->getId()==NULL_FIDUCIAL_ID || chk_fiducial == fiducial)
				continue;
			
			hit_point = intersects(origin, first_hit_point, chk_fiducialCorners);
			
			if (hit_point.x >= 0 && hit_point.y >= 0)
			{
				dist = origin.distance(hit_point);
				if (dist < min_dist)
				{
					min_dist = dist;
					first_hit_point = hit_point;
					
					hit_fiducial = chk_fiducial;
//					ofNotifyEvent(this->fiducialRayIntersectionHitEvt, fid_ray_args);
				}
			}
		}

		fiducialEdgePrev = fiducialEdges.find(fiducialId);
		if (hit_fiducial != NULL)
		{
			fid_ray_args.from	= fiducial;
			fid_ray_args.to		= hit_fiducial;
			fid_ray_args.pt		= first_hit_point;
			
			if (fiducialEdgePrev->second == hit_fiducial)
				ofNotifyEvent(this->fiducialRayIntersectionUpdatedEvt, fid_ray_args);
			else
				ofNotifyEvent(this->fiducialRayIntersectionFoundEvt, fid_ray_args);
			
			fiducialEdgePrev->second = hit_fiducial;
		}
		
		if (fiducialEdgePrev != fiducialEdges.end()
			&& (!hit_fiducial || fiducialEdgePrev->second != hit_fiducial))
		{
			fid_ray_args.from	= fiducial;
			fid_ray_args.to		= fiducialEdgePrev->second;
			fid_ray_args.pt		= ofxPoint2f(0,0);
			ofNotifyEvent(this->fiducialRayIntersectionLostEvt, fid_ray_args);
		}
	}
}

//--------------------------------------------------------------
void TuiSystem::draw()
{
	if (!doDraw) return;
	
	float x_scale=myApp->window.width/videoSize.x;
	float y_scale=myApp->window.height/videoSize.y;
	
	// draw fullscreen fiducials
	float _x=0, _y=0, deg;
	for (list<ofxFiducial>::iterator fiducial = fiducialsList.begin(); fiducial != fiducialsList.end(); fiducial++) {
		_x = fiducial->current.xpos*x_scale;
		_y = fiducial->current.ypos*y_scale;
		// Draw root
		ofNoFill();
		ofSetRectMode(OF_RECTMODE_CENTER);
		glPushMatrix();
		glTranslatef(_x, _y, 0);
		deg = fiducial->getAngle()*180.0/PI; // get degree
		glRotatef(deg, 0, 0, 1.0); // must flip degrees to compensate for image flip
		ofSetColor(255, 0, 0);//set color red
		ofRect(0, 0, fiducial->r_size, fiducial->r_size); //draw root size red
		ofSetColor(0, 0, 255); //set color blue
		ofCircle(0, fiducial->l_size*4, fiducial->l_size); //draw leaf size blue
		ofSetColor(0, 255, 0); //set color green
		ofDrawBitmapString(ofToString( fiducial->fidId ), 0, 0); //draw fiducial number green
		glPopMatrix(); // fiducial
		ofSetRectMode(OF_RECTMODE_CORNER);
		ofSetColor(255,255,255);
		
		// Draw corners
		fiducial->computeCorners();
		ofSetColor(0, 255, 0);
		ofNoFill();
		glPushMatrix();
		if (fiducial->cornerPoints.size() > 0)
		{
			for(int i = 0; i < fiducial->cornerPoints.size(); i++) {
				_x = fiducial->cornerPoints[i].x*x_scale;
				_y = fiducial->cornerPoints[i].y*y_scale;
				ofCircle(_x, _y, 6);
				ofDrawBitmapString(ofToString(i), _x, _y);
				//printf("corner %d.x: %f corner %d.y %f\n", i, fiducial->cornerPoints[i].x, i, fiducial->cornerPoints[i].y);
			}
		}
		glPopMatrix(); // corners
		ofSetColor(255,255,255);
		
		ofSetColor(0,0,255);//set color to blue
		//if mouse (- 20 to compensate for drawing at 20) is inside fiducial then fill
		if (fiducial->isPointInside(myApp->mouseX - 20, myApp->mouseY - 20)) ofFill();
		else ofNoFill();// else dont
		ofCircle(myApp->mouseX, myApp->mouseY, 10);//draw mouse position
		ofSetColor(255,255,255);//reset color to white
		//like this one below
		//cout << "fiducial " << fiducial->getId() << " found at ( " << fiducial->getX() << "," << fiducial->getY() << " )" << endl;
		//take a good look at the fiducial class to get all the info and a few helper functions
	}
 
	
	if (fingersList.size())
	{
		//draw the fingers
		ofNoFill();
		ofSetRectMode(OF_RECTMODE_CENTER);
		ofSetColor(255, 0, 0);//set color red

		for (list<ofxFinger>::iterator finger = fingersList.begin(); finger != fingersList.end(); finger++)
		{
			glPushMatrix();
			glTranslatef(finger->current.xpos + _x, finger->current.ypos + _y, 0);
			ofRect(0, 0, finger->width, finger->height); //draw finger red
			ofDrawBitmapString(ofToString( finger->fingerID ), 0, finger->height+2); //draw finger number
			glPopMatrix();
		}

		ofSetRectMode(OF_RECTMODE_CORNER);
		ofSetColor(255,255,255);
	 }
}

VideoFilter* TuiSystem::createFiducialFilter(ofxFiducial* fiducial) 
{
	VideoFilterPtr filter;
	int chk_id = fiducial->getId() % 19;
	if (chk_id == NULL_FIDUCIAL_ID)
		filter = new PassthroughFilter();
	
	else switch (chk_id)
	{
		case 0:
			filter = new PassthroughFilter();
			break;
		case 1:
			filter = new GrayscalePassthroughFilter();
			break;
		case 2:
			filter = new HomographyFilter();
			break;
		case 3:
			filter = new ZivkovicBgFilter();
			break;
		case 4:
			filter = new LiBgFilter();
			break;
#ifdef USE_OPENCL
#ifdef USE_GPU_VIS
		case 5:
			filter = new SobelFilterOcl(*(myApp->renderSystem.clScheduler));
			break;
		case 6:
			filter = new VoxelBufferFilter(*(myApp->renderSystem.clScheduler),
										   myApp->renderSystem.rayTracer);
			break;
#endif
#endif
		case 7:
			filter = new DifferencingFilter();
			break;
		case 8:
			filter = new MotionHistoryImageFilter();
			break;
		case 9:
			filter = new SimpleThresholdingFilter();
			break;
		case 10:
			filter = new DistanceTransformFilter();
			break;
		case 11:
			filter = new ColorspaceConversionFilter();
			break;
		case 12:
			filter = new FiducialTrackingFilter();
			break;
		case 13:
			filter = new CaptureFilter();
			break;
/*		case 14:
			filter = new CalibrationFilter();
			break;
*/		case 15:
			filter = new SobelFilter();
			break;
		case 16:
			filter = new ContourFindingFilter();
			break;
		case 17:
			filter = new ThresholdingFilter();
			break;
#ifdef USE_OPENCV_TRUNK
		case 18:
			filter = new DenseOpticalFlowFilter();
			break;
		case 19:
			filter = new AdaptiveSkinFilter();
			break;
#endif
		case 20:
			filter = new BackgroundSubtractionFilter();
			break;
		case 21:
			filter = new AdaptiveThresholdingFilter();
			break;
		case 22:
			filter = new SparseOpticalFlowFilter();
			break;
		case 23:
			filter = new CannyEdgeFilter();
			break;
		default:
			filter = new PassthroughFilter();
//			filter = mirror();
	}
	
	filter->fiducial = fiducial;

	return filter;
}

void TuiSystem::toggleDraw() 
{
	doDraw = !doDraw;
}

void TuiSystem::destroy() 
{
	if (verbose) printf("TuiSystem::destroy()\n");
}

ofxPoint2f TuiSystem::intersects_window_edge(ofxPoint2f origin, ofxPoint2f endpoint) 
{
	return intersects(origin, endpoint, window_corners);
}

//http://newsgroups.archived.at/comp/graphics.algorithms/200603/12/0603122551.html
ofxPoint2f intersects(ofxPoint2f origin, ofxPoint2f endpoint,
					  vector<ofPoint>& corners)
{
	ofxPoint2f hit_point = endpoint;
	
	double min_dist = origin.distance(endpoint);
	double d, la, lb, dist;
	int num_corners = corners.size();
	for (int c=0,next_c=1; c<num_corners; c++, next_c++) {
		if (next_c==num_corners)
			next_c=0;

		ofxPoint2f dp(corners[c].x - origin.x, corners[c].y - origin.y);
		ofxPoint2f qa(endpoint.x - origin.x, endpoint.y - origin.y);
		ofxPoint2f qb(corners[next_c].x - corners[c].x, corners[next_c].y - corners[c].y);
		
		d  = qa.y * qb.x - qb.y * qa.x;
		la = (qb.x * dp.y - qb.y * dp.x)/d;
		lb = (qa.x * dp.y - qa.y * dp.x)/d;
		
		if (0 <= la && la <= 1 && 0 <= lb && lb <= 1)
		{
			hit_point.set(origin.x + la * qa.x,
						  origin.y + la * qa.y);
			dist = origin.distance(hit_point);
			
			if (d-(5*DBL_EPSILON) <= dist && dist <= d+(5*DBL_EPSILON))
				printf("Trust d value");
			
			if (dist < min_dist)
			{
				min_dist = dist;
				endpoint = hit_point;
			}
		}
	}
	return endpoint;
}

//http://newsgroups.archived.at/comp/graphics.algorithms/200603/12/0603122551.html
ofxPoint2f intersects(ofxPoint2f origin, ofxPoint2f endpoint,
					  ofxPoint2f box_origin, double box_angle, int box_w, int box_h)
{
	ofxPoint2f corners[BOX_CORNERS];
	corners[0].set(box_origin.x,		box_origin.y);
	corners[1].set(box_origin.x+box_w,	box_origin.y);
	corners[2].set(box_origin.x+box_w,	box_origin.y+box_h);
	corners[3].set(box_origin.x,		box_origin.y+box_h);
	
	ofxPoint2f ref = box_origin;
	ofxPoint2f hit_point = endpoint;
	
	for (int c=0; c<BOX_CORNERS; c++)
		corners[c].rotateRad(box_angle, ref);

	vector<ofPoint> of_corners(BOX_CORNERS);
	for (int c=0; c<BOX_CORNERS; c++)
		of_corners[c].set(corners[c].x, corners[c].y);

	return intersects(origin, endpoint, of_corners);
}	
/*
ofxPoint2f intersects_window_edge(ofxPoint2f origin, double angle) 
{
	int w=ofGetWidth();
	int h=ofGetHeight();
	ofxPoint2f endpoint;
	double slope=tan(angle), wall_y;
	if (slope >=0)
	{
		if (angle < PI) {	// Quadrant 1 [TR]
			wall_y = slope*(w - origin.x) + origin.y;
			if (wall_y < h)	// Right!
				endpoint.set(w, wall_y);
			else			// Top!
				endpoint.set(origin.x+(h-origin.y)/slope, h);
		}
		else {				// Quadrant 3 [BL]
			wall_y = -slope*(w-origin.x) + origin.y;
			if (wall_y > 0)	// Left!
				endpoint.set(0, wall_y);
			else			// Bottom!
				endpoint.set(origin.x + (-origin.y/slope), 0);
		}
	}
	else {
		if (angle < PI)	{	// Quadrant 2 [TL]
			wall_y = -slope*(w - origin.x) + origin.y;
			if (wall_y < h)	// Left!
				endpoint.set(0, wall_y);
			else			// Top!
				endpoint.set(origin.x + origin.y/slope, h);
		}
		else {				// Quadrant 4 [BR]
			wall_y = slope*(w - origin.x) + origin.y;
			if (wall_y > 0)	// Right!
				endpoint.set(w, wall_y);
			else			// Bottom!
				endpoint.set(origin.x - origin.y/slope, 0);
		}
	}
	return endpoint;
}
*/

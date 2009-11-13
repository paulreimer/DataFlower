#pragma once

#include "TuiSystem.h"
#include "VideoSystem.h"
#include "testApp.h"

extern testApp* myApp;

//#define SCREEN_TO_VIDEO();
//#define VIDEO_TO_SCREEN();

class fiducial_list_by_id {
public:
	const int id;
	fiducial_list_by_id(const int _id) : id(_id) {}
	bool operator()(ofxFiducial& fiducial) {
		return (fiducial.getId()==id);
	}
};
/*
class fiducial_map_by_id {
public:
	const int id;
	fiducial_map_by_id(const int _id) : id(_id) {}
	bool operator()(std::pair<const int, int>& fiducial) {
		return (fiducial.first==id);
	}
};
*/

TuiSystem::TuiSystem()
: fiducialsList(fidtracker.fidFinder.fiducialsList),
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

//	int w = myApp->window.width,
//		h = myApp->window.height;

	int w = videoSize.x,
		h = videoSize.y;

	window_corners.resize(4);
	window_corners[0].set(0, 0);
	window_corners[1].set(w, 0);
	window_corners[2].set(w, h);
	window_corners[3].set(0, h);

//	markerfinder.markerFinder.storeFilename = ofToDataPath(MARKERS_FILE);
//	pipe.addFilter(&markerfinder);
}

//--------------------------------------------------------------
void TuiSystem::update()
{
//	my_fiducial_iter fiducial_status_it;
	fiducial_iter	fiducial_it;
	ofxFiducial		*fiducial;
	fiducialIndex	fiducialId;
	int				fiducialLife;

	fiducialEvtArgs fid_args;
/*
	// Decrement all fiducial life counts
	for (fiducial_it = fiducialsList.begin();
		 fiducial_it != fiducialsList.end();
		 fiducial_it++)
	{
		cout << "Fiducial " << fiducial_it->getId() << " has life " << fiducial_it->life << endl;
	}
*/	
	fiducialsList_copy.clear();
//	fingersList_copy.clear();
//	copy(fiducialsList.begin(), fiducialsList.end(), fiducialsList_copy.begin());
//	copy(fingersList.begin(), fingersList.end(), fingersList_copy.begin());
	fiducialsList_copy	= fiducialsList;
//	fingersList_copy	= fingersList;
	
	// Scan for new fiducials
	for (fiducial_it =	fiducialsList_copy.begin();
		 fiducial_it !=	fiducialsList_copy.end();
		 fiducial_it++)
	{
		fiducial = &(*fiducial_it);
		fiducialId = fiducial->getId();
		
		// TODO: this should be optional
		fiducialsCornersMap[fiducialId] = fiducial->cornerPoints;
		
		fid_args.fiducial = fiducial;
		fid_args.cornerPts = &fiducialsCornersMap[fiducialId];
/*		
		cout << "Fiducial\t" << fiducialId << " has life\t" << fiducial->life
			 << " in state\t";
		if (fiducial->state==FIDUCIAL_FOUND)
			cout << "FOUND" << endl;
		else
			cout << "LOST" << endl;
*/
		if (fiducial->life == MAX_FIDUCIAL_LIFE-1)
		{
			if (fiducial->state == FIDUCIAL_FOUND)
				ofNotifyEvent(this->fiducialFoundEvt, fid_args);
		}
		else if (fiducial->life == MIN_FIDUCIAL_LIFE-1)
		{
			if (fiducial->state == FIDUCIAL_LOST)
				ofNotifyEvent(this->fiducialLostEvt, fid_args);
		}
		else if (fiducial->life == MAX_FIDUCIAL_LIFE)
		{
			if (fiducial->state == FIDUCIAL_FOUND)
				ofNotifyEvent(this->fiducialUpdatedEvt, fid_args);
		}
	}
	
	// Check intersections of fiducials
	fiducial_iter	chk_fiducial_it, prev_hit_fiducial_it;
	ofxFiducial		*chk_fiducial, *hit_fiducial;
	fiducialIndex	chk_fiducialId, hit_fiducialId;
	int				chk_fiducialLife;
	
	fiducial_edge_iter fiducialEdge, fiducialEdgePrev;

	ofxPoint2f first_hit_point, edge_hit_point, hit_point;
	ofxPoint2f origin, endpoint;
	ofxPoint2f midPoint;

	fiducial_corners_iter	fiducialCorners_it;
	vector<ofPoint>			fiducialCorners;
	
	float angle, box_angle;
	float dist, min_dist;
	
	fiducialRayIntersectionEvtArgs fid_ray_args;

	if (fiducialsList.size() == 1)
		return;

	for (fiducial_it = fiducialsList_copy.begin();
		 fiducial_it != fiducialsList_copy.end();
		 fiducial_it++)
	{
		fiducial = &(*fiducial_it);
		fiducialId = fiducial->getId();

		if (fiducial->life < MIN_FIDUCIAL_LIFE)
			continue;
		
		angle = fiducial->getAngle();
		
		fiducialCorners_it = fiducialsCornersMap.find(fiducialId);
		if (fiducialCorners_it == fiducialsCornersMap.end())
			continue;

		fiducialCorners = fiducialCorners_it->second;

		origin = ofxPoint2f(fiducialCorners[1])
					.getMiddle(ofxPoint2f(fiducialCorners[2]));
//		origin.set	(fiducialCorners[1],
//					 fiducialAABB.y+(fiducialAABB.height)/2);

	
		endpoint.set(origin.x + myApp->window.width + myApp->window.height, origin.y);
		endpoint.rotateRad(angle, origin);
	
		first_hit_point = edge_hit_point = intersects_window_edge(origin, endpoint);
	
		min_dist = origin.distance(first_hit_point);
	
		hit_fiducial = NULL;
	
		// Check if this candidate fiducial intersects any others, and choose the one
		// with minimum distance intersection
		for (chk_fiducial_it = fiducialsList_copy.begin();
			 chk_fiducial_it != fiducialsList_copy.end();
			 chk_fiducial_it++)
		{
			chk_fiducial = &(*chk_fiducial_it);
			chk_fiducialId = chk_fiducial->getId();
			if (chk_fiducial->life < MIN_FIDUCIAL_LIFE)
				continue;			
			
			vector<ofPoint>& chk_fiducialCorners = fiducialsCornersMap[chk_fiducialId];
		
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
					hit_fiducialId = chk_fiducialId;
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
		
			if (fiducialEdgePrev->second == hit_fiducialId)
				ofNotifyEvent(this->fiducialRayIntersectionUpdatedEvt, fid_ray_args);
			else {
				ofNotifyEvent(this->fiducialRayIntersectionFoundEvt, fid_ray_args);				

				fiducialEdgePrev->second = hit_fiducialId;
			}
		}
	
		if (fiducialEdgePrev != fiducialEdges.end()
			&& (!hit_fiducial || fiducialEdgePrev->second != hit_fiducialId))
		{
			prev_hit_fiducial_it = find_if(fiducialsList_copy.begin(),
										   fiducialsList_copy.end(),
										   fiducial_list_by_id(hit_fiducialId));
			
			fid_ray_args.from	= fiducial;
			fid_ray_args.to		= &(*prev_hit_fiducial_it);
			fid_ray_args.pt		= ofxPoint2f(0,0);
			ofNotifyEvent(this->fiducialRayIntersectionLostEvt, fid_ray_args);

			fiducialEdges.erase(fiducialEdgePrev);
		}
	}
}

//--------------------------------------------------------------
void TuiSystem::draw() const
{
	if (!doDraw) return;
	ofScale(x_scale, y_scale, 0.0);
/*
	// draw fullscreen fiducials
	fiducial_iter fiducial_it;
	for (fiducial_it = fiducialsList.begin();
		 fiducial_it != fiducialsList.end();
		 fiducial_it++)
	{
		fiducial_it->draw(0,0);
		fiducial_it->drawCorners(0,0);
	}

	finger_iter finger_it;
	for (finger_it = fingersList.begin();
		 finger_it != fingersList.end();
		 finger_it++)
		finger_it->draw(0,0);
*/
	ofScale(1.0/x_scale, 1.0/y_scale, 0.0);
}

VideoFilter* TuiSystem::createFiducialFilter(ofxFiducial* const fiducial)
{
	VideoFilterPtr filter;
	fiducialIndex chk_id = fiducial->getId() % 19;
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
//			filter = new ColorspaceConversionFilter();
			filter = new PassthroughFilter();
			cout << "Broken filter: ColorspaceConversionFilter" << endl;
			break;
		case 12:
			filter = new FiducialTrackingFilter();
			break;
		case 13:
			filter = new CaptureFilter();
			break;
		case 14:
//			filter = new CalibrationFilter();
			filter = new PassthroughFilter();
			cout << "Unfinished filter: CalibrationFilter" << endl;
			break;
		case 15:
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

//	filter->fiducial = fiducial;

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

ofxPoint2f TuiSystem::intersects_window_edge(const ofxPoint2f& origin, const ofxPoint2f& endpoint) const
{
	return intersects(origin, endpoint, window_corners);
}

//http://newsgroups.archived.at/comp/graphics.algorithms/200603/12/0603122551.html
ofxPoint2f intersects(const ofxPoint2f& origin, const ofxPoint2f& endpoint,
					  const vector<ofPoint>& corners)
{
	ofxPoint2f hit_point = endpoint, first_hit_point;

	double min_dist = origin.distance(endpoint);
	double d, la, lb, dist;
	int num_corners = corners.size();
	for (int c=0,next_c=1; c<num_corners; c++, next_c++)
	{
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
				first_hit_point = hit_point;
			}
		}
	}
	return first_hit_point;
}

//http://newsgroups.archived.at/comp/graphics.algorithms/200603/12/0603122551.html
ofxPoint2f intersects(const ofxPoint2f& origin, const ofxPoint2f& endpoint,
					  const ofxPoint2f& box_origin, double box_angle,
					  int box_w, int box_h)
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
		of_corners[c] = corners[c];

	return intersects(origin, endpoint, of_corners);
}

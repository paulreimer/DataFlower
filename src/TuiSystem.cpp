#include "TuiSystem.h"
#include "VideoSystem.h"
#include "testApp.h"

extern testApp* myApp;

//#define SCREEN_TO_VIDEO();
//#define VIDEO_TO_SCREEN();

/*
class fiducial_pair_by_id {
public:
	const int id;
	fiducial_pair_by_id(const int _id) : id(_id) {}
	bool operator()(ofxFiducial& fiducial) {
		return (fiducial.getId()==id);
	}
};

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
{
	verbose = SYSTEM_VERBOSE;
	if (verbose) printf("TuiSystem::TuiSystem()\n");
}

TuiSystem::~TuiSystem()
{
	if (verbose) printf("TuiSystem::~TuiSystem()\n");
	destroy();
}

void TuiSystem::setup()
{	
	myApp->videoSystem.addPipeline(&pipe);
	pipe.setPos(280, 72);
	pipe.bUseFiducials = false; // that would be krazy

	pipe.addFilter(new HomographyFilter());
	pipe.addFilter(new SimpleThresholdingFilter());
	fidtracker = (FiducialTrackingFilter*)pipe.addFilter(new FiducialTrackingFilter());
//	markerfinder = (MarkerFindingFilter*)pipe.addFilter(new MarkerFindingFilter());
//	markerfinder->markerFinder.storeFilename = ofToDataPath(MARKERS_FILE);

	videoSize.set(fidtracker->output_ref().width, fidtracker->output_ref().height);
	
	fiducial_physics.tracker = &(fidtracker->fidFinder);
	fiducial_physics.width	= videoSize.x;
	fiducial_physics.height	= videoSize.y;
	fiducial_physics.setup();
}

//--------------------------------------------------------------
void TuiSystem::update()
{
/*	
	if (	videoSize.x != fiducial_physics.width
		||	videoSize.y != fiducial_physics.height)
	{
		fiducial_physics.width	= videoSize.x;
		fiducial_physics.height	= videoSize.y;
		fiducial_physics.setup();
	}
*/	
	fiducial_physics.update();
}

//--------------------------------------------------------------
void TuiSystem::draw() const
{
/*
	ofScale(x_scale, y_scale, 0.0);
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
	ofScale(1.0/x_scale, 1.0/y_scale, 0.0);
*/
}

VideoFilter* TuiSystem::createFiducialFilter(ofxFiducial* const fiducial)
{
	VideoFilterPtr filter;
	int chk_id = fiducial->getId();
	if (chk_id < FIRST_FILTER_FIDUCIAL_ID || chk_id > LAST_FILTER_FIDUCIAL_ID)
		return new PassthroughFilter();

	else {
		int filter_type = (chk_id-FIRST_FILTER_FIDUCIAL_ID) % VIDEO_FILTER_TYPE_COUNT;
		switch (filter_type)
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
		}
	}

	return filter;
}

void TuiSystem::destroy()
{
	if (verbose) printf("TuiSystem::destroy()\n");
}

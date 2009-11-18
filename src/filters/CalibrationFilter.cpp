#include "CalibrationFilter.h"

CalibrationFilter::CalibrationFilter()
{
	if (verbose) printf("CalibrationFilter::CalibrationFilter()\n");
}

CalibrationFilter::~CalibrationFilter()
{
	if (verbose) printf("CalibrationFilter::~CalibrationFilter()\n");
	destroy();
}

void CalibrationFilter::setup() 
{
	ColorFilter::setup();

	grayInput.allocate(videoSize.x, videoSize.y);
	tempImage.allocate(videoSize.x, videoSize.y);
	shownCapture.allocate(videoSize.x, videoSize.y);
	
	addContent("Output", &output);
	showCapturesSlider = addSlider("Previous Captures", &settings.show_capture, 0, settings.min_captures);
	addContent("Selected Capture", &shownCapture);
	addButton("Capture", &settings.capture);
//	addToggle("Once", &settings.once);
//	addSlider2d("Chessboard Corners", &targetCorners, 2.0, 7.0, 2.0, 7.0);
	addToggle("Interpolate", &settings.do_interpolate);
	addButton("Calculate", &settings.calc_undistort);
	addToggle("Undistort Stream", &settings.do_undistort);
	addToggle("Guess Lens Intrinsics", &settings.guess_intrinsics);
}


void CalibrationFilter::update() 
{
	if (captures.size() > 0)
	{
		shownCapture = *(captures[settings.show_capture-1]);
	}
	if (settings.capture == true)
	{
		settings.capture = !settings.once;
		grayInput = input;
		if (findTarget()) {
			showCapturesSlider->max = MAX(captures.size(), settings.min_captures);
			settings.show_capture++;
		}
	}
	
	if (settings.calc_undistort == true)
	{
		settings.calc_undistort = false;
		if (captures.size() >= settings.min_captures)
			calibrateCamera();
	}

	if (settings.do_undistort == true &&
		camera.undistMap != NULL)
	{
		cvUnDistort(input.getCvImage(),
					output.getCvImage(),
					camera.undistMap,
					settings.do_interpolate);
	}
}

bool CalibrationFilter::findTarget() 
{
	int numExpectedPoints = chessBoardCalibObject.numPoints;

	CvPoint2D32f* points = new CvPoint2D32f[numExpectedPoints];
	int numFoundPoints;
	
#ifdef USE_CHESSBOARD
	CvSize boardSize = cvSize(CHESSBOARD_CORNERS_X, CHESSBOARD_CORNERS_Y);
	
	int found_target = cvFindChessBoardCorners(input.getCvImage(),
											   boardSize,
											   points,
											   &numFoundCorners
											   CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
#endif
	
	if (found_target)
	{
		cvFindCornerSubPix(grayInput.getCvImage(),
						   points,
						   numFoundPoints,
						   cvSize(11,11), cvSize(-1,-1),
						   cvTermCriteria(
								CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,
								30,				 0.1));
		
#ifdef USE_CHESSBOARD_ONLY
		cvDrawChessboardCorners(input, boardSize, points, numFoundPoints, found_target);
#endif
		
		CalibrationCapture* capture = new CalibrationCapture(chessBoardPoints);
		capture.image = input;
		capture.setPoints(points, numFoundPoints);
		
		captures.push_back(capture);
	}
	
	delete points;
	return found_target;
}

bool CalibrationFilter::calibrateCamera() 
{
	int numImages = captures.size();
//	int numExpectedPoints = targetCorners.nx*targetCorners.ny;
	
//	int numPoints = numImages*numExpectedPoints;

	CvPoint2D32f* screenPoints = new CvPoint2D32f[numPoints];
    for( int i=0; i<numPoints; ++i ) {
        screenPoints[i].x = found_points[i].x;
        screenPoints[i].y = found_points[i].y;
		
    }
    
	CvPoint3D32f* worldPoints = new CvPoint3D32f[numPoints];
    for( int i=0; i<numPoints; ++i ) {
        screenPoints[i].x = calibObject.x;
        screenPoints[i].y = calibObject.y;
        screenPoints[i].z = calibObject.y;
    }
	
	
	CvVect32f translation_vectors = NULL;
	CvMatr32f rotation_matrices = NULL;
	
	if (settings.calc_obj_pos)
	{
		translation_vectors = new float[3*numImages];
		rotation_matrices = new float[9*numImages];
	}

	cvCalibrateCamera2(screenPoints,				//Pointer 2D points in screen space. 
					   worldPoints,					//Pointer 3D points in real space
					   numPoints,
					   cvSize(videoSize.x, videoSize.y),			//Size of the image.
					   camera.distCoeffs,			//output: 4 distortion coefficients
					   camera.intrinsics,			//output: intrinsic camera matrix
					   translation_vectors,			//output: Array of translations vectors
					   rotation_matrices,			//output: Array of rotation matrices
					   settings.guess_intrinsics);	//intrisic guess needed

	delete numFoundPoints;
	delete screenPoints;
	delete worldPoints;
	
	if (settings.calc_obj_pos)
	{
		delete translation_vectors;
		delete rotation_matrices;
	}
	
	if (settings.do_interpolate != camera.was_interpolated)
	{
		delete camera.undistMap;

		int mult = settings.do_interpolate? 3 : 1;
		camera.undistMap = cvCreateImage(cvSize(mult*videoSize.x, mult*videoSize.y), CV_32S, 1);
	}

	cvUnDistortInit(input.getCvImage(),
					camera.undistMap,				//undistortion map (calc once, use many times)
					camera.intrinsics,				//intrinsic camera matrix
					camera.distCoeffs,				//4 distortion coefficients
					settings.do_interpolate);		//
	
	camera.was_interpolated = settings.do_interpolate;
}

void CalibrationFilter::destroy() 
{	
	if (verbose) printf("CalibrationFilter::destroy()\n");
	ColorFilter::destroy();

	grayInput.clear();
	tempImage.clear();
	
	shownCapture.clear();
}

#pragma once

#include "VideoFilter.h"

#define CHESSBOARD_CORNERS_X 7
#define CHESSBOARD_CORNERS_Y 7

class CalibrationFilter : public ColorFilter  
{
public:
	CalibrationFilter();
	virtual ~CalibrationFilter();

	void setup();
	void update();

	bool findTarget();
	bool calibrateCamera();

	void destroy();

	class CalibrationSettings {
		friend class CalibrationFilter;
	protected:
		bool capture;
		bool once;
		bool calc_undistort;
		bool do_interpolate;
		bool do_undistort;
		bool calc_obj_pos;
		
		int guess_intrinsics;

		int min_captures;
		int show_capture;
		
		CalibrationSettings() {
			capture			= false;
			once			= true;
			
			calc_undistort	= false;
			do_interpolate	= false;
			do_undistort	= false;
			calc_obj_pos	= 0;
			
			guess_intrinsics= 0;
			
			min_captures	= 1;
			show_capture	= 0;
		}
	} settings;
	
	class CameraParams {
	public:
		CvVect32f distCoeffs;
		CvMatr32f intrinsics;
		IplImage* undistMap;
		
		bool was_interpolated;
		
		CameraParams() {
			was_interpolated = false;
			distCoeffs = new float[4];
			intrinsics = new float[9];
			
			undistMap = NULL;
		}
		virtual ~CameraParams() {
			delete distCoeffs;
			delete intrinsics;
			delete undistMap;
		};
	};
	
	CameraParams camera;

	class CalibrationObject {
	public:
		CvPoint3D32f* objectPoints;
		int numPoints;

		CalibrationObject(CvPoint3D32f objPts, int nPts)
	{
			objectPoints = scrPts;
			numPoints = nPts;
		}

		CalibrationObject(/* default to chessboard */) {
			numPoints = CHESSBOARD_CORNERS_X*CHESSBOARD_CORNERS_Y;
			objectPoints = new CvPoint3D32f[numPoints];
			int x, y, offset;
			for(y=0; y<CHESSBOARD_CORNERS_Y; y++) {
				for(x=0; x<CHESSBOARD_CORNERS_X; x++ ) {
					offset = numPoints + y*CHESSBOARD_CORNERS_X + x;
					objectPoints[offset].x = x;
					objectPoints[offset].y = y;
					objectPoints[offset].z = 0;
				}
			}
		}
		virtual ~CalibrationObject() {
			delete objectPoints;
		}
	} calibObject;
	
	class CalibrationCapture {
	public:
		ofxCvColorImage image;
		CvPoint2D32f foundPoints;
		int numPoints;
		
		CalibrationCapture() {
			image.allocate(videoSize.x, videoSize.y);
		}
		
		void setPoints(CvPoint2D32f pts, int numPts)
	{
			
			for(int i=0; i<numPts; i++) {
				capture.foundPoints[i] = points[i];
			}
		}
		
		
protected:
	vector <CalibrationCapture*>	captures;

	ofxCvGrayscaleImage		grayInput;
	ofxCvGrayscaleImage		tempImage;

	ofxCvColorImage			shownCapture;
	
	ofxSimpleGuiSliderInt*	showCapturesSlider;
};

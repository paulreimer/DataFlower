#pragma once

#include "VideoFilter.h"

#define MAX_CORNERS 500


class OpticalFlowFilter : public VideoFilter  {
public:
	OpticalFlowFilter(string name = "Optical Flow");
	virtual ~OpticalFlowFilter();

	void setup();
	void update();

	void destroy();

	struct OpticalFlowSettings {
		int		dv_min_error;
		int		dv_max_error;
		float	dv_scale;
		int		max_iter;
		int		win_size;
		float	eps;
		int		max_features;
		int		pyr_levels;
	} settings;
	
private:
	ofxCvGrayscaleImage		grayInput;
	ofxCvGrayscaleImage 	grayOutput;

	ofxCvColorImage 		inputPrev;
	ofxCvGrayscaleImage		grayInputPrev;
	
	ofxCvFloatImage			velx;
	ofxCvFloatImage			vely;
	ofxCvGrayscaleImage		dv;
	
	CvSize					img_sz;
	
	IplImage*				eig_image;
	IplImage*				tmp_image;
	
	CvPoint2D32f*			cornersImg;
	CvPoint2D32f*			cornersPrev;
	CvSize					pyr_sz;
	IplImage*				pyrImg;
	IplImage*				pyrPrev;
	
	int						corner_count;
	char					features_found[ MAX_CORNERS ];
	float					feature_errors[ MAX_CORNERS ];

};

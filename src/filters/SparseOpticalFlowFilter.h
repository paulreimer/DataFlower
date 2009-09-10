#pragma once

#include "VideoFilter.h"

#define MAX_CORNERS 500


class SparseOpticalFlowFilter : public GrayscaleFilter  {
public:
	SparseOpticalFlowFilter();
	virtual ~SparseOpticalFlowFilter();

	void setup();
	void update();

	void destroy();

	class SparseOpticalFlowSettings {
		friend class SparseOpticalFlowFilter;
	protected:
		int		dv_min_error;
		int		dv_max_error;
		float	dv_scale;
		int		max_iter;
		int		win_size;
		float	eps;
		int		max_features;
		int		pyr_levels;

		SparseOpticalFlowSettings() {			
			dv_min_error	= 0;
			dv_max_error	= 550;
			dv_scale		= 1.0;
			max_iter		= 20;
			win_size		= 10;
			eps				= 0.03;
			max_features	= 100;
			pyr_levels		= 5;
		}
	} settings;

private:
	ofxCvGrayscaleImage 	inputPrev;

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

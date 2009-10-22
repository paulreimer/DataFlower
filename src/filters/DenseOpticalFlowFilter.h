#pragma once

#include "VideoFilter.h"

//#include <opencv/cv.h>
//#include <opencv/cv.hpp>
#include "cv.hpp"

using namespace cv;

class DenseOpticalFlowFilter : public GrayscaleFilter  
{
public:
	DenseOpticalFlowFilter();
	virtual ~DenseOpticalFlowFilter();

	void setup();
	void update();

	void destroy();

	class DenseOpticalFlowSettings {
		friend class DenseOpticalFlowFilter;
	protected:
		int		max_iter;
		int		win_size;
		float	pyr_scale;
		int		pyr_levels;
		float	poly_n;
		float	poly_sigma;
		float	scale_min;
		float	scale_max;

		DenseOpticalFlowSettings() {
			max_iter	= 2;
			win_size	= 5;
			pyr_scale	= 0.5;
			pyr_levels	= 2;
			poly_n		= 7;
			poly_sigma	= 1.5;
			scale_min	= 0.0;
			scale_max	= 1.0;
		}
	} settings;

private:
	ofxCvGrayscaleImage		grayInputPrev;

	ofxCvGrayscaleImage		flowX, flowY;
	IplImage*				flowIpl;
	IplImage*				grayFlowIpl;

	cv::Mat inputMatPrev, inputMat, flowMat;
};

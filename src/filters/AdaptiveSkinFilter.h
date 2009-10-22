#pragma once

#ifdef USE_OPENCV_TRUNK

#include "VideoFilter.h"
#include "cvaux.hpp"

class AdaptiveSkinFilter : public ColorFilter 
{
public:
	AdaptiveSkinFilter();
	virtual ~AdaptiveSkinFilter();

	void setup();
	void update();

	void destroy();

	class AdaptiveSkinSettings {
		friend class AdaptiveSkinFilter;
	protected:
		int		sampling_scale;
		int		morphing_mode;
		
		AdaptiveSkinSettings() {
			sampling_scale	= 1;
			morphing_mode	= cv::CvAdaptiveSkinDetector::MORPHING_METHOD_ERODE;
		}
	} settings;

protected:
	ofxCvColorImage			bgrInput;
	ofxCvGrayscaleImage 	grayOutput;

	cv::CvAdaptiveSkinDetector	filter;
};

#endif
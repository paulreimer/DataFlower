#pragma once

#ifdef USE_OPENCV_TRUNK

#include "VideoFilter.h"
#include "cvaux.h"

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
			morphing_mode	= CvAdaptiveSkinDetector::MORPHING_METHOD_ERODE;
		}
	} settings;

protected:
	ofxCvColorImage			bgrInput;
	ofxCvGrayscaleImage 	grayOutput;

	CvAdaptiveSkinDetector	filter;
};

#endif
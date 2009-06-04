#pragma once

#include "VideoFilter.h"
#include "cvaux.h"

class AdaptiveSkinFilter : public VideoFilter {
public:
	AdaptiveSkinFilter();
	virtual ~AdaptiveSkinFilter();

	void setup();
	void update();

	void destroy();

	struct AdaptiveSkinSettings {
		int		sampling_scale;
		int		morphing_mode;
	} settings;

protected:
	ofxCvColorImage			bgrInput;
	ofxCvGrayscaleImage 	grayOutput;

	CvAdaptiveSkinDetector	filter;
};

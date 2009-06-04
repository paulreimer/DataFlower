#pragma once

#include "VideoFilter.h"

class AdaptiveThresholdingFilter : public VideoFilter  {
public:
	AdaptiveThresholdingFilter();
	virtual ~AdaptiveThresholdingFilter();

	void setup();
	void update();

	void destroy();
		
	struct ThresholdSettings {
		int		blocksize;
		int		offset;
		bool	invert;
		bool	use_gauss;
	} settings;

protected:
	ofxCvGrayscaleImage 	grayInput;
	ofxCvGrayscaleImage 	grayOutput;

};

#pragma once

#include "VideoFilter.h"

class ThresholdingFilter : public VideoFilter  {
public:
	ThresholdingFilter();
	virtual ~ThresholdingFilter();

	void setup();
	void update();

	void destroy();

	struct ThresholdSettings {
		int		threshold;
		int		max_value;
		int		mode;
		bool	use_otsu;
	} settings;

protected:
	ofxCvGrayscaleImage 	grayInput;
	ofxCvGrayscaleImage 	grayOutput;

};

#pragma once

#include "VideoFilter.h"

class SimpleThresholdingFilter : public VideoFilter  {
public:
	SimpleThresholdingFilter();
	virtual ~SimpleThresholdingFilter();

	void setup();
	void update();

	void destroy();

	struct SimpleThresholdSettings {
		int		threshold;
		bool	invert;
	} settings;

protected:
	ofxCvGrayscaleImage 	grayOutput;

};

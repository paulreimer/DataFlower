#pragma once

#include "VideoFilter.h"

class ThresholdingFilter : public VideoFilter  {
public:
	ThresholdingFilter(string name = "Threshold");
	virtual ~ThresholdingFilter();

	void setup();
	void update();

	void destroy();

	struct ThresholdSettings {
		int		threshold;
	} settings;

protected:
//	ofxCvGrayscaleImage		grayInput;
	ofxCvGrayscaleImage 	grayOutput;

};

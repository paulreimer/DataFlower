#pragma once

#include "VideoFilter.h"

class CannyEdgeFilter : public VideoFilter  {
public:
	CannyEdgeFilter();
	virtual ~CannyEdgeFilter();

	void setup();
	void update();

	void destroy();

	struct CannyEdgeSettings {
		int low_threshold;
		int high_threshold;
		int aperture_size;
	} settings;
	
protected:
	ofxCvGrayscaleImage 	grayInput;
	ofxCvGrayscaleImage 	grayOutput;
};

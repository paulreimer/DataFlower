#pragma once

#include "VideoFilter.h"

class GrayscaleFilter : public VideoFilter  {
public:
	GrayscaleFilter(string = "Grayscale");
	virtual ~GrayscaleFilter();

	void setup();
	void update();

	void destroy();

protected:
	ofxCvGrayscaleImage 	grayOutput;
};

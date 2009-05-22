#pragma once

#include "VideoFilter.h"

class ContourFindingFilter : public VideoFilter {
public:
	ContourFindingFilter(string name = "Contour Finder");
	virtual ~ContourFindingFilter();

	void setup();
	void update();

	void destroy();
	
private:
	ofxCvContourFinder      contourFinder;
	ofxCvGrayscaleImage		grayInput;
};

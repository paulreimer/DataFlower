#pragma once

#include "VideoFilter.h"

class ContourFindingFilter : public VideoFilter {
public:
	ContourFindingFilter();
	virtual ~ContourFindingFilter();

	void setup();
	void update();
	void draw();

	void destroy();

	struct ContourFindingSettings {
		int		num_blobs;
		bool	find_holes;
	} settings;

	ofxCvContourFinder      contourFinder;
private:
	ofxCvGrayscaleImage		grayInput;
};

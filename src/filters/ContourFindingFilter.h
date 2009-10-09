#pragma once

#include "VideoFilter.h"

class ContourFindingFilter : public GrayscaleFilter 
{
public:
	ContourFindingFilter();
	virtual ~ContourFindingFilter();

	void setup();
	void update();
	void draw();

	void destroy();

	class ContourFindingSettings {
		friend class ContourFindingFilter;
	protected:
		int		num_blobs;
		bool	find_holes;

		ContourFindingSettings() {
			num_blobs	= 10;
			find_holes	= false;
		}
	} settings;

	ofxCvContourFinder      contourFinder;
};

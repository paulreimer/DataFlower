#pragma once

#include "VideoFilter.h"

class CannyEdgeFilter : public GrayscaleFilter  
{
public:
	CannyEdgeFilter();
	virtual ~CannyEdgeFilter();

	void setup();
	void update();

	void destroy();

	class CannyEdgeSettings {
		friend class CannyEdgeFilter;
	protected:
		int low_threshold;
		int high_threshold;
		int aperture_size;
		
		CannyEdgeSettings() {
			low_threshold	= 200;
			high_threshold	= 400;
			aperture_size	= 5;
		}
	} settings;
};

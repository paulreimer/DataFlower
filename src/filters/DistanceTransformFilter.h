#pragma once

#include "VideoFilter.h"

class DistanceTransformFilter : public GrayscaleFilter  
{
public:
	DistanceTransformFilter();
	virtual ~DistanceTransformFilter();

	void setup();
	void update();

	void destroy();

	class DistanceTransformSettings {
		friend class DistanceTransformFilter;
	protected:
		int distance_type;
		int mask_size;
		
		DistanceTransformSettings() {
			distance_type	= CV_DIST_L2;
			mask_size		= 3;
		}
	} settings;
};

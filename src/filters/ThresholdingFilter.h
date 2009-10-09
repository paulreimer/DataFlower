#pragma once

#include "VideoFilter.h"

class ThresholdingFilter : public GrayscaleFilter  
{
public:
	ThresholdingFilter();
	virtual ~ThresholdingFilter();

	void setup();
	void update();

	void destroy();

	class ThresholdSettings {
		friend class ThresholdingFilter;
	protected:
		int		threshold;
		int		max_value;
		int		mode;
		bool	use_otsu;
		
		ThresholdSettings() {
			threshold	= 120;
			max_value	= 255;
			mode		= CV_THRESH_BINARY;
			use_otsu	= false;
		}
	} settings;
};

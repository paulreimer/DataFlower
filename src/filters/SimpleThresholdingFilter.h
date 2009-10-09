#pragma once

#include "VideoFilter.h"

class SimpleThresholdingFilter : public GrayscaleFilter  
{
public:
	SimpleThresholdingFilter();
	virtual ~SimpleThresholdingFilter();

	void setup();
	void update();

	void destroy();

	class SimpleThresholdSettings {
		friend class SimpleThresholdingFilter;
	protected:
		int		threshold;
		bool	invert;

		SimpleThresholdSettings() {
			threshold = 120;
			invert = false;
		}
	} settings;
};

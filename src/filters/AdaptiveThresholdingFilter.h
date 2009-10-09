#pragma once

#include "VideoFilter.h"

class AdaptiveThresholdingFilter : public GrayscaleFilter  
{
public:
	AdaptiveThresholdingFilter();
	virtual ~AdaptiveThresholdingFilter();

	void setup();
	void update();

	void destroy();
		
	class ThresholdSettings {
		friend class AdaptiveThresholdingFilter;
	protected:
		int		blocksize;
		int		offset;
		bool	invert;
		bool	use_gauss;
		
		ThresholdSettings() {
			blocksize	= 9;
			offset		= 0;
			invert		= false;
			use_gauss	= false;
		}
	} settings;
};

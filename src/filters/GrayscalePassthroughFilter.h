#pragma once

#include "VideoFilter.h"

class GrayscalePassthroughFilter : public GrayscaleFilter  {
public:
	GrayscalePassthroughFilter();
	virtual ~GrayscalePassthroughFilter();

	void setup();
	void update();

	void destroy();
};

#pragma once

#include "VideoFilter.h"

class PassthroughFilter : public VideoFilter  {
public:
	PassthroughFilter();
	virtual ~PassthroughFilter();

	void setup();
	void update();

	void destroy();
};

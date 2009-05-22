#pragma once

#include "VideoFilter.h"

class PassthroughFilter : public VideoFilter  {
public:
	PassthroughFilter(string name = "Passthrough");
	virtual ~PassthroughFilter();

	void setup();
	void update();

	void destroy();
};

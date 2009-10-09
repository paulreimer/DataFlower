#pragma once

#include "VideoFilter.h"

class PassthroughFilter : public ColorFilter  
{
public:
	PassthroughFilter();
	virtual ~PassthroughFilter();

	void setup();
	void update();

	void destroy();
};

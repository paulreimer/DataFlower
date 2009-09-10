#pragma once

#include "VideoFilter.h"
#include "CvPixelBackgroundGMM.h"

//"Efficient Adaptive Density Estimation per Image Pixel for the Task of Background Subtraction"
//Z.Zivkovic, F. van der Heijden 
//Pattern Recognition Letters, vol. 27, no. 7, pages 773-780, 2006.

class ZivkovicBgFilter : public ColorFilter  {
public:
	ZivkovicBgFilter();
	virtual ~ZivkovicBgFilter();
	
	void setup();
	void update();
	
	void destroy();
	
	class ZivkovicBgSettings {
		friend class ZivkovicBgFilter;
	protected:
		int		numMorphingOps;
		bool	startOpenMorph;

		ZivkovicBgSettings() {
			numMorphingOps = 0;			
		}
	} settings;
	
protected:
	CvPixelBackgroundGMM*	pGMM;
	
	ofxCvGrayscaleImage	fgMask;
};

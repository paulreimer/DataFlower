#pragma once

#include "VideoFilter.h"
#include "cvaux.h"

//Foreground Object Detection from Videos Containing Complex Background                             
//Li, Huan, Gu, Tian 2003 9p
//http://muq.org/~cynbe/bib/foreground-object-detection-from-videos-containing-complex-background.pdf

class LiBgFilter : public ColorFilter  {
public:
	LiBgFilter();
	virtual ~LiBgFilter();
	
	void setup();
	void update();
	
	void destroy();
	
	class LiBgSettings {
		friend class LiBgFilter;
	protected:
		int		numMorphingOps;
		bool	startOpenMorph;
		CvFGDStatModelParams*	params;
		
		LiBgSettings() {
			numMorphingOps = 0;
			startOpenMorph = true;
			
			params = new CvFGDStatModelParams();
			// Used to allow the first N1c vectors to adapt over time to changing background.
			// Quantized levels per 'color' component. Power of two, typically 32, 64 or 128.
			params->Lc = CV_BGFG_FGD_LC;
			// Number of color vectors used to model normal background color variation at a given pixel.
			params->N1c = CV_BGFG_FGD_N1C;
			// Number of color vectors retained at given pixel.  Must be > N1c, typically ~ 5/3 of N1c.
			params->N2c = CV_BGFG_FGD_N2C;
			
			// Used to allow the first N1cc vectors to adapt over time to changing background.
			// Quantized levels per 'color co-occurrence' component.  Power of two, typically 16, 32 or 64.
			params->Lcc = CV_BGFG_FGD_LCC;
			
			// Number of color co-occurrence vectors used to model normal background color variation at a given pixel.
			params->N1cc = CV_BGFG_FGD_N1CC;
			// Number of color co-occurrence vectors retained at given pixel.  Must be > N1cc, typically ~ 5/3 of N1cc.
			params->N2cc = CV_BGFG_FGD_N2CC;
			
			// These erase one-pixel junk blobs and merge almost-touching blobs. Default value is 1.
			// If TRUE we ignore holes within foreground blobs. Defaults to TRUE.
			params->is_obj_without_holes = TRUE;
			// Number of erode-dilate-erode foreground-blob cleanup iterations.
			params->perform_morphing = 0;
			
			// How quickly we forget old background pixel values seen.  Typically set to 0.1
			params->alpha1 = CV_BGFG_FGD_ALPHA_1;
			// "Controls speed of feature learning". Depends on T. Typical value circa 0.005.
			params->alpha2 = CV_BGFG_FGD_ALPHA_2;
			// Alternate to alpha2, used (e.g.) for quicker initial convergence. Typical value 0.1.
			params->alpha3 = CV_BGFG_FGD_ALPHA_3;
			
			// Affects color and color co-occurrence quantization, typically set to 2.
			params->delta = CV_BGFG_FGD_DELTA;
			// "A percentage value which determines when new features can be recognized as new background." (Typically 0.9).
			params->T = CV_BGFG_FGD_BG_UPDATE_TRESH;
			// Discard foreground blobs whose bounding box is smaller than this threshold.	
			params->minArea = CV_BGFG_FGD_MINAREA;
		}
	} settings;
	
protected:
	ofxCvGrayscaleImage		fgMask;
	CvBGStatModel*			pFGD;
	
};

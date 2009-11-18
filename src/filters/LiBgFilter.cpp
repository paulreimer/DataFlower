#include "LiBgFilter.h"

LiBgFilter::LiBgFilter() {	
	if (verbose) printf("LiBgFilter::LiBgFilter()\n");
}

LiBgFilter::~LiBgFilter()
{
	if (verbose) printf("LiBgFilter::~LiBgFilter()\n");
	destroy();
}

void LiBgFilter::setup() 
{
	ColorFilter::setup();
	
	fgMask.allocate(videoSize.x, videoSize.y);

	pFGD = cvCreateFGDStatModel(input.getCvImage(), settings.params);

	addContent("FG Mask", fgMask);
	addContent("Output", output);
	
	addSlider("[Lc] Levels / Color", settings.params->Lc, 0, 256);
	addSlider("[N1c] # Colors", settings.params->N1c, 0, 50);
	addSlider("[N2c] # Kept colors", settings.params->N2c, 50, 100);
	
	addSlider("[Lcc] Co-occurence Levels", settings.params->Lcc, 0, 128);
	addSlider("[N1cc] # Co-occ. Colors", settings.params->N1cc, 0, 50);
	addSlider("[N2cc] # Kept Co-occ. Colors", settings.params->N2cc, 50, 100);
	
	addSlider("Ignore Holes", settings.params->is_obj_without_holes, 0, 1);
	addSlider("Number of Erode-Dilate-Erosions", settings.numMorphingOps, 0, 10);
	addToggle("Start w/Open Morph", settings.startOpenMorph);
	
	addSlider("[alpha1]", settings.params->alpha1, 0.0, 1.0);
	addSlider("[alpha2]", settings.params->alpha2, 0.0, 0.1);
	addSlider("[alpha3]", settings.params->alpha3, 0.0, 1.0);
	
	addSlider("[delta] Quantization / color", settings.params->delta, 0.0, 10.0);
	addSlider("[T] New BG Threshold", settings.params->T, 0.0, 1.0);
	addSlider("[minArea] Your blobs must be this tall", settings.params->minArea, 0.0, 60.0);
}

void LiBgFilter::update() 
{
	cvUpdateBGStatModel(input.getCvImage(), pFGD);
	fgMask = pFGD->foreground;

	if (settings.numMorphingOps > 0)
	{
		int first_op =	settings.startOpenMorph? CV_MOP_OPEN : CV_MOP_CLOSE;
		int second_op=	settings.startOpenMorph? CV_MOP_CLOSE : CV_MOP_OPEN;
		//				for (int op_num = 0; op_num < numMorphingOps; op_num++) {
		cvMorphologyEx(fgMask.getCvImage(), fgMask.getCvImage(), 0, 0, first_op,  settings.numMorphingOps);
		cvMorphologyEx(fgMask.getCvImage(), fgMask.getCvImage(), 0, 0, second_op,  settings.numMorphingOps);
		//				}
	}
	
	fgMask.flagImageChanged();
	
	cvSetZero(output.getCvImage());
	cvCopy(input.getCvImage(), output.getCvImage(), fgMask.getCvImage());
	output.flagImageChanged();
}

void LiBgFilter::destroy() 
{
	if (verbose) printf("LiBgFilter::destroy()\n");	
	ColorFilter::destroy();

	cvReleaseBGStatModel(&pFGD);
	fgMask.clear();
}

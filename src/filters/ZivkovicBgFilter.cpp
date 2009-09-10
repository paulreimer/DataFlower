#pragma once

#include "ZivkovicBgFilter.h"

ZivkovicBgFilter::ZivkovicBgFilter() {
	pGMM=cvCreatePixelBackgroundGMM(videoSize.x, videoSize.y);
	// K - max number of Gaussians per pixel
	pGMM->nM = 4;
	// Tb - the threshold - n var
	pGMM->fTb = 4*4;
	// Tbf - the threshold
	pGMM->fTB = 0.9f;//1-cf from the paper
	// Tgenerate - the threshold
	pGMM->fTg = 3.0f*3.0f;//update the mode or generate new
	pGMM->fSigma= 11.0f;//sigma for the new mode
	// alpha - the learning factor
	pGMM->fAlphaT=0.001f;
	// complexity reduction prior constant
	pGMM->fCT=0.05f;
	//shadow
	// Shadow detection
	pGMM->bShadowDetection = true;//turn on
	pGMM->fTau = 0.5f;// Tau - shadow threshold
	
	if (verbose) printf("ZivkovicBgFilter::ZivkovicBgFilter()\n");
}

ZivkovicBgFilter::~ZivkovicBgFilter() {
	if (verbose) printf("ZivkovicBgFilter::~ZivkovicBgFilter()\n");
	destroy();
}

void ZivkovicBgFilter::setup() {
	ColorFilter::setup();
	
	fgMask.allocate(videoSize.x, videoSize.y);
		
	addContent("FG Mask", fgMask);
	addContent("Output", output);
	addSlider("[nM] Max Gaussians per pixel", pGMM->nM, 0, 12);
	addSlider("[Tb] Threshold n var", pGMM->fTb, 0.0, 255.0, 0.0);
	addSlider("[TB] Threshold", pGMM->fTB, 0.0, 1.0, 0.0);
	addSlider("[Tg] Generate Mode Threshold", pGMM->fTg, 0.0, 100.0, 0.0);
	addSlider("[Sigma] Sigma for new mode", pGMM->fSigma, 0.0, 100.0, 0.0);
	addSlider("[Alpha] Learning Rate", pGMM->fAlphaT, 0.0, 0.1, 0.0);
	addSlider("[CT] Complexity Reduction Const.", pGMM->fCT, 0.0, 1.0, 0.0);
	addToggle("Shadow Detection", pGMM->bShadowDetection);
	addSlider("[Tau] Shadow Threshold", pGMM->fTau, 0.0, 1.0, 0.0);
	addSlider("Number of Erode-Dilate-Erosions", settings.numMorphingOps, 0, 10);
	addToggle("Start w/Open Morph", settings.startOpenMorph);
}

void ZivkovicBgFilter::update() {
	cvUpdatePixelBackgroundGMM(pGMM,(unsigned char*)input.getCvImage()->imageData,
							   (unsigned char*)fgMask.getCvImage()->imageData);

	if (settings.numMorphingOps > 0) {
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

void ZivkovicBgFilter::destroy() {
	if (verbose) printf("ZivkovicBgFilter::destroy()\n");
	
	cvReleasePixelBackgroundGMM(&pGMM);
	fgMask.clear();
}

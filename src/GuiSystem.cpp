#pragma once

#include "GuiSystem.h"
#include "testApp.h"

extern testApp* myApp;

GuiSystem::GuiSystem() {
	printf("GuiSystem::GuiSystem()\n");
}

GuiSystem::~GuiSystem() {
	printf("GuiSystem::~GuiSystem()\n");
	destroy();
}

GuiSystem& GuiSystem::setup() {
#ifdef USE_GUI
#ifdef USE_FLUID
	//	gui.addPage("Fluid");

	PhysicsSystem* phy = &myApp->physicsSystem;
	gui.addToggle("Draw Fluid",
				  &phy->settings.drawFluid);
	gui.addSlider("fs.viscocity",
				  &phy->fluidSolver.viscocity, 0.0, 0.0002, 0.5);
	gui.addSlider("fs.colorDiffusion",
				  &phy->fluidSolver.colorDiffusion, 0.0, 0.0003, 0.5);
	gui.addSlider("fs.fadeSpeed",
				  &phy->fluidSolver.fadeSpeed, 0.0, 0.1, 0.5);
	gui.addSlider("fs.solverIterations",
				  &phy->fluidSolver.solverIterations, 1, 20);
	gui.addSlider("fd.drawMode",
				  &phy->fluidDrawer.drawMode, 0, FLUID_DRAW_MODE_COUNT-1);
	gui.addToggle("fs.doRGB",
				  &phy->fluidSolver.doRGB);
	gui.addToggle("fs.doVorticityConfinement",
				  &phy->fluidSolver.doVorticityConfinement);
	gui.addToggle("drawFluid",
				  &phy->settings.drawFluid);
	gui.addToggle("drawParticles",
				  &phy->settings.drawParticles); 
	gui.addToggle("renderUsingVA",
				  &phy->settings.renderUsingVA); 
#endif	
#endif
}

void GuiSystem::update() {
}

void GuiSystem::draw() {
#ifdef USE_GUI
	gui.draw();
#endif
}

void GuiSystem::destroy() {
	printf("GuiSystem::destroy()\n");
}

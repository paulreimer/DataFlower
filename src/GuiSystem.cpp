#pragma once

#include "GuiSystem.h"
#include "testApp.h"

extern testApp* myApp;

GuiSystem::GuiSystem()
{
	verbose = SYSTEM_VERBOSE;
	if (verbose) printf("GuiSystem::GuiSystem()\n");
	startPage = 1;
}

GuiSystem::~GuiSystem()
{
	if (verbose) printf("GuiSystem::~GuiSystem()\n");
	destroy();
}

void GuiSystem::setup() 
{
#ifdef USE_FLUID
	gui.addPage("Fluid");

	PhysicsSystem& phy = myApp->physicsSystem;
	gui.addToggle("Draw Fluid",
				  phy.settings.drawFluid);
	gui.addSlider("fs.viscocity",
				  phy.fluidSolver.viscocity, 0.0, 0.0002, 0.5);
	gui.addSlider("fs.colorDiffusion",
				  phy.fluidSolver.colorDiffusion, 0.0, 0.0003, 0.5);
	gui.addSlider("fs.fadeSpeed",
				  phy.fluidSolver.fadeSpeed, 0.0, 0.1, 0.5);
	gui.addSlider("fs.solverIterations",
				  phy.fluidSolver.solverIterations, 1, 20);
	gui.addSlider("fd.drawMode",
				  phy.fluidDrawer.drawMode, 0, FLUID_DRAW_MODE_COUNT-1);
	gui.addToggle("fs.doRGB",
				  phy.fluidSolver.doRGB);
	gui.addToggle("fs.doVorticityConfinement",
				  phy.fluidSolver.doVorticityConfinement);
	gui.addToggle("drawFluid",
				  phy.settings.drawFluid);
	gui.addToggle("drawParticles",
				  phy.settings.drawParticles);
	gui.addToggle("renderUsingVA",
				  phy.settings.renderUsingVA);
#endif
	
#ifdef USE_VBO
	gui.addPage("Elements");
	gui.addSlider2d("Rendering Offset",
					myApp->settings.render_offset, 0, myApp->window.width, 0, myApp->window.height);
	gui.addSlider("Inter-frame Spacing",
				  myApp->settings.frame_spacing, 0.0, 10.0, 0.0);
	gui.addPage("Rendering");
	gui.addSlider("Zoom",
				  myApp->settings.render_offset.z, -1000.0, 0.0, 0.0);
	gui.addSlider("X Rotation",
				  myApp->settings.rot.x, 0.0, 360.0, 0.0);
	gui.addSlider("Y Rotation",
				  myApp->settings.rot.y, 0.0, 360.0, 0.0);
	gui.addSlider("Z Rotation",
				  myApp->settings.rot.z, 0.0, 360.0, 0.0);
	gui.addSlider("GL Draw Mode",
				  myApp->settings.gl_draw_mode, 0, 9);
#endif
#ifdef USE_GPU_VIS
	RenderSystem& re = myApp->renderSystem;
	gui.addPage("Render Position");
	gui.addSlider2d("Rendering Offset",
					*(re.settings.offset), 0, 1, 0, 1);
	gui.addPage("Rendering");
	gui.addSlider("Zoom",
				  re.settings.offset->z, -10.0, 10.0, 0.0);
	gui.addSlider("X Rotation",
				  re.settings.rot->x, 0.0, 360.0, 0.0);
	gui.addSlider("Y Rotation",
				  re.settings.rot->y, 0.0, 360.0, 0.0);
	gui.addSlider("Z Rotation",
				  re.settings.rot->z, 0.0, 360.0, 0.0);
#endif
	
	gui.setPage(startPage);
}

void GuiSystem::toggleDraw() 
{
	gui.toggleDraw();
}

void GuiSystem::draw()
{
	gui.draw();
}

void GuiSystem::destroy() 
{
	if (verbose) printf("GuiSystem::destroy()\n");
}

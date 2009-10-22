#pragma once

#include "RenderSystem.h"
#include "testApp.h"

extern testApp* myApp;

RenderSystem::RenderSystem()
{
	verbose = SYSTEM_VERBOSE;
	if (verbose) printf("RenderSystem::RenderSystem()\n");
}

RenderSystem::~RenderSystem()
{
	if (verbose) printf("RenderSystem::~RenderSystem()\n");
	destroy();
}

void RenderSystem::setup() 
{
	settings.do_draw = true;

	rayTracer.loadFromFile("volumeRender.cl");
	clScheduler->initKernel(rayTracer);
	
//	pipe.addFilter(new SobelFilterOcl(*clScheduler));
//	pipe.addFilter(new VoxelBufferFilter(*clScheduler, rayTracer));

//	pipe.setPos(164, 300);

//	myApp->videoSystem.addPipeline(&pipe);
}

void RenderSystem::toggleDraw() 
{
	settings.do_draw = !settings.do_draw;
}

void RenderSystem::update() 
{
	clScheduler->enqueueKernel(rayTracer);
}

void RenderSystem::draw()
{
	if (!settings.do_draw) return;
	
	float pbo_offset[2];
	pbo_offset[0] = ofClamp(settings.offset->x, 0, 1);
	pbo_offset[1] = ofClamp(1-settings.offset->y, 0, 1);
	
	rayTracer.viewTranslation[0] = settings.offset->x - pbo_offset[0];
	rayTracer.viewTranslation[1] = (1-settings.offset->y) - pbo_offset[1];
	
	// Begin ray tracing draw
	glViewport(0, 0, rayTracer.width, rayTracer.height);
	
    glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
    glLoadIdentity();
	
    glMatrixMode(GL_PROJECTION);
	glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
	
    // use OpenGL to build view matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
	
	//	glRotatef(180, 1.0, 0.0, 0.0);
	//	glTranslatef(-viewTranslation[0], -viewTranslation[1], -viewTranslation[2]);
	//	glTranslatef(rayTracer.volumeSize[0]/2, rayTracer.volumeSize[1]/2, rayTracer.volumeSize[2]/2);
	glRotatef(-settings.rot->x, 1.0,   0,   0);
	// swap y, z axes (more intuitive interface)
	glRotatef(settings.rot->z,   0, 1.0,   0);
	glRotatef(180-settings.rot->y,   0,   0, 1.0);

	glTranslatef(rayTracer.viewTranslation[0], rayTracer.viewTranslation[1], settings.offset->z);
	
	GLfloat modelView[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
    glPopMatrix();
	
    rayTracer.invViewMatrix[0] = modelView[0];
	rayTracer.invViewMatrix[1] = modelView[4];
	rayTracer.invViewMatrix[2] = modelView[8];
	rayTracer.invViewMatrix[3] = modelView[12];
    rayTracer.invViewMatrix[4] = modelView[1];
	rayTracer.invViewMatrix[5] = modelView[5];
	rayTracer.invViewMatrix[6] = modelView[9];
	rayTracer.invViewMatrix[7] = modelView[13];
    rayTracer.invViewMatrix[8] = modelView[2];
	rayTracer.invViewMatrix[9] = modelView[6];
	rayTracer.invViewMatrix[10] = modelView[10];
	rayTracer.invViewMatrix[11] = modelView[14];
	
    // display results
    glClear(GL_COLOR_BUFFER_BIT);
	
    // draw image from PBO
    glDisable(GL_DEPTH_TEST);
	
	GLfloat rpos[4];
	GLboolean valid;
	
	// Invert y-axis
    glRasterPos2f(pbo_offset[0], pbo_offset[1]);
	//	printf("Render offset: (%f, %f)\n", settings.offset.x, settings.offset.y);
	
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, rayTracer.pbo);
    glDrawPixels(rayTracer.width, rayTracer.height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
    glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

	
void RenderSystem::destroy() 
{
	if (verbose) printf("RenderSystem::destroy()\n");
}

void RenderSystem::keyPressed(int key) 
{
	switch (key)
	{
		case '=':
			rayTracer.density += 0.01;
			break;
		case '-':
			rayTracer.density -= 0.01;
			break;
		case '+':
			rayTracer.density += 0.1;
			break;
		case '_':
			rayTracer.density -= 0.1;
			break;
			
		case ']':
			rayTracer.brightness += 0.1;
			break;
		case '[':
			rayTracer.brightness -= 0.1;
			break;
			
		case ';':
			rayTracer.transferOffset += 0.01;
			break;
		case '\'':
			rayTracer.transferOffset -= 0.01;
			break;
			
		case '.':
			rayTracer.transferScale += 0.01;
			break;
		case ',':
			rayTracer.transferScale -= 0.01;
			break;
	}
}

void RenderSystem::mouseMoved(int x, int y) 
{
	float dx, dy;
    dx = x - ox;
    dy = y - oy;
	
	//	viewRotation[0] += dy / 5.0;
	//	viewRotation[1] += dx / 5.0;
	
	ox = x;
    oy = y;
}

void RenderSystem::onMouseDragOver(int x, int y, int button) 
{
	float dx, dy;
    dx = x - ox;
    dy = y - oy;
	
	switch (button)
	{
		case 0:
			// left = zoom
			settings.offset->z += dy / 100.0;
			break;
		case 2:
			// right = translate
			settings.offset->x += dx / 100.0;
			settings.offset->y -= dy / 100.0;
			break;
	}
    ox = x;
    oy = y;
}
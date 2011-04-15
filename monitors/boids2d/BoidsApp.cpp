#include <glog/logging.h>
#include "BoidsApp.h"
#include "Fps.h"
#include <iostream>
#include <FTGL/ftgl.h>

#define FONT_PATH	"/home/arild/lib/FreeMono.ttf"
#define	W			1000
#define H			1000

BoidsApp::BoidsApp()
{
}

BoidsApp::~BoidsApp()
{
}

/**
 * Sets up SDL and then tells SDL to use OpenGl
 *
 * This method is intended to be called by the thread that will
 * do all the rendering, see Start() and _RenderForever().
 */
void BoidsApp::_InitSdlAndOpenGl()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		cout << "SDL_Init() failed" << endl;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	_screen = SDL_SetVideoMode(W, H, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL);
	if (_screen == NULL) {
		cout << "Unable to get video surface: " << SDL_GetError() << endl;
		exit(1);
	}

	glClearColor(0, 0, 0, 0);
	glViewport(0, 0, W, H);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 100, 0, 100, 0, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void BoidsApp::Start()
{
	_running = true;
	_thread = boost::thread(&BoidsApp::_RenderForever, this);
}

void BoidsApp::Stop()
{
	LOG(INFO) << "Stopping BoidsApp...";
	_running = false;
	_thread.join();
	SDL_Quit();
	LOG(INFO) << "BoidsApp stopped";
}

void BoidsApp::CreateBoid(double startX, double startY, BoidSharedContext *ctx)
{
	Boid *boid = new Boid();
	boid->ctx = ctx;
	((Entity *)boid)->tx = startX;
	((Entity *)boid)->ty = startY;
	Entity::entityList.push_back((Entity *)boid);
}

void BoidsApp::RemoveBoid(Boid *Boid)
{

}

void BoidsApp::_RenderForever()
{
	_InitSdlAndOpenGl();
	_DrawAxis();
	srand(SDL_GetTicks());
	SDL_Event event;

	glViewport(100, 100, W - 200, H - 200);
	glEnable(GL_SCISSOR_TEST);
	glScissor(100, 100, W - 200, H - 200);

	while (_running) {
		// Check for ctrl-c
		SDL_PumpEvents(); // Let SDL look for external input
		if (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_QUITMASK) == 1)
			break;

		glClear(GL_COLOR_BUFFER_BIT);

		for (int i = 0; i < Entity::entityList.size(); i++)
			Entity::entityList[i]->OnLoop();

		for (int i = 0; i < Entity::entityList.size(); i++)
			Entity::entityList[i]->OnRender(_screen);

		SDL_GL_SwapBuffers();

		Fps::fpsControl.OnLoop();
		char Buffer[255];
		sprintf(Buffer, "FPS: %d  |  Num Boids: %d", Fps::fpsControl.GetFPS(), Entity::entityList.size());
		SDL_WM_SetCaption(Buffer, Buffer);
	}
}

void BoidsApp::_DrawAxis()
{
//	glEnable(GL_POLYGON_SMOOTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3ub(0, 255, 0);

	glViewport(95, 100, 5, H - 200);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(100, 0);
	glVertex2f(100, 100);
	glVertex2f(0, 100);
	glEnd();

	glViewport(100, 95, W - 200, 5);
	glBegin(GL_QUADS);
	glVertex2f(0, 0);
	glVertex2f(100, 0);
	glVertex2f(100, 100);
	glVertex2f(0, 100);
	glEnd();

	// Create a pixmap font from a TrueType file.
	FTGLPixmapFont font(FONT_PATH);

	// If something went wrong, bail out.
	if(font.Error())
	    LOG(INFO) << "Font failed";

	// Set the font size and render a small text.
	font.FaceSize(12);
	font.Render("Hello World!");

	SDL_GL_SwapBuffers();
}


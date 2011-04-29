#include <glog/logging.h>
#include "BoidsApp.h"
#include "Fps.h"
#include <iostream>
#include <FTGL/ftgl.h>

#define FONT_PATH	"/home/arild/lib/times.ttf"
#define	SCREEN_WIDTH	1024
#define SCREEN_HEIGHT	768

//#define R	120
//#define BOID_X	20
//#define BOID_Y	20

//
double R = 150;
double BOID_X = 50;
double BOID_Y =	50;


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
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		LOG(INFO) << "SDL_Init() failed: " << SDL_GetError();
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER
			| SDL_OPENGL);
	if (_screen == NULL) {
		LOG(INFO) << "Unable to get video surface: " << SDL_GetError();
		exit(1);
	}

	glClearColor(0, 0, 0, 0);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	LOG(INFO) << _orthoLeft << " | " << _orthoRight << " | " << _orthoBottom << " | " << _orthoTop;
	glOrtho(0, R, 0, R, 0, R);
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
	((Entity *) boid)->tx = startX;
	((Entity *) boid)->ty = startY;
	Entity::entityList.push_back((Entity *) boid);
}

void BoidsApp::RemoveBoid(Boid *Boid)
{

}

void BoidsApp::SetView(double x, double y, double width, double height)
{
	double r = R / (double)100;
	_orthoLeft = x * (double)r;
	_orthoRight = (x + width) * (double)r;
	_orthoBottom = y * (double)r;
	_orthoTop = (y + height) * (double)r;
	_updateOrtho = true;
}

void BoidsApp::_RenderForever()
{
	_InitSdlAndOpenGl();
	srand(SDL_GetTicks());
	SDL_Event event;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(_orthoLeft, _orthoRight, _orthoBottom, _orthoTop, 0, R);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

//	glEnable(GL_SCISSOR_TEST);
//	glScissor((BOID_X / (double) R) * SCREEN_WIDTH, (BOID_Y / (double) R) * SCREEN_HEIGHT,
//			SCREEN_WIDTH, SCREEN_HEIGHT);

	LOG(INFO) << "BoidsApp entering infinite loop";
	while (_running) {
//		if (_updateOrtho) {
//			LOG(INFO) << "NEW ORTHO";
//			glMatrixMode(GL_PROJECTION);
//			glLoadIdentity();
//			glOrtho(_orthoLeft, _orthoRight, _orthoBottom, _orthoTop, 0, R);
//			glMatrixMode(GL_MODELVIEW);
//			glLoadIdentity();
//			_updateOrtho = false;
//		}

		// Check for ctrl-c
		SDL_PumpEvents(); // Let SDL look for external input
		if (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_QUITMASK) == 1)
			break;

		glClear(GL_COLOR_BUFFER_BIT);
		_DrawAxis();
		_DrawBoidDescription();

		for (int i = 0; i < Entity::entityList.size(); i++)
			Entity::entityList[i]->OnLoop();

		glPushMatrix();
		glTranslated(BOID_X, BOID_Y, 0);

		for (int i = 0; i < Entity::entityList.size(); i++)
			Entity::entityList[i]->OnRender(_screen);
		glPopMatrix();

		SDL_GL_SwapBuffers();

		Fps::fpsControl.OnLoop();
		char Buffer[255];
		sprintf(Buffer, "FPS: %d  |  Num Boids: %d", Fps::fpsControl.GetFPS(),
				Entity::entityList.size());
		SDL_WM_SetCaption(Buffer, Buffer);
	}
}

void BoidsApp::_DrawAxis()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3ub(0, 255, 0);

	float w = 0.5;

	glBegin(GL_QUADS);
	glVertex2f(BOID_X - w, BOID_Y - w);
	glVertex2f(BOID_X, BOID_Y - w);
	glVertex2f(BOID_X, R);
	glVertex2f(BOID_X - w, R);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(BOID_X - w, BOID_Y - w);
	glVertex2f(R, BOID_Y - w);
	glVertex2f(R, BOID_Y);
	glVertex2f(BOID_X - w, BOID_Y);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(BOID_X - w, R - w);
	glVertex2f(R, R - w);
	glVertex2f(R, R);
	glVertex2f(BOID_X - w, R);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(R - w, BOID_Y - w);
	glVertex2f(R, BOID_Y - w);
	glVertex2f(R, R);
	glVertex2f(R - w, R);
	glEnd();

	//FTGLPixmapFont font(FONT_PATH);
	FTGLPixmapFont font(FONT_PATH);
	FTPoint point(R, BOID_Y/2);
	//font.CharMap(ft_encoding_latin_1);
	if (font.Error())
		LOG(INFO) << "Font failed";
	font.FaceSize(25);
	string text = "Utilization";
	font.Render(text.c_str(), text.length(), point);
}

void BoidsApp::_DrawBoidDescription()
{

}




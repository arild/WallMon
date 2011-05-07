#include <glog/logging.h>
#include "Config.h" // FONT_PATH
#include "BoidsApp.h"
#include "Fps.h"
#include "Scene.h"
#include "NameDrawer.h"
#include <iostream>
#include "WallView.h"

BoidsApp::BoidsApp(int screenWidth, int screenHeight) :
	_screenWidth(screenWidth), _screenHeight(screenHeight)
{
	_boidScene = new Scene(1024, 200, (6 * 1024) - 200, (4 * 768) - 400, 100, 100);
	_nameDrawerScene = new Scene(0, 0, 1024, 4 * 768, 1024, 4 * 768);

	_font = new FTGLTextureFont(FONT_PATH);
	if (_font->Error())
		LOG(INFO) << "Font failed";
	_nameTagList = NULL;
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

	_screen = SDL_SetVideoMode(_screenWidth, _screenHeight, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER
			| SDL_OPENGL);

	if (_screen == NULL) {
		LOG(INFO) << "Unable to get video surface: " << SDL_GetError();
		exit(1);
	}
	glClearColor(0, 0, 0, 0);
	glViewport(0, 0, _screenWidth, _screenHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	LOG(INFO) << _orthoLeft << " | " << _orthoRight << " | " << _orthoBottom << " | "
			<< _orthoTop;
	glOrtho(0, 100, 0, 100, 0, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/**
 * Dispatches the animation thread
 */
void BoidsApp::Start()
{
	_running = true;
	_thread = boost::thread(&BoidsApp::_RenderForever, this);
}

/**
 * Terminates the animation thread
 */
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
	((Entity *) boid)->scene = _boidScene;
	((Entity *) boid)->tx = startX;
	((Entity *) boid)->ty = startY;
	Entity::entityList.push_back((Entity *) boid);
}

void BoidsApp::RemoveBoid(Boid *Boid)
{
}

NameTagList *BoidsApp::CreateNameTagList()
{
	_nameTagList = new NameTagList();
	NameDrawer *nameDrawer = new NameDrawer(_nameTagList, new FTGLTextureFont(FONT_PATH));
	((Entity *) nameDrawer)->scene = _nameDrawerScene;
	Entity::entityList.push_back((Entity *) nameDrawer);
	return _nameTagList;
}

void BoidsApp::SetDisplayArea(double x, double y, double width, double height)
{
	_orthoLeft = x;
	_orthoRight = x + (double) width;
	_orthoBottom = y;
	_orthoTop = y + (double) height;
	_updateOrtho = true;
}

void BoidsApp::_RenderForever()
{
	_InitSdlAndOpenGl();
	srand(SDL_GetTicks());
	SDL_Event event;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(_orthoLeft, _orthoRight, _orthoBottom, _orthoTop, 0, 100);
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

		//glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int i = 0; i < Entity::entityList.size(); i++)
			Entity::entityList[i]->OnLoop();
		for (int i = 0; i < Entity::entityList.size(); i++) {
			Entity::entityList[i]->scene->Load();
			Entity::entityList[i]->OnRender();
			Entity::entityList[i]->scene->Unload();
		}
		_DrawAxis();

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
	_boidScene->Load();
	glColor3ub(0, 255, 0);

	float w = 0.3;
	float l = -1 - w;
	float r = 100 + 1 + w;
	float b = -1 - w;
	float t = 100 + 1 + w;

	// Left
	glBegin(GL_QUADS);
	glVertex2f(l - w, b - w);
	glVertex2f(l, b - w);
	glVertex2f(l, t + w);
	glVertex2f(l - w, t + w);
	glEnd();

	// Right
	glBegin(GL_QUADS);
	glVertex2f(r, b - w);
	glVertex2f(r + w, b - w);
	glVertex2f(r + w, t + w);
	glVertex2f(r, t + w);
	glEnd();

	// Bottom
	glBegin(GL_QUADS);
	glVertex2f(l - w, b - w);
	glVertex2f(r + w, b - w);
	glVertex2f(r + w, b);
	glVertex2f(l - w, b);
	glEnd();
	//	_font->FaceSize(12, 200);
	//	_font->Render("Utilization");

	// Top
	glBegin(GL_QUADS);
	glVertex2f(l - w, t);
	glVertex2f(r + w, t);
	glVertex2f(r + w, t + w);
	glVertex2f(l - w, t + w);
	glEnd();

	_boidScene->Unload();
}

void BoidsApp::_DrawBoidDescription()
{
	//	FTGLPixmapFont font(FONT_PATH);
	//	//FTPoint point(R, BOID_Y/2);
	//	FTPoint point(900, 400);
	//	//font.CharMap(ft_encoding_latin_1);
	//	if (font.Error())
	//		LOG(INFO) << "Font failed";
	//	font.FaceSize(25);
	//	string text = "Utilization";
	//	font.Render(text.c_str(), text.length(), point);
}


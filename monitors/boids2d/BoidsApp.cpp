#include <glog/logging.h>
#include "Config.h" // FONT_PATH
#include "BoidsApp.h"
#include "Fps.h"
#include "Scene.h"
#include "NameDrawer.h"
#include <iostream>
#include "BoidAxis.h"


BoidsApp::BoidsApp(int screenWidth, int screenHeight) :
	_screenWidth(screenWidth), _screenHeight(screenHeight)
{
	_boidScene = new Scene(1024, 200, (6 * 1024) - 200, (4 * 768) - 400, 100, 100);
	_nameDrawerScene = new Scene(0, 0, 1024, 4 * 768, 1024, 4 * 768);

	_scenes.push_back(_boidScene);
	_scenes.push_back(_nameDrawerScene);

	BoidAxis *axis = new BoidAxis();
	axis->Set(0, 100, 25);
	_boidScene->entityList.push_back((Entity *)axis);

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
	((Entity *) boid)->tx = startX;
	((Entity *) boid)->ty = startY;
	_boidScene->entityList.push_back((Entity *) boid);
}

void BoidsApp::RemoveBoid(Boid *Boid)
{
}

NameTagList *BoidsApp::CreateNameTagList()
{
	_nameTagList = new NameTagList();
	NameDrawer *nameDrawer = new NameDrawer(_nameTagList, new FTGLTextureFont(FONT_PATH));
	_nameDrawerScene->entityList.push_back((Entity *) nameDrawer);
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
		for (int i = 0; i < _scenes.size(); i++)
			_scenes[i]->Run();
		//_DrawAxis();
		SDL_GL_SwapBuffers();

		Fps::fpsControl.OnLoop();
		char Buffer[255];
		sprintf(Buffer, "FPS: %d  |  Num Boids: %d", Fps::fpsControl.GetFPS(),
				Entity::entityList.size());
		SDL_WM_SetCaption(Buffer, Buffer);
	}
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


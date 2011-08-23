
/**
 * @file   BoidsApp.cpp
 * @Author Arild Nilsen
 * @date   April, 2011
 *
 * Main class of the boids animation.
 *
 * This class encapsulates the thread that runs the main loop,
 * and offers most of the API.
 */

#include <glog/logging.h>
#include <iostream>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include "Config.h" // FONT_PATH
#include "BoidsApp.h"
#include "Fps.h"
#include "Scene.h"
#include "BoidAxis.h"
#include "ControlPanel.h"
#include "WallView.h"
#include "TouchEvent.h"
#include "Button.h"
#include "ControlPanel.h"

boost::condition mainLoopThreadCondition;
boost::mutex mainLoopThreadMutex;

BoidsApp::BoidsApp(int screenWidth, int screenHeight, EventSystemBase *eventSystem) :
	_screenWidth(screenWidth), _screenHeight(screenHeight)
{
	_eventSystem = eventSystem;
	_SetupScenes();
	_PopulateScenes();
}

BoidsApp::~BoidsApp()
{
}

/**
 * Dispatches the animation thread
 */
void BoidsApp::Start()
{
	_running = true;
	_thread = boost::thread(&BoidsApp::_RenderForever, this);
	//mainLoopThreadCondition.wait(mainLoopThreadMutex);
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

void BoidsApp::CreateBoid(BoidSharedContext *ctx)
{
	_boidScene->AddEntity(new Boid(ctx));
}

NameTable *BoidsApp::GetNameTable()
{
	return _nameTable;
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
	//mainLoopThreadCondition.notify_one();
	LOG(INFO) << "BoidsApp entering infinite loop";
	while (_running) {
		if (_updateOrtho) {
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(_orthoLeft, _orthoRight, _orthoBottom, _orthoTop, 0, 100);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			_updateOrtho = false;
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_mainScene->Run();
		_HandleTouchEvents();

		SDL_GL_SwapBuffers();
		Fps::fpsControl.OnLoop();
		char Buffer[255];
		sprintf(Buffer, "FPS: %d  |  Total Num Objects: %d", Fps::fpsControl.GetFps(), 0);
		SDL_WM_SetCaption(Buffer, Buffer);
	}


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
}

void BoidsApp::_SetupScenes()
{
	float w = TILE_SCREEN_HEIGHT;
	float h = TILE_SCREEN_HEIGHT;

	_mainScene = new Scene(0, 0, WALL_SCREEN_WIDTH, WALL_SCREEN_HEIGHT, WALL_SCREEN_WIDTH, WALL_SCREEN_HEIGHT);
	_controlPanelScene = _mainScene->CreateSubScene(0, 0, w * 3, h * 4, 100, 200);
	_boidScene = _mainScene->CreateSubScene(w * 3, h/2 + 75, w * 3.5, h * 3.3, 100, 100);
	_tableScene = _mainScene->CreateSubScene(w * 7, 0, w * 4, h * 3.5, 50, 100);

	// The event system will recursively check scenes within the provided scene
	_eventSystem->AddScene(_mainScene);

//	_mainScene = new Scene(0, 0, WALL_SCREEN_WIDTH, WALL_SCREEN_HEIGHT, 230, 100);
//	_controlPanelScene = _mainScene->CreateSubScene(0, 0, 80, 100, 100, 200);
//	_boidScene = _mainScene->CreateSubScene(80, 20, 80, 60, 100, 100);
//	_tableScene = _mainScene->CreateSubScene(170, 0, 60, 90, 50, 100);

}

void BoidsApp::_PopulateScenes()
{
	ControlPanel *panel = new ControlPanel();
	_controlPanelScene->AddEntity(panel);

	BoidAxis *axis = new BoidAxis();
	axis->Set(0, 100, 25);
	_boidScene->AddEntity(axis);

	_nameTable = new NameTable();
	_tableScene->AddEntity(_nameTable);
}

void BoidsApp::_HandleTouchEvents()
{
	_eventSystem->PollEvents();
	int numEventesProcessed = 0;
	while (_eventSystem->eventQueue->GetSize() > 0) {
		EventQueueItem item = _eventSystem->eventQueue->Pop();
		Entity *entity = item.get<0>();
		TT_touch_state_t event = item.get<1>();

		_VisualizeShoutEvent(event.loc.x, event.loc.y);
		if (++numEventesProcessed == 5) {
			// Process no more than 5 events at a time, and
			// discard remaining events
			_eventSystem->eventQueue->Clear();
			break;
		}
		entity->HandleHit(event);

	}
	if (numEventesProcessed > 0)
		LOG(INFO) << "Num event processed: " << numEventesProcessed;
}

void BoidsApp::_VisualizeShoutEvent(float x, float y)
{
	float w = 50;
	float h = 50;
	x -= (w / 2);
	y -= (h / 2);

	glColor3f(255, 255, 0);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();

	SDL_GL_SwapBuffers();
}

int BoidsApp::_CountTotalNumObjects()
{
	return 1;
//	int numObjects = 0;
//	BOOST_FOREACH(Scene *s, Scene::scenes)
//				{
//					numObjects += s->entityList.size();
//				}
//	return numObjects;
}


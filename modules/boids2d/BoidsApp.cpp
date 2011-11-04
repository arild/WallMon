
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
#include "System.h"
#include "Xserver.h"
#include "Stat.h"

boost::condition mainLoopThreadCondition;
boost::mutex mainLoopThreadMutex;


BoidsApp::BoidsApp(int screenWidth, int screenHeight, EventSystemBase *eventSystem, WallView *wallView) :
	_screenWidth(screenWidth), _screenHeight(screenHeight)
{
	Font::Init(wallView->GetTotalPixelWidth());
	_eventSystem = eventSystem;
	_wallView = wallView;
	_SetupAndPopulateScenes();
}

BoidsApp::~BoidsApp()
{
	Font::Close();
}

/**
 * Dispatches the animation thread
 */
void BoidsApp::Start()
{
	_running = true;
	_thread = boost::thread(&BoidsApp::_RenderForever, this);
	mainLoopThreadCondition.wait(mainLoopThreadMutex);
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

Boid *BoidsApp::CreateBoid(BoidSharedContext *ctx)
{
	Boid *boid = new Boid(ctx);
	_boidScene->AddEntity(boid);
	return boid;
}

void BoidsApp::RemoveBoid(Boid *boid)
{
	_boidScene->RemoveEntity(boid);
}

Table *BoidsApp::GetNameTable()
{
	return _table;
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
	mainLoopThreadCondition.notify_one();
	LOG(INFO) << "BoidsApp entering infinite loop";

	Stat<double> numEntitiesSample("Num Objects"), clearSample("Clear"), runScenesSample("Run Scenes"), handleTouchSample("Handle Events"),
			runCallbacksSample("Run Callbacks"), swapBuffersSample("Swap Buffers"),
			totalSample("Total"), otherSample("Other");

	while (_running) {
		numEntitiesSample.NewSample();
		clearSample.NewSample();
		runScenesSample.NewSample();
		handleTouchSample.NewSample();
		runCallbacksSample.NewSample();
		swapBuffersSample.NewSample();
		totalSample.NewSample();
		totalSample.Start();

		if (_updateOrtho) {
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(_orthoLeft, _orthoRight, _orthoBottom, _orthoTop, 0, 100);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			_updateOrtho = false;
		}
		clearSample.Start();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		clearSample.Sample();

		runScenesSample.Start();
		Scene::RunAllScenes();
		runScenesSample.Sample();

		handleTouchSample.Start();
		_HandleTouchEvents();
		handleTouchSample.Sample();

		runCallbacksSample.Start();
		EntityEvent::RunAllCallbacks();
		runCallbacksSample.Sample();

		swapBuffersSample.Start();
		SDL_GL_SwapBuffers();
		swapBuffersSample.Sample();

//		Xserver::BringToFront();
		totalSample.Sample();
		numEntitiesSample.Add((double)Scene::GetTotalNumEntities());
		char Buffer[255];
		sprintf(Buffer, "WallMon - FPS: %d  |  Total Num Objects: %d", Fps::fpsControl.GetFps(), Scene::GetTotalNumEntities());
		SDL_WM_SetCaption(Buffer, Buffer);
		Fps::fpsControl.OnLoop();


		if (clearSample.NumSamples() == 1000 && _wallView->IsLowerLeft()) {
			for (int i = 0; i < totalSample.NumSamples(); i++) {
				double sumOther = totalSample.Sum(i) - (clearSample.Sum(i) +
							runScenesSample.Sum(i) + handleTouchSample.Sum(i) +
							runCallbacksSample.Sum(i) + swapBuffersSample.Sum(i));
				otherSample.NewSample();
				otherSample.Add(sumOther);
			}

			clearSample.PrintToScreen();
			runScenesSample.PrintToScreen();
			handleTouchSample.PrintToScreen();
			runCallbacksSample.PrintToScreen();
			swapBuffersSample.PrintToScreen();
			otherSample.PrintToScreen();

			vector<Stat<double> > v;
			v.push_back(clearSample);
			v.push_back(runScenesSample);
			v.push_back(handleTouchSample);
			v.push_back(runCallbacksSample);
			v.push_back(swapBuffersSample);
			v.push_back(otherSample);
			v.push_back(numEntitiesSample);
			clearSample.Save(v, "/home/arild/WallMon/scripts/visualization_engine.dat", true);
		}
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

void BoidsApp::_SetupAndPopulateScenes()
{
	float w = _wallView->GetTotalPixelWidth();
	float h = _wallView->GetTotalPixelHeight();
	float offsetH = h * 0.13;
	float offsetW = w * 0.05;

	_tableScene = new Scene(0, 0, w*0.5, h, 100, 100);
	_boidScene = new Scene(w*0.45 + offsetW, offsetH, w*0.5 - offsetW*2, h - offsetH*1.5, 100, 100);
//	_controlPanelScene = new Scene(w*0.9, h*0.1, w*0.1, h*0.8, 100, 100);
	_boidScene = new Scene(w*0.5 + offsetW, offsetH, w*0.5 - offsetW*2, h - offsetH*1.5, 100, 100);

	Scene::AddScene(_boidScene);
	BoidAxis *axis = new BoidAxis();
	axis->Set(0, 100, 25);
	_boidScene->AddEntity(axis);

	Scene::AddScene(_tableScene);
	Scene::current = _tableScene;
	_table = new Table(_wallView->IsLowerLeft());
	_tableScene->AddEntity(_table);

//	Scene::AddScene(_controlPanelScene);
//	_controlPanelScene->AddEntity(new ControlPanel());
}

void BoidsApp::_HandleTouchEvents()
{
	_eventSystem->PollEvents();
	while (_eventSystem->eventQueue->GetSize() > 0) {
		EventQueueItem item = _eventSystem->eventQueue->Pop();
		Entity *entity = item.get<0>();
		TT_touch_state_t event = item.get<1>();
		entity->HandleHit(event);

		if (event.visualizeOnly) {
			_VisualizeShoutEvent(event.loc.x, event.loc.y);
			continue;
		}
	}
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
}


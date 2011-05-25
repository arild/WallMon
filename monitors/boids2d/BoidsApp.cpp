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
#include "boost/foreach.hpp"
#include "Config.h" // FONT_PATH
#include "BoidsApp.h"
#include "Fps.h"
#include "Scene.h"
#include "LeftColumn.h"
#include "BoidAxis.h"
#include "ControlPanel.h"
#include "WallView.h"
#include "SdlMouseEventFetcher.h"
#include "TouchEvent.h"
#include "Button.h"

BoidsApp::BoidsApp(int screenWidth, int screenHeight, EventHandlerBase *eventHandler) :
	_screenWidth(screenWidth), _screenHeight(screenHeight), _eventHandler(eventHandler)
{
	_SetupScenes();
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
	((IEntity *) boid)->tx = startX;
	((IEntity *) boid)->ty = startY;
	_boidScene->entityList.push_back((IEntity *) boid);
}

void BoidsApp::RemoveBoid(Boid *Boid)
{
}

NameTagList *BoidsApp::CreateNameTagList()
{
	_nameTagList = new NameTagList();
	LeftColumn *nameDrawer = new LeftColumn(_nameTagList, new FTGLTextureFont(FONT_PATH));
	_leftColumnScene->entityList.push_back((IEntity *) nameDrawer);
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

	Queue<EventQueueItem> *touchEventQueue = NULL;
	if (_eventHandler != NULL) {
		touchEventQueue = _eventHandler->GetOutputQueue();
		_eventHandler->Start();
	}

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
		for (int i = 0; i < Scene::scenes.size(); i++)
			Scene::scenes[i]->Run();

		if (_eventHandler != NULL) {
			//_eventHandler->PollEvents();
			while (touchEventQueue->GetSize() > 0) {
				EventQueueItem item = touchEventQueue->Pop();
				Scene *scene = item.get<0>();
				TouchEvent event = item.get<1>();
				if (event.visualizeOnly)
					_VisualizeShoutEvent(event.realX, event.realY);
				else {
					vector<IEntity *> entities = scene->TestForEntityHits(event.x, event.y);
					LOG(INFO) << "Num entity hits: " << entities.size();
					if (entities.size() == 0)
						// No entity hits within scene
						continue;
					LOG(INFO) << "Entity Hit";
					entities[0]->HandleHit(event);
				}
			}
		}
		SDL_GL_SwapBuffers();

		Fps::fpsControl.OnLoop();
		char Buffer[255];
		sprintf(Buffer, "FPS: %d  |  Total Num Objects: %d", Fps::fpsControl.GetFps(), _CountNumObjects());
		SDL_WM_SetCaption(Buffer, Buffer);
	}
}

void BoidsApp::_SetupScenes()
{
	float w = TILE_SCREEN_HEIGHT;
	float h = TILE_SCREEN_HEIGHT;
	float s = 100;

	// Create scenes
	_leftColumnScene = new Scene(0, 0, (w * 2), h * 4, w * 2, h * 4);
	_boidScene = new Scene(w * 2, h / 2, w * 3 + w / 2, (h * 3), 100, 100);
	_controlPanelScene = new Scene(w * 6, h, w * 2, h * 2, 100, 100);

	// Save scenes
	Scene::scenes.push_back(_leftColumnScene);
	Scene::scenes.push_back(_boidScene);
	Scene::scenes.push_back(_controlPanelScene);

	// Populate scenes with various entities
	Scene::current = _boidScene;
	BoidAxis *axis = new BoidAxis();
	axis->Set(0, 100, 25);
	_boidScene->entityList.push_back((IEntity *) axis);

	Scene::current = _controlPanelScene;
	Button *button = new Button(10, 10, 20, 20);
	_controlPanelScene->entityList.push_back((IEntity *)button);

	button = new Button(40, 10, 20, 20);
	_controlPanelScene->entityList.push_back((IEntity *)button);

	button = new Button(70, 10, 20, 20);
	_controlPanelScene->entityList.push_back((IEntity *)button);
}

void BoidsApp::_VisualizeShoutEvent(float x, float y)
{
	float w = 50;
	float h = 50;
	x -= (w / 2);
	y -= (h / 2);

	glPushMatrix();

	glTranslatef(0, 0, 0);
	glScalef(1, 1, 1);
	glColor3f(255, 255, 0);

	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();
	glPopMatrix();

	SDL_GL_SwapBuffers();
}

int BoidsApp::_CountNumObjects()
{
	int numObjects = 0;
	BOOST_FOREACH(Scene *s, Scene::scenes)
	{
		numObjects += s->entityList.size();
	}
	return numObjects;
}

//void BoidsApp::_DrawBoidDescription()
//{
//	FTGLPixmapFont font(FONT_PATH);
//	//FTPoint point(R, BOID_Y/2);
//	FTPoint point(900, 400);
//	//font.CharMap(ft_encoding_latin_1);
//	if (font.Error())
//		LOG(INFO) << "Font failed";
//	font.FaceSize(25);
//	string text = "Utilization";
//	font.Render(text.c_str(), text.length(), point);
//}


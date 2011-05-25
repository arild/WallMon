#ifndef BOIDSAPP_H_
#define BOIDSAPP_H_

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <boost/thread.hpp>
#include <vector>
#include "Scene.h"
#include "Boid.h"
#include "NameTagList.h"
#include "EventHandlerBase.h"
#include "TouchEvent.h"

using namespace std;

class BoidsApp {
public:
	BoidsApp(int screenWidth, int screenHeight, Queue<TouchEventQueueItem> *touchEventQueue);
	virtual ~BoidsApp();
	void Start();
	void Stop();
	void CreateBoid(double startX, double startY, BoidSharedContext *ctx);
	void RemoveBoid(Boid *boid);
	NameTagList *CreateNameTagList();
	void SetDisplayArea(double x, double y, double width, double height);
private:
	boost::thread _thread;
	bool _running;
	SDL_Surface *_screen;
	int _screenWidth, _screenHeight;
	bool _updateOrtho;
	double _orthoLeft, _orthoRight, _orthoBottom, _orthoTop;
	Scene *_boidScene, *_leftColumnScene, *_controlPanelScene;
	NameTagList *_nameTagList;
	Queue<TouchEventQueueItem> *_touchEventQueue;

	void _InitSdlAndOpenGl();
	void _RenderForever();
	void _SetupScenes();
	void _HandleTouchEvents();
	void _VisualizeShoutEvent(float x, float y);
	int _CountTotalNumObjects();
};

class ButtonCallbacks {
public:
	static void BoidTailCallback();
};

#endif /* BOIDSAPP_H_ */

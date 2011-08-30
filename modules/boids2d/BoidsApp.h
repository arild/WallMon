#ifndef BOIDSAPP_H_
#define BOIDSAPP_H_

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <boost/thread.hpp>
#include <vector>
#include "Scene.h"
#include "Boid.h"
#include "EventSystemBase.h"
#include "TouchEvent.h"
#include "Table.h"

using namespace std;

class BoidsApp {
public:
	BoidsApp(int screenWidth, int screenHeight, EventSystemBase *eventSystem);
	virtual ~BoidsApp();
	void Start();
	void Stop();
	void CreateBoid(BoidSharedContext *ctx);
	Table *GetNameTable();

	void SetDisplayArea(double x, double y, double width, double height);
private:
	boost::thread _thread;
	bool _running;
	SDL_Surface *_screen;
	int _screenWidth, _screenHeight;
	bool _updateOrtho;
	double _orthoLeft, _orthoRight, _orthoBottom, _orthoTop;
	Scene *_boidScene, *_tableScene, *_controlPanelScene;
	Table *_table;
	EventSystemBase *_eventSystem;

	void _InitSdlAndOpenGl();
	void _RenderForever();
	void _SetupAndPopulateScenes();
	void _HandleTouchEvents();
	void _VisualizeShoutEvent(float x, float y);
	int _CountTotalNumObjects();
};

#endif /* BOIDSAPP_H_ */

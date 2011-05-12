#ifndef BOIDS_H_
#define BOIDS_H_

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <FTGL/ftgl.h>
#include <boost/thread.hpp>
#include <vector>
#include "Scene.h"
#include "Boid.h"
#include "NameTagList.h"

using namespace std;

class BoidsApp {
public:
	BoidsApp(int screenWidth, int screenHeight);
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
	Scene *_boidScene, *_nameDrawerScene;
	vector<Scene *> _scenes;
	FTFont *_font;
	NameTagList *_nameTagList;
	void _InitSdlAndOpenGl();
	void _RenderForever();
	void _DrawAxis();
	void _DrawBoidDescription();
};

#endif /* BOIDS_H_ */

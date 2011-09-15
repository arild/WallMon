#ifndef TABLEITEM_H_
#define TABLEITEM_H_

#include <vector>
#include <boost/thread/mutex.hpp>
#include "Entity.h"
#include "BoidSharedContext.h"

using namespace std;

class TableItem {
public:
	string key;
	int r, g, b;
	int highlightNumber;
	float _score;
	string _procName, _hostName, _pid, _user, _time, _numThreads;
	TableItem(string key);
	void AddBoid(BoidSharedContext *subItem);
	vector<BoidSharedContext *> GetBoids();

	float GetScore();
	float SetScore(float score);
	void SetProcName(string procName);
	string GetProcName();
	void SetHostName(string hostName);
	string GetHostName();
	void SetPid(string pid);
	string GetPid();
	void SetUser(string user);
	string GetUser();
	void SetStartTime(string time);
	string GetStartTime();
	void SetNumThreads(string numThreads);
	string GetNumThreads();

private:
	boost::mutex _mutex;
	vector<BoidSharedContext *> _boids;
};

#endif /* TABLEITEM_H_ */

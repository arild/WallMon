/*
 * StatIo.h
 *
 *  Created on: Oct 31, 2011
 *      Author: arild
 */

#ifndef STATIO_H_
#define STATIO_H_

#include <vector>
#include <functional>
#include <numeric>
#include <cmath>
#include <string>
#include "Stat.h"

using namespace std;

template<class T>
class StatIo : public Stat<T> {
public:
	StatIo(string prefix = "no prefix") {_prefix = prefix;}
	bool Save(string filePath="");
	void PrintToScreen();
private:
	string _prefix;
};

template<class T>
bool StatIo<T>::Save(string filePath)
{
	return true;
}

template<class T>
void StatIo<T>::PrintToScreen()
{
	cout << "*** Sample data for: " << _prefix << " ***";
	cout << "----------------------------" << endl;
	cout << "sum total: " << SumTotal() << endl;
	cout << "avg      : " << SumAvg() << endl;
	cout << "avg var  : " << VarianceAvg() << endl;
	cout << "sd msec  : " << StandardDeviationAvg() << endl;
	cout << "sum min  : " << SumMin() << endl;
	cout << "sum max  : " << SumMax() << endl;
	cout << "----------------------------" << endl;
}



#endif /* STATIO_H_ */

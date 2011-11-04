/**
 * Data container with support for common statistical operations, such as
 * sum, average, variance and standard deviation. The abstraction is centered
 * around having several independent samples for the same scenario. Then several
 * operations can be applied on this set of samples, for example, Sum() computes
 * takes the sum of all samples into account, and computes the average (sum) of
 * these sums.
 */

#ifndef STAT_H_
#define STAT_H_

#include <vector>
#include <functional>
#include <numeric>
#include <cmath>
#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <glog/logging.h>
#include "System.h"

using namespace std;

template<class T>
class Stat {
public:
	Stat(string prefix="no prefix") {_prefix=prefix;}
	void Start() {_startTimeMsec = System::GetTimeInMsec();}
	void Sample() {Add(System::GetTimeInMsec() - _startTimeMsec);}
	void Add(T value) {Add(value, NumSamples() - 1);} // Assumes last sample
	void Add(T value, int sampleIdx) {_samples[sampleIdx].push_back(value);}
	T Get(int sampleIdx, int valueIdx) {return _samples[sampleIdx][valueIdx];} // returns a specified value from a specified sample
	void NewSample() {vector<T> s; _samples.push_back(s);}
	int NumSamples() {return _samples.size();}
	int NumSamples(int idx) {return _samples[idx].size();}
	int NumSamplesTotal();
	T Sum(int idx);
	T SumAvg();
	T SumTotal();
	T SumMin();
	T SumMax();
	T Avg(int idx);
	T AvgAvg();
	T Variance(int idx);
	T VarianceAvg();
	T StandardDeviation(int idx);
	T StandardDeviationAvg();
	T MinVal(int idx);
	T MinVal();
	T MaxVal(int idx);
	T MaxVal();
	string GetPrefix() {return _prefix;}
	bool Save(vector<Stat<T> > data, string filename="tmp.dat", bool generateHorizontalAxis=false);
	void PrintToScreen();
private:
	vector<vector<T> > _samples;
	double _startTimeMsec;
	string _prefix;

};

template<class T>
int Stat<T>::NumSamplesTotal()
{
	int num = 0;
	for (int i = 0; i < NumSamples(); i++)
		num += NumSamples(i);
	return num;
}

/**
 * Average of the different sums for the sample sets
 */
template<class T>
T Stat<T>::SumAvg()
{
	return SumTotal() / (T)NumSamples();
}

/**
 * Sum for a specified sample
 */
template<class T>
T Stat<T>::Sum(int idx)
{
	T sum = 0;
	for (int i = 0; i < NumSamples(idx); i++)
		sum += Get(idx, i);
	return sum;
}

/**
 * Combined sum of all samples
 */
template<class T>
T Stat<T>::SumTotal()
{
	T sum = 0;
	for (int i = 0; i < NumSamples(); i++)
		sum += Sum(i);
	return sum;
}

/**
 * Returns the smallest sum across all samples
 */
template<class T>
T Stat<T>::SumMin()
{
	double min = Sum(0);
	for (int i = 1; i < NumSamples(); i++) {
		T tmp = Sum(i);
		if (tmp < min)
			min = tmp;
	}
	return min;
}

/**
 * Returns the smallest sum across all samples
 */
template<class T>
T Stat<T>::SumMax()
{
	double max = Sum(0);
	for (int i = 1; i < NumSamples(); i++) {
		T tmp = Sum(i);
		if (tmp > max)
			max = tmp;
	}
	return max;
}

/**
 * Average of specified sample
 */
template<class T>
T Stat<T>::Avg(int idx)
{
	return Sum(idx) / (T)NumSamples(idx);
}

/**
 * Average of all averages (this is not the same as average of all values in all samples)
 */
template<class T>
T Stat<T>::AvgAvg()
{
	T avg = 0;
	for (int i = 0; i < NumSamples(); i++)
		avg += Avg(i);
	return avg / (T)NumSamples();
}

/**
 * Variance for specified sample
 */
template<class T>
T Stat<T>::Variance(int idx)
{
	T avg = Avg(idx);
	T var = 0;
	for (int i = 0; i < NumSamples(idx); i++)
		var += pow(Get(idx, i) - avg, 2.0);
	return var / (T)NumSamples(idx);
}

/**
 * Average variance for all samples
 */
template<class T>
T Stat<T>::VarianceAvg()
{
	T var = 0;
	for (int i = 0; i < NumSamples(); i++)
		var += Variance(i);
	return var / (T)NumSamples();
}

/**
 * SD for a specified sample
 */
template<class T>
T Stat<T>::StandardDeviation(int idx)
{
	return sqrt(Variance(idx));
}

/**
 * Average SD for all samples
 */
template<class T>
T Stat<T>::StandardDeviationAvg()
{
	T sd = 0;
	for (int i = 0; i < NumSamples(); i++)
		sd += StandardDeviation(i);
	return sd / (T)NumSamples();
}

/**
 * Returns the smallest value in specified sample
 */
template<class T>
T Stat<T>::MinVal(int idx)
{
	double min = Get(idx, 0);
	for (int i = 1; i < NumSamples(idx); i++)
		if (Get(idx, i) < min)
			min = Get(idx, i);
	return min;
}

/**
 * Returns the smallest value across all samples
 */
template<class T>
T Stat<T>::MinVal()
{
	double min = MinVal(0);
	for (int i = 1; i < NumSamples(); i++) {
		T tmp = MinVal(i);
		if (tmp < min)
			min = tmp;
	}
	return min;
}

/**
 * Returns the smallest value in specified sample
 */
template<class T>
T Stat<T>::MaxVal(int idx)
{
	double max = Get(idx, 0);
	for (int i = 1; i < NumSamples(idx); i++)
		if (Get(idx, i) > max)
			max = Get(idx, i);
	return max;
}

/**
 * Returns the smallest value across all samples
 */
template<class T>
T Stat<T>::MaxVal()
{
	double max = MaxVal(0);
	for (int i = 1; i < NumSamples(); i++) {
		T tmp = MaxVal(i);
		if (tmp > max)
			max = tmp;
	}
	return max;
}

/**
 * Assumes that each sample only contains one sub-sample.
 */
template<class T>
bool Stat<T>::Save(vector<Stat<T> > data, string filename, bool generateHorizontalAxis)
{
	stringstream firstLine;
	firstLine << "# Sample number";
	for (int i = 0; i < data.size(); i++)
		firstLine << " | " << data[i].GetPrefix();

	FILE *f = fopen(filename.c_str(), "w");
	if (f == NULL)
		LOG(FATAL) << "failed creating file for gnuplot data";
	fprintf(f, "%s\n", firstLine.str().c_str());

	for (int i = 0; i < data[0].NumSamples(); i++) {
		stringstream values;
		for (int j = 0; j < data.size(); j++)
			values << data[j].Get(i, 0) << " ";
		if (generateHorizontalAxis)
			fprintf(f, "%d %s\n", i, values.str().c_str());
		else
			fprintf(f, "%s\n", values.str().c_str());
	}
	fclose(f);
	LOG(INFO) << "done writing to file";

}

template<class T>
void Stat<T>::PrintToScreen()
{
	cout << "*** Sample data for: " << _prefix << " ***" << endl;
	cout << "----------------------------" << endl;
	cout << "sum total: " << SumTotal() << endl;
	cout << "avg      : " << SumAvg() << endl;
	cout << "avg var  : " << VarianceAvg() << endl;
	cout << "sd msec  : " << StandardDeviationAvg() << endl;
	cout << "sum min  : " << SumMin() << endl;
	cout << "sum max  : " << SumMax() << endl;
	cout << "----------------------------" << endl;
}



#endif /* STAT_H_ */

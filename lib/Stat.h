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

class StorageFlags {
public:
	string filename;
	bool avg, variance, stdDev, min, max, generateLinearStartColumn;
	vector<int> *startColumn;
	StorageFlags(string filename_="tmp.dat", bool avg_=true, bool variance_=false, bool stdDev_=false, bool min_=false,
			bool max_=false, bool generateLinearStartColumn_=false, vector<int> *startColumn_=NULL) : filename(filename_), avg(avg_), variance(variance_),
			stdDev(stdDev_), min(min_), max(max_), generateLinearStartColumn(generateLinearStartColumn_), startColumn(startColumn_) {}
};


template<class T>
class Stat {
public:
	Stat(string prefix = "no prefix") {_prefix = prefix;}
	void Start() {_startTimeMsec = System::GetTimeInMsec();}
	void Sample() {Add(System::GetTimeInMsec() - _startTimeMsec);}
	void Add(T value) {Add(value, NumSamples() - 1);} // Assumes last sample
	void Add(T value, int sampleIdx) {_samples[sampleIdx].push_back(value);}
	T Get(int sampleIdx, int valueIdx) {return _samples[sampleIdx][valueIdx];} // returns a specified value from a specified sample
	void Remove(int sampleIdx) {_samples.erase(_samples.begin() + sampleIdx);}
	void NewSample() {vector<T> s; _samples.push_back(s);}
	int NumSamples() {return _samples.size();}
	int NumSamples(int idx) {return _samples[idx].size();}
	int NumSamplesTotal();
	T Sum(int idx);
	double SumAvg();
	T SumTotal();
	T SumMin();
	T SumMax();
	double Avg(int idx);
	double AvgAvg();
	double Variance(int idx);
	double VarianceAvg();
	double StandardDeviation(int idx);
	double StandardDeviationAvg();
	T MinVal(int idx);
	T MinVal();
	T MaxVal(int idx);
	T MaxVal();
	string GetPrefix() {return _prefix;}
	bool Save(StorageFlags flags);
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
double Stat<T>::SumAvg()
{
	return SumTotal() / (double) NumSamples();
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
double Stat<T>::Avg(int idx)
{
	return Sum(idx) / (double) NumSamples(idx);
}

/**
 * Average of all averages (this is not the same as average of all values in all samples)
 */
template<class T>
double Stat<T>::AvgAvg()
{
	T avg = 0;
	for (int i = 0; i < NumSamples(); i++)
		avg += Avg(i);
	return avg / (double) NumSamples();
}

/**
 * Variance for specified sample
 */
template<class T>
double Stat<T>::Variance(int idx)
{
	double avg = Avg(idx);
	double var = 0;
	for (int i = 0; i < NumSamples(idx); i++)
		var += pow((double)Get(idx, i) - avg, 2.0);
	return var / (double) NumSamples(idx);
}

/**
 * Average variance for all samples
 */
template<class T>
double Stat<T>::VarianceAvg()
{
	double var = 0;
	for (int i = 0; i < NumSamples(); i++)
		var += Variance(i);
	return var / (double) NumSamples();
}

/**
 * SD for a specified sample
 */
template<class T>
double Stat<T>::StandardDeviation(int idx)
{
	return sqrt(Variance(idx));
}

/**
 * Average SD for all samples
 */
template<class T>
double Stat<T>::StandardDeviationAvg()
{
	double sd = 0;
	for (int i = 0; i < NumSamples(); i++)
		sd += StandardDeviation(i);
	return sd / (double) NumSamples();
}

/**
 * Returns the smallest value in specified sample
 */
template<class T>
T Stat<T>::MinVal(int idx)
{
	T min = Get(idx, 0);
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
	T max = Get(idx, 0);
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
bool Stat<T>::Save(StorageFlags flags)
{
	if (flags.startColumn != NULL) {
		LOG(INFO) << flags.startColumn->size() << " | " << NumSamples();
		CHECK(flags.startColumn->size() == NumSamples());
	}

	stringstream info;
	info << "# Statistical data for: " << GetPrefix();

	info << "\n# Data format for populations:\n#";
	if (flags.generateLinearStartColumn)
		info << " auto generated |";
	if (flags.startColumn != NULL)
		info << " custom defined |";
	if (flags.avg)
		info << " average |";
	if (flags.variance)
		info << " variance |";
	if (flags.stdDev)
		info << " std dev |";
	if (flags.min)
		info << " min |";
	if (flags.max)
		info << " max |";
	string path = "/home/" + System::GetCurrentUser() + "/WallMon/modules/gnuplot/data/" + flags.filename;// + "_" + System::GetHostname();
	FILE *f = fopen(path.c_str(), "w");
	if (f == NULL)
		LOG(FATAL) << "failed creating file for gnuplot data";
	string s = info.str().erase(info.str().length() - 1, 1);
	fprintf(f, "%s\n", s.c_str());

	for (int i = 0; i < NumSamples(); i++) {
		stringstream line;
		if (flags.avg)
			line << Avg(i) << " ";
		if (flags.variance)
			line << Variance(i) << " ";
		if (flags.stdDev)
			line << StandardDeviation(i) << " ";
		if (flags.min)
			line << MinVal(i) << " ";
		if (flags.max)
			line << MaxVal(i) << " ";

		if (flags.generateLinearStartColumn)
			fprintf(f, "%d ", i);
		if (flags.startColumn != NULL)
			fprintf(f, "%d ", flags.startColumn->at(i));
		fprintf(f, " %s", line.str().c_str());
		fprintf(f, " # Based on %d samples\n", NumSamples(i));
	}
	fprintf(f, "# avg variance = %f  |  avg std dev = %f\n", VarianceAvg(), StandardDeviationAvg());
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

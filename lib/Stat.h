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

using namespace std;

template<class T>
class Stat {
public:
	Stat() {}
	void Add(T value) {_samples[_samples.size() - 1].push_back(value);}
	void Add(T value, int idx) {_samples[idx].push_back(value);}
	void NewSample() {vector<T> s; _samples.push_back(s);}//_samples.resize(_samples.size() + 1);}
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
	bool Save(string filePath="");
private:
	vector<vector<T> > _samples;
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
		sum += _samples[idx][i];
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
		var += pow(_samples[idx][i] - avg, 2.0);
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
	double min = _samples[idx][0];
	for (int i = 1; i < NumSamples(idx); i++)
		if (_samples[idx][i] < min)
			min = _samples[idx][i];
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
	double max = _samples[idx][0];
	for (int i = 1; i < NumSamples(idx); i++)
		if (_samples[idx][i] > max)
			max = _samples[idx][i];
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

template<class T>
bool Stat<T>::Save(string filePath)
{

}


#endif /* STAT_H_ */

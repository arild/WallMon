/*
 * Common.cpp
 *
 *  Created on: Jul 3, 2011
 *      Author: arild
 */

#include "Common.h"
#include <glog/logging.h>

//const SAMPLE_INTERVAL SAMPLE_INTERVALS[] = {{1,30}, {2,30}, {4,30}, {8,30}, {16,30}, {32,30}, {64,30}, {128,30}, {256,30}, {512,30}};
const SAMPLE_INTERVAL SAMPLE_INTERVALS[] = {{1,30}, {2,30}, {4,30}, {8,60}, {16,60}, {32,60}, {64,120}, {128,200}, {256,200}, {512,200}};
//const SAMPLE_INTERVAL SAMPLE_INTERVALS[] = {{1,3}, {2,3}, {4,3}, {8,3}, {16,3}, {32,3}, {64,3}, {128,3}, {256,3}, {512,3}};
//const SAMPLE_INTERVAL SAMPLE_INTERVALS[] = {{1,30}, {512, 300}};


const int NUM_SAMPLE_INTERVALS = sizeof(SAMPLE_INTERVALS) / sizeof(SAMPLE_INTERVAL);

int get_total_num_samples()
{
	int total = 0;
	for (int i = 0; i < NUM_SAMPLE_INTERVALS; i++)
		total += get_num_samples(i);
	return total;
}

int get_total_num_samples_exclude_warmup()
{
	return get_total_num_samples() - get_num_samples(0);
}

vector<int> get_sample_frequencies_in_hz()
{
	vector<int> v;
	for (int i = 0; i < NUM_SAMPLE_INTERVALS; i++)
		v.push_back(get_frequency_in_hz(i));
	return v;
}

vector<int> get_sample_frequencies_in_hz_exclude_warmup()
{
	vector<int> v = get_sample_frequencies_in_hz();
	v.erase(v.begin());
	return v;
}

vector<double> get_intervals_durations_in_msec()
{
	vector<double> v;
	for (int i = 0; i < NUM_SAMPLE_INTERVALS; i++)
		v.push_back(get_frequency_in_msec(i) * (double)get_num_samples(i));
	return v;
}

double get_frequency_in_msec(int idx)
{
	CHECK(idx >= 0 && idx < NUM_SAMPLE_INTERVALS);
	return (1000 / (double)SAMPLE_INTERVALS[idx].hz);
}

int get_frequency_in_hz(int idx)
{
	CHECK(idx >= 0 && idx < NUM_SAMPLE_INTERVALS);
	return SAMPLE_INTERVALS[idx].hz;
}

int get_num_samples(int idx)
{
	CHECK(idx >= 0 && idx < NUM_SAMPLE_INTERVALS);
	return SAMPLE_INTERVALS[idx].numSamples;
}


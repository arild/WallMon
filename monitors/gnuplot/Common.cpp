/*
 * Common.cpp
 *
 *  Created on: Jul 3, 2011
 *      Author: arild
 */

#include "Common.h"
#include <glog/logging.h>

//const SAMPLE_INTERVAL SAMPLE_INTERVALS[] = {{1, 2}, {100, 2}, {1, 2}, {100, 2}};//{{2,6}, {1,5}, {2,10}, {4,20}, {10,40}, {20,40}, {40,40}, {60,40}};//, {100, 100}};
//const SAMPLE_INTERVAL SAMPLE_INTERVALS[] = {{10, 20}, {1,10}, {2,10}, {4,20}, {8,40}, {16,40}, {32,40}, {64,40}, {128,80}};//, {200, 200}, {300, 300}, {500, 400}};
const SAMPLE_INTERVAL SAMPLE_INTERVALS[] = {{10, 20}, {16,20}, {32,20}, {64,50}, {128,100}, {256, 100}, {512, 200}};


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

int get_frequency_in_msec(int idx)
{
	CHECK(idx >= 0 && idx < NUM_SAMPLE_INTERVALS);
	return (1000 / SAMPLE_INTERVALS[idx].hz);
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


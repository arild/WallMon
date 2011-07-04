/*
 * Common.cpp
 *
 *  Created on: Jul 3, 2011
 *      Author: arild
 */

#include "Common.h"

const SAMPLE_INTERVAL SAMPLE_INTERVALS[] = {{100, 5}, {50, 5}, {20, 5}};
const int NUM_SAMPLE_INTERVALS = sizeof(SAMPLE_INTERVALS) / sizeof(SAMPLE_INTERVAL);

int get_total_num_samples()
{
	int total = 0;
	for (int i = 0; i < NUM_SAMPLE_INTERVALS; i++)
		total += SAMPLE_INTERVALS[i].numSamples;
	return total;
}

vector<int> get_sample_frequencies()
{
	vector<int> v;
	for (int i = 0; i < NUM_SAMPLE_INTERVALS; i++)
		v.push_back(SAMPLE_INTERVALS[i].sampleFrequency);
	return v;
}


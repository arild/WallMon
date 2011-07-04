/*
 * Handler.cpp
 *
 *  Created on: Feb 1, 2011
 *      Author: arild
 */

#include <glog/logging.h>
#include <boost/foreach.hpp>
#include <stdio.h>
#include <sstream>
#include "GnuplotHandler.h"
#include "Common.h"

int TOTAL_NUM_SAMPLES;
int NUM_SAMPLES = 0;
double totalDriftMsec = 0;
double totalSampleTimeMsec = 0;

int expectedSeqNum = 0;
int NUM_NODES = 1;

template <typename T>
vector<string> _ToString(vector<T> v)
{
	vector<string> ret;
	stringstream ss;
	for (int i = 0; i < v.size(); i++) {
		ss << v[i];
		ret.push_back(ss.str());
		LOG(INFO) << "converting: " << v[i] << " -> " << ss.str();
		ss.str("");
	}
	return ret;
}

void GnuplotHandler::OnInit(Context *ctx)
{
	ctx->key = "gnuplot";
	_plot.set_title("test");
	_plot.savetops("test_file");

	_plot.set_xlabel("Sample Frequency in Milliseconds");
	_plot.set_ylabel("Average CPU Utilization");

	TOTAL_NUM_SAMPLES = get_total_num_samples() * NUM_NODES;
	LOG(INFO) << "total num samples: " << TOTAL_NUM_SAMPLES;
}

void GnuplotHandler::OnStop()
{
}

void GnuplotHandler::Handle(WallmonMessage *msg)
{
	const char *data = msg->data().c_str();
	int length = msg->data().length();

	if (_message.ParseFromArray(data, length) == false)
		LOG(FATAL) << "Protocol buffer parsing failed: ";

	totalSampleTimeMsec += msg->sampletimemsec();
	totalDriftMsec += msg->scheduledriftmsec();
	for (int i = 0; i < _message.processmessage_size(); i++) {
		ProcessCollectorMessage::ProcessMessage *procMsg = _message.mutable_processmessage(i);
		if (procMsg->processname().compare(TARGET_PROCESS) != 0)
			continue;

		DataContainer::iterator it = _data.find(msg->hostname());
		vector<double> *v;
		if (it == _data.end()) {
			v = new vector<double> ;
			_data[msg->hostname()] = v;
		} else {
			v = it->second;
		}

		double cpuUtil = procMsg->usercpuutilization() + procMsg->usercpuutilization();
		v->push_back(cpuUtil);

		if (++NUM_SAMPLES == TOTAL_NUM_SAMPLES) {
			LOG(INFO) << "sampling done, generating charts...";
			_GenerateGraph();
			exit(0);
		}
	}
}

void GnuplotHandler::_GenerateGraph()
{
	//			if (_yServer.size() > 0)
	//				_plot.plot_xy(_x, _yServer, "WallMon Server");
	LOG(INFO) << "Total drift msec: " << totalDriftMsec;
	LOG(INFO) << "Total sample time msec: " << totalSampleTimeMsec;
	LOG(INFO) << "Average sample time msec: " << totalSampleTimeMsec / NUM_SAMPLES;
	vector<int> xValues = get_sample_frequencies();
	vector<double> yValues = _ComputeAverage();

	vector<string> x = _ToString<int>(xValues);
	vector<string> y = _ToString<double>(yValues);

	if (x.size() != y.size())
		LOG(FATAL) << "num elements on x and y axis do not match";

	vector< vector<string> > xy;
	xy.push_back(x);
	xy.push_back(y);
	_SaveToFile(xy);
	_plot.cmd("plot \"tmp.dat\" using 1:2:(3) with boxes fs solid 1 title 'system'");
	LOG(INFO) << "chart generated, exiting";

}

vector<double> GnuplotHandler::_ComputeAverage()
{
	vector<vector<double> *> results;
	vector<double> average;
	BOOST_FOREACH (DataContainer::value_type &item, _data)
					results.push_back(item.second);

	// Validate that there are the same amount of samples in the results
	BOOST_FOREACH (vector<double> *item, results)
		if (item->size() != TOTAL_NUM_SAMPLES)
			LOG(ERROR) << "number of samples do not match: expected="
					<< TOTAL_NUM_SAMPLES << " | actual: " << item->size();

	// Go through the different sample intervals
	for (int i = 0, resultIndex = 0; i < NUM_SAMPLE_INTERVALS; i++) {

		// Go through the different results within a sample interval
		int numSamples = SAMPLE_INTERVALS[i].numSamples;
		for (int j = 0; j < NUM_NODES; j++) {

			// Within each result, compute the average value for given sample interval
			double sum = 0;
			for (int k = 0; k < numSamples; k++) {
				sum += (*results[j])[resultIndex + k];
			}
			average.push_back(sum / (double) numSamples);
		}

		resultIndex += numSamples;
	}
	return average;
}

void GnuplotHandler::_SaveToFile(vector< vector<string> > xy)
{
	FILE *f = fopen("tmp.dat", "w");
	LOG(INFO) << "xy size: " << xy.size();
	LOG(INFO) << "xy[0] size: " << xy[0].size();
	LOG(INFO) << "xy[1] size: " << xy[1].size();
	if (f == NULL)
		LOG(FATAL) << "failed creating file for gnuplot data";

	for (int i = 0; i < xy[0].size(); i++) {
		for (int j = 0; j < xy.size(); j++) {

			fprintf(f, "%s ", (xy[j])[i].c_str());
			LOG(INFO) << "writing: " << (xy[j])[i].c_str();
		}
		fprintf(f, "\n");
	}
	fclose(f);
	LOG(INFO) << "done writing to file";
}


// class factories - needed to bootstrap object orientation with dlfcn.h
extern "C" GnuplotHandler *create_handler()
{
	return new GnuplotHandler;
}

extern "C" void destroy_handler(GnuplotHandler *p)
{
	delete p;
}





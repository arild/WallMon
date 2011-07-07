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

double totalScheduleDurationMsec = 0;
double totalScheduleDriftMsec = 0;

double minSampleDurationMsec = 999999;
double maxSampleDurationMsec = 0;
double totalSampleDurationMsec = 0;

double minMemory = 999999;
double maxMemory = 0;
double totalMemory = 0;

unsigned int minNetOutBytes = 999999;
unsigned int maxNetOutBytes = 0;
unsigned int totalNetOutBytes = 0;


int expectedSeqNum = 0;
int NUM_NODES = 0;


template <typename T>
vector<string> _ToString(vector<T> v)
{
	vector<string> ret;
	stringstream ss;
	for (int i = 0; i < v.size(); i++) {
		ss << v[i];
		ret.push_back(ss.str());
		ss.str("");
	}
	return ret;
}

void GnuplotHandler::OnInit(Context *ctx)
{
	ctx->key = "gnuplot";
	_expectedNumSamplesPerNode = get_total_num_samples();
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

	if (msg->has_scheduledriftmsec())
		totalScheduleDriftMsec += msg->scheduledriftmsec();
	if (msg->has_scheduledurationmsec())
		totalScheduleDurationMsec += msg->scheduledurationmsec();
	totalSampleDurationMsec += msg->sampledurationmsec();
	minSampleDurationMsec = min(minSampleDurationMsec, msg->sampledurationmsec());
	maxSampleDurationMsec = max(maxSampleDurationMsec, msg->sampledurationmsec());


	LOG_EVERY_N(INFO, 100) << "min: " << minSampleDurationMsec << " | max: " << maxSampleDurationMsec;


	for (int i = 0; i < _message.processmessage_size(); i++) {
		ProcMsg *procMsg = _message.mutable_processmessage(i);
		if (procMsg->processname().compare(TARGET_PROCESS) != 0)
			continue;

		DataContainer::iterator it = _data.find(msg->hostname());
		vector<ProcMsg> *v;
		if (it == _data.end()) {
			v = new vector<ProcMsg>;
			NUM_NODES += 1;
			TOTAL_NUM_SAMPLES = _expectedNumSamplesPerNode * NUM_NODES;
			_data[msg->hostname()] = v;
		} else {
			v = it->second;
		}

		if (v->size() == _expectedNumSamplesPerNode) {
			LOG(WARNING) << "too many samples from: " << msg->hostname() << ", discarding sample";
			continue;
		}

		v->push_back(*procMsg);

		minMemory = min(minMemory, procMsg->memoryutilization());
		maxMemory = max(maxMemory, procMsg->memoryutilization());
		totalMemory += (double)procMsg->memoryutilization();

		minNetOutBytes = min(minNetOutBytes, procMsg->networkoutbytes());
		maxNetOutBytes = max(maxNetOutBytes, procMsg->networkoutbytes());
		totalNetOutBytes += procMsg->networkoutbytes();

		LOG_EVERY_N(INFO, 10) << "Memory: " << procMsg->memoryutilization();
		NUM_SAMPLES += 1;
		LOG_EVERY_N(INFO, 500) << "Sampling status: " << NUM_SAMPLES<< "/" << TOTAL_NUM_SAMPLES << " | " << msg->hostname();
		if (NUM_SAMPLES == TOTAL_NUM_SAMPLES) {

			// Discard the warm-up interval
			BOOST_FOREACH (DataContainer::value_type &item, _data) {
				vector<ProcMsg> *v = item.second;
				v->erase(v->begin(), v->begin() + get_num_samples(0));
			}

			LOG(INFO) << "----------------------------------";
			LOG(INFO) << "all samples received (" << NUM_SAMPLES << "), generating charts...";
			LOG(INFO) << "Average schedule duration msec: " << totalScheduleDurationMsec / (double)(NUM_NODES * NUM_SAMPLES);
			LOG(INFO) << "Average total schedule drift msec: " << totalScheduleDriftMsec / (double)NUM_NODES;
			LOG(INFO) << "----------------------------------";
			LOG(INFO) << "Min sample duration msec: " << minSampleDurationMsec;
			LOG(INFO) << "Max sample duration msec: " << maxSampleDurationMsec;
			LOG(INFO) << "Average sample duration msec: " << totalSampleDurationMsec / (double)(NUM_NODES * NUM_SAMPLES);
			LOG(INFO) << "Average total sample duration msec: " << totalSampleDurationMsec / (double)NUM_NODES;
			LOG(INFO) << "----------------------------------";
			LOG(INFO) << "Min memory: " << minMemory;
			LOG(INFO) << "Max memory: " << maxMemory;
			LOG(INFO) << "Avg memory: " << totalMemory / (double)(NUM_NODES * NUM_SAMPLES);
			LOG(INFO) << "----------------------------------";
			LOG(INFO) << "Min net out: " << minNetOutBytes;
			LOG(INFO) << "Max net out: " << maxNetOutBytes;
			LOG(INFO) << "Avg net out: " << totalNetOutBytes / (double)(NUM_NODES * NUM_SAMPLES);
			LOG(INFO) << "----------------------------------";


			_GenerateCpuChart();
			LOG(INFO) << "charts generated, exiting";
			exit(0);
		}
	}
}

void GnuplotHandler::_GenerateCpuChart()
{
	// Extract user- and system-cpu from map of vectors to vector of vectors
	vector<vector<double> *> userCpu, systemCpu;
	BOOST_FOREACH (DataContainer::value_type &item, _data) {
		vector<ProcMsg> *procMsgVector = item.second;
		userCpu.push_back(new vector<double>);
		systemCpu.push_back(new vector<double>);

		BOOST_FOREACH (ProcMsg procMsg, *procMsgVector) {
			userCpu.back()->push_back(procMsg.usercpuutilization());
			systemCpu.back()->push_back(procMsg.systemcpuutilization());
		}
	}

	// Validate that there are the same amount of samples in the results
	BOOST_FOREACH (vector<double> *item, systemCpu)
		if (item->size() != get_total_num_samples_exclude_warmup())
			LOG(FATAL) << "number of samples do not match: expected="
					<< get_total_num_samples_exclude_warmup() << " | actual: " << item->size();

	vector<int> xValues = get_sample_frequencies_in_hz_exclude_warmup();
	vector<double> yValues1 = _ComputeAverage(systemCpu);
	vector<double> yValues2 = _ComputeAverage(userCpu);

	vector<string> x = _ToString<int>(xValues);
	vector<string> y1 = _ToString<double>(yValues1);
	vector<string> y2 = _ToString<double>(yValues2);


	if (x.size() != y1.size()) {
		LOG(FATAL) << "num elements on x and y axis do not match: " << x.size() << " | " << y1.size();
	}
	vector< vector<string> > xy;
	xy.push_back(x);
	xy.push_back(y1);
	xy.push_back(y2);
	_SaveToFile(xy, "frequency  system  user");

	_plot.reset_all();
	_plot << "set terminal postscript enhanced color";
	_plot << "set key invert left top";
	_plot << "set grid noxtics nomxtics ytics nomytics noztics nomztics nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics";
	_plot << "set output \"cpu_chart.ps\"";
	_plot << "set boxwidth 0.75 absolute";
	_plot << "set style fill solid 1.00 border -1";
	_plot << "set style histogram rowstacked";
	_plot << "set style data histograms";
	_plot << "set ylabel \"Average CPU Utilization\"";
	_plot << "set xlabel \"Hz (Number of samples per second)\"";
	_plot << "plot \"tmp.dat\" using 2 t \"system-level\", '' using 3:xtic(1) t \"user-level\"";
	System::RunCommand("ps2pdf cpu_chart.ps");
	System::RunCommand("rm cpu_chart.ps");
	LOG(INFO) << "cpu chart generated";
}

vector<double> GnuplotHandler::_ComputeAverage(vector< vector<double> *> samples)
{
	vector<double> average;
	// Go through the different sample intervals
	// i starts at 1 in order to skip warm-up interval
	for (int i = 1, resultIndex = 0; i < NUM_SAMPLE_INTERVALS; i++) {

		// Go through each sample (one for each node) within current sample interval
		int numSamplesForInterval = get_num_samples(i);
		double sumAllNodes = 0;
		for (int j = 0; j < NUM_NODES; j++) {

			// Within each sample, compute the average value for given sample interval
			for (int k = 0; k < numSamplesForInterval; k++) {
				sumAllNodes += (*samples[j])[resultIndex + k];
			}
		}
		average.push_back(sumAllNodes / (double)(numSamplesForInterval * NUM_NODES));
		resultIndex += numSamplesForInterval;
	}
	return average;
}

void GnuplotHandler::_SaveToFile(vector< vector<string> > xy, string firstLineComment)
{
	FILE *f = fopen("tmp.dat", "w");
	fprintf(f, "# %s\n", firstLineComment.c_str());
	if (f == NULL)
		LOG(FATAL) << "failed creating file for gnuplot data";

	for (int i = 0; i < xy[0].size(); i++) {
		for (int j = 0; j < xy.size(); j++) {
			fprintf(f, "%s ", (xy[j])[i].c_str());
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





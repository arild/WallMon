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

#define TARGET_PROCESS		"wallmons"

int EXPECTED_TOTAL_NUM_SAMPLES;
int CURRENT_NUM_SAMPLES = 0;
int currentSampleFrequency = -1;


//double minScheduleDurationMsec;
//double maxScheduleDurationMsec;
double totalScheduleDurationMsec;
double totalScheduleDriftMsec;

double minSampleDurationMsec;
double maxSampleDurationMsec;
double totalSampleDurationMsec;

double minMemory;
double maxMemory;
double totalMemory;

unsigned int minNetBytes;
unsigned int maxNetBytes;
unsigned int totalNetBytes;

int NUM_NODES = 0;

void reset_statistics()
{

	totalScheduleDurationMsec = 0;
	totalScheduleDriftMsec = 0;

	minSampleDurationMsec = 999999;
	maxSampleDurationMsec = 0;
	totalSampleDurationMsec = 0;

	minMemory = 999999;
	maxMemory = 0;
	totalMemory = 0;

	minNetBytes = 999999;
	maxNetBytes = 0;
	totalNetBytes = 0;
}

void print_statistics()
{
	LOG(INFO) << "";
	LOG(INFO) << "***** Statistics for session: *****";
	LOG(INFO) << "----------------------------------";
	LOG(INFO) << "Avg schedule duration msec: " << totalScheduleDurationMsec / (double)(CURRENT_NUM_SAMPLES);
	LOG(INFO) << "Avg total schedule drift msec: " << totalScheduleDriftMsec / (double)NUM_NODES;
	LOG(INFO) << "----------------------------------";
	LOG(INFO) << "Min sample duration msec: " << minSampleDurationMsec;
	LOG(INFO) << "Max sample duration msec: " << maxSampleDurationMsec;
	LOG(INFO) << "Average sample duration msec: " << totalSampleDurationMsec / (double)(CURRENT_NUM_SAMPLES);
	LOG(INFO) << "Average total sample duration msec: " << totalSampleDurationMsec / (double)NUM_NODES;
	LOG(INFO) << "----------------------------------";
	LOG(INFO) << "Min memory: " << minMemory;
	LOG(INFO) << "Max memory: " << maxMemory;
	LOG(INFO) << "Avg memory: " << totalMemory / (double)(CURRENT_NUM_SAMPLES);
	LOG(INFO) << "----------------------------------";
	LOG(INFO) << "Min net out: " << minNetBytes;
	LOG(INFO) << "Max net out: " << maxNetBytes;
	LOG(INFO) << "Avg net out: " << totalNetBytes / (double)(CURRENT_NUM_SAMPLES);
	LOG(INFO) << "----------------------------------";
	LOG(INFO) << "";
}

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

/**
 *
 */
template <typename T>
vector<T> _ComputeAverage(vector< vector<T> > &samples)
{
	vector<T> average;
	// Go through the different sample intervals
	// i starts at 1 in order to skip warm-up interval
	for (int i = 1, resultIndex = 0; i < NUM_SAMPLE_INTERVALS; i++) {

		// Go through each sample (one for each node) within current sample interval
		int numSamplesForInterval = get_num_samples(i);
		double sumAllNodes = 0;
		for (int j = 0; j < NUM_NODES; j++) {

			// Within each sample, compute the average value for given sample interval
			for (int k = 0; k < numSamplesForInterval; k++) {
				sumAllNodes += samples[j][resultIndex + k];
			}
		}
		T avg = (T)(sumAllNodes / (T)(numSamplesForInterval * NUM_NODES));
		average.push_back(avg);
		resultIndex += numSamplesForInterval;
	}
	return average;
}

template <typename T>
vector<T> _ComputeSum(vector< vector<T> > &samples)
{
	vector<T> sum;
	// Go through the different sample intervals
	// i starts at 1 in order to skip warm-up interval
	for (int i = 1, resultIndex = 0; i < NUM_SAMPLE_INTERVALS; i++) {

		// Go through each sample (one for each node) within current sample interval
		int numSamplesForInterval = get_num_samples(i);
		double sumAllNodes = 0;
		for (int j = 0; j < NUM_NODES; j++) {

			// Within each sample, compute the average value for given sample interval
			for (int k = 0; k < numSamplesForInterval; k++) {
				sumAllNodes += samples[j][resultIndex + k];
			}
		}
		sum.push_back(sumAllNodes);
		resultIndex += numSamplesForInterval;
	}
	return sum;
}


template <typename T>
vector<T> _AverageToPerSecond(vector<T> &average)
{
	vector<T> averagePerSec(average.size());
	vector<int> hz = get_sample_frequencies_in_hz_exclude_warmup();
	for (int i = 0; i < hz.size(); i++) {
		averagePerSec[i] = average[i] * hz[i];
	}
	return averagePerSec;
}

void GnuplotHandler::OnInit(Context *ctx)
{
	ctx->key = "gnuplot";
	_expectedNumSamplesPerNode = get_total_num_samples();
	reset_statistics();
	_localSampler = new LocalSampler();
	_localSampler->Start();
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
	if (msg->has_hz())
		;//LOG(INFO) << "Current Hz: " << msg->hz();

	minSampleDurationMsec = min(minSampleDurationMsec, msg->sampledurationmsec());
	maxSampleDurationMsec = max(maxSampleDurationMsec, msg->sampledurationmsec());
	totalSampleDurationMsec += msg->sampledurationmsec();

	for (int i = 0; i < _message.processmessage_size(); i++) {
		ProcMsg *procMsg = _message.mutable_processmessage(i);
		if (procMsg->processname().compare(TARGET_PROCESS) != 0)
			continue;

		DataContainer::iterator it = _data.find(msg->hostname());
		vector<ProcMsg> *v;
		if (it == _data.end()) {
			v = new vector<ProcMsg>;
			NUM_NODES += 1;
			EXPECTED_TOTAL_NUM_SAMPLES = _expectedNumSamplesPerNode * NUM_NODES;
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


		unsigned int net = procMsg->networkoutbytes() + procMsg->networkinbytes();
//		LOG(INFO) << "External: " << net << " | Internal: " << msg->networkmessagesizebytes();
		minNetBytes = min(minNetBytes, net);
		maxNetBytes = max(maxNetBytes, net);
		totalNetBytes += net;

		CURRENT_NUM_SAMPLES += 1;
		LOG_EVERY_N(INFO, 500) << "Sampling status: " << CURRENT_NUM_SAMPLES<< "/" << EXPECTED_TOTAL_NUM_SAMPLES << " | " << msg->hostname();
		if (CURRENT_NUM_SAMPLES == EXPECTED_TOTAL_NUM_SAMPLES) {
			LOG(INFO) << "all samples received (" << CURRENT_NUM_SAMPLES << "), generating charts...";

			sleep(2);
			_localSampler->Stop();
			print_statistics();
			_GenerateCharts();

			LOG(INFO) << "charts generated, exiting";
			exit(0);
		}
	}

}

void GnuplotHandler::_GenerateCharts()
{
	// Discard the warm-up interval
	BOOST_FOREACH (DataContainer::value_type &item, _data) {
		vector<ProcMsg> *v = item.second;
		v->erase(v->begin(), v->begin() + get_num_samples(0));
	}
	vector<double> serverBandwidthMb = _localSampler->GetNetworkUsageInMb();
	serverBandwidthMb.erase(serverBandwidthMb.begin());

	// Extract user- and system-cpu from map of vectors to vector of vectors
	int numSamples = get_total_num_samples_exclude_warmup();
	vector<vector<double> > userCpu(NUM_NODES), systemCpu(NUM_NODES), memory(NUM_NODES);
	vector<vector<double> > network(NUM_NODES);

	int i = 0;
	BOOST_FOREACH (DataContainer::value_type &item, _data) {
		vector<ProcMsg> *procMsgVector = item.second;
		userCpu[i].resize(numSamples);
		systemCpu[i].resize(numSamples);
		memory[i].resize(numSamples);
		network[i].resize(numSamples);
		int j = 0;
		BOOST_FOREACH (ProcMsg procMsg, *procMsgVector) {
			userCpu[i][j] = procMsg.usercpuutilization();
			systemCpu[i][j] = procMsg.systemcpuutilization();
			memory[i][j] = procMsg.memoryutilization();
			unsigned int networkBytes = procMsg.networkoutbytes() + procMsg.networkinbytes();
			network[i][j] = networkBytes / (double)(1<<20);
			j += 1;
		}
		i += 1;
	}

	_GenerateCpuChart(systemCpu, userCpu);
	_GenerateMemoryChart(memory);
	_GenerateServerNetworkChart(network, serverBandwidthMb);
}

void GnuplotHandler::_GenerateCpuChart(vector<vector<double> > &systemCpu, vector<vector<double> > &userCpu)
{
	vector<int> xValues = get_sample_frequencies_in_hz_exclude_warmup();

	vector<double> yValues1 = _ComputeAverage<double>(systemCpu);
	vector<double> yValues2 = _ComputeAverage<double>(userCpu);

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
	_SaveToFile(xy, "frequency  kernel  user");

	_plot.reset_all();
	_plot << "set terminal postscript enhanced color";
	_plot << "set key invert left top";
	_plot << "set grid noxtics nomxtics ytics nomytics noztics nomztics nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics";
	_plot << "set output \"cpu_chart.ps\"";
	_plot << "set boxwidth 0.75 absolute";
	_plot << "set style fill solid 1.00 border -1";
	_plot << "set style histogram rowstacked";
	_plot << "set style data histograms";
	_plot << "set ylabel \"Average CPU Consumption in Percent\"";
	_plot << "set xlabel \"Hz (Number of samples per second)\"";
	_plot << "plot \"tmp.dat\" using 2 t \"kernel-level\", '' using 3:xtic(1) t \"user-level\"";
	System::RunCommand((string)"ps2pdf cpu_chart.ps");
	System::RunCommand((string)"rm cpu_chart.ps");
	LOG(INFO) << "cpu chart generated";
}

void GnuplotHandler::_GenerateMemoryChart(vector<vector<double> > & memory)
{
	vector<int> xValues = get_sample_frequencies_in_hz_exclude_warmup();
	vector<double> yValues= _ComputeAverage<double>(memory);

	vector<string> x = _ToString<int>(xValues);
	vector<string> y = _ToString<double>(yValues);

	vector< vector<string> > xy;
	xy.push_back(x);
	xy.push_back(y);
	_SaveToFile(xy, "Hz  memory_utilization");

	_plot.reset_all();
	_plot << "set terminal postscript enhanced color";
	_plot << "set grid noxtics nomxtics ytics nomytics noztics nomztics nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics";
	_plot << "set output \"memory_chart.ps\"";
	_plot << "set boxwidth 0.75 absolute";
	_plot << "set style fill solid 1.00 border -1";
	_plot << "set style data histogram";
	_plot << "set ylabel \"Average Memory Utilization in Percent\"";
	_plot << "set xlabel \"Hz (Number of samples per second)\"";
	_plot << "plot \"tmp.dat\" usi 2:xtic(1) w boxes title \"\"";
	System::RunCommand((string)"ps2pdf memory_chart.ps");
	System::RunCommand((string)"rm memory_chart.ps");
	LOG(INFO) << "memory chart generated";
}

void GnuplotHandler::_GenerateNetworkChart(vector<vector<unsigned int> > & network)
{
	vector<int> xValues = get_sample_frequencies_in_hz_exclude_warmup();
	vector<unsigned int> yValues = _ComputeAverage<unsigned int>(network);
	yValues = _AverageToPerSecond(yValues);

	vector<string> x = _ToString<int>(xValues);
	vector<string> y = _ToString<unsigned int>(yValues);

	vector< vector<string> > xy;
	xy.push_back(x);
	xy.push_back(y);
	_SaveToFile(xy, "Hz  network_in_kb");

	_plot.reset_all();
	_plot << "set terminal postscript enhanced color";
	_plot << "set grid noxtics nomxtics ytics nomytics noztics nomztics nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics";
	_plot << "set output \"network_chart.ps\"";
	_plot << "set boxwidth 0.75 absolute";
	_plot << "set style fill solid 1.00 border -1";
	_plot << "set style data histogram";
	_plot << "set ylabel \"Average Network Consumption in KB\"";
	_plot << "set xlabel \"Hz (Number of samples per second)\"";
	_plot << "plot \"tmp.dat\" usi 2:xtic(1) w boxes title \"\"";
	System::RunCommand((string)"ps2pdf network_chart.ps");
	System::RunCommand((string)"rm network_chart.ps");
	LOG(INFO) << "network chart generated";

}

void GnuplotHandler::_GenerateServerNetworkChart(vector<vector<double > > &networkCollector, vector<double> &networkHandler)
{
	vector<string> x = _ToString<int>(get_sample_frequencies_in_hz_exclude_warmup());
	vector<string> y1 = _ToString<double>(networkHandler);
	vector<string> y2 = _ToString<double>(_ComputeSum<double>(networkCollector));

	if (y1.size() != x.size()) {
		LOG(WARNING) << "num elements in handler sampled data do not match: " << y1.size() << " | " << x.size();
	}


	vector< vector<string> > xy;
	xy.push_back(x);
	xy.push_back(y1);
	xy.push_back(y2);
	_SaveToFile(xy, "Hz actual_usage ideal_usage");

	_plot.reset_all();
	_plot << "set terminal postscript enhanced color";
	_plot << "set grid noxtics nomxtics ytics nomytics noztics nomztics nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics";
	_plot << "set key left top";
	_plot << "set output \"server_network_chart.ps\"";
	_plot << "set boxwidth 0.75 absolute";
	_plot << "set style fill solid 1.00 border";
	_plot << "set style data histogram";
	_plot << "set style histogram cluster gap 1";
	_plot << "set ylabel \"Bandwidth (Mbytes/s)\"";
	_plot << "set xlabel \"Hz (Number of samples per second)\"";
	_plot << "plot \"tmp.dat\" using 2:xtic(1) t \"actual usage\", '' using 3 t \"expected usage\"";

	System::RunCommand((string)"ps2pdf server_network_chart.ps");
	System::RunCommand((string)"rm server_network_chart.ps");
	LOG(INFO) << "network chart generated";
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













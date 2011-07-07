
#include <vector>

using namespace std;

#define KEY					"gnuplot"
#define MESSAGE_BUF_SIZE	4096
#define TARGET_PROCESS		"wallmond"


typedef struct
{
    int hz;
    int numSamples;

} SAMPLE_INTERVAL;

extern const SAMPLE_INTERVAL SAMPLE_INTERVALS[];
extern const int NUM_SAMPLE_INTERVALS;

int get_total_num_samples();
int get_total_num_samples_exclude_warmup();
vector<int> get_sample_frequencies_in_hz();
vector<int> get_sample_frequencies_in_hz_exclude_warmup();
int get_frequency_in_msec(int idx);
int get_frequency_in_hz(int idx);
int get_num_samples(int idx);


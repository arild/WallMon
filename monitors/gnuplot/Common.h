
#include <vector>

using namespace std;

#define KEY					"gnuplot"
#define SERVER				"129.242.19.56"//"localhost"
#define SAMPLE_FREQUENCY	30
#define TARGET_PROCESS		"wallmond"
#define MESSAGE_BUF_SIZE	4096


typedef struct
{
    int sampleFrequency;
    int numSamples;

} SAMPLE_INTERVAL;

extern const SAMPLE_INTERVAL SAMPLE_INTERVALS[];
extern const int NUM_SAMPLE_INTERVALS;

int get_total_num_samples();
vector<int> get_sample_frequencies();



/*
 * BarChart.h
 *
 *  Created on: Feb 24, 2011
 *      Author: arild
 */

#include "plc++demos.h"
#include <map>

using std::string;
typedef std::map<string, int> LabelMap;

class BarChart {
public:
    BarChart();
    virtual ~BarChart();
    void Update(string label, int value);
    void _DrawBar( PLFLT, PLFLT );
    void _ClearBar(PLFLT, PLFLT);
private:
    LabelMap _labelMap;
    int _labelPos;
    plstream           *pls;
    static const PLFLT y0[];
    static PLFLT       pos[], red[], green[], blue[];
};

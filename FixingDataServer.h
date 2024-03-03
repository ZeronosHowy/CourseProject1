#pragma once
#include <vector>
#include <map>
#include "Global.h"
#include "Date.h"

/***********************************
- Task 9
- Author: Hong Jingqing
- Beginning of the code updates.
***********************************/

namespace minirisk {

struct FixingDataServer
{
public:
	FixingDataServer(const string& filename);

    // queries
    double get(const string& name, const Date& t) const;
    std::pair<double, bool> lookup(const string& name, const Date& t) const;

private:
    // for simplicity, assumes fixing server data can only have type double
    std::map<std::pair<string, Date>, double> m_data;
};

} // namespace minirisk

/***********************************
- End of the code updates.
***********************************/




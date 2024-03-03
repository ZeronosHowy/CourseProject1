#include "CurveFXSpot.h"
#include "Market.h"
#include "Streamer.h"

#include <cmath>

/***********************************
- Task 6 & 7
- Author: Yang Haixin & Hao Weitian
- Beginning of the code updates.
***********************************/

namespace minirisk {

    CurveFXSpot::CurveFXSpot(Market* mkt, const Date& today, const string& curve_name)
        : m_today(today)
        , m_name(curve_name)
        , m_rate(mkt->get_fx_spot(curve_name.substr(fx_spot_prefix.length(), 3), curve_name.substr(fx_spot_prefix.length()+4, 3))) //Market.cpp & Global.cpp
    {
    }

    double CurveFXSpot::spot() const
    {
        return m_rate;
    }

} // namespace minirisk

/***********************************
- End of the code updates.
***********************************/

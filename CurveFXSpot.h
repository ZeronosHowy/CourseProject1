#pragma once
#include "ICurve.h"

/***********************************
- Task 6 & 7
- Author: Yang Haixin & Hao Weitian
- Beginning of the code updates.
***********************************/

namespace minirisk {

	struct Market;

    struct CurveFXSpot : ICurveFXSpot
    {
        virtual string name() const { return m_name; }

        CurveFXSpot(Market* mkt, const Date& today, const string& curve_name);

        // return the FX spot price of currency ccy1 deniminated in ccy2 for delivery at time t
        double spot() const;

        virtual Date today() const { return m_today; }

    private:
        Date   m_today;
        string m_name;
        double m_rate;
    };

} // namespace minirisk

/***********************************
- End of the code updates.
***********************************/
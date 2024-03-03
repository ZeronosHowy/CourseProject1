#pragma once
#include "ICurve.h"

/***********************************
- Task 8
- Author: Hao Weitian
- Beginning of the code updates.
***********************************/

namespace minirisk {

	struct Market;

    struct CurveFXForward : ICurveFXForward
    {
        virtual string name() const { return m_name; }

        CurveFXForward(Market* mkt, const Date& today, const string& curve_name);

        // return the FX spot price of currency ccy1 deniminated in ccy2 for delivery at time t
        double fwd(const Date& t) const;

        virtual Date today() const { return m_today; }

    private:
        Date   m_today;
        string m_name;
        //double m_rate;
        ptr_disc_curve_t m_disc_ccy1;
        ptr_disc_curve_t m_disc_ccy2;
        ptr_fx_spot_curve_t m_spot;
    };

} // namespace minirisk

/***********************************
- End of the code updates.
***********************************/
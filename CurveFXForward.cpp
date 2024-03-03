#include "CurveFXForward.h"
#include "Market.h"
#include "Streamer.h"

#include <cmath>

/***********************************
- Task 8
- Author: Hao Weitian
- Beginning of the code updates.
***********************************/

namespace minirisk {

    CurveFXForward::CurveFXForward(Market* mkt, const Date& today, const string& curve_name)
        : m_today(today)
        , m_name(curve_name)
        , m_disc_ccy1(mkt->get_discount_curve(ir_curve_discount_name(curve_name.substr(fx_forward_prefix.length(), 3))))
        , m_disc_ccy2(mkt->get_discount_curve(ir_curve_discount_name(curve_name.substr(fx_forward_prefix.length()+4, 3))))
        , m_spot(mkt->get_fx_spot_curve(fx_spot_name(curve_name.substr(fx_forward_prefix.length(), 3), curve_name.substr(fx_forward_prefix.length()+4, 3))))
    {
    }

    double CurveFXForward::fwd(const Date& t) const
    {
        return m_spot->spot() * m_disc_ccy1->df(t) / m_disc_ccy2->df(t);
    }

} // namespace minirisk

/***********************************
- End of the code updates.
***********************************/

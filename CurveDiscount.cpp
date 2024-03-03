#include "CurveDiscount.h"
#include "Market.h"
#include "Streamer.h"

#include <cmath>


namespace minirisk {

    CurveDiscount::CurveDiscount(Market* mkt, const Date& today, const string& curve_name)
        : m_today(today)
        , m_name(curve_name)
        , m_rates(mkt->get_yield(curve_name.substr(ir_curve_discount_prefix.length(), 3))) //Market.cpp & Global.cpp
    {
    }
    /*
    double  CurveDiscount::df(const Date& t) const
    {
        MYASSERT((!(t < m_today)), m_name << ", DF not available beyond anchor date " << m_today << ", requested " << t);
        double dt = time_frac(m_today, t);
        return std::exp(-m_rate * dt);
    }*/

    double CurveDiscount::df(const Date& t) const
    {
        MYASSERT((!(t < m_today)), "Curve " << m_name << ", DF not available before anchor date " << m_today << ", requested " << t);
        MYASSERT((!((unsigned)(t - m_today) > m_rates.rbegin()->first)), "Curve " << m_name << ", DF not available beyond last tenor date " <<
            Date(m_today.get_serial() + m_rates.rbegin()->first) << ", requested " << t);
        auto r = m_rates.upper_bound(t - m_today);
        unsigned T_H = r->first;
        double rT_H = r->second;
        unsigned T_L = (--r)->first;
        double rT_L = r->second;
        unsigned dt = t - m_today;
        return std::exp(static_cast<double>(-rT_L - (rT_H - rT_L) / (T_H - T_L) * (dt - T_L)) / 365.0);
    }

} // namespace minirisk

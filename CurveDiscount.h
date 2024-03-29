#pragma once
#include "ICurve.h"

namespace minirisk {

struct Market;

struct CurveDiscount : ICurveDiscount
{
    virtual string name() const { return m_name; }

    CurveDiscount(Market *mkt, const Date& today, const string& curve_name);

    // compute the discount factor
    double df(const Date& t) const;

    virtual Date today() const { return m_today; }

private:
    Date   m_today;
    string m_name;
    //double m_rate
    std::map<unsigned, double> m_rates; // After TASK 4, we need more than 1 rates from "risk factors" to compute discount factor.
};

} // namespace minirisk

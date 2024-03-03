#include "Market.h"
#include "CurveDiscount.h"
#include "CurveFXSpot.h"
#include "CurveFXForward.h"

#include <vector>
#include <limits>

namespace minirisk {

template <typename I, typename T>
std::shared_ptr<const I> Market::get_curve(const string& name)
{
    ptr_curve_t& curve_ptr = m_curves[name];
    if (!curve_ptr.get())
        curve_ptr.reset(new T(this, m_today, name));
    std::shared_ptr<const I> res = std::dynamic_pointer_cast<const I>(curve_ptr);
    MYASSERT(res, "Cannot cast object with name " << name << " to type " << typeid(I).name());
    return res;
}

const ptr_disc_curve_t Market::get_discount_curve(const string& name)
{
    return get_curve<ICurveDiscount, CurveDiscount>(name);
}

/***********************************
- Task 7 & 8
- Author: Hao Weitian
- Beginning of the code updates.
***********************************/
const ptr_fx_spot_curve_t Market::get_fx_spot_curve(const string& name) {
    return get_curve<ICurveFXSpot, CurveFXSpot>(name);
}

const ptr_fx_forward_curve_t Market::get_fx_forward_curve(const string& name) {
    return get_curve<ICurveFXForward, CurveFXForward>(name);
}
/***********************************
- End of the code updates.
***********************************/

double Market::from_mds(const string& objtype, const string& name)
{
    auto ins = m_risk_factors.emplace(name, std::numeric_limits<double>::quiet_NaN());
    if (ins.second) { // just inserted, need to be populated
        MYASSERT(m_mds, "Cannot fetch " << objtype << " " << name << " because the market data server has been disconnnected");
        ins.first->second = m_mds->get(name);
    }
    return ins.first->second;
}
/*
const double Market::get_yield(const string& ccyname)
{
    string name(ir_rate_prefix + ccyname); //Global.cpp
    return from_mds("yield curve", name);
};
*/

const double Market::get_fx_spot(const string& name)
{
    return from_mds("fx spot", mds_spot_name(name));
}

/***********************************
- Task 6 & 7
- Author: Yang Haixin & Hao Weitian
- Beginning of the code updates.
***********************************/
const double Market::get_fx_spot(const string& ccy, const string& base){ //"CCY.BASE"
    double ccy_fx_rate, base_fx_rate;
    if (ccy == "USD")
        ccy_fx_rate = 1.0;
    else
        ccy_fx_rate = from_mds("fx spot", fx_spot_prefix + ccy);
    if (base == "USD")
        base_fx_rate = 1.0;
    else
        base_fx_rate = from_mds("fx spot", fx_spot_prefix + base);

    return 1.0 * ccy_fx_rate / base_fx_rate;
}
/***********************************
- End of the code updates.
***********************************/

void Market::set_risk_factors(const vec_risk_factor_t& risk_factors)
{
    clear();
    for (const auto& d : risk_factors) {
        auto i = m_risk_factors.find(d.first);
        MYASSERT((i != m_risk_factors.end()), "Risk factor not found " << d.first);
        i->second = d.second;
    }
}

Market::vec_risk_factor_t Market::get_risk_factors(const std::string& expr) const
{
    vec_risk_factor_t result;
    std::regex r(expr);
    for (const auto& d : m_risk_factors)
        if (std::regex_match(d.first, r))
            result.push_back(d);
    return result;
}

/***********************************
- Task 4
- Author: Wu Zhuonan
- Beginning of the code updates.
***********************************/
unsigned Market::convert_regex_to_days(const string& name, const string& ccy)
{
    unsigned p = std::stoi(name.substr(ir_rate_prefix.length(), name.length() - ccy.length() - ir_rate_prefix.length() - 2));//return num in the rate expr
    unsigned char c = *(name.end() - ccy.length() - 2);//return date unit type of expr
    unsigned q = 0;
    switch (c) {//convert unit to days
    case 'D': q = 1; break;
    case 'W': q = 7; break;
    case 'M': q = 30; break;
    case 'Y': q = 365; break;
    default: MYASSERT(c, "Cannot fetch yield curve because no such interest rate");
    }
    return p * q;//return rate in form of days
}

const std::map<unsigned, double> Market::get_yield(const string& ccyname)
{
    string matchingExpr = ir_rate_prefix + "\\d+[DWMY]." + ccyname;
    std::map<unsigned, double> rates;
    unsigned T;
    rates.emplace(0, 0.0);//intialize

    if (m_mds) {
        std::vector<std::string> names = m_mds->match(matchingExpr);
        for (const auto& n : names) {
            T = convert_regex_to_days(n, ccyname);
            rates.emplace(T, 1.0 * T * from_mds("yield curve", n));
        }
    }
    else {
        std::regex pattern(matchingExpr);
        for (const auto& r : m_risk_factors) {
            if (std::regex_match(r.first, pattern)) {
                T = convert_regex_to_days(r.first, ccyname);
                rates.emplace(T, 1.0 * T * from_mds("yield curve", r.first));
            }
        }
    }
    return rates;
}
/***********************************
- End of the code updates.
***********************************/

} // namespace minirisk

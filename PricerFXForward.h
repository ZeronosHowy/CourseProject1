#pragma once

#include "IPricer.h"
#include "TradeFXForward.h"

/***********************************
- Task 11
- Author: Hao Weitian
- Beginning of the code updates.
***********************************/

namespace minirisk {

struct PricerFXForward : IPricer
{
    PricerFXForward(const TradeFXForward& trd, const string& base);

    virtual double price(Market& m, const FixingDataServer& f) const;

private:
    double m_amt;
    double m_stk;
    Date   m_fixing_t;
    Date   m_settlement_t;
    string m_ir_curve_ccy1;
    string m_ir_curve_ccy2;
    string m_fx_spot_ccy2; //ccy2_base
    string m_fx_forward_curve;
    string m_fx_spot; //ccy1_ccy2
};

} // namespace minirisk

/***********************************
- End of the code updates.
***********************************/


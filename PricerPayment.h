#pragma once

#include "IPricer.h"
#include "TradePayment.h"

namespace minirisk {

struct PricerPayment : IPricer
{
    PricerPayment(const TradePayment& trd, const string& base);

    virtual double price(Market& m, const FixingDataServer& f) const;

private:
    double m_amt;
    Date   m_dt;
    string m_ir_curve;
    string m_fx_ccy;
};

} // namespace minirisk


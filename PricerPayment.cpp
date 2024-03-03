#include "PricerPayment.h"
#include "TradePayment.h"
#include "CurveDiscount.h"
#include "CurveFXSpot.h"

namespace minirisk {

    /***********************************
    - Task 7 
    - Author: Hao Weitian (Some modifications)
    - Beginning of the code updates.
    ***********************************/
PricerPayment::PricerPayment(const TradePayment& trd, const string& base)
    : m_amt(trd.quantity())
    , m_dt(trd.delivery_date())
    , m_ir_curve(ir_curve_discount_name(trd.ccy())) //Global.h
    , m_fx_ccy(trd.ccy() == base ? "" : fx_spot_name(trd.ccy(),base)) //Global.h //"FX.SPOT.CCY.BASE"
{
}

double PricerPayment::price(Market& mkt, const FixingDataServer& f) const
{
    ptr_disc_curve_t disc = mkt.get_discount_curve(m_ir_curve); //Market.cpp
    double df = disc->df(m_dt); // this throws an exception if m_dt<today //CurveDiscount.cpp

    // This PV is expressed in m_ccy. It must be converted in USD (before task 7).
    // After task 7, it is converted in the base ccy.
    if (!m_fx_ccy.empty()){
        //df *= mkt.get_fx_spot(m_fx_ccy); //Market.cpp
        df *= mkt.get_fx_spot_curve(m_fx_ccy)->spot();
    }

    return m_amt * df;
}
/***********************************
- End of the code updates.
***********************************/
} // namespace minirisk



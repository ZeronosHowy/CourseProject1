#include "PricerFXForward.h"
#include "TradeFXForward.h"
#include "CurveDiscount.h"
#include "CurveFXSpot.h"
#include "CurveFXForward.h"

/***********************************
- Task 11
- Author: Hao Weitian
- Beginning of the code updates.
***********************************/

namespace minirisk {

PricerFXForward::PricerFXForward(const TradeFXForward& trd, const string& base)
    : m_amt(trd.quantity())
    , m_stk(trd.strike())
    , m_fixing_t(trd.fixing_date())
    , m_settlement_t(trd.settlement_date())
    , m_ir_curve_ccy1(ir_curve_discount_name(trd.ccy1())) //Global.h
    , m_ir_curve_ccy2(ir_curve_discount_name(trd.ccy2()))
    , m_fx_spot_ccy2(trd.ccy2() == base ? "" : fx_spot_name(trd.ccy2(),base)) //Global.h //"FX.SPOT.CCY.BASE"
    , m_fx_forward_curve(fx_forward_name(trd.ccy1(), trd.ccy2()))
    , m_fx_spot(fx_spot_name(trd.ccy1(), trd.ccy2()))
{
}

double PricerFXForward::price(Market& mkt, const FixingDataServer& f) const
{
    double fxs = 1;
    if (!m_fx_spot_ccy2.empty()) {
        fxs = mkt.get_fx_spot_curve(m_fx_spot_ccy2)->spot();
    }

    ptr_disc_curve_t disc = mkt.get_discount_curve(m_ir_curve_ccy2); //Market.cpp
    double df = disc->df(m_settlement_t); // this throws an exception if m_settlement_t<today //CurveDiscount.cpp

    double fwd;

    if (mkt.today() < m_fixing_t) {
        ptr_fx_forward_curve_t ffc = mkt.get_fx_forward_curve(m_fx_forward_curve);
        fwd = ffc->fwd(m_fixing_t);
        return m_amt * df * fxs * (fwd - m_stk);
    }

    auto lkp = f.lookup(m_fx_spot, m_fixing_t);

    if (mkt.today() == m_fixing_t && (!lkp.second)) {
        ptr_fx_forward_curve_t ffc = mkt.get_fx_forward_curve(m_fx_forward_curve);
        fwd = ffc->fwd(m_fixing_t);
        return m_amt * df * fxs * (fwd - m_stk);
    }
    
    if (mkt.today() == m_fixing_t && lkp.second) {
        fwd = lkp.first;
        return m_amt * df * fxs * (fwd - m_stk);
    }

    fwd = f.get(m_fx_spot, m_fixing_t);

    return m_amt * df * fxs * (fwd - m_stk);
}

} // namespace minirisk

/***********************************
- End of the code updates.
***********************************/



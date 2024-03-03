#include "TradeFXForward.h"
#include "PricerFXForward.h"

/***********************************
- Task 11
- Author: Hao Weitian
- Beginning of the code updates.
***********************************/

namespace minirisk {

ppricer_t TradeFXForward::pricer(const string& base) const
{
    return ppricer_t(new PricerFXForward(*this, base));
}

} // namespace minirisk

/***********************************
- End of the code updates.
***********************************/

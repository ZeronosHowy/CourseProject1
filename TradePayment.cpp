#include "TradePayment.h"
#include "PricerPayment.h"

namespace minirisk {

ppricer_t TradePayment::pricer(const string& base) const
{
    return ppricer_t(new PricerPayment(*this, base));
}

} // namespace minirisk

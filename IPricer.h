#pragma once

#include <memory>

#include "IObject.h"
#include "Market.h"
#include "FixingDataServer.h"

namespace minirisk {

struct IPricer : IObject
{
    virtual double price(Market& m, const FixingDataServer& f) const = 0;
};


typedef std::shared_ptr<const IPricer> ppricer_t;

} // namespace minirisk

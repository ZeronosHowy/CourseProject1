#include "Global.h"
#include "PortfolioUtils.h"
#include "TradePayment.h"
#include "TradeFXForward.h"

#include <numeric>
#include <cmath>

namespace minirisk {

void print_portfolio(const portfolio_t& portfolio)
{
    std::for_each(portfolio.begin(), portfolio.end(), [](auto& pt){ pt->print(std::cout); }); //Trade.h print()
}

std::vector<ppricer_t> get_pricers(const portfolio_t& portfolio, const string& base)
{
    std::vector<ppricer_t> pricers(portfolio.size());
    std::transform( portfolio.begin(), portfolio.end(), pricers.begin()
                  , [base](auto &pt) -> ppricer_t { return pt->pricer(base); } );
    return pricers;
}

portfolio_values_t compute_prices(const std::vector<ppricer_t>& pricers, Market& mkt, const FixingDataServer& fds)
{
    portfolio_values_t prices(pricers.size());

    /*std::transform(pricers.begin(), pricers.end(), prices.begin()
        , [&mkt](auto &pp) -> double { return pp->price(mkt); }); //PricerPayment.cpp
    */

    //*** Task 5 ***
    std::transform(pricers.begin(), pricers.end(), prices.begin()
        , [&mkt, &fds](auto& pp) -> std::pair<double, string> {
            try {
                return std::make_pair(pp->price(mkt, fds), "");
            }
            catch (const std::exception& e) {
                return std::make_pair(std::numeric_limits<double>::quiet_NaN(), e.what());
            }
        }
    );
    return prices;
}
/*
double portfolio_total0(const portfolio_values_t& values)
{
    return std::accumulate(values.begin(), values.end(), 0.0);
}
*/

/***********************************
- Task 5
- Author: Hao Weitian
- Beginning of the code updates.
***********************************/
std::pair<double, std::vector<std::pair<size_t, std::string>>> portfolio_total(const portfolio_values_t& values) {
    double total_value = 0.0;
    std::vector<std::pair<size_t, std::string>> errors;
    size_t n = values.size();
    for (size_t i = 0; i < n; ++i) {
        if (!std::isnan(values[i].first)) {
            total_value += values[i].first;
        }
        else {
            errors.push_back(std::make_pair(i, values[i].second));
        }
    }
    return std::make_pair(total_value, errors);
}
/***********************************
- End of the code updates.
***********************************/

std::vector<std::pair<string, portfolio_values_t>> compute_pv01(const std::vector<ppricer_t>& pricers, const Market& mkt, const FixingDataServer& fds)
{
    std::vector<std::pair<string, portfolio_values_t>> pv01;  // PV01 per trade

    const double bump_size = 0.01 / 100;

    // filter risk factors related to IR
    auto base = mkt.get_risk_factors(ir_rate_prefix + "\\d+[DWMY].[A-Z]{3}");

    // Make a local copy of the Market object, because we will modify it applying bumps
    // Note that the actual market objects are shared, as they are referred to via pointers
    Market tmpmkt(mkt);

    // compute prices for perturbated markets and aggregate results
    pv01.reserve(base.size());
    for (const auto& d : base) {
        portfolio_values_t pv_up, pv_dn;
        std::vector<std::pair<string, double>> bumped(1, d);
        pv01.push_back(std::make_pair(d.first, portfolio_values_t(pricers.size())));

        // bump down and price
        bumped[0].second = d.second - bump_size;
        tmpmkt.set_risk_factors(bumped);
        pv_dn = compute_prices(pricers, tmpmkt, fds);

        // bump up and price
        bumped[0].second = d.second + bump_size; // bump up
        tmpmkt.set_risk_factors(bumped);
        pv_up = compute_prices(pricers, tmpmkt, fds);


        // restore original market state for next iteration
        // (more efficient than creating a new copy of the market at every iteration)
        bumped[0].second = d.second;
        tmpmkt.set_risk_factors(bumped);

        // compute estimator of the derivative via central finite differences
        double dr = 2.0 * bump_size;
        std::transform(pv_up.begin(), pv_up.end(), pv_dn.begin(), pv01.back().second.begin()
            , [dr](auto& hi, auto& lo) -> std::pair<double, string>{
                if (std::isnan(hi.first)) {
                    return std::make_pair(std::numeric_limits<double>::quiet_NaN(), hi.second);
                }
                if (std::isnan(lo.first)) {
                    return std::make_pair(std::numeric_limits<double>::quiet_NaN(), lo.second);
                }
                return std::make_pair((hi.first - lo.first) / dr, "");
            }
        );
    }

    return pv01;
}

/***********************************
- Task 4
- Author: Wu Zhuonan
- Beginning of the code updates.
***********************************/
std::vector<std::pair<string, portfolio_values_t>> compute_pv01_parallel(const std::vector<ppricer_t>& pricers, const Market& mkt, const FixingDataServer& fds) {
    
    std::vector<std::pair<string, portfolio_values_t>> pv01_parallel;  // PV01 per trade
    std::map<string, Market::vec_risk_factor_t> base_map;
    const double bump_size = 0.01 / 100;

    // filter risk factors related to IR
    auto base = mkt.get_risk_factors(ir_rate_prefix + "\\d+[DWMY].[A-Z]{3}");

    for (auto& b : base) {
        base_map[ir_rate_prefix + b.first.substr(b.first.length() - 3, 3)].push_back(b);
    }

    // Make a local copy of the Market object, because we will modify it applying bumps
    // Note that the actual market objects are shared, as they are referred to via pointers
    Market tmpmkt(mkt);

    // compute prices for perturbated markets and aggregate results
    pv01_parallel.reserve(base_map.size());
    for (const auto& d : base_map) {
        portfolio_values_t pv_up, pv_dn;
        std::vector<std::pair<string, double>> bumped;
        pv01_parallel.push_back(std::make_pair(d.first, portfolio_values_t(pricers.size())));

        // bump down and price
        for (unsigned i = 0; i < d.second.size(); ++i) {
            bumped.push_back(d.second[i]);
            bumped[i].second -= bump_size;
        }

        tmpmkt.set_risk_factors(bumped);
        pv_dn = compute_prices(pricers, tmpmkt, fds);

        // bump up and price
        for (unsigned i = 0; i < d.second.size(); ++i) {
            bumped[i].second += 2 * bump_size;
            //Modified by hwt. Note that bump is a vector of pair, 
            //even if the first of the pairs have the same value, it will still be in the vector.
            //So we can just modify the vector "bumped" that we got in the former for loop!
        }
        tmpmkt.set_risk_factors(bumped);
        pv_up = compute_prices(pricers, tmpmkt, fds);


        // restore original market state for next iteration
        // (more efficient than creating a new copy of the market at every iteration)
        for (unsigned i = 0; i < d.second.size(); ++i) {
            bumped[i].second = d.second[i].second;
        }
        tmpmkt.set_risk_factors(bumped);

        // compute estimator of the derivative via central finite differences
        double dr = 2.0 * bump_size;
        std::transform(pv_up.begin(), pv_up.end(), pv_dn.begin(), pv01_parallel.back().second.begin()
            , [dr](auto& hi, auto& lo) -> std::pair<double, string> {
                if (std::isnan(hi.first)) {
                    return std::make_pair(std::numeric_limits<double>::quiet_NaN(), hi.second);
                }
                if (std::isnan(lo.first)) {
                    return std::make_pair(std::numeric_limits<double>::quiet_NaN(), lo.second);
                }
                return std::make_pair((hi.first - lo.first) / dr, "");
            }
        );
    }

    return pv01_parallel;
}
/***********************************
- End of the code updates.
***********************************/

/***********************************
- Task 10
- Author: Wu Zhuonan & Zhu Yikai
- Beginning of the code updates.
***********************************/
std::vector<std::pair<string, portfolio_values_t>> compute_fx_delta(const std::vector<ppricer_t>& pricers, const Market& mkt, const FixingDataServer& fds)
{
    std::vector<std::pair<string, portfolio_values_t>> fx_delta;  // PV01 per trade

    const double bump_size = 0.1 / 100.0;

    // filter risk factors related to IR
    auto base = mkt.get_risk_factors(fx_spot_prefix + "[A-Z]{3}");

    // Make a local copy of the Market object, because we will modify it applying bumps
    // Note that the actual market objects are shared, as they are referred to via pointers
    Market tmpmkt(mkt);

    // compute prices for perturbated markets and aggregate results
    fx_delta.reserve(base.size());
    for (const auto& d : base) {
        portfolio_values_t pv_up, pv_dn;
        std::vector<std::pair<string, double>> bumped(1, d);
        fx_delta.push_back(std::make_pair(d.first, portfolio_values_t(pricers.size())));

        // bump down and price
        double bump_down = d.second * (1 - bump_size);
        bumped[0].second = bump_down;
        tmpmkt.set_risk_factors(bumped);
        pv_dn = compute_prices(pricers, tmpmkt, fds);

        // bump up and price
        double bump_up = d.second * (1 + bump_size);
        bumped[0].second = bump_up; // bump up
        tmpmkt.set_risk_factors(bumped);
        pv_up = compute_prices(pricers, tmpmkt, fds);


        // restore original market state for next iteration
        // (more efficient than creating a new copy of the market at every iteration)
        bumped[0].second = d.second;
        tmpmkt.set_risk_factors(bumped);

        // compute estimator of the derivative via central finite differences
        double dr = bump_up - bump_down;
        std::transform(pv_up.begin(), pv_up.end(), pv_dn.begin(), fx_delta.back().second.begin()
            , [dr](auto& hi, auto& lo) -> std::pair<double, string> {
                if (std::isnan(hi.first)) {
                    return std::make_pair(std::numeric_limits<double>::quiet_NaN(), hi.second);
                }
                if (std::isnan(lo.first)) {
                    return std::make_pair(std::numeric_limits<double>::quiet_NaN(), lo.second);
                }
                return std::make_pair((hi.first - lo.first) / dr, "");
            }
        );
    }

    return fx_delta;
}
/***********************************
- End of the code updates.
***********************************/

ptrade_t load_trade(my_ifstream& is)
{
    string name;
    ptrade_t p;

    // read trade identifier
    guid_t id;
    is >> id;

    if (id == TradePayment::m_id)
        p.reset(new TradePayment);
    else if(id == TradeFXForward::m_id)
        p.reset(new TradeFXForward);
    else
        THROW("Unknown trade type:" << id);

    p->load(is); //Trade.h

    return p;
}

void save_portfolio(const string& filename, const std::vector<ptrade_t>& portfolio)
{
    // test saving to file
    my_ofstream of(filename);
    for( const auto& pt : portfolio) {
        pt->save(of); //Trade.h
        of.endl();
    }
    of.close();
}

std::vector<ptrade_t> load_portfolio(const string& filename)
{
    std::vector<ptrade_t> portfolio;

    // test reloading the portfolio
    my_ifstream is(filename);
    while (is.read_line())
        portfolio.push_back(load_trade(is));

    return portfolio;
}

void print_price_vector(const string& name, const portfolio_values_t& values)
{
    auto values_to_print = portfolio_total(values);
    std::cout
        << "========================\n"
        << name << ":\n"
        << "========================\n"
        << "Total:  " << values_to_print.first << "\n"
        << "Errors: " << values_to_print.second.size() << "\n"
        << "\n========================\n";

    /*
    for (size_t i = 0, n = values.size(); i < n; ++i)
        std::cout << std::setw(5) << i << ": " << values[i] << "\n";
    */
    size_t n = values.size();
    for (size_t i = 0; i < n; ++i) {
        if (!std::isnan(values[i].first)) {
            std::cout << std::setw(5) << i << ": " << values[i].first << "\n";
        }
        else {
            std::cout << std::setw(5) << i << ": " << values[i].second << "\n";
        }
    }

    std::cout << "========================\n\n";
}

} // namespace minirisk

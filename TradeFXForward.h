#pragma once

#include "Trade.h"

/***********************************
- Task 11
- Author: Hao Weitian
- Beginning of the code updates.
***********************************/

namespace minirisk {

struct TradeFXForward : Trade<TradeFXForward>
{
    friend struct Trade<TradeFXForward>;

    static const guid_t m_id;
    static const std::string m_name;

    TradeFXForward() {}

    void init(const std::string& ccy1, const std::string& ccy2, double strike, double quantity, const Date& fixing_date, const Date& settlement_date)
    {
        Trade::init(quantity);
        m_ccy1 = ccy1;
        m_ccy2 = ccy2;
        m_strike = strike;
        m_fixing_date = fixing_date;
        m_settlement_date = settlement_date;
    }

    virtual ppricer_t pricer(const string& base) const;

    const double strike() const {
        return m_strike;
    }
    const string& ccy1() const
    {
        return m_ccy1;
    }

    const string& ccy2() const
    {
        return m_ccy2;
    }

    const Date& fixing_date() const
    {
        return m_fixing_date;
    }

    const Date& settlement_date() const
    {
        return m_settlement_date;
    }

private:
    void save_details(my_ofstream& os) const
    {
        os << m_ccy1 << m_ccy2 << m_strike << m_fixing_date << m_settlement_date;
    }

    void load_details(my_ifstream& is)
    {
        is >> m_ccy1 >> m_ccy2 >> m_strike >> m_fixing_date >> m_settlement_date;
    }

    void print_details(std::ostream& os) const
    {
        os << format_label("Strike level") << m_strike << std::endl;
        os << format_label("Base Currency") << m_ccy1 << std::endl;
        os << format_label("Quote Currency") << m_ccy2 << std::endl;
        os << format_label("Fixing Date") << m_fixing_date << std::endl;
        os << format_label("Settlement Date") << m_settlement_date << std::endl;
    }

private:
    string m_ccy1;
    string m_ccy2;
    double m_strike;
    Date m_fixing_date;
    Date m_settlement_date;
};

} // namespace minirisk

/***********************************
- End of the code updates.
***********************************/

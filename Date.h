#pragma once

#include "Macros.h"
#include <string>
#include <array>

namespace minirisk {

struct Date
{
public:
    static const unsigned first_year = 1900;
    static const unsigned last_year = 2200;
    static const unsigned n_years = last_year - first_year;

private:
    static std::string padding_dates(unsigned);

    // number of days elapsed from beginning of the year
    unsigned day_of_year(unsigned m_y, unsigned m_m, unsigned m_d, bool is_leap) const;

    friend long operator-(const Date& d1, const Date& d2);

    static const std::array<unsigned, 12> days_in_month;  // num of days in month M in a normal year
    static const std::array<unsigned, 12> days_ytd;      // num of days since 1-jan to 1-M in a normal year
    static const std::array<unsigned, 12> days_ytd_leap;    // num of days since 1-jan to 1-M in a leap year
    static const std::array<unsigned, n_years> days_epoch;   // num of days since 1-jan-1900 to 1-jan-yyyy (until 2200)

public:
    // Default constructor
    //Date() : m_y(1970), m_m(1), m_d(1), m_is_leap(false) {}

    Date() : m_serial(0) {}

    Date(unsigned ser) { init(ser); }

    // Constructor where the input value is checked.
    
    Date(unsigned year, unsigned month, unsigned day)
    {
        init(year, month, day);
    }

    void init(unsigned year, unsigned month, unsigned day)
    {
        check_valid(year, month, day);
        unsigned m_y = (unsigned short) year;
        unsigned m_m = (unsigned char) month;
        unsigned m_d = (unsigned char) day;
        bool is_leap = is_leap_year(m_y);
        m_serial = serial(m_y, m_m, m_d, is_leap);
    }
    

    void init(unsigned ser) {
        check_valid(ser);
        m_serial = ser;
    }

    static void check_valid(unsigned y, unsigned m, unsigned d);
    static void check_valid(unsigned s);

    bool operator<(const Date& d) const
    {
        //return (m_y < d.m_y) || (m_y == d.m_y && (m_m < d.m_m || (m_m == d.m_m && m_d < d.m_d)));
        return m_serial < d.m_serial;
    }

    bool operator==(const Date& d) const
    {
        //return (m_y == d.m_y) && (m_m == d.m_m) && (m_d == d.m_d);
        return m_serial == d.m_serial;
    }

    bool operator>(const Date& d) const
    {
        //return d < (*this);
        return m_serial > d.m_serial;
    }

    // number of days since 1-Jan-1900
    unsigned serial(unsigned m_y, unsigned m_m, unsigned m_d, bool is_leap) const
    {
        return days_epoch[m_y - 1900] + day_of_year(m_y, m_m, m_d, is_leap);
    }

    unsigned get_serial() const {
        return m_serial;
    }

    static bool is_leap_year(unsigned yr);

    /***********************************
    - Task 1 & 2
    - Author: Yang Haixin & Zhu Yikai
    - Beginning of the code updates.
    ***********************************/
    std::array<unsigned, 3> get_date() const {
        
        std::array<unsigned, 3> date;

        unsigned tmp_serial = m_serial;
        // return the first iterator whose value > tmp_serial, then compute the distance from 1900
        unsigned year = (unsigned)(std::upper_bound(days_epoch.begin(), days_epoch.end(), tmp_serial) - days_epoch.begin()) - 1; 
        date[0] = 1900 + year;
        tmp_serial -= days_epoch[year];

        unsigned month = 0;
        if (is_leap_year(date[0])) {
            month = (unsigned)(std::upper_bound(days_ytd_leap.begin(), days_ytd_leap.end(), tmp_serial) - days_ytd_leap.begin());
            tmp_serial -= days_ytd_leap[month - 1];
        }
        else {
            month = (unsigned)(std::upper_bound(days_ytd.begin(), days_ytd.end(), tmp_serial) - days_ytd.begin());
            tmp_serial -= days_ytd[month - 1];
        }
        date[1] = month;

        date[2] = tmp_serial + 1;

        return date;
    }
    /***********************************
    - End of the code updates.
    ***********************************/

    // In YYYYMMDD format
    std::string to_string(bool pretty = true) const
    {
        std::array<unsigned, 3> date = get_date();
        return pretty
            ? std::to_string((int)date[2]) + "-" + std::to_string((int)date[1]) + "-" + std::to_string(date[0])
            : std::to_string(date[0]) + padding_dates((int)date[1]) + padding_dates((int)date[2]);
    }

private:
    /*unsigned short m_y;
    unsigned char m_m;
    unsigned char m_d;
    bool m_is_leap;
    */
    unsigned m_serial;
};

long operator-(const Date& d1, const Date& d2);

inline double time_frac(const Date& d1, const Date& d2)
{
    return static_cast<double>(d2 - d1) / 365.0;
}

} // namespace minirisk

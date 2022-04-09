#include "time.hpp"

fe::time::time(const fe::time &time) noexcept
    {
        m_time = time.m_time;
    }

fe::time::time(const fe::time &&time) noexcept
    {
        m_time = time.m_time;
    }

fe::time fe::time::operator+(const fe::time &rhs) const
    {
        return fe::time(fe::microseconds(m_time + rhs.m_time));
    }

fe::time fe::time::operator-(const fe::time &rhs) const
    {
        return fe::time(fe::microseconds(m_time - rhs.m_time));
    }

void fe::time::operator+=(const fe::time &rhs)
    {
        m_time += rhs.m_time;
    }

void fe::time::operator-=(const fe::time &rhs)
    {
        m_time -= rhs.m_time;
    }

fe::time &fe::time::operator=(const fe::time &rhs)
    {
        if (this != &rhs)
            {
                m_time = rhs.m_time;
            }
        return *this;
    }

bool fe::time::operator>(const fe::time &rhs)
    {
        return m_time > rhs.m_time;
    }

bool fe::time::operator>=(const fe::time &rhs)
    {
        return m_time >= rhs.m_time;
    }

bool fe::time::operator<(const fe::time &rhs)
    {
        return m_time < rhs.m_time;
    }

bool fe::time::operator<=(const fe::time &rhs)
    {
        return m_time <= rhs.m_time;
    }

bool fe::time::operator==(const fe::time &rhs)
    {
        return m_time == rhs.m_time;
    }



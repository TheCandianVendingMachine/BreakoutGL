// time.hpp
// A class to store time values
#pragma once
#include "typeDefines.hpp"
#include <limits>

namespace fe 
    {
        class time
            {
                private:
                    // time as microseconds
                    fe::int64 m_time;

                public:
                    const fe::int64 &thisTime = m_time;

                    constexpr time() : m_time(0) {}
                    constexpr time(long long time) : m_time(time) {}
                    time(const time &time) noexcept;
                    time(const time &&time) noexcept;

                    constexpr double asSeconds() const 
                        {
                            // 0.000001 = 1e-6
                            return static_cast<double>(m_time) / 1'000'000;
                        }
                    constexpr fe::int64 asMilliseconds() const
                        {
                            return m_time / 1000;
                        }
                    constexpr fe::int64 asMicroseconds() const
                        {
                            return m_time;
                        }

                    time operator+(const time &rhs) const;
                    time operator-(const time & rhs) const;
                    void operator+=(const time &rhs);
                    void operator-=(const time &rhs);

                    time &operator=(const time &rhs);

                    template<typename T>
                    time operator*(T rhs);
                    template<typename T>
                    time operator/(T rhs);

                    template<typename T>
                    void operator*=(T rhs);
                    template<typename T>
                    void operator/=(T rhs);

                    bool operator>(const time &rhs);
                    bool operator>=(const time &rhs);
                    bool operator<(const time &rhs);
                    bool operator<=(const time &rhs);
                    bool operator==(const time &rhs);

                    static constexpr fe::int64 max = std::numeric_limits<fe::int64>::max();

            };

        template<typename T>
        constexpr time seconds(T in);

        template<typename T>
        constexpr time milliseconds(T in);

        template<typename T>
        constexpr time microseconds(T in);

        constexpr time zero();

        template<typename T>
        constexpr time seconds(T in)
            {
                return time(static_cast<std::int64_t>(in * static_cast<std::int64_t>(static_cast<T>(1000000))));
            }

        template<typename T>
        constexpr time milliseconds(T in)
            {
                return time(static_cast<std::int64_t>(in * static_cast<T>(1000)));
            }

        template<typename T>
        constexpr time microseconds(T in)
            {
                return time(static_cast<std::int64_t>(in));
            }

        constexpr time zero()
            {
                return time();
            }

        template<typename T>
        inline time time::operator*(T rhs)
            {
                return time(m_time * rhs);
            }

        template<typename T>
        inline time time::operator/(T rhs)
            {
                return time(m_time / rhs);
            }

        template<typename T>
        void time::operator*=(T rhs)
            {
                m_time *= rhs;
            }

        template<typename T>
        void time::operator/=(T rhs)
            {
                m_time /= rhs;
            }
    }


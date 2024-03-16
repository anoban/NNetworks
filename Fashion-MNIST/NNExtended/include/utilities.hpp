#pragma once
#ifndef __UTILITIES_H__
    #define __UTILITIES_H__
    #include <concepts>
    #include <random>

// header only implementations
namespace utils {

    // use this to fill an iterator range with random numbers between the specified lower limit and upper limit
    template<typename T> requires std::floating_point<T> class random {
            // T can be float, double or long double
            // class random() is intended to be used as a functor

        private:
            T                                 llimit {};
            T                                 ulimit {};
            std::mt19937_64                   rngenerator;
            std::uniform_real_distribution<T> udist;

        public:
            inline random(void) noexcept : llimit { -0.500 }, ulimit { 0.500 } {
                // creates a random device, that will provide a seed from a random number engine
                constexpr auto randdev { std::random_device {} };
                // mersenne twister 64 engine seeded with the random device of our choosing
                rngenerator = std::mt19937_64 { randdev };
                udist       = std::uniform_real_distribution<T> { -0.500, 0.500 };
                // a call to udist() with the random number generator engine will yield a T value between the range -0.5 and 0.5
            }
            // default constructor with -0.5 as lower limit and 0.5 as upper limit

            inline random(_In_ const T llim, _In_ const T ulim) noexcept : llimit { llim }, ulimit { ulim } {
                constexpr auto randdev { std::random_device {} };
                rngenerator = std::mt19937_64 { randdev };
                udist       = std::uniform_real_distribution<T> { llim, ulim };
            }

            inline T operator()(void) const noexcept { return udist(rngenerator); }

            ~random() = default;
    };

} // namespace utils

#endif // !__UTILITIES_H__


#ifndef __randomize_h__
#define __randomize_h__

#include <random>
#include <cstdint>
#include <memory>
#include <atomic>

void randomize(float& x, float& y, float& z);

class rng
{
public:
    typedef std::minstd_rand engine_t;
    typedef std::uniform_real_distribution<float> dist_t;

public:
    rng();
    rng(uint64_t seed);

    float operator()();

    static uint64_t& rseed();
    static uint64_t& offset();
    static rng* instance();

private:
    engine_t m_engine;
    dist_t   m_dist;
};

#endif

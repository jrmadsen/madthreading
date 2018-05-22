/* Copyright (c) 1993-2015, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <random>
#include "randomize.hpp"

//============================================================================//

void randomize(float& x, float& y, float& z)
{
    x = (*rng::instance())();
    y = (*rng::instance())();
    z = (*rng::instance())();
}

//============================================================================//

rng::rng()
: m_dist(0, 1.0f)
{
    m_engine.seed(std::random_device()());
}

//============================================================================//

rng::rng(uint64_t seed)
: m_dist(0, 1.0f)
{
    m_engine.seed(seed);
}

//============================================================================//

float rng::operator()()
{
    return m_dist(m_engine);
}
//============================================================================//

uint64_t& rng::rseed()
{
    static uint64_t _instance = time(NULL);
    return _instance;
}

//============================================================================//

uint64_t& rng::offset()
{
    static uint64_t _instance = std::numeric_limits<int16_t>::max();
    return _instance;
}

//============================================================================//

rng* rng::instance()
{
    typedef std::shared_ptr<rng> rng_ptr_t;
    static thread_local rng_ptr_t _instance = rng_ptr_t(nullptr);
    if(!_instance.get())
    {
        static std::atomic<int> _counter(0);
        int count = _counter++;
        _instance.reset(new rng(rseed() + count*offset()));
    }
    return _instance.get();
}

//============================================================================//

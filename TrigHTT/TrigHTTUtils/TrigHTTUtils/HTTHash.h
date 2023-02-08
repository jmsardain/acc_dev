#ifndef HTTHASH_H
#define HTTHASH_H

/**
 * @file HTTHash.h
 * @author Riley Xu - rixu@cern.ch
 * @date Feb 20 2021
 * @brief Defines several utilities related to hashing for unordered sets and maps
 */



#include <boost/functional/hash.hpp>

// A hash customization for std containers. Note that boost::hash_range is
// order-dependent, so don't use this to hash sets, but instead set_hash below.
template <typename Container>
struct container_hash
{
    inline std::size_t operator()(Container const& c) const
    {
        return boost::hash_range(c.begin(), c.end());
    }
};

namespace HTT
{

    // This is a fast hash, which should be suitable for simple purposes (low N,
    // not used for sensitive data).
    template <typename Set>
    struct set_hash_fast
    {
        inline std::size_t operator()(Set const& c) const
        {
            std::size_t h = 0;
            for (auto x : c)
            {
                h ^= std::hash<typename Set::key_type>{}(x);
            }
            return h;
        }
    };

    struct pair_hash
    {
        template <class T1, class T2>
        std::size_t operator() (const std::pair<T1,T2> &p) const
        {
            std::size_t seed = 0;
            boost::hash_combine(seed, p.first);
            boost::hash_combine(seed, p.second);
            return seed;
        }
    };

}

#endif

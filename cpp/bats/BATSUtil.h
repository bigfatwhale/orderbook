//
// Created by Chu Ming on 4/11/18.
//

#ifndef PITCH_SPIRIT_BATSUTIL_H
#define PITCH_SPIRIT_BATSUTIL_H

#include <boost/fusion/adapted/struct/adapt_struct.hpp>

//template< class Precision >
struct fixed_point
{
    uint32_t  int_part;
    uint32_t  dec_part;
};

BOOST_FUSION_ADAPT_STRUCT( fixed_point, int_part, dec_part )

namespace boost { namespace spirit { namespace traits {
            // template specialization converting from fixed_point struct to a double
            template<>
            struct assign_to_attribute_from_value<double, fixed_point>
            {
                static void call(fixed_point const &val, double &attr)
                {
                    attr = val.int_part + val.dec_part / 10000.0;
                }
            };
        }
    }
}

#endif //PITCH_SPIRIT_BATSUTIL_H

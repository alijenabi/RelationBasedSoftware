//
//  Index.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 03.02.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef INDEX_H
#define INDEX_H

#include "Space.h"
#include <numeric>

/**
 * The space namespace provides the means to handle matters related to the mathematical space.
 */
namespace rbs::space {

/**
 * @brief The Index class provides the means to manage Indexs for all space sizes.
 */
template<unsigned char SpaceDimension>
class Index :public Space<size_t, SpaceDimension> {
public:

    using Component = size_t;
    using Space = Space<Component, SpaceDimension>;

    /**
     * @brief The constructor.
     */
    Index();

    /**
     * @brief The constructor.
     * @param values The initializer list.
     * @throws If the values has any size but the space dimension
     */
    Index(std::initializer_list<Component> values) : Space(values) {}

    /**
     * @brief The copy constructor.
     * @param other The other Index to copy.
     */
    Index(const Index & other);

    /**
     * @brief The move constructor.
     * @param other The other Index to move.
     */
    Index(Index && other);

    /**
     * @brief Default virtual destructor.
     */
    virtual ~Index(){};

    /**
     * @brief Move/copy assign operator.
     * @param other The other Index to move or copy.
     * @return The new Index.
     */
    Index & operator =(Index other);

    /**
     * @brief Gives the name of the class.
     */
    virtual std::string name() const override;

    constexpr friend inline bool operator < (const Index &in1, const Index &in2) {
        for(unsigned char i = 0; i < SpaceDimension; ++i) {
            if(in1[i] < in2[i])
                return true;
            else if (in1[i] > in2[i])
                return false;
        }
        return false;
    }
    constexpr friend inline bool operator > (const Index &in1, const Index &in2) {
        for(unsigned char i = 0; i < SpaceDimension; ++i) {
            if(in1[i] < in2[i])
                return false;
            else if (in1[i] > in2[i])
                return true;
        }
        return false;
    }
    constexpr friend inline bool operator <= (const Index &in1, const Index &in2){
        return !operator > (in1, in2);
    }
    constexpr friend inline bool operator >= (const Index &in1, const Index &in2) {
        return !operator < (in1, in2);
    }

};

using ind3 = Index<3>;
using ind2 = Index<2>;
using ind1 = Index<1>;
using Int = ind1;

} // namespace rbs::space

#include "IndexImp.h"

#endif // INDEX_H

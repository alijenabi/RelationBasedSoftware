//
//  IndexImp.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 03.02.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef INDEXIMP_H
#define INDEXIMP_H

#ifndef INDEX_H
#   include "Index.h"
#endif // VEINDEX_H


namespace rbs::space {

template<unsigned char SpaceDimension>
Index<SpaceDimension>::Index() : Space() {}

template<unsigned char SpaceDimension>
Index<SpaceDimension>::Index(const Index &other) : Space(other) {}

template<unsigned char SpaceDimension>
Index<SpaceDimension>::Index(Index &&other) : Space(other) {}

template<unsigned char SpaceDimension>
Index<SpaceDimension> &Index<SpaceDimension>::operator =(Index other){
    Space::operator =(other);
    return *this;
}

template<unsigned char SpaceDimension>
std::string Index<SpaceDimension>::name() const {
    return "Index";
}

} // namespace rbs::space

#endif // INDEXIMP_H

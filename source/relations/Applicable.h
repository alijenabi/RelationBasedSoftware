//
//  Applicable.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabi on 22.12.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef APPLICABLE_H
#define APPLICABLE_H

/**
 *  The relations namestpase provides the means to connect (relate) two fields.
 */
namespace rbs::relations {

/**
 * @brief The Applicable class is an interface for applicable relation.
 */
class Applicable {
public:
    virtual void apply() = 0;
};

} // namespace rbs::relations

#endif // APPLICABLE_H

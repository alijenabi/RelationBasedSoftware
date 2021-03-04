//
//  Relation.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabi on 22.12.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef RELATION_H
#define RELATION_H

#include "Applicable.h"
#include <functional>

/**
 *  The relations namestpase provides the means to connect (relate) two fields.
 */
namespace rbs::relations {

/**
 * @brief The Relation class is the base class for all of the relations inside the relations namespace.
 * @note The Feeder cannot go under change when relationship applies to the relation.
 * @note The Feeder and Feedee can be the same.
 * @note The Feeder and Feedee relation can be two way but then two relation will be needed.
 * @tparam Feeder The data type that feeds the relationship with information.
 * @tparam Feedee The data type that the feeder feeds it with information.
 */
template<typename Feeder, typename Feedee>
class Relation : public Applicable
{
public:
    using Relationship = std::function<void(const Feeder&, Feedee&)>;

    /**
     * @brief The default copy constructor.
     */
    Relation(const Relation&) = default;

    /**
     * @brief The move constructor.
     */
    Relation(Relation&& relation);

    virtual ~Relation() = default;

    /**
     * @brief Move/copy assign operator.
     * @param other The other Space to move or copy.
     * @return The new Space.
     */
    Relation &operator =(Relation other);

    /**
     * @brief Applies the relation.
     */
    virtual void apply() override;

protected:

    /**
     * @brief The constructor.
     * @param feedee The object to be feeded with information. (i.e., object onder change).
     * @param feeder The object that feeds the relationship with information.
     * @param relationship The reationship between feeder and feedee.
     */
    explicit Relation(const Feeder& feeder, Feedee& feedee, Relationship relationship);

    /**
     * @brief Gives dircet access to the feedee.
     */
    Feedee& feedee();

    /**
     * @brief Gives dircet access to the feeder.
     */
    const Feeder& feeder();

    /**
     * @brief Gives dircet access to the relationship.
     */
    const Relationship& relationship();

    /**
     * @brief Sets the Feedee.
     */
    void setFeedee(Feedee& feedee);

    /**
     * @brief Sets the Feeder
     */
    void setFeeder(const Feeder& feeder);

    /**
     * @brief Sets the Relationship
     */
    void setRelationship(const Relationship& relationship);

private:
    /**
     * @brief Swaps the content of this object with an other objec of same type.
     * @param other The other instance.
     */
    void swap(Relation & other);

    std::shared_ptr<Feedee>       p_feedee;
    std::shared_ptr<const Feeder> p_feeder;
    Relationship p_relationship;

};

} // namespace rbs::relations

#ifndef RELATIONIMP_H
#include "RelationImp.h"
#endif // RELATIONIMP_H

#endif // RELATION_H

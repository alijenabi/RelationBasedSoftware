//
//  RelationImp.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabi on 22.12.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef RELATIONIMP_H
#define RELATIONIMP_H

#ifndef RELATION_H
#   include "Relation.h"
#endif // RELATION_H

namespace rbs::relations {

template<typename Feeder, typename Feedee>
void Relation<Feeder, Feedee>::apply() {
    if (p_feedee && p_feeder)
        return p_relationship(*p_feeder, *p_feedee);
    throw std::runtime_error("The relation is not complete.");
}

template<typename Feeder, typename Feedee>
Relation<Feeder, Feedee>::Relation(Relation &&relation) {
    p_feedee = std::move(relation.p_feedee);
    p_feeder = std::move(relation.p_feeder);
    p_relationship = std::move(relation.p_relationship);
}

template<typename Feeder, typename Feedee>
Relation<Feeder, Feedee>::Relation(const Feeder &feeder, Feedee &feedee, Relationship relationship)
    : p_feedee{std::shared_ptr<Feedee>(&feedee, [](auto&){})}
    , p_feeder{std::shared_ptr<const Feeder>(&feeder, [](auto&){})}
    , p_relationship{relationship}
{}

template<typename Feeder, typename Feedee>
Relation<Feeder, Feedee> &Relation<Feeder, Feedee>::operator =(Relation other) {
    swap(other);
    return *this;
}


template<typename Feeder, typename Feedee>
Feedee &Relation<Feeder, Feedee>::feedee() {
    return *p_feedee;
}

template<typename Feeder, typename Feedee>
const Feeder &Relation<Feeder, Feedee>::feeder() {
    return *p_feeder;
}

template<typename Feeder, typename Feedee>
const std::function<void(const Feeder&, Feedee&)> &Relation<Feeder, Feedee>::relationship() {
    return p_relationship;
}

template<typename Feeder, typename Feedee>
void Relation<Feeder, Feedee>::setFeedee(Feedee &feedee) {
    p_feedee = std::shared_ptr<Feedee>(&feedee, [](auto&){});
}

template<typename Feeder, typename Feedee>
void Relation<Feeder, Feedee>::setFeeder(const Feeder &feeder) {
    p_feeder = std::shared_ptr<const Feeder>(&feeder, [](auto&){});
}

template<typename Feeder, typename Feedee>
void Relation<Feeder, Feedee>::setRelationship(const Relation::Relationship &relationship) {
    p_relationship = relationship;
}

template<typename Feeder, typename Feedee>
void Relation<Feeder, Feedee>::swap(Relation &other) {
    std::swap(p_feedee,       other.p_feedee);
    std::swap(p_feeder,       other.p_feeder);
    std::swap(p_relationship, other.p_relationship);
}

} // namespace rbs::relations

#endif // RELATIONIMP_H

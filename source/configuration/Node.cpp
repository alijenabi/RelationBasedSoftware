//
//  Node.cpp
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 21.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#include "Node.h"

namespace rbs::configuration {

Node::Node(const space::Point<3> &initialPosition) {
    p_initialPosition = std::make_shared<Property>(initialPosition);
}

Node::Node(const Node::Index initialIndex) {
    p_initialPosition = std::make_shared<Property>(initialIndex);
}

Node::Node(Node &&node) {
    p_initialPosition = std::move(node.p_initialPosition);
    p_properties = std::move(node.p_properties);
}

Node &Node::operator =(Node other) {
    swap(other);
    return *this;
}

Node::~Node() = default;

const Node::Property &Node::at(const Node::Index index) const {
    const auto item = p_properties.find(index);
    if (item != p_properties.end())
        return *item->second;
    throw std::runtime_error("The is no property set at index: " + std::to_string(index) + ".");
}

bool Node::has(const Node::Index index) const {
    return p_properties.find(index) != p_properties.end();
}

const Node::Property &Node::initialPosition() const {
    return *p_initialPosition;
}

const Node::PropertyPtr &Node::reference(const Node::Index index) const {
    const auto item = p_properties.find(index);
    if (item != p_properties.end())
        return item->second;
    throw std::runtime_error("The is no property set at index: " + std::to_string(index) + ".");
}

Node::Property &Node::at(const Node::Index index) {
    const auto item = p_properties.find(index);
    if (item != p_properties.end())
        return *item->second;
    p_properties[index] = std::make_shared<Property>();
    return *p_properties[index];
}

void Node::dereferencelizeInitialPosition() {
    if (!p_initialPosition)
        throw std::runtime_error("The Node has no initial position.");
    p_initialPosition = std::make_shared<Property>(*p_initialPosition);
}

void Node::dereferencelizeProperties() {
    std::for_each(p_properties.begin(), p_properties.end(), [this](auto& pair) {
        if (!p_initialPosition)
            throw std::runtime_error("The Node property is missing.");
        pair.second = std::make_shared<Property>(*pair.second);
    });
}

void Node::dereferencelizePropertyAt(const Node::Index index) {
    if (index < p_properties.size())
        p_properties[index] = std::make_shared<Property>(*p_properties[index]);
    throw std::out_of_range("The Node has no property at index " + std::to_string(index) + ".");
}

void Node::erase(const Node::Index index) {
    p_properties.erase(index);
}

Node::PropertyPtr &Node::reference(const Node::Index index) {
    const auto item = p_properties.find(index);
    if (item != p_properties.end())
        return item->second;
    p_properties[index] = std::make_shared<Property>();
    return p_properties[index];
}

const Node::Property &Node::operator [](const Node::Index index) const {
    return at(index);
}

Node::Property &Node::operator [](const Node::Index index) {
    return at(index);
}

void Node::swap(Node &other) {
    std::swap(p_initialPosition, other.p_initialPosition);
    p_properties.swap(other.p_properties);
}

std::ostream &operator <<(std::ostream &out, const Node &node)
{
    return out << std::string(node);
}

configuration::Node::operator std::string() const {
    std::string ans = "Node:{";

    if (!p_properties.empty())
        ans = ans + "\n  ";
    ans = ans + "initial_position: " + std::string(*p_initialPosition);

    size_t itr = 0;
    for(const auto& property: p_properties) {
        if (!itr) {
            ans = ans + ",\n  property list:{ \n";
            ans = ans +    "    [" + std::to_string(property.first) + "] => " + ((property.second)? std::string(*property.second): "nullptr");
        } else {
            ans = ans + ",\n    [" + std::to_string(property.first) + "] => " + ((property.second)? std::string(*property.second): "nullptr");
        }
        itr++;
    }
    if (itr)
        ans = ans + ",\n}";
    return ans + "}";
}

} // namespace rbs::configuration

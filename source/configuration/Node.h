//
//  Node.h
//  Relation-Based Simulator (RBS)
//
//  Created by Ali Jenabidehkordi on 21.08.18.
//  Copyright © 2018 Ali Jenabidehkordi. All rights reserved.
//

#ifndef NODE_H
#define NODE_H

#include "../variables/Variant.h"
#include <map>
#include <memory>

/**
 * The configuration namespace provides the means to create configurations and use them.
 */
namespace rbs::configuration {

/**
 * @brief Each node presents part of the configuration's discretized body in the simulation.
 * @note Depending on the constitutive model, Nodes require storing different properties of the subdomain that they are representing.
 * @note This class only provides storage that is capable of storing various types.
 * @note If you need to introduce new type of Node, use this class as a protected base class and then implement the required properties.
 * @warning Avoid using this class directly in your simulation. For each supported constitutive model, there should be subclass of this class.
 */
class Node
{
public:
    using Index = size_t;
    using Property = variables::Variant;
    using PropertyPtr = std::shared_ptr<Property>;
    using PropertyMap = std::map<Index, PropertyPtr>;

    /**
     * @brief The constructor.
     * @param the initialPosition of the node.
     */
    explicit Node(const space::Point<3>& initialPosition);

    /**
     * @brief The constructor.
     * @param initialIndex The index of the initial position.
     */
    explicit Node(const Index initialIndex);

    /**
     * @brief The default copy constructor.
     */
    Node(const Node& node) = default;

    /**
     * @brief The move constructor.
     */
    Node(Node&& node);

    /**
     * @brief Move/copy assign operator.
     * @param other The other Space to move or copy.
     * @return The new Space.
     */
    Node &operator =(Node other);

    /**
     * @brief Disstructor.
     */
    virtual ~Node();

    /**
     * @brief Gives the property at a specific index.
     * @throws If there is no property stored at the index.
     */
    const Property& at(const Index index) const;

    /**
     * @brief Whether the node contains property at a specific index.
     */
    bool has(const Index index) const;

    /**
     * @brief Gives the initial postition of the node.
     */
    const Property& initialPosition() const;

    /**
     * @brief Gives a shared pointer to property at a specific index.
     * @throws If there is no property stored at the index.
     */
    const PropertyPtr& reference(const Index index) const;

    /**
     * @brief Gives direct access to the property at a specific index.
     */
    Property& at(const Index index);

    /**
     * @brief Dereferencelizes the initial position.
     * @details The nodes are pointing to their properties; thus, all the copied nodes will share the same properties.
     * @note This will create a new initial position for the node, but the properties will stay untouched.
     */
    void dereferencelizeInitialPosition();

    /**
     * @brief Dereferencelizes all the properties.
     * @details The nodes are pointing to their properties; thus, all the copied nodes will share the same properties.
     * @note This will create a new properties for this node, but the initial postition will stay untouched.
     */
    void dereferencelizeProperties();

    /**
     * @brief Dereferencelizes a property at a specific index.
     * @details The nodes are pointing to their properties; thus, all the copied nodes will share the same properties.
     * @note This will create a new property for this node, but all the other properties and initial position will stay untouched.
     * @param index The index of the property.
     */
    void dereferencelizePropertyAt(const Index index);

    /**
     * @brief Erases the property at the given index.
     */
    void erase(const Index index);

    /**
     * @brief Gives a shared pointer to property at a specific index.
     */
    PropertyPtr& reference(const Index index);

    /**
     * @brief Gives the property at a specific index.
     * @throws If there is no property stored at the index.
     */
    const Property& operator [](const Index index) const;

    /**
     * @brief Gives direct access to the property at a specific index.
     */
    Property& operator [](const Index index);

    /**
     * @brief Gives the string representation.
     */
    virtual operator std::string() const;

private:
    /**
     * @brief Swaps the content of this object with an other objec of same type.
     * @param other The other instance.
     */
    void swap(Node & other);

    PropertyPtr p_initialPosition;  // The initial position of the Node.
    PropertyMap p_properties;       // The map between properties index and their pointers.
};

std::ostream & operator << (std::ostream & out, const Node & node);

} // namespace rbs::configuration

#endif // NODE_H

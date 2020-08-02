# ECSlib

Build status: ![WinBuild](https://github.com/firststef/ECSlib/workflows/BuildWindows/badge.svg) ![LinuxBuild](https://github.com/firststef/ECSlib/workflows/BuildLinux/badge.svg)

A library with ECS classes in c++ using raylib

ECSlib was created as a collection of ECS classes for developing the Ciopillis game, found here: https://github.com/firststef/Ciopillis. It was separated as a library for use in other projects.

### Install

ECSlib is configured with cmake, you can either build it on Linux directly or generate a .sln file for Windows. 

### Features



### ECS

The game was built with the use of an [Entity Component System](https://en.wikipedia.org/wiki/Entity_component_system) framework, followingly referenced as ECSlib.

To understand the ECS design better we compare it to what was considered the natural way of implementing a game system: **object-oriented inheritance hierarchies**. Everything in the game was an instance of the class **Object** or its derived classes. Each object would interact most commonly with either it's base or derived classes instantiations, and in some cases with other classes. **The game logic** was based on the **coordination between the classes**, most often in the form of *hierarchical trees*, with parents controlling the actions of the children and the children transmitting responses.

But at a certain point, several problems arose. One of the most important was the process of maintaining the code: Adding new functionalities inevitably involved the use of increasingly complex issue-solving techniques, which even though was educational, proved tedious and hindered the development of the game.

With ECS, this aspect is solved:

> **Entity–component–system** (**ECS**) is an architectural pattern that is mostly used in the game development industry. ECS follows the composition over the inheritance principle that allows greater flexibility in defining game object types. The behavior of each entity can change depending on what components it holds at a certain moment.

 **Entities** (the correspondent of Objects) now held attributes called **Components** (essentially data units), that are evaluated independently in such a matter that these structures are seemingly dependency-free. **The new game logic** is driven by **Systems**, each taking a specific type of Component and editing the data in these units. Adding new functionalities now consists of adding a new System with an independent logic and a matching component (though this is not mandatory). Also see the [Appendix ](#Appendix ).

Because these systems are independent, the adding, removing, runtime disabling or enabling would not crash the game or impact in any way other modular functionalities. Also, the behavior of the main game unit, the Entity, can easily change during the game based on the contained components.

### Raylib

Raylib is an open-source, cross-platform library for video game written in c99 that is used by the ECSlib for rendering and physics computations.

### The framework architecture

A game instance is usually managed by an object of the class **ECSManager**. ECSManager ties together a **SystemManager**, an **Event Manager** and a **Resource Manager**. Each system can be initilalized and runs idependently, but can sometimes communicate with other systems by **Events**.

Just to mention a few important systems, ECSlib implements the **DrawSystem** (graphical manager), **InputSystem**, **AnimationSystem**, **PhysicsSystem** etc. Each system can run on a separate thread, however the current implementation works with an iterative approach to the concept.

# Appendix

How ECS Systems work
![Systems](https://drive.google.com/uc?export=download&id=1ONHaTytjKBFrBXvTDf1QOMA3MaFs5X4O)

*Each System only changes data by small, modular logic rules without interfering too much with other system's logic*.  

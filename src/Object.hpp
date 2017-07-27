/*
Objects are the base class for all things in the world, whether it is a player,
a monster, a tile, a weapon, or anything else.

All active game Objects are stored in each Map and are moved between each
seamlessly.

Objects stored in inventories lose their global id and gain a relative id.
*/
#pragma once
#include "ObjectTypes.hpp"
#include <cstdint>
#include <memory>

class Map;
class Object;
typedef std::shared_ptr<Object> ObjectPtr;

class Object {
  uint32_t mId;             // Id, can be global or relative depending on if in an inventory
  ObjectType mType;         // Type identifier, see ObjectTypes
  ObjectFlags mFlags;
  ObjectPtr mNext;                 // used for piles
  ObjectPtr mPrev;                 // used for piles
  ObjectPtr mParent;               // used for parenting (inventory item's parent)
  ObjectPtr mChild;                // used for first child of stored inventory
  std::shared_ptr<Map> mMap;       // Map this Object is on
  protected:
    int32_t mBaseWeight;      // weight in grams for a single count
    int32_t mWeight;          // total weight (mBaseWeight*mCount)
    int32_t mCount;           // amount of this object
  public:
    Object();
    Object(uint32_t id);

    ObjectPtr getPrev()   { return mPrev; };
    ObjectPtr getNext()   { return mNext; };
    ObjectPtr getParent() { return mParent; };
    ObjectPtr getChild() { return mChild; };
    int32_t adjustCount(int32_t amount);
    Object* operator++() { adjustCount(1); return this; };
    Object* operator--() { adjustCount(-1); return this; };
    Object* operator+=(int32_t count) { adjustCount(count); return this; };
    Object* operator-=(int32_t count) { adjustCount(count); return this; };

    ObjectPtr getChildObjectById(uint32_t id);
    bool removeChildObjectById(uint32_t id);

    virtual bool onBirth() = 0;
    virtual bool onDie() = 0;
    virtual bool onTouched(Object *other) = 0;
    virtual bool onThink() = 0;
};

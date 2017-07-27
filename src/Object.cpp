#include "Object.hpp"
#include "Log.hpp"

Object::Object() : mFlags(ObjectFlags::NONE), mType(ObjectType::NONE) {
}

Object::Object(uint32_t id) : mFlags(ObjectFlags::NONE), mType(ObjectType::NONE), mId(id) {
}

int32_t Object::adjustCount(int32_t amount) {
  // adjust count
  mCount += amount;
  // 0 items left, we should be deleted
  if (mCount <= 0) {
    // if we have a parent, then we're contained
    if (mParent != nullptr) {
      mParent->removeChildObjectById(mId);
      return 0;
    } else if (mMap != nullptr) {
      //mMap->removeObjectById(mId);
      return 0;
    } else {
      ERR << "Object " << mId << " is without a parent or map!";
    }
  } else {
  }
  // recalculate weight
  mWeight = mCount * mBaseWeight;
  return mCount;
}
/*
Iterate over children until object with id is found. Return true on removal,
false on failure
*/
bool Object::removeChildObjectById(uint32_t id) {
  ObjectPtr object = getChildObjectById(id);
  if (object == nullptr) return false;
  // found it, let's fix up relations
  object->mParent = nullptr;              // remove child's reference to this
  if (object == mChild) mChild = nullptr; // remove if first child
  // repair the list
  if (object->mPrev) object->mPrev->mNext = object->mNext;
  if (object->mNext) object->mNext->mPrev = object->mPrev;
  return true;
}

ObjectPtr Object::getChildObjectById(uint32_t id) {
  if (mChild == nullptr) return nullptr;
  for (Object *prev_node = nullptr, *node = mChild.get(); node != nullptr; prev_node = node, node = node->mNext.get()) {
    if (node->mId == id) {
      return ObjectPtr(node);
    }
    // travel through any found containers
    if (checkFlags(node->mFlags, ObjectFlags::CONTAINER) && node->removeChildObjectById(id) == true) {
      return nullptr;
    }
  }
  return nullptr;
}

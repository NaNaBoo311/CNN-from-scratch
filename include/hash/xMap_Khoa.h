/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt
 * to change this license Click
 * nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this
 * template
 */

/*
 * File:   xMap.h
 * Author: ltsach
 *
 * Created on October 11, 2024, 7:08 PM
 */

#ifndef XMAP_H
#define XMAP_H
#include <memory.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#include "hash/IMap.h"
#include "list/DLinkedList.h"

/*
 * xMap<K, V>:
 *  + K: key type
 *  + V: value type
 *  For example:
 *      xMap<string, int>: map from string to int
 */
template <class K, class V>
class xMap : public IMap<K, V> {
 public:
  class Entry;  // forward declaration

 protected:
  DLinkedList<Entry*>* table;
  int capacity;
  int count;
  float loadFactor;
  DLinkedList<int> list_clashes;
  int (*hashCode)(K&, int);
  bool (*keyEqual)(K&, K&);
  bool (*valueEqual)(V&, V&);
  void (*deleteKeys)(xMap<K, V>*);
  void (*deleteValues)(xMap<K, V>*);

 public:
  xMap(int (*hashCode)(K&, int),  // require
       float loadFactor = 0.75f, bool (*valueEqual)(V&, V&) = 0,
       void (*deleteValues)(xMap<K, V>*) = 0, bool (*keyEqual)(K&, K&) = 0,
       void (*deleteKeys)(xMap<K, V>*) = 0);

  xMap(const xMap<K, V>& map);                   // copy constructor
  xMap<K, V>& operator=(const xMap<K, V>& map);  // assignment operator
  ~xMap();

  // Inherit from IMap:BEGIN
  V put(K key, V value);
  V& get(K key);
  V remove(K key, void (*deleteKeyInMap)(K) = 0);
  bool remove(K key, V value, void (*deleteKeyInMap)(K) = 0,
              void (*deleteValueInMap)(V) = 0);
  bool containsKey(K key);
  bool containsValue(V value);
  bool empty();
  int size();
  void clear();
  string toString(string (*key2str)(K&) = 0, string (*value2str)(V&) = 0);
  DLinkedList<K> keys();
  DLinkedList<V> values();
  DLinkedList<int> clashes();
  // Inherit from IMap:END

  // Show map on screen: need to convert key to string (key2str) and value2str
  void println(string (*key2str)(K&) = 0, string (*value2str)(V&) = 0) {
    cout << this->toString(key2str, value2str) << endl;
  }
  int getCapacity() { return capacity; }

  ///////////////////////////////////////////////////
  // STATIC METHODS: BEGIN
  //      * Used to create xMap objects
  ///////////////////////////////////////////////////
  /*
   * sample hash function for keys of types integer and string:
   */
  static int intKeyHash(int& key, int capacity) { return key % capacity; }
  static int stringKeyHash(string& key, int capacity) {
    long long int sum = 0;
    for (int idx = 0; idx < key.length(); idx++) sum += key[idx];
    return sum % capacity;
  }
  /*
   * freeKey(xMap<K,V> *pMap):
   *  Purpose: a typical function for deleting keys stored in map
   *  WHEN to use:
   *      1. K is a pointer type; AND
   *      2. Users need xMap to free keys
   */
  static void freeKey(xMap<K, V>* pMap) {
    for (int idx = 0; idx < pMap->capacity; idx++) {
      DLinkedList<Entry*> list = pMap->table[idx];
      for (auto pEntry : list) {
        delete pEntry->key;
      }
    }
  }
  /*
   * freeValue(xMap<K,V> *pMap):
   *  Purpose: a typical function for deleting values stored in map
   *  WHEN to use:
   *      1. V is a pointer type; AND
   *      2. Users need xMap to free values
   */
  static void freeValue(xMap<K, V>* pMap) {
    for (int idx = 0; idx < pMap->capacity; idx++) {
      DLinkedList<Entry*> list = pMap->table[idx];
      for (auto pEntry : list) {
        delete pEntry->value;
      }
    }
  }
  /*
   * deleteEntry(Entry* ptr): a function pointer to delete pointer to Entry
   */
  static void deleteEntry(Entry* ptr) { delete ptr; }
  ///////////////////////////////////////////////////
  // STATIC METHODS: END
  //      * Used to create xMap objects
  ///////////////////////////////////////////////////

 protected:
  ////////////////////////////////////////////////////////
  ////////////////////////  UTILITIES ////////////////////
  ////////////////////////////////////////////////////////
  void ensureLoadFactor(int minCapacity);
  // future version:
  //   should add a method to trim table shorter when removing key (and value)
  void rehash(int newCapacity);
  void removeInternalData();
  void copyMapFrom(const xMap<K, V>& map);
  void moveEntries(DLinkedList<Entry*>* oldTable, int oldCapacity,
                   DLinkedList<Entry*>* newTable, int newCapacity);

  /*
   * keyEQ(K& lhs, K& rhs): verify the equality of two keys
   */
  bool keyEQ(K& lhs, K& rhs) {
    if (keyEqual != 0)
      return keyEqual(lhs, rhs);
    else
      return lhs == rhs;
  }
  /*
   *  valueEQ(V& lhs, V& rhs): verify the equality of two values
   */
  bool valueEQ(V& lhs, V& rhs) {
    if (valueEqual != 0)
      return valueEqual(lhs, rhs);
    else
      return lhs == rhs;
  }

  //////////////////////////////////////////////////////////////////////
  ////////////////////////  INNER CLASSES DEFNITION ////////////////////
  //////////////////////////////////////////////////////////////////////
 public:
  // Entry: BEGIN
  class Entry {
   private:
    K key;
    V value;
    friend class xMap<K, V>;

   public:
    Entry(K key, V value) {
      this->key = key;
      this->value = value;
    }
  };
  // Entry: END
};

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template <class K, class V>
xMap<K, V>::xMap(int (*hashCode)(K&, int),
       float loadFactor, bool (*valueEqual)(V&, V&),
       void (*deleteValues)(xMap<K, V>*), bool (*keyEqual)(K&, K&),
       void (*deleteKeys)(xMap<K, V>*)) {
  this->hashCode = hashCode;
  this->loadFactor = loadFactor;
  this->valueEqual = valueEqual;
  this->deleteValues = deleteValues;
  this->keyEqual = keyEqual;
  this->deleteKeys = deleteKeys;

  this->count = 0;
  this->capacity = 10;
  this->table = new DLinkedList<Entry*>[capacity];

  for (int idx = 0; idx < capacity; idx++) {
    DLinkedList<Entry*>& list = table[idx];
    for(auto pEntry: list){
      put(pEntry->key, pEntry->value);
    }
  }
}


template <class K, class V>
xMap<K, V>::xMap(const xMap<K, V>& map) {
  copyMapFrom(map);
  this->deleteKeys = nullptr;
  this->deleteValues = nullptr;
}

template <class K, class V>
xMap<K, V>& xMap<K, V>::operator=(const xMap<K, V>& map) {
  if (this != &map) {
    removeInternalData();
    copyMapFrom(map);
    this->deleteKeys = nullptr;
    this->deleteValues = nullptr;
  }
  return *this;
}

template <class K, class V>
xMap<K, V>::~xMap() {
  removeInternalData();
}

//////////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTATION of IMap    ///////////////////
//////////////////////////////////////////////////////////////////////

template <class K, class V>
V xMap<K, V>::put(K key, V value) {
  int index = this->hashCode(key, capacity);
  V retValue = value;  // Initialize retValue with the new value

  DLinkedList<Entry*>& list = this->table[index];
  for (auto& pEntry : list) {
    if (keyEQ(pEntry->key, key)) {
      retValue = pEntry->value;
      pEntry->value = value;
      return retValue;
    }
  }

  // Key not found, create new entry and add to the list
  Entry* newEntry = new Entry(key, value);
  list.add(newEntry);
  this->count++;

  // Check for load factor and rehash if necessary
  ensureLoadFactor(this->count);

  return retValue;
}

template <class K, class V>
V& xMap<K, V>::get(K key) {
  int index = hashCode(key, capacity);
  DLinkedList<Entry*>& list = table[index];

  for (int i = 0; i < list.size(); ++i) {
        Entry* pEntry = list.get(i);
        if (keyEQ(pEntry->key, key)) {
            return pEntry->value;
        }
    }

  // key: not found
  stringstream os;
  os << "key (" << key << ") is not found";
  throw KeyNotFound(os.str());
}

template <class K, class V>
V xMap<K, V>::remove(K key, void (*deleteKeyInMap)(K)) {
  int index = hashCode(key, capacity);
  DLinkedList<Entry*>& list = table[index];

  for (typename DLinkedList<Entry*>::Iterator it = list.begin(); it != list.end(); ++it) {
    Entry* pEntry = *it;
    if (keyEQ(pEntry->key, key)) {
      V retValue = pEntry->value;

      if (deleteKeyInMap != nullptr) {
        deleteKeyInMap(pEntry->key);
      }

      list.removeItem(pEntry, xMap<K, V>::deleteEntry);
      this->count--;

      return retValue;
    }
  }

  // key: not found
  stringstream os;
  os << "key (" << key << ") is not found";
  throw KeyNotFound(os.str());

  return V();
}

template <class K, class V>
bool xMap<K, V>::remove(K key, V value, void (*deleteKeyInMap)(K), void (*deleteValueInMap)(V)) {
  int index = hashCode(key, capacity);
  DLinkedList<Entry*>& list = table[index];

  for (typename DLinkedList<Entry*>::Iterator it = list.begin(); it != list.end(); ++it) {
    Entry* pEntry = *it;
    if (keyEQ(pEntry->key, key) ) {
      if(valueEQ(pEntry->value, value)){
        V retValue = pEntry->value;  // Backup value to return

        if (deleteKeyInMap != nullptr) {
        deleteKeyInMap(pEntry->key);
        }
        if (deleteValueInMap != nullptr) {
          deleteValueInMap(pEntry->value);
       }

        list.removeItem(pEntry, xMap<K, V>::deleteEntry);
        this->count--;
        return true;
      }
    return false;
    }
  }

  // key: not found
  stringstream os;
  os << "key (" << key << ") is not found";
  throw KeyNotFound(os.str()); 
}

template <class K, class V>
bool xMap<K, V>::containsKey(K key) {
  int index = hashCode(key, capacity);
  DLinkedList<Entry*>& list = table[index];

  for (auto& pEntry : list) {
    if (keyEQ(pEntry->key, key)) {
      return true;
    }
  }
  return false;
}

template <class K, class V>
bool xMap<K, V>::containsValue(V value) {
  for (int idx = 0; idx < capacity; idx++) {
    DLinkedList<Entry*>& list = table[idx];
    for (auto& pEntry : list) {
      if (valueEQ(pEntry->value, value)) {
        return true;
      }
    }
  }
  return false;
}
template <class K, class V>
bool xMap<K, V>::empty() {
  return this->size() == 0;
}

template <class K, class V>
int xMap<K, V>::size() {
  return this->count;
}
template <class K, class V>
void xMap<K, V>::clear() {
  removeInternalData();
  this->capacity = 10;
  this->count = 0;
  this->table = new DLinkedList<Entry*>[capacity];
}

template <class K, class V>
DLinkedList<K> xMap<K, V>::keys() {
  DLinkedList<K> keyList;
  for (int idx = 0; idx < capacity; idx++) {
    DLinkedList<Entry*>& list = table[idx];
    for (auto& pEntry : list) {
      keyList.add(pEntry->key);
    }
  }
  return keyList;
}

template <class K, class V>
DLinkedList<V> xMap<K, V>::values() {
  DLinkedList<V> valueList;
  for (int idx = 0; idx < capacity; idx++) {
    DLinkedList<Entry*>& list = table[idx];
    for (auto& pEntry : list) {
      valueList.add(pEntry->value);
    }
  }
  return valueList;
}
template <class K, class V>
DLinkedList<int> xMap<K, V>::clashes() {
  DLinkedList<int> clashCounts;
  for (int idx = 0; idx < capacity; idx++) {
    DLinkedList<Entry*>& list = table[idx];
    clashCounts.add(list.size());
  }
  return clashCounts;
}

template <class K, class V>
string xMap<K, V>::toString(string (*key2str)(K&), string (*value2str)(V&)) {
  stringstream os;
  string mark(50, '=');
  os << mark << endl;
  os << setw(12) << left << "capacity: " << capacity << endl;
  os << setw(12) << left << "size: " << count << endl;
  for (int idx = 0; idx < capacity; idx++) {
    DLinkedList<Entry*> list = table[idx];

    os << setw(4) << left << idx << ": ";
    stringstream itemos;
    for (auto pEntry : list) {
      itemos << " (";

      if (key2str != 0)
        itemos << key2str(pEntry->key);
      else
        itemos << pEntry->key;
      itemos << ",";
      if (value2str != 0)
        itemos << value2str(pEntry->value);
      else
        itemos << pEntry->value;

      itemos << ");";
    }
    string valuestr = itemos.str();
    if (valuestr.length() > 0)
      valuestr = valuestr.substr(0, valuestr.length() - 1);
    os << valuestr << endl;
  }
  os << mark << endl;

  return os.str();
}

////////////////////////////////////////////////////////
//                  UTILITIES
//              Code are provided
////////////////////////////////////////////////////////

/*
 * moveEntries:
 *  Purpose: move all entries in the old hash table (oldTable) to the new table
 * (newTable)
 */
template <class K, class V>
void xMap<K, V>::moveEntries(DLinkedList<Entry*>* oldTable, int oldCapacity,
                             DLinkedList<Entry*>* newTable, int newCapacity) {
  for (int old_index = 0; old_index < oldCapacity; old_index++) {
    DLinkedList<Entry*>& oldList = oldTable[old_index];
    for (auto oldEntry : oldList) {
      int new_index = this->hashCode(oldEntry->key, newCapacity);
      DLinkedList<Entry*>& newList = newTable[new_index];
      newList.add(oldEntry);
    }
  }
}

/*
 * ensureLoadFactor:
 *  Purpose: ensure the load-factor,
 *      i.e., the maximum number of entries does not exceed
 * "loadFactor*capacity"
 */
template <class K, class V>
void xMap<K, V>::ensureLoadFactor(int current_size) {
  int maxSize = (int)(loadFactor * capacity);

  // cout << "ensureLoadFactor: count = " << count << "; maxSize = " << maxSize
  // << endl;
  if (current_size > maxSize) {
    int oldCapacity = capacity;
    // int newCapacity = oldCapacity + (oldCapacity >> 1);
    int newCapacity = 1.5 * oldCapacity;
    rehash(newCapacity);
  }
}

/*
 * rehash(int newCapacity)
 *  Purpose:
 *      1. create a new hash-table with newCapacity, and
 *      2. move all the old table to to new one
 *      3. free the old table.
 */
template <class K, class V>
void xMap<K, V>::rehash(int newCapacity) {
  DLinkedList<Entry*>* pOldMap = this->table;
  int oldCapacity = capacity;

  // Create new table:
  this->table = new DLinkedList<Entry*>[newCapacity];
  this->capacity = newCapacity;  // keep "count" not changed

  moveEntries(pOldMap, oldCapacity, this->table, newCapacity);

  // remove old data: only remove nodes in list, no entry
  for (int idx = 0; idx < oldCapacity; idx++) {
    DLinkedList<Entry*>& list = pOldMap[idx];
    list.clear();
  }
  // Remove oldTable
  delete[] pOldMap;
}

/*
 * removeInternalData:
 *  Purpose:
 *      1. Remove all keys and values if users require,
 *          i.e., deleteKeys and deleteValues are not nullptr
 *      2. Remove all entry
 *      3. Remove table
 */
template <class K, class V>
void xMap<K, V>::removeInternalData() {
  // Remove user's data
  if (deleteKeys != nullptr) deleteKeys(this);
  if (deleteValues != nullptr) deleteValues(this);

  // Remove all entries in the current map
  for (int idx = 0; idx < this->capacity; idx++) {
    DLinkedList<Entry*>& list = this->table[idx];
    for (auto pEntry : list) delete pEntry;
    list.clear();
  }

  // Remove table
  delete[] table;
}

/*
 * copyMapFrom(const xMap<K,V>& map):
 *  Purpose:
 *      1. Remove all the entries of the current hash-table
 *      2. Copy (Shallow-copy only) all the entries in the input map
 *          to the current table
 */

template <class K, class V>
void xMap<K, V>::copyMapFrom(const xMap<K, V>& map) {
  //! removeInternalData(); ???

  this->capacity = 10;
  this->count = 0;
  this->table = new DLinkedList<Entry*>[capacity];

  this->hashCode = map.hashCode;
  this->loadFactor = map.loadFactor;

  this->valueEqual = map.valueEqual;
  this->keyEqual = map.keyEqual; 
  // SHOULD NOT COPY: deleteKeys, deleteValues => delete ONLY TIME in map if
  // needed

  // copy entries
  for (int idx = 0; idx < map.capacity; idx++) {
    DLinkedList<Entry*>& list = map.table[idx];
    for (auto pEntry : list) {
      this->put(pEntry->key, pEntry->value);
    }
  }
}
#endif /* XMAP_H */

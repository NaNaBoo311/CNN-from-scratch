/*
 * File:   XArrayList.h
 */

#ifndef XARRAYLIST_H
#define XARRAYLIST_H
#include "list/IList.h"
#include <memory.h>
#include <sstream>
#include <iostream>
#include <type_traits>
using namespace std;

template <class T>
class XArrayList : public IList<T>
{
public:
    class Iterator; // forward declaration

protected:
    T *data;                                 // dynamic array to store the list's items
    int capacity;                            // size of the dynamic array
    int count;                               // number of items stored in the array
    bool (*itemEqual)(T &lhs, T &rhs);       // function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(XArrayList<T> *); // function pointer: be called to remove items (if they are pointer type)

public:
    XArrayList(
        void (*deleteUserData)(XArrayList<T> *) = 0,
        bool (*itemEqual)(T &, T &) = 0,
        int capacity = 10);
    XArrayList(const XArrayList<T> &list);
    XArrayList<T> &operator=(const XArrayList<T> &list);
    ~XArrayList();

    // Inherit from IList: BEGIN
    void add(T e);
    void add(int index, T e);
    T removeAt(int index);
    bool removeItem(T item, void (*removeItemData)(T) = 0);
    bool empty();
    int size();
    void clear();
    T &get(int index);
    int indexOf(T item);
    bool contains(T item);
    string toString(string (*item2str)(T &) = 0);
    // Inherit from IList: BEGIN

    void println(string (*item2str)(T &) = 0)
    {
        cout << toString(item2str) << endl;
    }
    void setDeleteUserDataPtr(void (*deleteUserData)(XArrayList<T> *) = 0)
    {
        this->deleteUserData = deleteUserData;
    }

    Iterator begin()
    {
        return Iterator(this, 0);
    }
    Iterator end()
    {
        return Iterator(this, count);
    }

    /** free:
     * if T is pointer type:
     *     pass THE address of method "free" to XArrayList<T>'s constructor:
     *     to:  remove the user's data (if needed)
     * Example:
     *  XArrayList<Point*> list(&XArrayList<Point*>::free);
     *  => Destructor will call free via function pointer "deleteUserData"
     */
    static void free(XArrayList<T> *list)
    {
        typename XArrayList<T>::Iterator it = list->begin();
        while (it != list->end())
        {
            delete *it;
            it++;
        }
    }

protected:
    void checkIndex(int index);     // check validity of index for accessing
    void ensureCapacity(int index); // auto-allocate if needed

    /** equals:
     * if T: primitive type:
     *      indexOf, contains: will use native operator ==
     *      to: compare two items of T type
     * if T: object type:
     *      indexOf, contains: will use native operator ==
     *      to: compare two items of T type
     *      Therefore, class of type T MUST override operator ==
     * if T: pointer type:
     *      indexOf, contains: will use function pointer "itemEqual"
     *      to: compare two items of T type
     *      Therefore:
     *      (1): must pass itemEqual to the constructor of XArrayList
     *      (2): must define a method for comparing
     *           the content pointed by two pointers of type T
     *          See: definition of "equals" of class Point for more detail
     */
    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
    {
        if (itemEqual == 0)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }

    void copyFrom(const XArrayList<T> &list);

    void removeInternalData();

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  INNER CLASSES DEFNITION ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    // Iterator: BEGIN
    class Iterator
    {
    private:
        int cursor;
        XArrayList<T> *pList;

    public:
        Iterator(XArrayList<T> *pList = 0, int index = 0)
        {
            this->pList = pList;
            this->cursor = index;
        }
        Iterator &operator=(const Iterator &iterator)
        {
            cursor = iterator.cursor;
            pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            T item = pList->removeAt(cursor);
            if (removeItemData != 0)
                removeItemData(item);
            cursor -= 1; // MUST keep index of previous, for ++ later
        }

        T &operator*()
        {
            return pList->data[cursor];
        }
        bool operator!=(const Iterator &iterator)
        {
            return cursor != iterator.cursor;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            this->cursor++;
            return *this;
        }
        // Postfix ++ overload
        Iterator operator++(int)
        {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };
    // Iterator: END
};

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
XArrayList<T>::XArrayList(
    void (*deleteUserData)(XArrayList<T> *),
    bool (*itemEqual)(T &, T &),
    int capacity)
{
    // TODO
    count = 0;
    this->deleteUserData = deleteUserData;
    this->itemEqual = itemEqual;
    this->capacity = capacity;
    data = new T[capacity];
}

template <class T>
void XArrayList<T>::copyFrom(const XArrayList<T> &list)
{
    /*
     * Copies the contents of another XArrayList into this list.
     * Initializes the list with the same capacity as the source list and copies all elements.
     * Also duplicates user-defined comparison and deletion functions, if applicable.
     */
    // TODO
    this->capacity = list.capacity;
    data = new T[capacity];
    this->count = list.count;
    for (int i = 0; i < list.count; i++)
    {
        data[i] = list.data[i];
    }
    this->deleteUserData = list.deleteUserData;
    this->itemEqual = list.itemEqual;
    return;
}

template <class T>
void XArrayList<T>::removeInternalData()
{
    /*
     * Clears the internal data of the list by deleting the dynamic array and any user-defined data.
     * If a custom deletion function is provided, it is used to free the stored elements.
     * Finally, the dynamic array itself is deallocated from memory.
     */
    // TODO
    if (deleteUserData)
    {
        deleteUserData(this);
    }
    delete[] data;
    count = 0;
    capacity = 0;
    deleteUserData = nullptr;
    itemEqual = nullptr;
    return;
}

template <class T>
XArrayList<T>::XArrayList(const XArrayList<T> &list)
{
    // TODO
    copyFrom(list);
}

template <class T>
XArrayList<T> &XArrayList<T>::operator=(const XArrayList<T> &list)
{
    // TODO
    if (this != &list)
    {
        clear();
        this->capacity = list.capacity;
        this->count = list.count;
        this->data = list.data;
        this->deleteUserData = list.deleteUserData;
        this->itemEqual = list.itemEqual;
    }
    return *this;
}

template <class T>
XArrayList<T>::~XArrayList()
{
    // TODO
    if (deleteUserData != nullptr)
        deleteUserData(this);

    delete[] data;
}

template <class T>
void XArrayList<T>::add(T e)
{
    // TODO
    ensureCapacity(count);
    data[count++] = e;
    return;
}

template <class T>
void XArrayList<T>::add(int index, T e)
{
    // TODO
    if (index < 0 || index > count)
        throw out_of_range("Index is out of range!");
    ensureCapacity(count + 1);
    for (int i = count; i > index; i--)
    {
        data[i] = data[i - 1];
    }
    data[index] = e;
    count++;
    return;
}

template <class T>
T XArrayList<T>::removeAt(int index)
{
    // TODO
    if (index < 0 || index > count - 1)
        throw out_of_range("Index is out of range!");
    T value = data[index];
    for (int i = index; i < count - 1; i++)
    {
        data[i] = data[i + 1];
    }
    count--;
    return value;
}

template <class T>
bool XArrayList<T>::removeItem(T item, void (*removeItemData)(T))
{
    // TODO
    for (int i = 0; i < count; i++)
    {
        bool result = equals(data[i], item, itemEqual);
        if (result)
        {
            if (removeItemData)
            {
                removeItemData(data[i]); // Cha biet de lam gi
                removeAt(i);

                return true;
            }
            removeAt(i);
            return true;
        }
    }
    return false;
}

template <class T>
bool XArrayList<T>::empty()
{
    // TODO
    return count == 0;
}

template <class T>
int XArrayList<T>::size()
{
    // TODO
    return count;
}

template <class T>
void XArrayList<T>::clear()
{
    // TODO
    if (deleteUserData != nullptr)
    {
        deleteUserData(this);
    }

    delete[] data;
    count = 0;
    capacity = 10;
    data = new T[capacity];
    return;
}

template <class T>
T &XArrayList<T>::get(int index)
{
    // TODO
    if (index < 0 || index > count - 1)
        throw out_of_range("Index is out of range!");
    return data[index];
}

template <class T>
int XArrayList<T>::indexOf(T item)
{
    // TODO
    for (int i = 0; i < count; i++)
    {

        bool result = equals(data[i], item, itemEqual);
        if (result)
            return i;
    }
    return -1;
}
template <class T>
bool XArrayList<T>::contains(T item)
{
    // TODO
    for (int i = 0; i < count; i++)
    {
        bool result = equals(data[i], item, itemEqual);
        if (result)
            return true;
    }
    return false;
}

template <class T>
string XArrayList<T>::toString(string (*item2str)(T &))
{
    /**
     * Converts the array list into a string representation, formatting each element using a user-defined function.
     * If no function is provided, it uses the default string representation for each element.
     * Example: Given an array list with elements {1, 2, 3} and a function that converts integers to strings, calling toString would return "[1, 2, 3]".
     *
     * @param item2str A function pointer for converting items of type T to strings. If null, default to the string conversion of T.
     * @return A string representation of the array list with elements separated by commas and enclosed in square brackets.
     */

    // TODO
    string result = "[";
    // Pointer type
    if (item2str != 0)
    {
        for (int i = 0; i < count; i++)
        {
            result += item2str(data[i]) + ", ";
        }
    }

    // Object & Primitive type
    else
    {
        ostringstream oss;
        for (int i = 0; i < count; i++)
        {
            oss << data[i] << ", ";
        }
        result += oss.str();
    }

    if (result.length() > 1)
    {
        result.pop_back();
        result.pop_back();
    }

    result += "]";
    return result;
}

//////////////////////////////////////////////////////////////////////
//////////////////////// (private) METHOD DEFNITION //////////////////
//////////////////////////////////////////////////////////////////////
template <class T>
void XArrayList<T>::checkIndex(int index)
{
    /**
     * Validates whether the given index is within the valid range of the list.
     * Throws an std::out_of_range exception if the index is negative or exceeds the number of elements.
     * Ensures safe access to the list's elements by preventing invalid index operations.
     */
    // TODO
    try
    {
        if (index >= count || index < 0)
            throw std::out_of_range("Index is out of range!");
    }
    catch (out_of_range &e)
    {
        cout << e.what();
    }
    return;
}
template <class T>
void XArrayList<T>::ensureCapacity(int index)
{
    /**
     * Ensures that the list has enough capacity to accommodate the given index.
     * If the index is out of range, it throws an std::out_of_range exception. If the index exceeds the current capacity,
     * reallocates the internal array with increased capacity, copying the existing elements to the new array.
     * In case of memory allocation failure, catches std::bad_alloc.
     */
    // TODO
    if (index < 0)
        throw out_of_range("Index is out of range!");
    if (index >= capacity)
    {
        try
        {
            // create new data and copying
            capacity = index + 1;
            T *new_data = new T[capacity];

            for (int i = 0; i < count; i++)
            {
                new_data[i] = data[i];
            }

            // delete old data
            delete[] data;
            data = new_data;
        }

        catch (bad_alloc &e)
        {
            throw;
        }
    }
    return;
}

#endif /* XARRAYLIST_H */

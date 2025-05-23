/*
 * File:   DLinkedList.h
 */

#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

#include "list/IList.h"

#include <sstream>
#include <iostream>
#include <type_traits>
using namespace std;

template <class T>
class DLinkedList : public IList<T>
{
public:
    class Node;        // Forward declaration
    class Iterator;    // Forward declaration
    class BWDIterator; // Forward declaration

protected:
    Node *head; // this node does not contain user's data
    Node *tail; // this node does not contain user's data
    int count;
    bool (*itemEqual)(T &lhs, T &rhs);        // function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(DLinkedList<T> *); // function pointer: be called to remove items (if they are pointer type)

public:
    DLinkedList(
        void (*deleteUserData)(DLinkedList<T> *) = 0,
        bool (*itemEqual)(T &, T &) = 0);
    DLinkedList(const DLinkedList<T> &list);
    DLinkedList<T> &operator=(const DLinkedList<T> &list);
    ~DLinkedList();

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
    // Inherit from IList: END

    void println(string (*item2str)(T &) = 0)
    {
        cout << toString(item2str) << endl;
    }
    void setDeleteUserDataPtr(void (*deleteUserData)(DLinkedList<T> *) = 0)
    {
        this->deleteUserData = deleteUserData;
    }

    bool contains(T array[], int size)
    {
        int idx = 0;
        for (DLinkedList<T>::Iterator it = begin(); it != end(); it++)
        {
            if (!equals(*it, array[idx++], this->itemEqual))
                return false;
        }
        return true;
    }

    /*
     * free(DLinkedList<T> *list):
     *  + to remove user's data (type T, must be a pointer type, e.g.: int*, Point*)
     *  + if users want a DLinkedList removing their data,
     *      he/she must pass "free" to constructor of DLinkedList
     *      Example:
     *      DLinkedList<T> list(&DLinkedList<T>::free);
     */
    static void free(DLinkedList<T> *list)
    {
        typename DLinkedList<T>::Iterator it = list->begin();
        while (it != list->end())
        {
            delete *it;
            it++;
        }
    }

    /* begin, end and Iterator helps user to traverse a list forwardly
     * Example: assume "list" is object of DLinkedList

     DLinkedList<char>::Iterator it;
     for(it = list.begin(); it != list.end(); it++){
            char item = *it;
            std::cout << item; //print the item
     }
     */
    Iterator begin()
    {
        return Iterator(this, true);
    }
    Iterator end()
    {
        return Iterator(this, false);
    }

    /* last, beforeFirst and BWDIterator helps user to traverse a list backwardly
     * Example: assume "list" is object of DLinkedList

     DLinkedList<char>::BWDIterator it;
     for(it = list.last(); it != list.beforeFirst(); it--){
            char item = *it;
            std::cout << item; //print the item
     }
     */
    BWDIterator bbegin()
    {
        return BWDIterator(this, true);
    }
    BWDIterator bend()
    {
        return BWDIterator(this, false);
    }

protected:
    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
    {
        if (itemEqual == 0)
            return lhs == rhs;
        else
            return itemEqual(lhs, rhs);
    }
    void copyFrom(const DLinkedList<T> &list);
    void removeInternalData();
    Node *getPreviousNodeOf(int index);

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  INNER CLASSES DEFNITION ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    class Node
    {
    public:
        T data;
        Node *next;
        Node *prev;
        friend class DLinkedList<T>;

    public:
        Node(Node *next = 0, Node *prev = 0)
        {
            this->next = next;
            this->prev = prev;
        }
        Node(T data, Node *next = 0, Node *prev = 0)
        {
            this->data = data;
            this->next = next;
            this->prev = prev;
        }
    };

    //////////////////////////////////////////////////////////////////////
    class BWDIterator
    {
    private:
        DLinkedList<T> *pList;
        Node *pNode;

    public:
        BWDIterator(DLinkedList<T> *pList = 0, bool begin = true)
        {
            if (begin)
            {
                if (pList != 0)
                    this->pNode = pList->tail->prev;
                else
                    pNode = 0;
            }
            else
            {
                if (pList != 0)
                    this->pNode = pList->head;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        BWDIterator &operator=(const BWDIterator &BWDIterator)
        {
            this->pNode = BWDIterator.pNode;
            this->pList = BWDIterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pNext = pNode->next; // MUST next, so BWDIterator++ will go to end
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pNext;
            pList->count -= 1;
        }

        T &operator*()
        {
            return pNode->data;
        }
        bool operator!=(const BWDIterator &BWDIterator)
        {
            return pNode != BWDIterator.pNode;
        }
        // Prefix ++ overload
        BWDIterator &operator++()
        {
            pNode = pNode->prev;
            return *this;
        }
        // Postfix ++ overload
        BWDIterator operator++(int)
        {
            BWDIterator BWDIterator = *this;
            ++*this;
            return BWDIterator;
        }
    };

    class Iterator
    {
    private:
        DLinkedList<T> *pList;
        Node *pNode;

    public:
        Iterator(DLinkedList<T> *pList = 0, bool begin = true)
        {
            if (begin)
            {
                if (pList != 0)
                    this->pNode = pList->head->next;
                else
                    pNode = 0;
            }
            else
            {
                if (pList != 0)
                    this->pNode = pList->tail;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        Iterator &operator=(const Iterator &iterator)
        {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pNext = pNode->prev; // MUST prev, so iterator++ will go to end
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pNext;
            pList->count -= 1;
        }

        T &operator*()
        {
            return pNode->data;
        }
        bool operator!=(const Iterator &iterator)
        {
            return pNode != iterator.pNode;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            pNode = pNode->next;
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
};
//////////////////////////////////////////////////////////////////////
// Define a shorter name for DLinkedList:

template <class T>
using List = DLinkedList<T>;

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
DLinkedList<T>::DLinkedList(
    void (*deleteUserData)(DLinkedList<T> *),
    bool (*itemEqual)(T &, T &))
{
    // TODO
    this->itemEqual = itemEqual;
    this->deleteUserData = deleteUserData;
    count = 0;
    head = new Node();
    tail = new Node();
    head->next = tail;
    tail->prev = head;
}

template <class T>
DLinkedList<T>::DLinkedList(const DLinkedList<T> &list)
{
    // TODO
    this->count = list.count;
    this->deleteUserData = list.deleteUserData;
    this->head = list.head;
    this->tail = list.tail;
    this->itemEqual = list.itemEqual;
}

template <class T>
DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list)
{
    // TODO
    if (this != &list)
    {
        clear();
        this->head = list.head;
        this->tail = list.tail;
        this->deleteUserData = list.deleteUserData;
        this->itemEqual = list.itemEqual;
        this->count = list.count;
    }
    return *this;
}

template <class T>
DLinkedList<T>::~DLinkedList()
{
    // TODO
    if (count == 0) {
        return;
    }

    if (deleteUserData)
    {
        deleteUserData(this);
    }

    Node *temp = head->next;
    while (temp != tail)
    {
        Node *temp_next = temp->next;
        delete temp;
        temp = temp_next;
    }

    delete head;
    delete tail;
}

template <class T>
void DLinkedList<T>::add(T e)
{
    // TODO
    Node *new_node = new Node(e, tail, tail->prev);

    tail->prev->next = new_node;
    tail->prev = new_node;
    count++;
    return;
}
template <class T>
void DLinkedList<T>::add(int index, T e)
{
    // TODO
    if (index < 0 || index > count)
        throw out_of_range("Index is out of range!");

    // Insert first or last
    if (index == count)
    {
        add(e);
        return;
    }

    if (index == 0)
    {
        Node *new_node = new Node(e, head->next, head);
        head->next->prev = new_node;
        head->next = new_node;
        count++;
        return;
    }

    // Insert anywhere
    Node *temp = head;
    for (int i = 0; i < index; i++)
    {
        temp = temp->next;
    }

    Node *new_node = new Node(e, temp->next, temp);
    temp->next->prev = new_node;
    temp->next = new_node;
    count++;
    return;
}

template <class T>
typename DLinkedList<T>::Node *DLinkedList<T>::getPreviousNodeOf(int index)
{
    /**
     * Returns the node preceding the specified index in the doubly linked list.
     * If the index is in the first half of the list, it traverses from the head; otherwise, it traverses from the tail.
     * Efficiently navigates to the node by choosing the shorter path based on the index's position.
     */
    // TODO
    if (index < 0 || index >= count)
        throw out_of_range("Index is out of range!");
    if (index == 0)
        return nullptr;
    if (index == count - 1)
        return tail->prev->prev;

    int dis_to_head = index;
    int dis_to_tail = count - index;
    // Go from head
    if (dis_to_head <= dis_to_tail)
    {
        Iterator it = begin();
        for (int i = 1; i < dis_to_head; i++)
            it++;
        return it.pNode;
    }

    // Go from tail
    else
    {
        Node *temp = tail->prev;
        for (int i = 1; i < dis_to_tail; i++)
        {
            temp = temp->prev;
        }
        return temp->prev;
    }
    return nullptr;
}

template <class T>
T DLinkedList<T>::removeAt(int index)
{
    // TODO
    if (index < 0 || index >= count)
        throw out_of_range("Index is out of range!");
    T value;

    // Easy index
    if (index == 0 && count > 0)
    {
        Node *temp = head->next; // for delete
        value = head->next->data;
        head->next = head->next->next;
        head->next->prev = head;
        count--;
        delete temp;
        return value;
    }
    if (index == count - 1)
    {
        Node *temp = tail->prev; // for delete
        value = tail->prev->data;
        tail->prev->prev->next = tail;
        tail->prev = tail->prev->prev;
        count--;
        delete temp;
        return value;
    }

    // Anywhere
    Node *prev_target = head->next;
    for (int i = 1; i < index; i++)
    { // Go before target
        prev_target = prev_target->next;
    }
    Node *temp = prev_target->next;
    prev_target->next->next->prev = prev_target;
    prev_target->next = prev_target->next->next;
    count--;
    delete temp;
    return value;
}

template <class T>
bool DLinkedList<T>::empty()
{
    // TODO
    return count == 0;
}

template <class T>
int DLinkedList<T>::size()
{
    // TODO
    return count;
}

template <class T>
void DLinkedList<T>::clear()
{
    // TODO
    if (deleteUserData != nullptr)
    {
        deleteUserData(this);
    }

    count = 0;
    Node *temp = head->next;
    while (temp != tail)
    {
        Node *temp_next = temp->next;
        delete temp;
        temp = temp_next;
    }

    delete head;
    delete tail;
    return;
}

template <class T>
T &DLinkedList<T>::get(int index)
{
    // TODO
    if (index < 0 || index >= count)
        throw out_of_range("Index is out of range!");

    Node *temp = head->next;
    for (int i = 0; i < index; i++)
    {
        temp = temp->next;
    }
    return temp->data;
}

template <class T>
int DLinkedList<T>::indexOf(T item)
{
    // TODO
    Node *temp = head->next;
    int index = 0;
    while (temp != tail)
    {
        bool result = equals(temp->data, item, itemEqual);
        if (result)
            return index;
        index++;
        temp = temp->next;
    }
    return -1;
}

template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T))
{
    // TODO
    Node *temp = head->next;
    int index = 0;
    while (temp != tail)
    {
        bool result = equals(temp->data, item, itemEqual);
        if (result)
        {
            if (removeItemData)
            {
                removeItemData(temp->data);
                removeAt(index);
                return true;
            }
            removeAt(index);
            return true;
        }
        index++;
        temp = temp->next;
    }

    return false;
}

template <class T>
bool DLinkedList<T>::contains(T item)
{
    // TODO
    Node *temp = head->next;
    while (temp != tail)
    {
        bool result = equals(temp->data, item, itemEqual);
        if (result)
            return true;
        temp = temp->next;
    }
    return false;
}

template <class T>
string DLinkedList<T>::toString(string (*item2str)(T &))
{
    /**
     * Converts the list into a string representation, where each element is formatted using a user-provided function.
     * If no custom function is provided, it directly uses the element's default string representation.
     * Example: If the list contains {1, 2, 3} and the provided function formats integers, calling toString would return "[1, 2, 3]".
     *
     * @param item2str A function that converts an item of type T to a string. If null, default to string conversion of T.
     * @return A string representation of the list with elements separated by commas and enclosed in square brackets.
     */
    // TODO
    string result = "[";
    // Pointer type
    if (item2str != 0)
    {
        Node *temp = head->next;
        while (temp != tail)
        {
            result += item2str(temp->data) + ", ";
            temp = temp->next;
        }
    }

    // Object & Primitive type
    else
    {
        ostringstream oss;
        Node *temp = head->next;
        ;
        while (temp != tail)
        {
            oss << temp->data << ", ";
            temp = temp->next;
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

template <class T>
void DLinkedList<T>::copyFrom(const DLinkedList<T> &list)
{
    /**
     * Copies the contents of another doubly linked list into this list.
     * Initializes the current list to an empty state and then duplicates all data and pointers from the source list.
     * Iterates through the source list and adds each element, preserving the order of the nodes.
     */
    // TODO
    if (this != &list)
    {
        clear();
        Node *source_head = list.head->next;
        while (source_head != list.tail)
        {
            add(source_head->data); // Duplicates all data not make 2 pointers point to the same address so that the value is the same.
            source_head = source_head->next;
        }
        this->deleteUserData = list.deleteUserData;
        this->itemEqual = list.itemEqual;
        this->count = list.count;
    }
}

template <class T>
void DLinkedList<T>::removeInternalData()
{
    /**
     * Clears the internal data of the list by deleting all nodes and user-defined data.
     * If a custom deletion function is provided, it is used to free the user's data stored in the nodes.
     * Traverses and deletes each node between the head and tail to release memory.
     */
    // TODO
    clear();
    itemEqual = nullptr;
    return;
}

#endif /* DLINKEDLIST_H */

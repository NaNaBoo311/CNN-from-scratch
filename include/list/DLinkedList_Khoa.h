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
    class BWDIterator {
    private:
    DLinkedList<T> *pList;
    Node *currentNode;

public:
    BWDIterator(DLinkedList<T> *pList, bool begin) {
      this->pList = pList;
      if (begin) {
        currentNode = pList->tail->prev;
      } else {
        currentNode = pList->head;
      }
    }

    BWDIterator &operator=(const BWDIterator &iterator) {
        this->currentNode = iterator.currentNode;
        this->pList = iterator.pList;
        return *this;
    }
    void remove(void (*removeItemData)(T) = 0) {
      Node* pNode = currentNode;
      pNode->next->prev = pNode->prev;
      pNode->prev->next = pNode->next;
      Node* pPrev = pNode->next; 
      if (removeItemData != 0) removeItemData(pNode->data);
      delete pNode;
      pNode = pPrev;
      pList->count -= 1;
    }

    T &operator*() { 
      return currentNode->data; 
    }

    bool operator!=(const BWDIterator &iterator) {
        return this->currentNode != iterator.currentNode;
    }

    BWDIterator &operator--() {
      if (currentNode != nullptr) {
        currentNode = currentNode->prev;
      }
      return *this;
    }

    BWDIterator operator--(int) {
      BWDIterator temp = *this;  
      --(*this); 
      return *this;
    }

    BWDIterator &operator++() {
      if (currentNode != nullptr) {
        currentNode = currentNode->prev;
      }
      return *this;
    }

    BWDIterator operator++(int) {
      BWDIterator temp = *this;  
      ++(*this); 
      return *this;
    }
};

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

template <typename T>
DLinkedList<T>::DLinkedList(
    void (*deleteUserData)(DLinkedList<T> *),
    bool (*itemEqual)(T &, T &))
{
    // Initialize head and tail nodes
    this->head = new Node();
    this->tail = new Node();
    // Link head and tail
    this->head->next = this->tail;
    this->tail->prev = this->head;
    // Initialize count
    this->count = 0;
    // Assign function pointers
    this->deleteUserData = deleteUserData;
    this->itemEqual = itemEqual;
    // Initialize head and tail nodes
    this->head->prev = nullptr;
    this->tail->next = nullptr;
}

template <class T>
DLinkedList<T>::DLinkedList(const DLinkedList<T> &list)
{
     // Initialize head and tail nodes
    this->head = new Node();
    this->tail = new Node();

    // Link head and tail
    this->head->next = this->tail;
    this->tail->prev = this->head;
    this->head->prev = nullptr;
    this->tail->next = nullptr;

    // Initialize count
    this->count = 0;

    // Copy the function pointers
    this->deleteUserData = list.deleteUserData;
    this->itemEqual = list.itemEqual;

    // Iterate through the source list and add each element to the new list
    Node* current = list.head->next;
    while (current != list.tail) {
        add(current->data);
        current = current->next;
    }
}

template <class T>
DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list)
{
    // Check for self-assignment
    if (this == &list) {
        return *this;
    }
    // Remove all internal data
    removeInternalData();
    // Copy the contents from the given list
    copyFrom(list);

    return *this;
}

template <class T>
DLinkedList<T>::~DLinkedList()
{
    // Remove all internal data
    removeInternalData();
    // Delete the head and tail nodes
    delete this->head;
    delete this->tail;
}

template <class T>
void DLinkedList<T>::add(T e)
{
    // Create a new node with the given element
    Node* newNode = new Node(e);

    // Link the new node with the current tail's previous node and the tail
    newNode->prev = this->tail->prev;
    newNode->next = this->tail;

    // Update the previous node's next pointer and the tail's previous pointer
    this->tail->prev->next = newNode;
    this->tail->prev = newNode;

    // Increment the count of elements in the list
    this->count++;
}
template <class T>
void DLinkedList<T>::add(int index, T e)
{
    if (index < 0 || index > this->count)
    {
        throw std::out_of_range("Index is out of range!");
    }

    Node* newNode = new Node(e);
    Node* prevNode = getPreviousNodeOf(index);

    newNode->next = prevNode->next;
    newNode->prev = prevNode;
    prevNode->next->prev = newNode;
    prevNode->next = newNode;

    this->count++;
}

template <class T>
typename DLinkedList<T>::Node *DLinkedList<T>::getPreviousNodeOf(int index)
{
    /**
     * Returns the node preceding the specified index in the doubly linked list.
     * If the index is in the first half of the list, it traverses from the head; otherwise, it traverses from the tail.
     * Efficiently navigates to the node by choosing the shorter path based on the index's position.
     */
    if (index < 0 || index > this->count) {
        throw std::out_of_range("Index is out of range!");
    }

    Node* current;
    if (index < this->count / 2) {
        current = this->head;
        for (int i = 0; i <= index; ++i) {
            current = current->next;
        }
    } else {
        current = this->tail;
        for (int i = this->count; i > index; --i) {
            current = current->prev;
        }
    }

    return current->prev;
}

template <class T>
T DLinkedList<T>::removeAt(int index)
{
    if (index < 0 || index >= this->count)
    {
        throw std::out_of_range("Index is out of range!");
    }

    // Get the node to be removed
    Node* nodeToRemove = getPreviousNodeOf(index)->next;

    // Unlink the node from the list
    nodeToRemove->prev->next = nodeToRemove->next;
    nodeToRemove->next->prev = nodeToRemove->prev;

    // Store the data to return
    T data = nodeToRemove->data;

    // Delete the node
    delete nodeToRemove;

    // Decrement the count
    this->count--;

    return data;
}

template <class T>
bool DLinkedList<T>::empty()
{
    if(count == 0) return true;
    return false;
}

template <class T>
int DLinkedList<T>::size()
{
    return this->count;
}

template <class T>
void DLinkedList<T>::clear()
{
    removeInternalData();
}

template <class T>
T &DLinkedList<T>::get(int index)
{
    if (index < 0 || index >= this->count)
    {
        throw std::out_of_range("Index is out of range!");
    }

    Node* current = getPreviousNodeOf(index)->next;
    return current->data;
}

template <class T>
int DLinkedList<T>::indexOf(T item)
{
    Node* current = this->head->next;
    int index = 0;

    while (current != this->tail) {
        if (equals(current->data, item, this->itemEqual)) {
            return index;
        }
        current = current->next;
        index++;
    }

    return -1; // Item not found
}

template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T))
{
    Node* current = this->head->next;
    while (current != this->tail) {
        if (equals(current->data, item, this->itemEqual)) {
            // Unlink the node from the list
            current->prev->next = current->next;
            current->next->prev = current->prev;

            // If a custom remove function is provided, call it
            if (removeItemData != nullptr) {
                removeItemData(current->data);
            }

            // Delete the node
            delete current;

            // Decrement the count
            this->count--;

            return true;
        }
        current = current->next;
    }
    return false;
}

template <class T>
bool DLinkedList<T>::contains(T item)
{
    // Start from the first element
    Node* current = head->next;
    // Traverse until the tail
    while (current != tail) { 
        if (equals(current->data, item, itemEqual)) {
            return true;
        }
        current = current->next; // Move to the next node
    }
    return false; // Item not found
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
    ostringstream oss;
    oss << "[";
    // Iterate through the list and convert each element to a string
    Node* current = head->next;
    while (current != tail) {
        if (item2str) {
            oss << item2str(current->data);
        } else {
            oss << current->data;
        }
        // Add a comma and space if it's not the last element
        if (current->next != tail) {
            oss << ", ";
        }
        current = current->next;
    }
    oss << "]";
    return oss.str();
}

template <class T>
void DLinkedList<T>::copyFrom(const DLinkedList<T> &list)
{
    /**
     * Copies the contents of another doubly linked list into this list.
     * Initializes the current list to an empty state and then duplicates all data and pointers from the source list.
     * Iterates through the source list and adds each element, preserving the order of the nodes.
     */
    // Iterate through the source list and add each element to the new list
    Node* current = list.head->next;
    while (current != list.tail) {
        add(current->data);
        current = current->next;
    }
}

template <class T>
void DLinkedList<T>::removeInternalData()
{
    /**
     * Resets the list to its initial state by deleting all nodes between head and tail.
     * If deleteUserData is set, it will be called to delete user data.
     */
    Node* current = this->head->next;
    while (current != this->tail) {
        Node* nextNode = current->next;
        if (this->deleteUserData) {
            this->deleteUserData(this);
        }
        delete current;
        current = nextNode;
    }
    this->head->next = this->tail;
    this->tail->prev = this->head;
    this->count = 0;
}



#endif /* DLINKEDLIST_H */


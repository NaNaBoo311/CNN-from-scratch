#include <iostream>
#include "hash/xMap.h"
#include "heap/Heap.h"
using namespace std;


void hash1() { //test put
    int nsamples = 10;
    int keys[] = {2, 12, 42, 72, 3, 45, 76, 30, 21, 2};
    int values[] = {35, 67, 100, 23, 68, 68, 72, 45, 19, 22};
    xMap<int, int> hash(&xMap<int, int>::intKeyHash);

    for (int idx = 0; idx < nsamples; idx++) {
        hash.put(keys[idx], values[idx]);
    }
    hash.println();
    /*Output: 
        ==================================================
        capacity:   15
        size:       9
        0   :  (45,68); (30,45)
        1   :  (76,72)
        2   :  (2,22)
        3   :  (3,68)
        4   :
        5   :
        6   :  (21,19)
        7   :
        8   :
        9   :
        10  :
        11  :
        12  :  (12,67); (42,100); (72,23)
        13  :
        14  :
        ==================================================
        */
}

void hash2() { //test get
    int nsamples = 10;
    int keys[] = {2, 12, 42, 72, 3, 45, 76, 30, 21, 2};
    int values[] = {35, 67, 100, 23, 68, 68, 72, 45, 19, 22};
    xMap<int, int> hash(&xMap<int, int>::intKeyHash);

    for (int idx = 0; idx < nsamples; idx++) {
        hash.put(keys[idx], values[idx]);
    }

    for (int i = 0; i < nsamples; i++) {
        cout << hash.get(keys[i]) << " "; //Output: 22 67 100 23 68 68 72 45 19 22
    }

    cout << endl << hash.get(100) << endl; //Output: key(100) not found
}

void hash3() { //test contains
    int nsamples = 10;
    int keys[] = {2, 12, 42, 72, 3, 45, 76, 30, 21, 2};
    int values[] = {35, 67, 100, 23, 68, 68, 72, 45, 19, 22};
    xMap<int, int> hash(&xMap<int, int>::intKeyHash);

    for (int idx = 0; idx < nsamples; idx++) {
        hash.put(keys[idx], values[idx]);
    }

    cout << hash.containsKey(0) << endl;
    cout << hash.containsValue(-100) << endl;

}

void hash4() { //test keys, values
    int nsamples = 10;
    int keys[] = {2, 12, 42, 72, 3, 45, 76, 30, 21, 2};
    int values[] = {35, 67, 100, 23, 68, 68, 72, 45, 19, 22};
    xMap<int, int> hash(&xMap<int, int>::intKeyHash);
    xMap<int, int> empty_hash(&xMap<int, int>::intKeyHash);

    for (int idx = 0; idx < nsamples; idx++) {
        hash.put(keys[idx], values[idx]);
    }  

    DLinkedList<int> table_keys = hash.keys();
    DLinkedList<int> table_values = hash.values();

    table_keys.println();   //Output: [45, 30, 76, 2, 3, 21, 12, 42, 72]
    table_values.println(); //Output: [68, 45, 72, 22, 68, 19, 67, 100, 23]

    DLinkedList<int> empty_table_keys = empty_hash.keys();
    DLinkedList<int> empty_table_values = empty_hash.values();

    empty_table_keys.println();     //Output: []
    empty_table_values.println();   //Output: []

    
}

void hash5() { //test clashes
    int nsamples = 10;
    int keys[] = {2, 12, 22, 32, 3, 13, 33, 4, 5, 6};
    int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    xMap<int, int> hash(&xMap<int, int>::intKeyHash);

    for (int idx = 0; idx < nsamples; idx++) {
        hash.put(keys[idx], values[idx]);
    }

    hash.println();
    DLinkedList<int> table_clashes = hash.clashes();
    table_clashes.println(); //Output: [0, 0, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0]


}

void hash6(int index = 0) {//test remove

    if (index == 0) {
        int nsamples = 10;
        int keys[] = {2, 12, 42, 72, 3, 45, 76, 30, 21, 2};
        int values[] = {35, 67, 100, 23, 68, 68, 72, 45, 19, 22};
        xMap<int, int> hash(&xMap<int, int>::intKeyHash);

        for (int idx = 0; idx < nsamples; idx++) {
            hash.put(keys[idx], values[idx]);
        } 
        cout << "==================Before=============" << endl;
        hash.println();
        cout << "==================After=============" << endl;
        hash.remove(2);
        hash.remove(12,67);
        hash.println();
    }


    else if (index == 1) {
        int nsamples = 10;
        int keys[] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
        int values[] = {35, 67, 100, 23, 68, 68, 72, 45, 19, 22};
        xMap<int, int> hash(&xMap<int, int>::intKeyHash);

        for (int idx = 0; idx < nsamples; idx++) {
            hash.put(keys[idx], values[idx]);
        } 

        hash.remove(2);
        hash.println(); //Output: no more keys and values
        hash.put(2, 10);
        hash.println();
    }

}

void hash7() { //test without '&' and with '&'
    int nsamples = 10;
    int keys[] = {2, 12, 42, 72, 3, 45, 76, 30, 21, 2};
    int values[] = {35, 67, 100, 23, 68, 68, 72, 45, 19, 22};
    xMap<int, int> hash(&xMap<int, int>::intKeyHash);
    for (int i = 0; i < nsamples; i++) {
        hash.put(keys[i], values[i]);
    }
    
    hash.xMap_Test1();

}

void hash8() { //test clear
    int nsamples = 3;
    string keys[] = {"first", "second", "third"};
    int values[] = {1, 2, 3};
    xMap<string, int> hash(&xMap<string, int>::stringKeyHash);
    for (int i = 0; i < nsamples; i++) {
        hash.put(keys[i], values[i]);
    }
    hash.println();
    hash.clear();
    hash.put(keys[0], values[0]);
    hash.println();
}

void hash9() { //test operator '=' and copyFromMap
    int nsamples = 3;
    string keys[] = {"first", "second", "third"};
    int values[] = {1, 2, 3};
    xMap<string, int> hash(&xMap<string, int>::stringKeyHash);
    for (int i = 0; i < nsamples; i++) {
        hash.put(keys[i], values[i]);
    }
    xMap<string, int> hash_2(hash);
    hash_2.put("fourth", 4);
    hash = hash_2;
    hash_2.println();
    hash.println();

}

/* 
=====================================EXPERIENCES===================================
    1/ it will call xMap destructor first, then list

*/

int MinHeapCompartor(int& lhs, int& rhs){
    if(lhs < rhs) return -1;
    else if(lhs > rhs) return +1;
    else return 0;
}

int MaxHeapCompartor(int& lhs, int& rhs){
    if(lhs < rhs) return +1;
    else if(lhs > rhs) return -1;
    else return 0;
}


void heap1() {//test pop
    Heap<int> minHeap(&MinHeapCompartor);
    int nsamples = 4;
    int arr[] = {50, 20, 15, 10, 8, 6, 7, 23}; 
    int arr2[] = {25, 18, 15, 15};
    for (int i = 0; i < nsamples; i++) {
        minHeap.push(arr2[i]);
    }
    minHeap.println();
    minHeap.pop();
    minHeap.pop();
    minHeap.pop();
    minHeap.pop();
    minHeap.println();
    for (int i = 0; i < nsamples; i++) {
        minHeap.push(arr2[i]);
    }
    minHeap.println();
}

void heap2() {//test pop
    Heap<int> maxHeap(&MaxHeapCompartor);
    int nsamples = 7;
    int arr[] = {50, 20, 15, 10, 8, 6, 7, 23, 27, 18, 19, 11}; 
    int arr2[] = {25, 18, 15, 20, 60, 19, 10}; 
    for (int i = 0; i < nsamples; i++) {
        maxHeap.push(arr2[i]);
    }
    maxHeap.pop();
    maxHeap.println();

}

void heap3() {//test clear
    Heap<int> maxHeap(&MaxHeapCompartor);
    int nsamples = 7;
    int arr[] = {50, 20, 15, 10, 8, 6, 7, 23, 27, 18, 19, 11}; 
    int arr2[] = {25, 18, 15, 20, 60, 19, 10}; 
    for (int i = 0; i < nsamples; i++) {
        maxHeap.push(arr2[i]);
    }
    maxHeap.clear();
    maxHeap.println();
}

void heap4() {//test copyFrom, operator "="
    Heap<int> maxHeap(&MaxHeapCompartor);
    int nsamples = 7;
    int arr[] = {50, 20, 15, 10, 8, 6, 7, 23, 27, 18, 19, 11}; 
    int arr2[] = {25, 18, 15, 20, 60, 19, 10}; 
    for (int i = 0; i < nsamples; i++) {
        maxHeap.push(arr2[i]);
    }  

    Heap<int> maxHeap_2(maxHeap);
    maxHeap_2.println();
    // maxHeap_2.clear();
    maxHeap_2 = maxHeap;
    maxHeap_2.println();

}

void heap5() {//test heapify
    Heap<int> maxHeap(&MaxHeapCompartor);
    Heap<int> minHeap(&MinHeapCompartor);
    int nsamples = 7;
    int arr[] = {50, 20, 15, 10, 8, 6, 7, 23, 27, 18, 19, 11}; 
    int arr2[] = {25, 18, 15, 20, 60, 19, 10}; 

    maxHeap.heapify(arr, 12);
    minHeap.heapify(arr, 12);

    maxHeap.println();
    minHeap.println();

    cout << "Clear" << endl;
    maxHeap.clear();
    minHeap.clear();

    maxHeap.heapify(arr, 7);
    minHeap.heapify(arr, 7);

    maxHeap.println();
    minHeap.println();    

}


int main() {
    // hash1();
    // hash2();
    // hash3();
    // hash4();
    // hash5();
    // hash6(0);
    // hash7();
    // hash8();
    // hash9();



    // heap1();
    // heap2();
    // heap3();
    // heap4();
    heap5();

    
}
#include <iostream>
#include <string>
#include <vector>
#include <functional>

template <typename T>
class HashMap
{
public:
    HashMap(double max_load = 0.75); // constructor
    
    ~HashMap(); // destructor; deletes all of the items in the hashmap
    
    int size() const; // return the number of associations in the hashmap
    
    // The insert method associates one item (key) with another (value).
    // If no association currently exists with that key, this method inserts
    // a new association into the hashmap with that key/value pair. If there is
    // already an association with that key in the hashmap, then the item
    // associated with that key is replaced by the second parameter (value).
    // Thus, the hashmap must contain no duplicate keys.
    void insert(const std::string& key, const T& value);

     // If no association exists with the given key, return nullptr; otherwise,
     // return a pointer to the value associated with that key. This pointer can be
     // used to examine that value within the map.
     const T* find(const std::string& key) const;
    
     // If no association exists with the given key, return nullptr; otherwise,
     // return a pointer to the value associated with that key. This pointer can be
     // used to examine that value or modify it directly within the map.
     T* find(const std::string& key) {
     const auto& hm = *this;
     return const_cast<T*>(hm.find(key));
     }
    
    // Defines the bracket operator for HashMap, so you can use your map like this:
    // your_map["david"] = 2.99;
    // If the key does not exist in the hashmap, this will create a new entry in
    // the hashmap and map it to the default value of type T. Then it will return a
    // reference to the newly created value in the map.
    T& operator[](const std::string& key);
private:
    struct Node {
        std::string key;
        T value;
        Node* next;
        Node(const std::string& k, const T& v): key(k), value(v), next(nullptr) {} //Constructor
    };
    std::vector<Node*> buckets; //a vector containing pointers to Nodes
    size_t numAssociations;
    double m_maxLoad;
    size_t currentCapacity;
    //list<string, T> buckets[];
    
    void rehash(){
        //need to create new internal hash map with double the current number of buckets
        //rehash all items from the current hash map into the new, larger hash map
        //replace the current hash map with the new hash map
        //free the memory associated with the original, smaller hash map
//        list<map<string, T>> new_buckets[] = new list<map<string, T>>[buckets.size()*2];
        //#1
        std::vector<Node*> new_buckets = std::vector<Node*>(currentCapacity * 2, nullptr);
//        for (int i = 0; i < new_buckets.size(); i++){
//            new_buckets[i] = new list<map<string, T>>;
//        }
        //#2
        for (int i = 0; i < currentCapacity; i++){
            Node* curr = buckets[i];
            while (curr != nullptr){
                Node* nextNode = curr->next;
                size_t hashValue = std::hash<std::string>()(curr->key);
                size_t bucketNum = hashValue % (currentCapacity * 2);
                curr->next = new_buckets[bucketNum];
                new_buckets[bucketNum] = curr;
                curr = nextNode;
            }
        }
//        //#4
//        for (size_t i = 0; i < currentCapacity; i++){
//            Node* current = buckets[i];
//            while (current != nullptr){
//                Node* n = current;
//                current = current->next;
//                delete n;
//            }
//        }
        
        //#4
        for (size_t i = 0; i < currentCapacity; i++){
            buckets[i] = nullptr; // Clear original buckets (optional)
        }
        
        //#3
        buckets.swap(new_buckets);
        currentCapacity = currentCapacity * 2;
    }
};

//HASHMAP IMPLEMENTATIONS
template <typename T>
HashMap<T>::HashMap(double max_load){
    if (max_load <= 0) m_maxLoad = 0.75;
    m_maxLoad = max_load;
    numAssociations = 0;
    currentCapacity = 10;
    buckets = std::vector<Node*>(currentCapacity, nullptr);
    //buckets is a vector with 10 buckets, each holding a linked list of Nodes
//    for (int i = 0; i < 10; i++){
//        buckets[i] = new list<map<string, T>>;
//    }
}

template <typename T>
HashMap<T>::~HashMap(){
    for (size_t i = 0; i < currentCapacity; i++){
        Node* current = buckets[i];
        while (current != nullptr){
            Node* n = current;
            current = current->next;
            delete n;
        }
    }
}

template <typename T>
int HashMap<T>::size() const{
    return static_cast<int>(numAssociations);
}

template <typename T>
/*load factor = max # of values to insert / total buckets in the array */
void HashMap<T>::insert(const std::string& key, const T& value){
    size_t hashValue = std::hash<std::string>()(key);
    size_t bucketNum = hashValue % currentCapacity;
    Node* curr = buckets[bucketNum];
    while (curr != nullptr){
        if (curr->key == key){
            curr->value = value; //if key already exists in hash table, update its value
            return;
        }
        curr = curr->next;
    }
    
    //if the key was not found, insert it into the hash table into the front
    Node* newNode = new Node(key, value);
    newNode->next = buckets[bucketNum];
    buckets[bucketNum] = newNode;
    numAssociations++;
    
    if ((size())/(currentCapacity) > m_maxLoad){
        rehash();
    }
}

template <typename T>
const T* HashMap<T>::find(const std::string& key) const{
    size_t hashValue = std::hash<std::string>()(key);
    size_t bucketNum = hashValue % currentCapacity;
    Node* curr = buckets[bucketNum];
    while (curr != nullptr){
        if (curr->key == key){
            return &curr->value;
        }
        curr = curr->next;
    }
    return nullptr;
}

template <typename T>
T& HashMap<T>::operator[](const std::string& key){
    T* result = find(key);
    if (result == nullptr){
        //add the key and temporary value into the list
        insert(key, T()); //calls T's default constructor
        return *find(key);
    }
    return *result;
}

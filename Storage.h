#pragma once
// Generic template class for storing entities in a fixed-size array

#ifndef STORAGE_H
#define STORAGE_H

template <typename T>
class Storage {
private:
    T data[100];  
    int count;    // for counting current number of entities

public:
    // Constructor
    Storage() : count(0) {}

    // Destructor
    ~Storage() {}

    // Add an entity to the storage
    // Returns true if added successfully, false if storage is full
    bool add(const T& item) {
        if (count >= 100) return false;
        data[count] = item;
        count++;
        return true;
    }

    // Remove an entity by its ID
    // Returns true if removed, false if not found
    bool removeById(int id) {
        for (int i = 0; i < count; i++) {
            if (data[i].getId() == id) {
                // Shift remaining elements left
                for (int j = i; j < count - 1; j++) {
                    data[j] = data[j + 1];
                }
                count--;
                return true;
            }
        }
        return false;
    }

    // Find an entity by ID,returns pointer or nullptr if not found
    T* findById(int id) {
        for (int i = 0; i < count; i++) {
            if (data[i].getId() == id) {
                return &data[i];
            }
        }
        return nullptr;
    }

    // Get a const pointer to entity by ID  
    const T* findById(int id) const {
        for (int i = 0; i < count; i++) {
            if (data[i].getId() == id) {
                return &data[i];
            }
        }
        return nullptr;
    }

    // Get pointer to the internal array (for iteration)
    T* getAll() {
        return data;
    }

    // Get const pointer to the internal array
    const T* getAll() const {
        return data;
    }

    // Get a specific element by index
    T& getAt(int index) {
        return data[index];
    }

    // Get const reference by index
    const T& getAt(int index) const {
        return data[index];
    }

    // Get current count of stored entities
    int size() const {
        return count;
    }

    // Check if storage is empty
    bool isEmpty() const {
        return count == 0;
    }

    // Check if storage is full
    bool isFull() const {
        return count >= 100;
    }

    // Clear all entries
    void clear() {
        count = 0;
    }

    // Find the maximum ID in the storage
    int getMaxId() const {
        int maxId = 0;
        for (int i = 0; i < count; i++) {
            if (data[i].getId() > maxId) {
                maxId = data[i].getId();
            }
        }
        return maxId;
    }

    // Update an entity (replace the entity with matching ID)
    bool update(const T& item) {
        for (int i = 0; i < count; i++) {
            if (data[i].getId() == item.getId()) {
                data[i] = item;
                return true;
            }
        }
        return false;
    }

    // Subscript operator for easy access
    T& operator[](int index) {
        return data[index];
    }

    const T& operator[](int index) const {
        return data[index];
    }
};

#endif

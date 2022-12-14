#pragma once

#include <stdlib.h>
#include <string.h>

#include "core.h"

namespace sim {
  template <typename T>
  struct Vector {
    T* data;
    usize length;
    usize capacity;

    Vector() : data(nullptr), length(0), capacity(0) {}

    // Unchecked bounds for now.
    T& operator[](usize i) { return data[i]; }
    T& back() { return data[length - 1]; }
    const T& operator[](usize i) const { return data[i]; }
    const T& back() const { return data[length - 1]; }

    void reserve(usize new_capacity) {
      if (new_capacity <= capacity)
        return;
      
      // We assume allocation always works.
      T* old = data;
      data = (T*)malloc(new_capacity * sizeof(T));
      memcpy(data, old, length * sizeof(T));
      free(old);

      capacity = new_capacity;
    }

    void clear() {
      for (usize i = 0; i < length; ++i)
        data[i].~T();

      length = 0;
    }

    void release() {
      clear();
      free(data);

      data = nullptr;
      capacity = 0;
    }

    // Returns a reference to the element that was just added.
    void push(const T& t) {
      if (length == capacity)
        reserve((usize)((f64)capacity * 1.5) + 1);
      
      data[length++] = t;
    }

    void pop() {
      if (!length)
        return;

      data[--length].~T();
    }
  };
}

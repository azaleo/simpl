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

    // Unchecked bounds for now
    T& operator[](usize i) { return data[i]; }
    const T& operator[](usize i) const { return data[i]; }

    void reserve(usize new_capacity) {
      if (new_capacity <= capacity)
        return;
      
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

    void push(const T& t) {
      if (length == capacity)
        reserve(capacity * 2 + 1);
      
      data[length++] = t;
    }

    void pop() {
      if (!length)
        return;

      data[--length].~T();
    }
  };
}

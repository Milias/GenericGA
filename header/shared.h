/*
The MIT License (MIT)
Copyright (c) 2016 Francisco García Flórez

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once
#include <functional>
#include <stdio.h>
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <queue>
#include <random>
#include <ctime>

using namespace std::placeholders;

/*
struct Wrapper<T>:
  Class that encapsulates a pointer to an object of type T,
  so it is possible to sort a list of T objects without making
  any copy.

  The encapsulated object can be accessed by the overloaded
  operator ->, and some operators (<, ==, !=) are implemented
  using those of the underlying object.

  DeleteObject() shouldn't be used lightly.
*/
template <class T> struct Wrapper
{
  T * Object;

  Wrapper() : Object(NULL) {}
  Wrapper(const Wrapper<T> & w) : Object(w.Object) {}
  ~Wrapper() {}

  Wrapper<T> & operator=(const Wrapper<T> & w) {
    Object = w.Object;
    return *this;
  }

  Wrapper<T> & operator=(T * w) {
    if (Object != NULL) delete Object;
    Object = w;
    return *this;
  }

  T * operator->() const { return Object; }
  bool operator<(const Wrapper<T> & w) const { return Object != NULL ? (*Object) < (*w.Object) : false; }
  bool operator==(const Wrapper<T> & w) const { return Object != NULL ? (*Object) == (*w.Object) : false; }
  bool operator!=(const Wrapper<T> & w) const { return Object != NULL ? (*Object) != (*w.Object) : true; }

  void DeleteObject() {
    delete Object;
    Object = NULL;
  }
};

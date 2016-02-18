/*
The MIT License (MIT)
Copyright (c) 2016 Francisco García Flórez

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once
#include "shared.h"

/*
class Chromosome<T1,T2>:
  General templated class for chromosomes. It is a pure virtual
  class, as Mutation(), Crossover() and Fitness() have to be
  implemented.

  T1 is the type of the SharedData variable, which is not used
  directly by this class, but by GeneticAlgorithm and derived
  classes. As its name suggest, SharedData points to an object
  accessible by any other Chromosome, much in a way a static variable
  works.

  A basic implementation can be seen in "ga.hpp", and further
  particularizations should be derived from it. GeneticAlgorithm
  assumes that the variables defined in this base class are
  defined in the used type.

  T2 is the type of LocalData, where values specific to this
  Chromosome are stored. A basic implementation can be also
  seen in "ga.hpp".
*/
template <class T1, class T2> class Chromosome
{
protected:
  T1 * SharedData;
  T2 LocalData;

public:
  double FitnessValue;

  Chromosome() : SharedData(NULL), FitnessValue(0.0) {}
  Chromosome(const Chromosome<T1,T2> & c) : SharedData(c.SharedData), LocalData(c.LocalData), FitnessValue(c.FitnessValue) {}
  Chromosome(const Chromosome<T1,T2> && c) : SharedData(c.SharedData), LocalData(c.LocalData), FitnessValue(c.FitnessValue) {
    c.SharedData = NULL;
    c.FitnessValue = 0.0;
  }

  Chromosome<T1,T2> & operator=(const Chromosome<T1,T2> & c) {
    SharedData = c.SharedData;
    LocalData = c.LocalData;
    FitnessValue = c.FitnessValue;
    return *this;
  }

  Chromosome<T1,T2> * operator=(const Chromosome<T1,T2> && c) {
    std::swap(SharedData, c.SharedData);
    std::swap(LocalData, c.LocalData);
    std::swap(FitnessValue, c.FitnessValue);
    return *this;
  }

  virtual ~Chromosome() {}

  bool operator<(const Chromosome<T1,T2> & c) const { return FitnessValue < c.FitnessValue; }
  bool operator==(const Chromosome<T1,T2> & c) const { return LocalData == c.LocalData; }
  bool operator!=(const Chromosome<T1,T2> & c) const { return LocalData != c.LocalData; }

  void Initialize(T1 * Shared, T2 Local) {
    SharedData = Shared;
    LocalData = Local;
  }

  /*
  void Mutation():
    Virtual method that has to be implemented in derived classes.
    Executed usually to modify LocalData.
  */
  virtual void Mutation() = 0;

  /*
  void Crossover(parent, children1, children2):
    Even if this function is not pure virtual, it should be
    implemented in derived classes, changing the arguments
    to pointers of the derived class. The structure should
    remain the same, as it's assumed by the default genetic
    algorithm.
  */
  virtual void Crossover(Chromosome<T1,T2> * p, Chromosome<T1,T2> * c1, Chromosome<T1,T2> * c2) {}

  /*
  void Fitness():
    Pure virtual method that should modify FitnessValue,
    executed before sorting the population.
  */
  virtual void Fitness() = 0;

  T2 * GetLocalData() { return & LocalData; }
};

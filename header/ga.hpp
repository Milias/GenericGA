/*
The MIT License (MIT)
Copyright (c) 2016 Francisco García Flórez

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once
#include "shared.h"
#include "chromo.hpp"

/*
struct BasicSharedData and struct BasicLocalData:
  Basic implementations of chromosomes' data types, with
  every required variable already defined.
*/
struct BasicSharedData {
  uint32_t Elite = 0;
  double MutationRate = 0.0, ParentRate = 0.0;

  virtual ~BasicSharedData() {}
};

struct BasicLocalData {
  bool Chosen = false, Elite = false;

  virtual ~BasicLocalData() {}
};

/*
class GeneticAlgorithm<SDType, LDType, CType>:
  Default implementation of a genetic algorithm. It assumes
  the existance of a few variables defined in the previous
  basic structures.

  ChromosomeType has to be a derived class from Chromosome<SharedDataType, LocalDataType>.
*/
template <class SharedDataType, class LocalDataType, class ChromosomeType> class GeneticAlgorithm
{
private:
  uint32_t Population, Generation, StoredGenerations;
  Wrapper<ChromosomeType> * Chromosomes;
  SharedDataType * SharedData;

  std::queue<std::function<void()>> OperationsQueue;

  std::default_random_engine Generator;
  std::uniform_real_distribution<double> RandomRealdistribution;

  std::function<double()> RandomDouble;

public:
  /*
  (constructor):
    By default, two populations are created, so the next one
    will be allocated at the same time as the previous. This
    number can be increased as pleased, if a custom algorithm
    is implemented making use of previous generations' individuals.
  */
  GeneticAlgorithm(uint32_t pop, uint32_t store = 2) : Population(pop), Generation(0), StoredGenerations(store) {
    Chromosomes = new Wrapper<ChromosomeType>[StoredGenerations*Population];
    SharedData = new SharedDataType;
    RandomRealdistribution = std::uniform_real_distribution<double>(0.0,1.0);

    RandomDouble = [this]() { return this->RandomRealdistribution(this->Generator); };
  }

  ~GeneticAlgorithm() {
    for (uint32_t i = 0; i < Population; i++) { Chromosomes[i].DeleteObject(); }
    delete[] Chromosomes;
    delete SharedData;
  }

  void Initialize() {
    LocalDataType Local;
    Initialize(Local);
  }

  void Initialize(LocalDataType & Local) {
    Generation = 0;
    for (uint32_t i = 0; i < StoredGenerations * Population; i++) {
      Chromosomes[i] = new ChromosomeType;
      Chromosomes[i]->Initialize(SharedData, Local);
    }
  }

  void Initialize(std::function<LocalDataType(void)> & Local) {
    Generation = 0;
    for (uint32_t i = 0; i < StoredGenerations * Population; i++) {
      Chromosomes[i] = new ChromosomeType;
      Chromosomes[i]->Initialize(SharedData, Local());
    }
  }

  void Initialize(uint32_t i, LocalDataType & Local) {
    if (i >= Population) return;
    Chromosomes[Generation % StoredGenerations * Population + i]->Initialize(SharedData, Local);
  }

  void ProcessQueue() {
    for (; !OperationsQueue.empty(); ) {
      OperationsQueue.front()();
      OperationsQueue.pop();
    }
  }

  /*
  Wrapper<ChromosomeType> * WeightedSelection():
    Implementation of roulette choosing. A random individual
    is picked out of the population (those ones that are not
    elite nor haven't been picked before) with a probability
    proportional to its fitness value.

    If something goes wrong, a NULL pointer will be returned,
    so checking is required. Otherwise, a pointer to the
    picked element is returned.

    This approach can be problematic when defining negative
    definite fitness functions, so a different computation
    is advised.
  */
  Wrapper<ChromosomeType> * WeightedSelection() {
    double * fitness_sum = new double[Population];
    uint32_t origin = Generation % StoredGenerations * Population;

    /*
    If an element cannot be picked, its fitness value is set to zero in
    the array fitness_sum, so the probability of choosing it will be zero.
    */
    for (uint32_t i = 0; i < Population; i++) {
      fitness_sum[i] = Chromosomes[origin + i]->GetLocalData()->Elite || Chromosomes[origin + i]->GetLocalData()->Chosen ? Chromosomes[origin + i]->FitnessValue : 0.0;
    }

    for (uint32_t i = 1; i < Population; i++) { fitness_sum[i] += fitness_sum[i - 1]; }
    for (uint32_t i = 0; i < Population; i++) { fitness_sum[i] /= fitness_sum[Population - 1]; }

    double random = RandomDouble();
    for (uint32_t i = 1; i < Population; i++) {
      if (random < fitness_sum[i]) {
        Chromosomes[origin + i - 1]->GetLocalData()->Chosen = true;
        delete[] fitness_sum;
        return Chromosomes + origin + i - 1;
      }
    }

    delete[] fitness_sum;
    return NULL;
  }

  /*
  void BreedPopulation():
    Virtual method that should be overwritten if a custom algorithm
    is required.

    That is the central piece of the program, as this function
    will be called on each generation.

    Firstly, chosen and elite chromosomes will be reset, and sorted
    by their fitness values. Then, if the number of elite members is
    non-zero the ones with larger fitness values will be picked and
    passed to the next generation, as well as their children, without
    being mutated.

    Then, the remaining population is filled by choosing individuals
    at random using the previous method, and breeding them at random.
    All of them, including their children, can be mutated.

    Finally, after the next generation is in place, all the operations
    (mutations and crossovers) are executed in a FIFO queue, so mutations
    will be done after crossovers.
  */
  virtual void BreedPopulation() {
    uint32_t origin, final, nextOrigin;
    origin = Generation % StoredGenerations * Population;
    final = origin + Population;
    nextOrigin = ( Generation + 1 ) % StoredGenerations * Population;

    for (uint32_t i = origin; i < final; Chromosomes[i++]->Fitness()) {
      Chromosomes[i]->GetLocalData()->Chosen = false;
      Chromosomes[i]->GetLocalData()->Elite = false;
    }
    std::sort(Chromosomes + origin, Chromosomes + final);

    for (uint32_t i = 0; i < SharedData->Elite;  i += 2) {
      Chromosomes[nextOrigin + 2 * i] = Chromosomes[final - SharedData->Elite + i];
      Chromosomes[nextOrigin + 2 * i + 1] = Chromosomes[final - SharedData->Elite + i + 1];
      Chromosomes[nextOrigin + 2 * i]->GetLocalData()->Elite = true;
      Chromosomes[nextOrigin + 2 * i + 1]->GetLocalData()->Elite = true;

      OperationsQueue.push(std::bind(
        &ChromosomeType::Crossover,
        Chromosomes[nextOrigin + 2 * i].Object,
        Chromosomes[nextOrigin + 2 * i + 1].Object,
        Chromosomes[nextOrigin + 2 * i + 2].Object,
        Chromosomes[nextOrigin + 2 * i + 3].Object)
      );
    }

    nextOrigin += 2 * SharedData->Elite;

    Wrapper<ChromosomeType> * chosen_chromosome = NULL, * parent = NULL;
    for (uint32_t i = 0; i < Population - 2 * SharedData->Elite; i++) {
      chosen_chromosome = WeightedSelection();
      if (chosen_chromosome == NULL) continue;
      Chromosomes[nextOrigin] = *chosen_chromosome;
      if (RandomDouble() < SharedData->MutationRate) {
        OperationsQueue.push(std::bind(&ChromosomeType::Mutation, Chromosomes[nextOrigin].Object));
      }

      if (RandomDouble() < SharedData->ParentRate && (i + 2 < Population - 2 * SharedData->Elite)) {
        if (parent) {
          OperationsQueue.push(std::bind(
            &ChromosomeType::Crossover,
            parent->Object,
            chosen_chromosome->Object,
            Chromosomes[nextOrigin + 1].Object,
            Chromosomes[nextOrigin + 2].Object)
          );

          if (RandomDouble() < SharedData->MutationRate) {
            OperationsQueue.push(std::bind(&ChromosomeType::Mutation, Chromosomes[nextOrigin + 1].Object));
          }

          if (RandomDouble() < SharedData->MutationRate) {
            OperationsQueue.push(std::bind(&ChromosomeType::Mutation, Chromosomes[nextOrigin + 2].Object));
          }

          parent = NULL;
          nextOrigin += 2;
          i += 2;
        } else {
          parent = chosen_chromosome;
        }
      }
      nextOrigin++;
    }
    ProcessQueue();
  }

  /*
  void Simulate(uint32_t N):
    This method simply executes N times the method BreedPopulation,
    and finally sorts the population.
  */
  void Simulate(uint32_t N) {
    for (uint32_t i = 0; i < N; i++) {
      BreedPopulation();
    }
    std::sort(Chromosomes + Generation % StoredGenerations * Population, Chromosomes + Generation % StoredGenerations * Population + Population);
  }

  Wrapper<ChromosomeType> * GetChromosomes() { return Chromosomes + Generation % StoredGenerations * Population; }
  SharedDataType * GetSharedData() { return SharedData; }
};

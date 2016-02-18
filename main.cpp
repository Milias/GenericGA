/*
The MIT License (MIT)
Copyright (c) 2016 Francisco García Flórez

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "ga.hpp"

/*
  This file implementes a basic example using the default
  GeneticAlgorithm class. It can be modified easily to look
  for maxima of minima of analitic functions.

  If executed, three parameters has to be provided: size of
  the population, generations and elite members. Remember
  that population has to be at least double the number of
  elite members, and there could be zero elites.
*/
struct ChromosomeSharedData : BasicSharedData
{
  double MutationRate = 0.001, ParentRate = 0.5;
};

struct ChromosomeLocalData : BasicLocalData
{
  double x = 0.0;

  ChromosomeLocalData() : x(0.0) {}
  ChromosomeLocalData(double x) : x(x) {}
  ChromosomeLocalData(const ChromosomeLocalData & c) : x(c.x) {}
  ChromosomeLocalData & operator=(const ChromosomeLocalData & c) {
    x = c.x;
    return *this;
  }
};

class TestChromosome : public Chromosome<ChromosomeSharedData, ChromosomeLocalData>
{
protected:
  static std::default_random_engine Generator;
  static std::uniform_real_distribution<double> UniformDistribution;
  static std::uniform_real_distribution<double> RandomRealDistribution;

public:
  void Mutation() {
    LocalData.x += UniformDistribution(Generator);
  }

  void Crossover(TestChromosome * p, TestChromosome * c1, TestChromosome * c2) {
    double x = RandomRealDistribution(Generator);
    c1->GetLocalData()->x = x * LocalData.x + (1.0 - x) * p->GetLocalData()->x;
    c2->GetLocalData()->x = (1.0 - x) * LocalData.x + x * p->GetLocalData()->x;
  }

  void Fitness() {
    FitnessValue = -std::cos(LocalData.x);
  }
};

std::default_random_engine TestChromosome::Generator(time(NULL));
std::uniform_real_distribution<double> TestChromosome::UniformDistribution(-0.1, 0.1);
std::uniform_real_distribution<double> TestChromosome::RandomRealDistribution(0.0,1.0);

int main(int argc, char ** argv)
{
  std::default_random_engine Generator(time(NULL));
  std::uniform_real_distribution<double> RandomRealDistribution(0,  3.5);

  std::function<ChromosomeLocalData()> f = [&Generator, &RandomRealDistribution]() {
    return ChromosomeLocalData(RandomRealDistribution(Generator));
  };

  uint32_t N = std::atoi(argv[1]), N_G = std::atoi(argv[2]), Elite = std::atoi(argv[3]);

  GeneticAlgorithm<ChromosomeSharedData, ChromosomeLocalData, TestChromosome> ga(N);
  ga.GetSharedData()->Elite = Elite;
  ga.Initialize(f);
  ga.Simulate(N_G);

  printf("Maximum: %f, Fitness: %f\n", ga.GetChromosomes()[N - 1]->GetLocalData()->x, ga.GetChromosomes()[N - 1]->FitnessValue);

  return 0;
}

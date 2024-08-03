#pragma once

#include <random>
#include "MathHelper.h"

class RandomGenerator
{
  std::mt19937 generator;
  std::random_device rd;

public:
  double minVal;
  double maxVal;

  RandomGenerator()
  {
    generator.seed(rd());
  }
  ~RandomGenerator() {};

  double RandVal(double mean, double stddev = .05)
  {
    std::normal_distribution<double> normal(mean, stddev);
    // Because min and max are defined as macros somewhere, we need to use the function call syntax
    minVal = (normal.min)();
    maxVal = (normal.max)();
    return normal(generator);
  }

  double RandDist(double min, double max)
  {
    std::uniform_real_distribution<double> nomVal(min, max);
    return RandVal(nomVal(generator));
  }

  double RandAngle()
  {
    std::uniform_real_distribution<double> nomVal(0.0, 360.0);
    return RandVal(nomVal(generator));
  }

  double RandRadAngle()
  {
    std::uniform_real_distribution<double> nomVal(0.0, MathHelper::pi2<double>());
    return RandVal(nomVal(generator));
  }

  int RandDist( int min, int max )
  {
    std::uniform_int_distribution<> dis( min, max );
    return dis( rd );
  }

  template<typename T>
  void Shuffle( T& deck)
  {
    std::shuffle( deck.begin(), deck.end(), generator );
//     std::shuffle( deck.begin(), deck.end(), g );
//     std::shuffle( deck.begin(), deck.end(), g );
//     std::shuffle( deck.begin(), deck.end(), g );
//     std::shuffle( deck.begin(), deck.end(), g );
  }
};


#ifndef PERLIN_NOISE_HPP
#define PERLIN_NOISE_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>

// A Perlin noise generator, for generating Terrain heights
class PerlinNoise {
public:
  // Constructor
  PerlinNoise(unsigned w, unsigned h, unsigned seed=0);
  // Reseed the RNG used to generate height map
  void reseed(unsigned seed);
  // Generate and return a height map of w*h integers
  int* generateHeightMap();

private:
  unsigned width;
  unsigned height;
  unsigned seed;
  std::shared_ptr<std::mt19937> engine;

  // Permutation table (for Perlin algorithm)
  std::vector<int> permTable;
  // Gradients (for Perlin algorithm)
  std::vector<double> gradients;

  // Generate gradients randomly
  void generateGradients();
  // Math helper functions for Perlin noise
  double fade(double t) const;
  double lerp(double t, double a, double b) const;  // linear interpolation
  double grad(int hash, double x, double y, double z) const;  // gradient
  double noise(double x, double y, double z) const;
  // 'dramatizes' a point, making extremes more extreme
  double dramatize(double y) const;
  double sigmoid(double y) const;
};

#endif

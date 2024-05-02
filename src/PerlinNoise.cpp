#include "PerlinNoise.hpp"

PerlinNoise::PerlinNoise(unsigned w, unsigned h, unsigned _seed) : width(w), height(h), seed(_seed) {
  // Initialize permutation table
  int max_dim = std::max(w,h);
  permTable.resize(max_dim);
  std::iota(permTable.begin(), permTable.end(), 0);
  engine = std::make_shared<std::mt19937>(seed);
  std::shuffle(permTable.begin(), permTable.end(), *engine);

  // Generate gradients
  gradients.resize(max_dim * 3);
  generateGradients();
}

void PerlinNoise::reseed(unsigned seed) {
  // Reseed RNG
  engine->seed(seed);

  // Reshuffle perm table, and recalculate gradients
  std::shuffle(permTable.begin(), permTable.end(), *engine);
  generateGradients();
}

void PerlinNoise::generateGradients() {
  std::uniform_real_distribution<double> distribution(-1.0, 1.0);
  for (int i = 0; i < gradients.size(); i += 3) {
    double length = 0.0;

    do {
      gradients[i] = distribution(*engine);
      gradients[i + 1] = distribution(*engine);
      gradients[i + 2] = distribution(*engine);
      length = std::sqrt(gradients[i] * gradients[i] + gradients[i + 1] * gradients[i + 1] + gradients[i + 2] * gradients[i + 2]);
    } while (length >= 1.0);

    double scale = 1.0 / length;
    gradients[i] *= scale;
    gradients[i + 1] *= scale;
    gradients[i + 2] *= scale;
  }
}

double PerlinNoise::noise(double x, double y, double z) const {
  int max = std::max(width, height);
  int X = static_cast<int>(std::floor(x)) % max;
  int Y = static_cast<int>(std::floor(y)) % max;
  int Z = static_cast<int>(std::floor(z)) % max;
  // std::cout << "XYZ:" << X << ' ' << Y << ' ' << Z << '\n';

  x -= std::floor(x);
  y -= std::floor(y);
  z -= std::floor(z);

  double u = fade(x);
  double v = fade(y);
  double w = fade(z);

  int A  = (permTable[X] + Y) % max;
  // std::cout << "A:" << A << '\n';
  int AA = (permTable[A] + Z) % max;
  // std::cout << "AA:" << AA << '\n';
  int AB = (permTable[A + 1] + Z) % max;
  // std::cout << "AB:" << AB << '\n';
  int B  = (permTable[X + 1] + Y) % max;
  // std::cout << "B:" << B << '\n';
  int BA = (permTable[B] + Z) % max;
  // std::cout << "BA:" << BA << '\n';
  int BB = (permTable[B + 1] + Z) % max;
  // std::cout << "BB:" << BB << '\n';

  return lerp(w, lerp(v, 
                      lerp(u, grad(permTable[AA], x, y, z),
                           grad(permTable[BA], x - 1, y, z)),
                      lerp(u, grad(permTable[AB], x, y - 1, z),
                           grad(permTable[BB], x - 1, y - 1, z))),
                 lerp(v, 
                      lerp(u, grad(permTable[AA + 1], x, y, z - 1),
                           grad(permTable[BA + 1], x - 1, y, z - 1)),
                      lerp(u, grad(permTable[AB + 1], x, y - 1, z - 1),
                           grad(permTable[BB + 1], x - 1, y - 1, z - 1))));
}

double PerlinNoise::fade(double t) const {
  // std::cout << "fade" << t << '\n';
  return t * t * t * (t * (t * 6 - 15) + 10);
}

double PerlinNoise::lerp(double t, double a, double b) const {
  // std::cout << "lerp\n";
  return a + t * (b - a);
}

double PerlinNoise::grad(int hash, double x, double y, double z) const {
  // std::cout << "grad\n";
  int h = hash & 15;
  double u = h < 8 ? x : y;
  double v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
  return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

double PerlinNoise::dramatize(double y) const {
  return std::max((y * y) / 10.0, 0.0);
}

double PerlinNoise::sigmoid(double y) const {
  return y / (1.0 + std::abs(y));
}

int* PerlinNoise::generateHeightMap() {
  std::cout << "Generating Perlin Noise height map...\n";

  // Generate height map
  int* heightMap = new int[width * height];
  double maxHeight = 0.0;
  int freq = 4;
  double skew = 0.15;  // skew numbers upward
  int amp = 150;       // scale numbers

  for (int z = 0; z < height; ++z) {
    for (int x = 0; x < width; ++x) {
      double nx = x / static_cast<double>(width) - 0.5;
      double nz = z / static_cast<double>(height) - 0.5;
      double value = noise(freq * nx, freq * nz, 0) + skew; // Adjust frequency as needed
      heightMap[x + z * width] = static_cast<int>(dramatize(sigmoid(value/2) * amp)); 
      maxHeight = std::max(maxHeight, std::abs(value));
    }
  }

  // Cap height at plateau height
  int plateauHeight = 120;

  // Normalize heights
  for (int z = 0; z < height; ++z) {
    for (int x = 0; x < width; ++x) {
      // Adjust max height as needed
      heightMap[x + z * width] = std::min(plateauHeight, 
          static_cast<int>(heightMap[x + z * width] / maxHeight));
    }
  }

  // Return final map
  return heightMap;
}

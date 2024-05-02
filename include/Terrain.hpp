/** @file Terrain.hpp
 *  @brief Create a terrain
 *  
 *  More...
 *
 *  @author Mike
 *  @bug No known bugs.
 */
#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "VertexBufferLayout.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Image.hpp"
#include "Object.hpp"
#include "PerlinNoise.hpp"

#include <vector>
#include <string>

class Terrain : public Object {
public:
  // Takes in a Terrain and a filename for the heightmap.
  Terrain (std::string vertShader, std::string fragShader, unsigned xSegs, unsigned zSegs, std::string fileName);
  // Alternatively, takes a seed for a Perlin terrain
  Terrain (std::string vertShader, std::string fragShader, unsigned xSegs, unsigned zSegs, unsigned seed=0);
  // Destructor
  ~Terrain ();
  // override the initialization routine.
  void Init();
  // Loads a heightmap based on a PPM image
  // This then sets the heights of the terrain.
  void LoadHeightMap(Image image);
  // Load textures
  void LoadTextures(std::string colormap, std::string detailmap);
  // Is this Perlin-noise terrain?
  bool IsPerlin() { return m_perlin != nullptr; }
  // Resets Perlin height map
  void ResetPerlinHeight(unsigned seed);

private:
  // data
  unsigned int m_xSegments;
  unsigned int m_zSegments;
  std::shared_ptr<PerlinNoise> m_perlin;

  // Store the height in a "multidimensional" array
  int* m_heightData;
};

#endif

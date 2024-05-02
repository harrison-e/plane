#include "Terrain.hpp"
#include "Image.hpp"
#include "PerlinNoise.hpp"

#include <iostream>

// Constructor for our object (non-Perlin)
// Calls the initialization method
Terrain::Terrain(std::string vertShader, std::string fragShader, unsigned int xSegs, unsigned int zSegs, std::string fileName) 
  : Object(vertShader, fragShader), m_xSegments(xSegs), m_zSegments(zSegs) {
  std::cout << "(Terrain.cpp) Constructor called \n";
  
  m_perlin = nullptr;

  // Load up some image data
  Image heightMap(fileName);
  heightMap.LoadPPM(true);
  float scale = 4.5f; // Note that this scales down the values to make
                      // the image a bit more flat.

  // Set the height data for the image
  for(unsigned int z=0; z < m_zSegments; ++z){
    for(unsigned int x=0; x < m_xSegments; ++x){
      m_heightData[x+z*m_xSegments] = (float)heightMap.GetPixelR(z,x)/scale;
    }
  }

  // Initialize the terrain
  Init();
}

// Alternative Constructor (Perlin)
// Calls initialization method
Terrain::Terrain(std::string vertShader, std::string fragShader, unsigned xSegs, unsigned zSegs, unsigned seed) 
  : Object(vertShader, fragShader), m_xSegments(xSegs), m_zSegments(zSegs) {
  std::cout << "(Terrain.cpp) Constructor called \n";
  
  // Create Perlin object with given seed
  m_perlin = std::make_shared<PerlinNoise>(xSegs, zSegs, seed);
  m_heightData = m_perlin->generateHeightMap();

  // Initialize terrain
  m_geometry = Geometry();
  Init();
}

// Destructor
Terrain::~Terrain(){
  // Delete our allocatted heightMap data
  if(m_heightData!=nullptr){
    delete m_heightData;
  }
}


// Creates a grid of segments
// This article has a pretty handy illustration here:
// http://www.learnopengles.com/wordpress/wp-content/uploads/2012/05/vbo.png
// of what we are trying to do.
void Terrain::Init(){
  // Create the initial grid of vertices.
  for(unsigned int z=0; z < m_zSegments; ++z){
    for(unsigned int x =0; x < m_xSegments; ++x){
      float u = 1.0f - ((float)x/(float)m_xSegments);
      float v = 1.0f - ((float)z/(float)m_zSegments);
      // Calculate the correct position and add the texture coordinates
      m_geometry.AddVertex(x,m_heightData[x+z*m_xSegments],z,u,v);
    }
  }
  
  // Figure out which indices make up each triangle
  // By writing out a few of the indices you can figure out
  // the pattern here. Note there is an offset.
  for(unsigned int z=0; z < m_zSegments-1; ++z){
    for(unsigned int x =0; x < m_xSegments-1; ++x){
      m_geometry.AddIndex(x+(z*m_zSegments));
      m_geometry.AddIndex(x+(z*m_zSegments)+m_xSegments);
      m_geometry.AddIndex(x+(z*m_zSegments+1));

      m_geometry.AddIndex(x+(z*m_zSegments)+1);
      m_geometry.AddIndex(x+(z*m_zSegments)+m_xSegments);
      m_geometry.AddIndex(x+(z*m_zSegments)+m_xSegments+1);
    }
  }


   // Finally generate a simple 'array of bytes' that contains
   // everything for our buffer to work with.
   m_geometry.Gen();  
   // Create a buffer and set the stride of information
   m_vertexBufferLayout.CreateNormalBufferLayout(m_geometry.GetBufferDataSize(),
                                        m_geometry.GetIndicesSize(),
                                        m_geometry.GetBufferDataPtr(),
                                        m_geometry.GetIndicesDataPtr());
}



// Loads an image and uses it to set the heights of the terrain.
void Terrain::LoadHeightMap(Image image){

}

// Reset the Perlin heightmap for this terrain.
void Terrain::ResetPerlinHeight(unsigned seed) {
  if (m_perlin != nullptr) {
    m_perlin->reseed(seed);
    delete[] m_heightData;
    m_geometry.Clear();
    m_heightData = m_perlin->generateHeightMap();
    Init();
  }
}

void Terrain::LoadTextures(std::string colormap, std::string detailmap){ 
  // Load our actual textures
  m_textureDiffuse.LoadTexture(colormap); // Found in object
  m_detailMap.LoadTexture(detailmap);     // Found in object
}

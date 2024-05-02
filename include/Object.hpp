/** @file Object.hpp
 *  @brief Sets up an OpenGL camera..
 *  
 *  More...
 *
 *  @author Mike
 *  @bug No known bugs.
 */
#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <glad/glad.h>

#include <vector>
#include <string>
#include <memory>

// Forward declarations
#include "VertexBufferLayout.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "Geometry.hpp"
#include "Camera.hpp"
#include "Shader.hpp"

#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Purpose:
// An abstraction to create multiple objects
class Object {
public:
  // Object Constructor (no file loading)
  Object(std::string vertShader, std::string fragShader);
  // Object Constructor (load from .obj)
  Object(std::string vertShader, std::string fragShader, std::string objFilepath);
  // Object destructor
  ~Object();
  // Load a texture
  void LoadTexture(std::string fileName);
  // Create a textured quad
  void MakeTexturedQuad(std::string fileName);
  // Update function
  virtual void Update(glm::mat4 projectionMatrix, Camera* camera);
  // How to draw the object
  virtual void Render();
  // Helper method for when we are ready to draw or update our object
  void Bind();
  // Returns an objects transform
  Transform& GetTransform();

protected: // Classes that inherit from Object are intended to be overridden.
  // Store the object's transformations 
  Transform m_transform;
  // For now we have one buffer per object.
  VertexBufferLayout m_vertexBufferLayout;
  // For now we have one diffuse map
  Texture m_textureDiffuse;
  // Terrains are often 'multitextured' and have multiple textures.
  Texture m_detailMap; // NOTE: Note yet supported
  // Store the objects Geometry
  Geometry m_geometry;
  // Store the object's Shader 
  std::shared_ptr<Shader> m_shader;

private:
  void ParseObj(std::string objFilepath);
  void ParseMtl(std::string mtlFilepath);
};

#endif

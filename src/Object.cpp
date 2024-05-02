#include "Object.hpp"
#include "Error.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
namespace fs = std::filesystem;


Object::Object(std::string vertShader, std::string fragShader){
  std::cout << "(Object) Constructor called\n";
  // Create shader
  m_shader = std::make_shared<Shader>();

	// Setup shaders for the node.
	std::string vertexShader   = m_shader->LoadShader(vertShader);
	std::string fragmentShader = m_shader->LoadShader(fragShader);

	// Actually create our shader
	m_shader->CreateShader(vertexShader,fragmentShader);       
}

Object::Object(std::string vertShader, std::string fragShader, std::string objFilepath){
  std::cout << "(Object) Constructor called\n";
  // Create shader
  m_shader = std::make_shared<Shader>();

	// Setup shaders for the node.
	std::string vertexShader   = m_shader->LoadShader(vertShader);
	std::string fragmentShader = m_shader->LoadShader(fragShader);

	// Actually create our shader
	m_shader->CreateShader(vertexShader,fragmentShader);
  
  // Load obj data into m_geometry and m_texture
  ParseObj(objFilepath);
}

void Object::ParseObj(std::string objFilepath) {
  // Open .obj file and keep track of positions, normals, tex coords
  std::ifstream objFile;
  objFile.open(objFilepath);
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> textureCoords;

  if (objFile.is_open()) {
    std::cout << "Opened " << objFilepath << " successfully\n";
    std::string line = "";

    while (std::getline(objFile, line)) {
      std::stringstream stream(line);

      // Read each chunk, delimited by spaces 
      std::string chunk = "";
      while (stream >> chunk) {
        // Parse comments
        if (chunk[0] == '#') {
          std::cout << line << std::endl;
          break;
        }
        // Parse .mtl filepath 
        else if (chunk == "mtllib") {
          stream >> chunk;
          chunk = fs::path(objFilepath).replace_filename(chunk).string();
          ParseMtl(chunk);
          break;
        }
        // Parse Position
        else if (chunk[0] == 'v' && chunk.length() == 1) {
          glm::vec3 toAdd = glm::vec3(0.f, 0.f, 0.f);
          stream >> chunk;
          toAdd.x = std::stof(chunk);
          stream >> chunk;
          toAdd.y = std::stof(chunk);
          stream >> chunk;
          toAdd.z = std::stof(chunk);
          positions.push_back(toAdd);
        }
        // Parse Normals 
        else if (chunk.substr(0,2) == "vn") {
          // for now, nothing 
        }
        // Parse Texture Coords 
        else if (chunk.substr(0,2) == "vt") {
          glm::vec2 toAdd = glm::vec2(0.f, 0.f);
          stream >> chunk;
          toAdd.x = std::stof(chunk);
          stream >> chunk;
          toAdd.y = std::stof(chunk);
          textureCoords.push_back(toAdd);
        }
        // Triangle
        else if (chunk[0] == 'f') {
          // For all three vertices
          for (int i = 0; i < 3; ++i) {
            glm::vec3 position;
            glm::vec2 texture;
            stream >> chunk;
            std::stringstream chunkStream(chunk);
            std::string data;
            std::getline(chunkStream, data, '/');
            int pIndex = std::stoi(data); 
            position = positions[pIndex - 1]; 
            std::getline(chunkStream, data, '/');
            if (data != "") {
              int tIndex = std::stoi(data);
              texture = textureCoords[tIndex - 1];
            }
            std::getline(chunkStream, data, '/');
            if (data != "") {
              int nIndex = std::stoi(data);
              // setVertexNormals(v, normals[nIndex - 1]);
            }
            m_geometry.AddVertex(
                position.x,
                position.y,
                position.z,
                texture.s,
                texture.t);
          }
          unsigned indicesSize = m_geometry.GetIndicesSize();
          m_geometry.MakeTriangle(indicesSize, indicesSize+1, indicesSize+2);
        }
      }
    }
    objFile.close();

    // Finally generate a simple 'array of bytes' that contains
    // everything for our buffer to work with.
    m_geometry.Gen();  
    // Create a buffer and set the stride of information
    m_vertexBufferLayout.CreateNormalBufferLayout(m_geometry.GetBufferDataSize(),
                                        m_geometry.GetIndicesSize(),
                                        m_geometry.GetBufferDataPtr(),
                                        m_geometry.GetIndicesDataPtr());
  }
  // In case of failure
  else {
    std::cout << "Could not open " << objFilepath << "\n";
    exit(EXIT_FAILURE);
  }
}

void Object::ParseMtl(std::string mtlFilepath) {
  // Open .mtl file 
  std::ifstream mtlFile;
  mtlFile.open(mtlFilepath);

  if (mtlFile.is_open()) {
    std::cout << "Opened " << mtlFilepath << " successfully\n";
    std::string line = "";

    while (std::getline(mtlFile, line)) {
      std::stringstream stream(line);

      // Read chunks, looking for diffuse color map
      std::string chunk = "";
      while (stream >> chunk) {
        if (chunk == "map_Kd") {
          // Extract ppm filepath, and create texture
          // Assumes 1 texture per object, and 1 texture per .mtl
          stream >> chunk;
          chunk = fs::path(mtlFilepath).replace_filename(chunk).string();
          m_textureDiffuse.LoadTexture(chunk);
        }
      }
    }
    mtlFile.close();
  }
  // In case of failure
  else {
    std::cout << "Could not open " << mtlFilepath << "\n";
    exit(EXIT_FAILURE);
  }
}

Object::~Object(){
  
}

// TODO: In the future it may be good to 
// think about loading a 'default' texture
// if the user forgets to do this action!
void Object::LoadTexture(std::string fileName){
  // Load our actual textures
  m_textureDiffuse.LoadTexture(fileName);
}

// Initialization of object as a 'quad'
//
// This could be called in the constructor or
// otherwise 'explicitly' called this
// so we create our objects at the correct time
void Object::MakeTexturedQuad(std::string fileName){

  // Setup geometry
  // We are using a new abstraction which allows us
  // to create triangles shapes on the fly
  // Position and Texture coordinate 
  m_geometry.AddVertex(-1.0f,-1.0f, 0.0f, 0.0f, 0.0f);
  m_geometry.AddVertex( 1.0f,-1.0f, 0.0f, 1.0f, 0.0f);
  m_geometry.AddVertex( 1.0f, 1.0f, 0.0f, 1.0f, 1.0f);
  m_geometry.AddVertex(-1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
      
  // Make our triangles and populate our
  // indices data structure	
  m_geometry.MakeTriangle(0,1,2);
  m_geometry.MakeTriangle(2,3,0);

  // This is a helper function to generate all of the geometry
  m_geometry.Gen();

  // Create a buffer and set the stride of information
  // NOTE: How we are leveraging our data structure in order to very cleanly
  //       get information into and out of our data structure.
  m_vertexBufferLayout.CreateNormalBufferLayout(m_geometry.GetBufferDataSize(),
                                                m_geometry.GetIndicesSize(),
                                                m_geometry.GetBufferDataPtr(),
                                                m_geometry.GetIndicesDataPtr());

  // Load our actual texture
  // We are using the input parameter as our texture to load
  m_textureDiffuse.LoadTexture(fileName.c_str());
}

// Bind everything we need in our object
// Generally this is called in update() and render()
// before we do any actual work with our object
void Object::Bind(){
  // Make sure we are updating the correct 'buffers'
  m_vertexBufferLayout.Bind();
  // Diffuse map is 0 by default, but it is good to set it explicitly
  m_textureDiffuse.Bind(0);
  // Detail map
  // m_detailMap.Bind(1); // NOTE: Not yet supported
  // Bind shader 
  m_shader->Bind();
}

// Update this object 
void Object::Update(glm::mat4 projectionMatrix, Camera* camera){
  // Call our helper function to just bind everything
  Bind();
  // Set the uniforms in our current shader

  // For our object, we apply the texture in the following way
  // Note that we set the value to 0, because we have bound
  // our texture to slot 0.
  m_shader->SetUniform1i("u_DiffuseMap",0);  
  m_shader->SetUniform1i("u_DetailMap",1);  
  // Set the MVP Matrix for our object
  // Send it into our shader
  m_shader->SetUniformMatrix4fv("model", &m_transform.GetInternalMatrix()[0][0]);
  m_shader->SetUniformMatrix4fv("view", &camera->GetWorldToViewmatrix()[0][0]);
  m_shader->SetUniformMatrix4fv("projection", &projectionMatrix[0][0]);

  // Create a 'light'
  // Create a first 'light'
  // The sun
  m_shader->SetUniform3f("pointLights[0].lightColor",1.0f,1.0f,1.0f);
  m_shader->SetUniform3f("pointLights[0].lightPos",512.f,512.f,1024.f);
  m_shader->SetUniform1f("pointLights[0].ambientIntensity",0.9f);
  m_shader->SetUniform1f("pointLights[0].specularStrength",1.0f);
  m_shader->SetUniform1f("pointLights[0].constant",1.0f);
  m_shader->SetUniform1f("pointLights[0].linear",0.00175f);
  m_shader->SetUniform1f("pointLights[0].quadratic",0.0f);

  // Create a second light
  m_shader->SetUniform3f("pointLights[1].lightColor",1.0f,0.0f,0.0f);
  m_shader->SetUniform3f("pointLights[1].lightPos",
     camera->GetEyeXPosition() + camera->GetViewXDirection(),
     camera->GetEyeYPosition() + camera->GetViewYDirection(),
     camera->GetEyeZPosition() + camera->GetViewZDirection());
  m_shader->SetUniform1f("pointLights[1].ambientIntensity",0.9f);
  m_shader->SetUniform1f("pointLights[1].specularStrength",0.5f);
  m_shader->SetUniform1f("pointLights[1].constant",1.0f);
  m_shader->SetUniform1f("pointLights[1].linear",0.09f);
  m_shader->SetUniform1f("pointLights[1].quadratic",0.032f);
}

// Render our geometry
void Object::Render(){
  // Call our helper function to just bind everything
  Bind();
	//Render data
  glDrawElements(GL_TRIANGLES,
                 m_geometry.GetIndicesSize(), // The number of indices, not triangles.
                 GL_UNSIGNED_INT,             // Make sure the data type matches
                 nullptr);                    // Offset pointer to the data. 
                                              // nullptr because we are currently bound
}

Transform& Object::GetTransform() {
  return m_transform;
}


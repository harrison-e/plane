#ifndef AIRPLANE_HPP
#define AIRPLANE_HPP

#include "Object.hpp"
#include "glm/gtc/quaternion.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

// Represents an Airplane object 
class Airplane : public Object {
public:
  // Object Constructor
  Airplane();
  // Object Destructor
  ~Airplane();
  // Turn airplane left/right by given radians 
  void Yaw(float radians);
  // Tilt airplane up/down by given radians
  void Pitch(float radians);
  // Tilt airplane left/right by given radians 
  void Roll(float radians);
  // Stabilize the roll, yaw, and pitch by delta 
  void Stabilize(float delta);
  // Updates the airplane
  void Update(glm::mat4 projectionMatrix, Camera* camera) override;
  // Returns where the plane is pointing
  glm::vec3 NoseDirection() { return m_rollAxis; }

private:
  // Vectors representing where the airplane's direction
  // Analogous to forward, up, and right vectors 
  glm::vec3 m_yawAxis;
  glm::vec3 m_pitchAxis;
  glm::vec3 m_rollAxis;
  
  // Quaternion to track rotation in data
  // glm::quat m_quaternion;
  // Velocity
  float m_velocity;
  // Physical properties
  float m_airDensity;
  float m_wingArea;
}; 

#endif 

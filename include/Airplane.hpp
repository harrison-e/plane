#ifndef AIRPLANE_HPP
#define AIRPLANE_HPP

#include "Object.hpp"

#define PITCH_MAX 0.6f 
#define ROLL_MAX  0.35f

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
  // Updates the airplane
  void Update(glm::mat4 projectionMatrix, Camera* camera) override;
  // Returns where the plane is pointing
  glm::vec3 NoseDirection();

private:
  // Velocity
  float m_velocity;
  // Physical properties
  float m_airDensity;
  float m_wingArea;
}; 

#endif 

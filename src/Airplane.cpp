#include "Airplane.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtc/constants.hpp"
#include <iostream>

Airplane::Airplane()
  : Object("./shaders/vert.glsl", "./shaders/frag.glsl", "./assets/windmill.obj") { 
  // For now, windmill, not airplane
  std::cout << "(Airplane) Constructor called\n";
  m_yawAxis    = glm::vec3(+0.f, -1.f, +0.f);
  m_pitchAxis  = glm::vec3(+1.f, +0.f, +0.f);
  m_rollAxis   = glm::vec3(+0.f, +0.f, +1.f);
  m_velocity   = +0.04f;
  m_airDensity = +1;
  m_wingArea   = +1;
}

void Airplane::Roll(float radians) {
  // glm::vec3 rollAngles(+0.f, +0.f, radians);
  // glm::quat rollQuat(rollAngles);
  // m_quaternion = m_quaternion * rollQuat;
  // std::cout << "Roll:  " << glm::roll(m_quaternion) << '\n';
  // m_transform.Rotate(radians, 0.f, 0.f, glm::roll(m_quaternion));
  // glm::quat Q = glm::angleAxis(radians, m_rollAxis);
  // m_yawAxis   = glm::rotate(Q,   m_yawAxis); 
  // m_pitchAxis = glm::rotate(Q, m_pitchAxis);
  // std::cout << "Roll:  " << glm::roll(Q) << '\n';
  // m_yawAxis   = glm::rotate(m_yawAxis,   radians, m_rollAxis);
  if ((m_pitchAxis.y < +0.6f && m_pitchAxis.y > -0.6f)
      || (m_pitchAxis.y > +0.6f && radians < 0.f) 
      || (m_pitchAxis.y < -0.6f && radians > 0.f)) 
  {
    m_pitchAxis = glm::rotate(m_pitchAxis, radians, m_rollAxis);
    m_transform.Rotate(radians, m_rollAxis.x, m_rollAxis.y, m_rollAxis.z);
  }
}

void Airplane::Yaw(float radians) {
  // glm::vec3 yawAngles(+0.f, radians, +0.f);
  // glm::quat yawQuat(yawAngles);
  // m_quaternion = m_quaternion * yawQuat;
  // std::cout << "Yaw:   " << glm::yaw(m_quaternion) << '\n';
  // m_transform.Rotate(radians, 0.f, glm::yaw(m_quaternion), 0.f);
  // glm::quat Q = glm::angleAxis(radians, m_yawAxis);
  // m_pitchAxis = glm::rotate(Q, m_pitchAxis);
  // m_rollAxis  = glm::rotate(Q,  m_rollAxis); 
  // std::cout << "Yaw:   " << glm::yaw(Q) << '\n';
  m_pitchAxis = glm::rotate(m_pitchAxis, radians, m_yawAxis);
  m_rollAxis  = glm::rotate(m_rollAxis,  radians, m_yawAxis);
  m_transform.Rotate(radians, 0.f, m_yawAxis.y, 0.f);
}

void Airplane::Pitch(float radians) {
  // glm::vec3 pitchAngles(radians, +0.f, +0.f);
  // glm::quat pitchQuat(pitchAngles);
  // m_quaternion = m_quaternion * pitchQuat;
  // std::cout << "Pitch: " << glm::pitch(m_quaternion) << '\n';
  // glm::quat Q = glm::angleAxis(radians, m_pitchAxis);
  // m_yawAxis   = glm::rotate(Q,   m_yawAxis);
  // m_rollAxis  = glm::rotate(Q,  m_rollAxis); 
  // std::cout << "Pitch: " << glm::pitch(Q) << '\n';
  // m_yawAxis   = glm::rotate(m_yawAxis,   radians, m_pitchAxis);
  if ((m_rollAxis.y < +0.35f && m_rollAxis.y > -0.35f)
      || (m_rollAxis.y > +0.35f && radians > 0.f) 
      || (m_rollAxis.y < -0.35f && radians < 0.f)) 
  {
    m_rollAxis  = glm::rotate(m_rollAxis,  -radians, m_pitchAxis);
    m_transform.Rotate(-radians, m_pitchAxis.x, m_pitchAxis.y, m_pitchAxis.z);
  }
}

void Airplane::Stabilize(float delta) {
  // // Stabilize along pitch axis 
  // if (m_rollAxis.y != 0.f) {
  //   float deltaSign = m_rollAxis.y < 0.f ? -1.f : +1.f;
  //   Pitch(delta*deltaSign);
  // }
  // Stabilize along roll axis
  // if (m_pitchAxis.y > 0.f || m_pitchAxis.y < 0.f) {
  //   float deltaSign = m_pitchAxis.y < 0.f ? +1.f : -1.f;
  //   Roll(delta*deltaSign);
  // }
}

void Airplane::Update(glm::mat4 projectionMatrix, Camera* camera) {
  // First, call superclass implementation
  Object::Update(projectionMatrix, camera);

  // Then, do airplane-specific updates
  // Calculate AoA
  glm::vec3 flatRoll = glm::normalize(glm::vec3(m_rollAxis.x, 0.f, m_rollAxis.z));
  float aoaDot = glm::dot(flatRoll, glm::normalize(m_rollAxis));
  std::cout << "AoA dot: " << aoaDot << "\n";
  float angleOfAttack = glm::acos(glm::dot(flatRoll, glm::normalize(m_rollAxis)));
  float pi = glm::pi<float>();

  // Lift coefficient and Lift
  float cL = angleOfAttack < (pi / 8.f) || angleOfAttack > (7.f * pi / 8.f)
    ? glm::sin(6.f * angleOfAttack)
    : glm::sin(2.f * angleOfAttack);
  float L  = cL * m_airDensity * (m_velocity * m_velocity / 2.f) * m_wingArea;

  // Drag coefficient and Drag
  float cD = 1.f - glm::cos(2.f * angleOfAttack);
  float D  = cD * m_airDensity * (m_velocity * m_velocity / 2.f) * m_wingArea;  

  // Translate vertically by Lift - Drag
  m_transform.Translate(0.f, L - D, 0.f);

  // if the dot of (cross pitch roll) and yaw is > 0.001, yaw by theta  
  // where theta = that dot * sign of pitch.y 
  glm::vec3 flatPitch = glm::normalize(glm::vec3(m_pitchAxis.x, 0.f, m_pitchAxis.z));
  float yawDot = glm::dot(flatPitch, glm::normalize(m_pitchAxis));
  std::cout << "yaw dot: " << yawDot << "\n\n";
  float yawAngle = glm::acos(glm::dot(flatPitch, glm::normalize(m_pitchAxis)));
  if (yawAngle > 0.01f) {
    Yaw(yawAngle * (m_pitchAxis.y > 0.f ? -0.01f : +0.01f));
  }
  
  // Translate along roll axis by velocity
  glm::vec3 forward = m_rollAxis * m_velocity;
  m_transform.Translate(forward.x, forward.y, forward.z);
}

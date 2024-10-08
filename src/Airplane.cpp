#include "Airplane.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtc/constants.hpp"
#include <iostream>


Airplane::Airplane()
  : Object("./shaders/vert.glsl", "./shaders/frag.glsl", "./assets/windmill.obj") { 
  // For now, windmill, not airplane
  std::cout << "(Airplane) Constructor called\n";
  m_velocity   = +0.1f;
  m_airDensity = +1;
  m_wingArea   = +1;
}

glm::vec3 Airplane::NoseDirection() {
  glm::mat4 m = m_transform.GetInternalMatrix();
  return { m[0][2], m[1][2], m[2][2]};
}

void Airplane::Roll(float radians) {
  // if ((m_pitchAxis.y < +PITCH_MAX && m_pitchAxis.y > -PITCH_MAX)
  //     || (m_pitchAxis.y > +PITCH_MAX && radians < 0.f) 
  //     || (m_pitchAxis.y < -PITCH_MAX && radians > 0.f)) 
  // {
  // Roll transform matrix
  Transform rollMatrix = Transform({
    +cos(radians),    -sin(radians),      0,
    +sin(radians),    +cos(radians),      0,
    0,                   0,                     1 
  });

  m_transform *= rollMatrix;
  // }
}

void Airplane::Yaw(float radians) {
  // Yaw transform matrix
  Transform yawMatrix = Transform({
    +cos(radians),  0,                     sin(radians),
    0,                   1,                     0,
    -sin(radians),  0,                     cos(radians)
  });
  
  m_transform *= yawMatrix;
}

void Airplane::Pitch(float radians) {
  // if ((m_rollAxis.y < +ROLL_MAX && m_rollAxis.y > -ROLL_MAX)
  //     || (m_rollAxis.y > +ROLL_MAX && radians > 0.f) 
  //     || (m_rollAxis.y < -ROLL_MAX && radians < 0.f)) 
  // {
  // Pitch transform matrix
  Transform pitchMatrix = Transform({
    1,                    0,                    0,
    0,                    +cos(radians),     -sin(radians),
    0,                    +sin(radians),     +cos(radians)
  });
  // glm::mat4 p = pitchMatrix.GetInternalMatrix();
  // 
  // m_forward = {
  //   p[0][0] * m_forward.x + p[0][1] * m_forward.y + p[0][2] * m_forward.z + p[0][3],
  //   p[1][0] * m_forward.x + p[1][1] * m_forward.y + p[1][2] * m_forward.z + p[1][3],
  //   p[2][0] * m_forward.x + p[2][1] * m_forward.y + p[2][2] * m_forward.z + p[2][3],
  // };
  m_transform *= pitchMatrix;
  // }
}

void Airplane::Update(glm::mat4 projectionMatrix, Camera* camera) {
  // First, call superclass implementation
  Object::Update(projectionMatrix, camera);

  // Then, do airplane-specific updates
  // Calculate AoA
  // glm::vec3 flatRoll = glm::normalize(glm::vec3(m_rollAxis.x, 0.f, m_rollAxis.z));
  // float aoaDot = glm::dot(flatRoll, glm::normalize(m_rollAxis));
  // std::cout << "AoA dot: " << aoaDot << "\n";
  // float angleOfAttack = glm::acos(glm::dot(flatRoll, glm::normalize(m_rollAxis)));
  // float pi = glm::pi<float>();

  // Lift coefficient and Lift
  // float cL = angleOfAttack < (pi / 8.f) || angleOfAttack > (7.f * pi / 8.f)
  //   ? glm::sin(6.f * angleOfAttack)
  //   : glm::sin(2.f * angleOfAttack);
  // float L  = cL * m_airDensity * (m_velocity * m_velocity / 2.f) * m_wingArea;

  // Drag coefficient and Drag
  // float cD = 1.f - glm::cos(2.f * angleOfAttack);
  // float D  = cD * m_airDensity * (m_velocity * m_velocity / 2.f) * m_wingArea;  

  // Translate vertically by Lift - Drag
  // m_transform.Translate(0.f, L - D, 0.f);

  // If the dot of (cross pitch roll) and yaw is > 0.001, yaw by theta  
  // Where theta = that dot * sign of pitch.y 
  // glm::vec3 flatPitch = glm::normalize(glm::vec3(m_pitchAxis.x, 0.f, m_pitchAxis.z));
  // float yawDot = glm::dot(flatPitch, glm::normalize(m_pitchAxis));
  // std::cout << "yaw dot: " << yawDot << "\n\n";
  // float yawAngle = glm::acos(glm::dot(flatPitch, glm::normalize(m_pitchAxis)));
  // if (yawAngle > 0.01f) {
  //   Yaw(yawAngle * (m_pitchAxis.y > 0.f ? -0.01f : +0.01f));
  // }
  
  m_transform.Translate(0, 0, m_velocity);
}

#include "Camera.hpp"

#include "glm/gtx/transform.hpp"

#include "glm/gtx/rotate_vector.hpp"

#include <iostream>

void Camera::MouseLook(int mouseX, int mouseY){
  // Record our new position as a vector
  glm::vec2 newMousePosition(mouseX, mouseY);
  static bool firstLook = true;
  // Note: we need to handle the 'first' mouse event
  // Note: We should also 'center' the mouse in our
  //       program when we execute it.
  if(true == firstLook){
    firstLook=false;
    m_oldMousePosition = newMousePosition;
  }
  // Detect how much the mouse has moved since
  // the last time
  glm::vec2 mouseDelta = m_oldMousePosition - newMousePosition; 
  mouseDelta *= 0.2f; // mouse sensitivity.

  // Update the view direction and up vector
  glm::vec3 yAxis = glm::vec3(0.f, 1.f, 0.f);
  m_viewDirection = glm::rotate(m_viewDirection,glm::radians(mouseDelta.x),yAxis);
  m_upVector = glm::rotate(m_upVector,glm::radians(mouseDelta.x),yAxis);

  // First, calculate a potential new right vector and view direction
  glm::vec3 newRightVector = glm::cross(m_viewDirection, m_upVector);
  glm::vec3 newViewDirection = glm::rotate(m_viewDirection, glm::radians(mouseDelta.y), newRightVector);
  // Make sure that our view direction doesn't move "backwards"
  // That is, make sure that we dont rotate past the y-axis
  // Because it is disorienting!
  if ((newViewDirection.y < m_viewDirection.y && mouseDelta.y < 0.f) 
      || (newViewDirection.y > m_viewDirection.y && mouseDelta.y > 0.f))  {
    m_viewDirection = newViewDirection;
    m_rightVector = newRightVector;
    m_upVector = glm::rotate(m_upVector, glm::radians(mouseDelta.y), m_rightVector);
  } 


  // std::cout << "m_viewDirection.x: " << m_viewDirection.x << std::endl;
  // std::cout << "m_viewDirection.y: " << m_viewDirection.y << std::endl;
  // std::cout << "m_viewDirection.z: " << m_viewDirection.z << std::endl;

  // Update our old position after we have made changes 
  m_oldMousePosition = newMousePosition;
}

void Camera::MoveForward(float speed){
    // Move the camera in the 'forward' direction
    glm::vec3 yAxis = glm::vec3(0.f, 1.f, 0.f); 
    glm::vec3 forward = glm::cross(yAxis, m_rightVector);
    // Move in the speed
    forward = forward * speed;
    // Update the position
    m_eyePosition += forward;
}

void Camera::MoveBackward(float speed){
    // Move the camera in the 'forward' direction
    glm::vec3 yAxis = glm::vec3(0.f, 1.f, 0.f); 
    glm::vec3 forward = glm::cross(yAxis, m_rightVector);
    // Move in the speed
    forward = forward * speed;
    // Update the position
    m_eyePosition -= forward;
}

// Move along the 'right vector'
void Camera::MoveLeft(float speed){
  glm::vec3 right = m_rightVector * speed;
  m_eyePosition -= right;
}

void Camera::MoveRight(float speed){
  glm::vec3 right = m_rightVector * speed;
  m_eyePosition += right;
}

void Camera::MoveUp(float speed){
    m_eyePosition.y += speed;
}

void Camera::MoveDown(float speed){
    m_eyePosition.y -= speed;
}

// Set the position for the camera
void Camera::SetCameraEyePosition(float x, float y, float z){
    m_eyePosition.x = x;
    m_eyePosition.y = y;
    m_eyePosition.z = z;
}

void Camera::VectorLook(float x, float y, float z){
  glm::vec3 newForward = glm::vec3(x,y,z);
  glm::vec3 newRight = glm::cross(newForward, m_upVector);

  m_viewDirection = newForward;
  m_rightVector = newRight;
}

float Camera::GetEyeXPosition(){
    return m_eyePosition.x;
}

float Camera::GetEyeYPosition(){
    return m_eyePosition.y;
}

float Camera::GetEyeZPosition(){
    return m_eyePosition.z;
}

float Camera::GetViewXDirection(){
    return m_viewDirection.x;
}

float Camera::GetViewYDirection(){
    return m_viewDirection.y;
}

float Camera::GetViewZDirection(){
    return m_viewDirection.z;
}


Camera::Camera(){
    std::cout << "(Constructor) Created a Camera!\n";
	// Position us at the origin.
    m_eyePosition = glm::vec3(0.0f,0.0f, 0.0f);
	// Looking down along the z-axis initially.
	// Remember, this is negative because we are looking 'into' the scene.
    m_viewDirection = glm::vec3(0.0f,0.0f, -1.0f);
	// For now--our upVector always points up along the y-axis
    m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    m_rightVector = glm::cross(m_viewDirection, m_upVector);
}

glm::mat4 Camera::GetWorldToViewmatrix() const{
    // Think about the second argument and why that is
    // setup as it is.
    return glm::lookAt( m_eyePosition,
                        m_eyePosition + m_viewDirection,
                        m_upVector);
}

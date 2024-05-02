#include "SceneNode.hpp"

#include <string>
#include <iostream>

// The constructor
SceneNode::SceneNode(std::shared_ptr<Object> ob){
	std::cout << "(SceneNode.cpp) Constructor called\n";
	m_object = ob;

  // By default no parent.
  m_parent = nullptr;
}

// The destructor 
SceneNode::~SceneNode(){
  // Remove all of the children
  for(int i=0; i < m_children.size(); i++){
    delete m_children[i];
  }
}

// Adds a child node to our current node.
void SceneNode::AddChild(SceneNode* n){
	// For the node we have added, we can set
	// it's parent now to our current node.
	// 'this' is the current instance of our
	// object, which is a pointer to our current
	// SceneNode.
	n->m_parent = this;
	// Add a child node into our SceneNode
	m_children.push_back(n);
}

// Draw simply draws the current nodes
// object and all of its children. This is done by calling directly
// the objects draw method.
void SceneNode::Draw(){
	// Render our object
	if(m_object!=nullptr){
		// Render our object
		m_object->Render();
		// For any 'child nodes' also call the drawing routine.
		for(int i =0; i < m_children.size(); ++i){
			m_children[i]->Draw();
		}
	}	
}

// Update simply updates the current nodes
// object. This is done by calling directly
// the objects update method.
// TODO: Consider not passting projection and camera here
void SceneNode::Update(glm::mat4 projectionMatrix, Camera* camera){
  if(m_object!=nullptr){
    // Update the object 
    m_object->Update(projectionMatrix, camera);
    
    // Iterate through all of the children
    for(int i =0; i < m_children.size(); ++i){
      m_children[0]->Update(projectionMatrix, camera);
    }
  }
}

// Returns the actual local transform stored in our SceneNode
// which can then be modified
Transform& SceneNode::GetLocalTransform(){
  return m_localTransform; 
}

// Returns the worled  transform stored in our SceneNode
// which can then be modified
Transform& SceneNode::GetWorldTransform(){
  return m_worldTransform; 
}

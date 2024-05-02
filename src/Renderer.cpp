#include "Renderer.hpp"
#include <cmath>


// Sets the height and width of our renderer
Renderer::Renderer(unsigned int w, unsigned int h){
  m_screenWidth = w;
  m_screenHeight = h;
  m_downscale = 4;
  m_pixelate = true;

  // Add our free camera
  Camera* freeCamera = new Camera();
  m_cameras.push_back(freeCamera);

  // Initialize the root in our scene
  m_root = nullptr;

  // By derfaflt create one framebuffer within the renderere.
  Framebuffer* regFramebuffer = new Framebuffer();
  regFramebuffer->Create(w,h);
  m_framebuffers.push_back(regFramebuffer);

  // Also create a pixelated framebuffer
  Framebuffer* pixFramebuffer = new Framebuffer();
  pixFramebuffer->Create(w/m_downscale,h/m_downscale);
  m_framebuffers.push_back(pixFramebuffer);
}

// Sets the height and width of our renderer
Renderer::~Renderer(){
  // Delete all of our camera pointers
  for(int i=0; i < m_cameras.size(); i++){
    delete m_cameras[i];
  }
  // Delete all of of our framebuffers
  for(int i=0; i < m_framebuffers.size(); i++){
    delete m_framebuffers[i];
  }
  // Remove all managed objects 
  ObjectManager::Instance().RemoveAll();
}

void Renderer::Update(){
  // Here we apply the projection matrix which creates perspective.
  // The first argument is 'field of view'
  // Then perspective
  // Then the near and far clipping plane.
  // Note I cannot see anything closer than 0.1f units from the screen.
  m_projectionMatrix = glm::perspective(glm::radians(45.0f),((float)m_screenWidth)/((float)m_screenHeight),0.1f,1024.0f);

  // Perform the update
  if(m_root!=nullptr){
    // TODO: By default, we will only have one camera
    //       You may otherwise not want to hardcode
    //       a value of '0' here.
    m_root->Update(m_projectionMatrix, m_cameras[0]);
  }
  
  // Update all managed objects
  ObjectManager::Instance().UpdateAll(m_projectionMatrix, m_cameras[0]);
}

// Initialize clear color
// Setup our OpenGL State machine
// Then render the scene
void Renderer::Render(){
  // Setup our uniforms
  // In reality, only need to do this once for this
  // particular fbo because the texture data is 
  // not going to change.
  unsigned fbIndex = m_pixelate ? 1 : 0;

  m_framebuffers[fbIndex]->Update();
  // Bind to our framebuffer
  m_framebuffers[fbIndex]->Bind();


  // What we are doing, is telling opengl to create a depth(or Z-buffer) 
  // for us that is stored every frame.
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D); 


  // This is the background of the screen.
  unsigned viewFactor = m_pixelate ? m_downscale : 1;
  glViewport(0, 0, m_screenWidth/viewFactor, m_screenHeight/viewFactor);
  glClearColor( 0.5f, 0.7f, 1.0f, 1.f );
  // Clear color buffer and Depth Buffer
  // Remember that the 'depth buffer' is our
  // z-buffer that figures out how far away items are every frame
  // and we have to do this every frame!
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  // Nice way to debug your scene in wireframe!
  // TODO: Read this
  // Understand that you should only see a single quad
  // after rendering this, because we are only drawing.
  // one quad
  // This is how we know things are working with our FBO
  //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  // Test to see if the 'w' key is pressed for a quick view to toggle
  // the wireframe view.
  const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
  if( currentKeyStates[ SDL_SCANCODE_TAB ] )
  {
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  }else if( currentKeyStates[ SDL_SCANCODE_P ] ){
    m_pixelate = !m_pixelate;
    SDL_Delay(50);
  }else{
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  }
  
  // Now we render our objects from our scenegraph
  if(m_root!=nullptr){
    m_root->Draw();
  }

  // Now we render all objects from ObjectManager 
  ObjectManager::Instance().RenderAll();

  // Finish with our framebuffer
  m_framebuffers[fbIndex]->Unbind();
  // Now draw a new scene
  // We do not need depth since we are drawing a '2D'
  // image over our screen.
  glDisable(GL_DEPTH_TEST);
  // Clear everything away
  // Clear the screen color, and typically I do this
  // to something 'different' than our original as an
  // indication that I am in a FBO. But you may choose
  // to match the glClearColor
  glClearColor(1.0f,1.0f,1.0f,1.0f);
  // We only have 'color' in our buffer that is stored
  glClear(GL_COLOR_BUFFER_BIT); 
  // Use our new 'simple screen shader'
  m_framebuffers[fbIndex]->m_fboShader->Bind();
  glViewport(0, 0, m_screenWidth, m_screenHeight);
  // Overlay our 'quad' over the screen
  m_framebuffers[fbIndex]->DrawFBO();    
  // Unselect our shader and continue
  m_framebuffers[fbIndex]->m_fboShader->Unbind();
}

// Determines what the root is of the renderer, so the
// scene can be drawn.
void Renderer::setRoot(std::shared_ptr<SceneNode> startingNode){
  m_root = startingNode;
}



#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Util.h>

//Separate class for printing stuff in NGL
#include <ngl/NGLStream.h>


#include <iostream>



NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("Blank NGL");

  /************
   * ADDED CODE
   * **********/
  m_view = ngl::lookAt(ngl::Vec3(2,2,2),
                       ngl::Vec3(0,0,0),
                       ngl::Vec3(0,1,0));

}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

//Two resive versions because of Qt version differences
void NGLScene::resizeGL(QResizeEvent *_event)
{
  m_width=_event->size().width()*devicePixelRatio();
  m_height=_event->size().height()*devicePixelRatio();


  /************
   * ADDED CODE
   * **********/
  //Create the projection based on the current window aspect ratio
  m_project = ngl::perspective(45.0f,
                               float (m_width/m_height),
                               0.2f,
                               20.0f);
}

void NGLScene::resizeGL(int _w , int _h)
{
  m_width=_w*devicePixelRatio();
  m_height=_h*devicePixelRatio();

  /************
   * ADDED CODE
   * **********/
  //Same as code in other resize method
  m_project = ngl::perspective(45.0f,
                               float (m_width/m_height),
                               0.2f,
                               20.0f);
}



void NGLScene::initializeGL()
{
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  /***************
   * ADDED CODE *
   * *************/

  //Create a sphere and put it into a VAO
  //This only needs to be done once and can then be duplicated
  ngl::VAOPrimitives::instance()->createSphere("sphere",1.0,80);

  //Get a pointer to the shader lib
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();

  //Create the shader program
  shader->createShaderProgram("diffuse");

  //Create the shaders
  shader->attachShader("diffuseVertex",ngl::ShaderType::VERTEX);
  shader->attachShader("diffuseFragment",ngl::ShaderType::FRAGMENT);

  //Load the shader source
  shader->loadShaderSource("diffuseVertex","shaders/DiffuseVertex.glsl");
  shader->loadShaderSource("diffuseFragment","shaders/DiffuseFragment.glsl");

  //Compile the shaders
  shader->compileShader("diffuseVertex");
  shader->compileShader("diffuseFragment");

  //Attach shaders to the program
  shader->attachShaderToProgram("diffuse","diffuseVertex");
  shader->attachShaderToProgram("diffuse","diffuseFragment");

  //Link it all
  shader->linkProgramObject("diffuse");
  shader->use("diffuse");

  //Set some uniform variables
  shader->setRegisteredUniform("colour",1.0f,0.0f,0.0f,1.0f);
  shader->setRegisteredUniform("lightPos",0.0f,2.0f,0.0f);
  shader->setRegisteredUniform("lightDiffuse",1.0f,1.0f,1.0f,1.0f);
}



void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_width,m_height);

  /***************
   * ADDED CODE *
   * *************/

  ngl::ShaderLib *shader = ngl::ShaderLib::instance();

  //Create a new variable and store the key in it
  ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
  shader->setRegisteredUniform("colour",1.0f,0.0f,0.0f,0.5f);
  //NEW PRIMITIVE DRAWING
  //Set transformation
  //Pass to shaders
  //Draw

  //E.G.

  //Reset the transformation matrix
  m_transform.reset();
  //Change the scale
  m_transform.setScale(0.1,1.0,0.1);

  loadMatricesToShader();
  prim->draw("sphere");

  //Print out the viewing and projection matrices (useful for debugging)
  // std::cout<<m_view<<"\n"<<m_project<<"\n"

  //Draw a new primitive
  m_transform.reset();
  m_transform.setScale(0.2,0.2,0.2);
  m_transform.setPosition(0.0,1.0,0.0);
  loadMatricesToShader();
  prim->draw("sphere");

  drawPrimitive(ngl::Vec3(1.0,0.1,0.1),ngl::Vec3(0.0,0.0,0.0),prim);
  drawPrimitive(ngl::Vec3(0.1,0.1,1.0),ngl::Vec3(0.0,0.0,0.0),prim);

  //Change the base colour
  shader->setRegisteredUniform("colour",0.0f,0.0f,1.0f,0.5f);

  drawPrimitive(ngl::Vec3(0.2,0.2,0.2),ngl::Vec3(0.0,-1.0,0.0),prim);
  drawPrimitive(ngl::Vec3(0.2,0.2,0.2),ngl::Vec3(1.0,0.0,0.0),prim);
  drawPrimitive(ngl::Vec3(0.2,0.2,0.2),ngl::Vec3(-1.0,0.0,0.0),prim);
  drawPrimitive(ngl::Vec3(0.2,0.2,0.2),ngl::Vec3(0.0,0.0,1.0),prim);
  drawPrimitive(ngl::Vec3(0.2,0.2,0.2),ngl::Vec3(0.0,0.0,-1.0),prim);

}



void NGLScene::drawPrimitive(ngl::Vec3 m_scale, ngl::Vec3 m_pos,ngl::VAOPrimitives* m_draw)
{
    m_transform.reset();
    m_transform.setScale(m_scale);
    m_transform.setPosition(m_pos);
    loadMatricesToShader();
    m_draw->draw("sphere");

}



void NGLScene::loadMatricesToShader()
{
    //Create the new shader
    //Load the viewing matrices into it
    ngl::ShaderLib *shader = ngl::ShaderLib::instance();
    ngl::Mat4 MVP = m_transform.getMatrix()*m_view*m_project;

    //Stores the uniforms internally (so they don't have to be looked up)
    shader->setRegisteredUniform("MVP",MVP);
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{

}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
  default : break;
  }
  // finally update the GLWindow and re-draw

    update();
}

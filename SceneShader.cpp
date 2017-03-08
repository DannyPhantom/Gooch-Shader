/*
 * SceneShader.cpp
 *
 *  Created on: Nov 17, 2015
 *      Author: acarocha
 */

#include "SceneShader.h"

static float PI = 3.14159265359;


SceneShader::SceneShader(): Shader()
{
	_programPlane = 0;
	_programMesh = 0;
	_programTrackbar = 0;
	_planeVertexArray = -1;
	_planeVertexArray = -1;
	_mvUniform = -1;
	_projUniform = -1;
	_zTranslation = 1.0;
	_aspectRatio = 1.0;
	_xRot = 0.0;
	_yRot = 0.0;
	lightPosition = glm::vec3(0.5, 0.5, 0.5);

}


void SceneShader::readMesh( std::string filename )
{
	_mesh = trimesh::TriMesh::read(filename.c_str());

	_mesh->need_bbox();
	_mesh->need_faces();
	_mesh->need_normals();
	_mesh->need_bsphere();

	for(unsigned int i = 0; i < _mesh->faces.size(); i++)
	{
   	  _triangleIndices.push_back(_mesh->faces[i][0]);
	  _triangleIndices.push_back(_mesh->faces[i][1]);
	  _triangleIndices.push_back(_mesh->faces[i][2]);
	}
}



void SceneShader::createVertexBuffer()
{
	//create plane geometry
	static const GLfloat quadData[] =
	{
                        -1.0f, 0.0f, -1.0f,
                        -1.0f, 0.0f, 1.0f,
                        1.0f, 0.0f, -1.0f,
                        1.0f, 0.0f, 1.0f,
	};

	//passing model attributes to the GPU
	//plane
	glGenVertexArrays(1, &_planeVertexArray);
	glBindVertexArray(_planeVertexArray);

	glGenBuffers(1, &_planeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _planeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof (quadData), quadData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	//read and create mesh geometry
	readMesh("./models/bunny.ply"); //normalized vertices coordinates

	//triangle mesh
	glGenVertexArrays(1, &_meshVertexArray);
	glBindVertexArray(_meshVertexArray);

	glGenBuffers(1, &_meshVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _meshVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,  _mesh->vertices.size() * sizeof (trimesh::point), _mesh->vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &_meshNormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _meshNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, _mesh->normals.size() * sizeof(trimesh::vec), _mesh->normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &_meshIndicesBuffer );
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshIndicesBuffer );
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _triangleIndices.size()*sizeof(unsigned int), _triangleIndices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

}

//Creates trackbars for every changable value
void SceneShader::createTrackbars() {
	//create the trackbar
	blueTrackbar = new TrackBar(0.0, 1.0, 0.4, 0.015, _programTrackbar, "./textures/B.png");
	//adjust its position
	blueTrackbar -> setCenter(glm::vec2(-0.55, 0.95));
	//set the color
	blueTrackbar -> setColor(glm::vec3(0.0, 0.0, 0.8));
	//and the default value
	blueTrackbar -> setCurrentValue(0.5);

	yellowTrackbar = new TrackBar(0.0, 1.0, 0.4, 0.015, _programTrackbar, "./textures/Y.png");
	yellowTrackbar -> setCenter(glm::vec2(-0.55, 0.90));
	yellowTrackbar -> setColor(glm::vec3(0.8, 0.8, 0.0));
	yellowTrackbar -> setCurrentValue(0.5);

	alphaTrackbar = new TrackBar(0.0, 1.0, 0.4, 0.015, _programTrackbar, "./textures/alpha.png");
	alphaTrackbar -> setCenter(glm::vec2(-0.55, 0.85));
	alphaTrackbar -> setColor(glm::vec3(0.2, 0.2, 0.2));
	alphaTrackbar -> setCurrentValue(0.3);

	betaTrackbar = new TrackBar(0.0, 1.0, 0.4, 0.015, _programTrackbar, "./textures/beta.png");
	betaTrackbar -> setCenter(glm::vec2(-0.55, 0.80));
	betaTrackbar -> setColor(glm::vec3(0.2, 0.2, 0.2));
	betaTrackbar -> setCurrentValue(0.1);

	redColorTrackbar = new TrackBar(0.0, 1.0, 0.4, 0.015, _programTrackbar, "./textures/RC.png");
	redColorTrackbar -> setCenter(glm::vec2(-0.55, 0.75));
	redColorTrackbar -> setColor(glm::vec3(0.5, 0.0, 0.0));
	redColorTrackbar -> setCurrentValue(0.7);

	greenColorTrackbar = new TrackBar(0.0, 1.0, 0.4, 0.015, _programTrackbar, "./textures/GC.png");
	greenColorTrackbar -> setCenter(glm::vec2(-0.55, 0.70));
	greenColorTrackbar -> setColor(glm::vec3(0.0, 0.5, 0.0));
	greenColorTrackbar -> setCurrentValue(0.2);

	blueColorTrackbar = new TrackBar(0.0, 1.0, 0.4, 0.015, _programTrackbar, "./textures/BC.png");
	blueColorTrackbar -> setCenter(glm::vec2(-0.55, 0.65));
	blueColorTrackbar -> setColor(glm::vec3(0.0, 0.0, 0.5));
	blueColorTrackbar -> setCurrentValue(0.9);
}


void SceneShader::startup()
{
	_programPlane = compile_shaders("./shaders/plane.vert", "./shaders/plane.frag");

	_programMesh = compile_shaders("./shaders/mesh.vert", "./shaders/mesh.frag");

	_programLight = compile_shaders("./shaders/light.vert", "./shaders/light.frag");

	_programTrackbar = compile_shaders("./shaders/trackbar.vert", "./shaders/trackbar.frag");

	createVertexBuffer();
	createTrackbars();
}

void SceneShader::renderPlane()
{
	glBindVertexArray(_planeVertexArray);

	glUseProgram(_programPlane);

	//scene matrices and camera setup
	glm::vec3 eye(0.0f,0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);

	_modelview = glm::lookAt( eye, center, up);

	glm::mat4 identity(1.0f);
	_projection = glm::perspective( 45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot * PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationY = glm::rotate(identity, _xRot * PI/180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 translationZ = glm::translate(identity, glm::vec3(0.0f, 0.0f, _zTranslation));
        _modelview *=  translationZ * rotationY * rotationX;
	
	//Uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programPlane, "modelview"), 1, GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programPlane, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

	glUniform3fv(glGetUniformLocation(_programPlane, "lightPosition"), 1, glm::value_ptr(lightPosition) );

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

void SceneShader::renderMesh()
{
	glBindVertexArray(_meshVertexArray);

	glUseProgram(_programMesh);

	//scene matrices and camera setup
	glm::vec3 eye(0.0f, 0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);

	_modelview = glm::lookAt( eye, center, up);

	_projection = glm::perspective( 45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 identity(1.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot * PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationY = glm::rotate(identity, _xRot * PI/180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 translationZ = glm::translate(identity, glm::vec3(0.0f, 0.0f, _zTranslation));
        _modelview *=  translationZ * rotationY * rotationX;

	//uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programMesh, "modelview"), 1, GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programMesh, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

	glUniform3fv(glGetUniformLocation(_programMesh, "lightPosition"), 1, glm::value_ptr(lightPosition) );

	//goosh shading info
	glUniform1f(glGetUniformLocation(_programMesh, "b"), blueTrackbar->getCurrentValue());
	glUniform1f(glGetUniformLocation(_programMesh, "y"), yellowTrackbar->getCurrentValue());
	glUniform1f(glGetUniformLocation(_programMesh, "alpha"), alphaTrackbar->getCurrentValue());
	glUniform1f(glGetUniformLocation(_programMesh, "beta"), betaTrackbar->getCurrentValue());
	glm::vec3 objectColor = glm::vec3(redColorTrackbar->getCurrentValue(), greenColorTrackbar->getCurrentValue(), blueColorTrackbar->getCurrentValue());
	glUniform3fv(glGetUniformLocation(_programMesh, "objectColor"), 1, glm::value_ptr(objectColor));

	glDrawElements( GL_TRIANGLES, _mesh->faces.size()*3, GL_UNSIGNED_INT, 0 );

	glBindVertexArray(0);
}


void SceneShader::renderLight()
{
	glUseProgram(_programLight);

	//scene matrices and camera setup
	glm::vec3 eye(0.0f, 0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);

	_modelview = glm::lookAt( eye, center, up);

	_projection = glm::perspective( 45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 identity(1.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot * PI/180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationY = glm::rotate(identity, _xRot * PI/180.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 translationZ = glm::translate(identity, glm::vec3(0.0f, 0.0f, _zTranslation));
        _modelview *=  translationZ * rotationY * rotationX;

	//uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programLight, "modelview"), 1, GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programLight, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

	glUniform3fv(glGetUniformLocation(_programLight, "lightPosition"), 1, glm::value_ptr(lightPosition) );

	glPointSize(30.0f);
	glDrawArrays( GL_POINTS, 0, 1);

}

void SceneShader::render()
{
	renderPlane();
	renderMesh();
	renderLight();

	blueTrackbar -> render();
	yellowTrackbar -> render();
	alphaTrackbar -> render();
	betaTrackbar -> render();
	redColorTrackbar -> render();
	greenColorTrackbar -> render();
	blueColorTrackbar -> render();
}

void SceneShader::setZTranslation(float z)
{
	_zTranslation = z;
}

void SceneShader::setAspectRatio(float ratio)
{
	_aspectRatio = ratio;
}

void SceneShader::setRotationX( float x )
{
	_xRot = x;
}

void SceneShader::setRotationY( float y )
{
	_yRot = y;
}

void SceneShader::shutdown()
{
	glDeleteBuffers(1, &_meshVertexBuffer);
	glDeleteBuffers(1, &_meshNormalBuffer);
	glDeleteBuffers(1, &_meshIndicesBuffer );
	glDeleteVertexArrays(1, &_meshVertexArray);
	glDeleteVertexArrays(1, &_planeVertexArray);
}

void SceneShader::updateLightPositionX(float x)
{
	lightPosition.x += x;
}

void SceneShader::updateLightPositionY(float y)
{
	lightPosition.y += y;
}

void SceneShader::updateLightPositionZ(float z)
{
	lightPosition.z += z;
}

SceneShader::~SceneShader()
{
	shutdown();

	delete blueTrackbar;
	delete yellowTrackbar;
	delete alphaTrackbar;
	delete betaTrackbar;
	delete redColorTrackbar;
	delete greenColorTrackbar;
	delete blueColorTrackbar;
}

bool SceneShader::mouseStuckToTrackBar() {
	return getBarMouseStuckTo() != NULL;
}

void SceneShader::mousePressed(float x, float y) {
	//when the mouse is pressed we have to tell each trackbar about the event.
	//We stop as soon as one of the trackbars says that they've captured the event.
	blueTrackbar -> mousePressed(x, y) ||
	yellowTrackbar -> mousePressed(x, y) ||
	alphaTrackbar -> mousePressed(x, y) ||
	betaTrackbar -> mousePressed(x, y) ||
	redColorTrackbar -> mousePressed(x, y) ||
	greenColorTrackbar -> mousePressed(x, y) ||
	blueColorTrackbar -> mousePressed(x, y);

}

void SceneShader::mouseReleased() {
	//tell every trackbar about the event
	blueTrackbar -> mouseReleased();
	yellowTrackbar -> mouseReleased();
	alphaTrackbar -> mouseReleased();
	betaTrackbar -> mouseReleased();
	redColorTrackbar -> mouseReleased();
	greenColorTrackbar -> mouseReleased();
	blueColorTrackbar -> mouseReleased();
}

//get the trackbar we are adjust values for
TrackBar* SceneShader::getBarMouseStuckTo() {
	if (blueTrackbar -> getMouseStuck()) return blueTrackbar;
	if (yellowTrackbar -> getMouseStuck()) return yellowTrackbar;
	if (alphaTrackbar -> getMouseStuck()) return alphaTrackbar;
	if (betaTrackbar -> getMouseStuck()) return betaTrackbar;
	if (redColorTrackbar -> getMouseStuck()) return redColorTrackbar;
	if (greenColorTrackbar -> getMouseStuck()) return greenColorTrackbar;
	if (blueColorTrackbar -> getMouseStuck()) return blueColorTrackbar;

	return NULL;
}

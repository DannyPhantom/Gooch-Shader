/*
 * SceneShader.h
 *
 *  Created on: Nov 17, 2015
 *      Author: acarocha
 */

#ifndef SCENESHADER_H_
#define SCENESHADER_H_

#include "Shader.h"
#include <vector>

#define GLM_FORCE_RADIANS

#include "TriMesh.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TrackBar.h"

class SceneShader : public Shader
{
public:

	SceneShader();
	~SceneShader();

	void startup ();
	void shutdown ();
	void render();

	void renderPlane();
	void renderMesh();
	void renderLight();

	void setAspectRatio( float ratio );
	void setZTranslation(float z);
	void setRotationX( float x );
	void setRotationY( float y );

	void updateLightPositionX( float x );
	void updateLightPositionY( float y );
	void updateLightPositionZ( float z );

	bool mouseStuckToTrackBar();
	void mousePressed(float x, float y);
	void mouseReleased();
	TrackBar *getBarMouseStuckTo();

private:

	/*methods*/

	void readMesh( std::string filename );
	void createVertexBuffer();
	void createTrackbars();


	/*variables*/
	GLuint _programLight;
	GLuint _programPlane;
	GLuint _programMesh;
	GLuint _programTrackbar;

	GLuint _planeVertexArray;
	GLuint _planeVertexBuffer;

	GLuint _meshVertexArray;
	GLuint _meshVertexBuffer;
	GLuint _meshNormalBuffer;
	GLuint _meshIndicesBuffer;

        GLint _mvUniform, _projUniform;

	/* Matrices */
	glm::mat4 _modelview;
	glm::mat4 _projection;

	float _zTranslation;
	float _xRot;
	float _yRot;
	float _aspectRatio;

	/* Textures */
	GLuint _textureID;

	/*vertices*/
	std::vector<float> _vertices;

	trimesh::TriMesh* _mesh;
	std::vector<unsigned int> _triangleIndices;

	glm::vec3 lightPosition;

	/*track bars*/
	TrackBar* blueTrackbar;
	TrackBar* yellowTrackbar;
	TrackBar* alphaTrackbar;
	TrackBar* betaTrackbar;
	TrackBar* redColorTrackbar;
	TrackBar* greenColorTrackbar;
	TrackBar* blueColorTrackbar;
};

#endif /* SCENESHADER_H_ */

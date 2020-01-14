#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/qopenglframebufferobject>
#include <QVector>
#include <QVector3D>
#include <QFileInfo>
#include <QDebug>
#include <QString>
#include <QtGui/qimage.h>
#include <time.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Scenery
{
public:
	QOpenGLShaderProgram* mountainShaderProgram;
	QOpenGLShader* mountainVertexShader;
	QOpenGLShader* mountainFragmentShader;
	QOpenGLVertexArrayObject mountainVao;
	QOpenGLBuffer vvbo;
	
	QOpenGLShaderProgram* waterShaderProgram;
	QOpenGLShader* waterVertexShader;
	QOpenGLShader* waterFragmentShader;
	QOpenGLVertexArrayObject waterVao;
	QOpenGLBuffer waterVbo;

	QOpenGLShaderProgram* depthShaderProgram;
	QOpenGLShader* depthVertexShader;
	QOpenGLShader* depthFragmentShader;
	QOpenGLVertexArrayObject depthVao;
	//QOpenGLFramebufferObject frameBuffer;

	QVector<QVector3D> vertices;
	QVector<QVector2D> textureCord;
	QOpenGLBuffer tvbo;
	QOpenGLBuffer nvbo;
	float t = 0;
	int triangleCount;
	int waterCount;
	int textureCount;
	int waterHeight;

public:
	Scenery();
	void InitVAO();
	void InitVBO();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath, int mode);
	void PaintMountain(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QVector3D eyeDir);
	void PaintMountainShadow(QOpenGLShaderProgram *sp);
	void PaintWater(GLfloat * ProjectionMatrix, GLfloat * ModelViewMatrix, QVector3D eyeDir);
	void End(int mode);
	void static matrixFormat(GLfloat source[], GLfloat target[][4]);
	void static matrixFormat(QMatrix4x4 source, GLfloat target[][4]);
	void Begin(int mode);
};

//Reference:http://www.jayconrod.com/posts/34/water-simulation-in-glsl
//Ãö©óVAO VBO:https://www.photoneray.com/opengl-vao-vbo/
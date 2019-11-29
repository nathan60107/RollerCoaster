#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QVector>
#include <QVector3D>
#include <QFileInfo>
#include <QDebug>
#include <QString>
#include <QtGui/qimage.h>
#include <time.h>


class myTriangle
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

	QVector<QVector3D> vertices;
	QVector<QVector2D> textureCord;
	QOpenGLBuffer tvbo;
	float t = 0;
	int triangleCount;
	int waterCount;
	int textureCount;
	int waterHeight;

public:
	myTriangle();
	void InitVAO();
	void InitVBO();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath, bool isWater);
	void PaintMountain(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QVector3D eyeDir);
	void PaintWater(GLfloat * ProjectionMatrix, GLfloat * ModelViewMatrix, QVector3D eyeDir);
	void End(bool isWater);
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	void Begin(bool isWater);
};

//Reference:http://www.jayconrod.com/posts/34/water-simulation-in-glsl
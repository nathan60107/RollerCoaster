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


class myTriangle
{
public:
	QOpenGLShaderProgram* shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	QVector<QVector3D> vertices;
	QVector<QVector2D> textureCord;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer cvbo;
	QOpenGLBuffer tvbo;
	int t = 0;

public:
	myTriangle();
	void Init();
	void InitVAO();
	void InitVBO();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QVector3D eyeDir);
	void End();
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	void Begin();
};

//Reference:http://www.jayconrod.com/posts/34/water-simulation-in-glsl

#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QtOpenGL/QtOpenGL> 
#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QFileInfo>
#include <QOpenGLTexture>
#include <QImage>
#include <QDebug>
#include <QString>

class Skybox
{
public:
	QOpenGLShaderProgram* shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	QOpenGLTexture *texture;
	GLuint textureIndex = 2;
	QVector<QVector3D> vertices;
	QVector<QVector2D> uvs;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer uvbo;

public:
	Skybox(int tIndex);
	void Init();
	void InitVAO();
	void InitVBO();
	void InitTexture();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	void Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QVector3D eyePos);
};
//Reference:https://amin-ahmadi.com/2019/07/28/creating-a-skybox-using-c-qt-and-opengl/
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QFile>
//#include <QtCore/QTextStream>
//#include <QtCore/QVarLengthArray>
#include <QtOpenGL/QtOpenGL>
#include <QVector3D>
#include <QVector2D>

#include <math.h>

#include "point3d.h"
#define PI 3.1415926535897932384626433832795

struct obj2{
	QVector<QVector3D> vertexs;
	QVector<QVector3D> normals;
	QVector<QVector2D> texCoords;

	QOpenGLBuffer vvbo;
	QOpenGLBuffer nvbo;
	QOpenGLBuffer tvbo;

	int textureNumber;
	bool isWheel = 0;
	QVector3D center;
};

class Tunnel {
public:
	QVector<Point3d> vertex;
	QVector<Point3d> normal;
	QVector<Point3d> texCoord;
	QVector<int> face[35][3];

	obj2 trainPart[35];
	int objCounter = -1;
	float wheelTime = 0;
	float scale = 1;
	Point3d pos, front, orient;
	float rX, rY, rZ;
	QVector3D trainPos;

	QOpenGLShaderProgram* shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;

	Tunnel(const QString &filePath, const QString &vsPath, const QString &fsPath);
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	void drawTrain(GLfloat * ProjectionMatrix, GLfloat * ModelViewMatrix, QVector3D trainPos_);
	void drawTrainShadow(QOpenGLShaderProgram * sp);
};
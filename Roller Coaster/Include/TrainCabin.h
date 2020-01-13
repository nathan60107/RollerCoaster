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
#include "TrainHead.h"
#define PI 3.1415926535897932384626433832795
/*
struct obj{
	QVector<QVector3D> vertexs;
	QVector<QVector3D> normals;
	QVector<QVector2D> texCoords;

	QOpenGLBuffer vvbo;
	QOpenGLBuffer nvbo;
	QOpenGLBuffer tvbo;

	int textureNumber;
};*/

class TrainCabin {
public:
	QVector<Point3d> vertex;
	QVector<Point3d> normal;
	QVector<Point3d> texCoord;
	QVector<int> face[17][3];

	obj trainPart[20];
	int objCounter = -1;
	float wheelTime = 0;
	float scale = 1;

	QOpenGLShaderProgram* shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;

	TrainCabin(const QString &filePath, const QString &vsPath, const QString &fsPath);
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	void drawTrain(GLfloat * ProjectionMatrix, GLfloat * ModelViewMatrix, QVector3D pos, float rX, float rY);
};
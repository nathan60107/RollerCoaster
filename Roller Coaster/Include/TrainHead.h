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

struct obj{
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

class TrainHead {
public:
	QVector<Point3d> vertex;
	QVector<Point3d> normal;
	QVector<Point3d> texCoord;
	QVector<int> face[17][3];

	obj trainPart[20];
	int objCounter = -1;
	float wheelTime = 0;
	float scale = 1;
	Point3d pos, front, orient;

	QOpenGLShaderProgram* shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;

	TrainHead(const QString &filePath, const QString &vsPath, const QString &fsPath);
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);
	void DimensionTransformation(GLfloat source[], GLfloat target[][4]);
	void drawTrain(GLfloat * ProjectionMatrix, GLfloat * ModelViewMatrix, QVector3D trainPos, Point3d p, Point3d dir, Point3d ori, float rX, float rY, float rZ);
};
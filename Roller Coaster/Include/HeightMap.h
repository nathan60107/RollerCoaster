#pragma once
#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QtGui/qimage.h>

class heightMap {
public:
	QVector<QVector3D> vertices;//for mountain
	QVector<QVector3D> normal;
	QVector<QVector3D> triangle; // stroe 3 vertices index
	QVector<QVector3D> verticesVbo;
	QVector<QVector2D> textureCordVbo;
	QVector<QVector3D> normalVbo;

	QVector<QVector3D> water_verticesVbo;
	
	int mapSize;
	float waterHeight;
	QImage heightMapImage;

	heightMap(QString path, int size, int baseHeight, float waterH);
	void generateWater();
};
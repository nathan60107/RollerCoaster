#pragma once
#include <QVector>
#include <QVector3D>
#include <QtGui/qimage.h>

class heightMap {
public:
	QVector<QVector3D> vertices;
	QVector<QVector3D> normal;
	QVector<QVector3D> triangle; // stroe 3 vertices index
	QVector<QVector3D> verticesVbo;
	QVector<QVector3D> normalVbo;
	int mapSize;
	QImage heightMapImage;

	heightMap::heightMap(QString path, int size, int baseHeight);
};
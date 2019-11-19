#ifndef MODEL_H
#define MODEL_H

#include <QtCore/QString>
#include <QtCore/QVector>

#include <math.h>

#include "point3d.h"
#define PI 3.1415926535897932384626433832795

class Model
{
public:
	Model() {}
	Model(const QString &filePath, int s, Point3d p);

	void render(bool wireframe = false, bool normals = false) const;

	QString fileName() const { return m_fileName; }
	int faces() const { return m_pointIndices.size() / 3; }
	int edges() const { return m_edgeIndices.size() / 2; }
	int points() const { return m_points.size(); }

	void changePos(Point3d p, Point3d dir, Point3d ori);
	float* matmat(float a[3][3], float b[3][3]);

public:
	QString m_fileName;
	QVector<Point3d> m_points, origin;
	QVector<Point3d> m_normals;
	QVector<int> m_edgeIndices;
	QVector<int> m_pointIndices;
	int modelscale;
	Point3d bounds, modelBoundsMin;
	float rotateX, rotateY, rotateZ;
	Point3d pos, front, orient;
};

#endif
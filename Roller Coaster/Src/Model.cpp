#include "model.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QVarLengthArray>

#include <QtOpenGL/QtOpenGL>

Model::Model(const QString &filePath, int s, Point3d p)
	: m_fileName(QFileInfo(filePath).fileName()), modelscale(s)
{
	QFile file(filePath);

	QFileInfo info(filePath);
	if (!info.exists())
	{
		qDebug() << info.filePath() << " can't be found";
	}

	if (!file.open(QIODevice::ReadOnly))
		return;

	Point3d boundsMin( 1e9, 1e9, 1e9);
	Point3d boundsMax(-1e9,-1e9,-1e9);

	QTextStream in(&file);
	while (!in.atEnd()) {
		QString input = in.readLine();
		if (input.isEmpty() || input[0] == '#')
			continue;

		QTextStream ts(&input);
		QString id;
		ts >> id;
		if (id == "v") {
			Point3d p;
			for (int i = 0; i < 3; ++i) {
				ts >> p[i];
				boundsMin[i] = qMin(boundsMin[i], p[i]);
				boundsMax[i] = qMax(boundsMax[i], p[i]);
			}
			m_points << p;
		} else if (id == "f" || id == "fo") {
			QVarLengthArray<int, 4> p;

			while (!ts.atEnd()) {
				QString vertex;
				ts >> vertex;
				const int vertexIndex = vertex.split('/').value(0).toInt();
				if (vertexIndex)
					p.append(vertexIndex > 0 ? vertexIndex - 1 : m_points.size() + vertexIndex);
			}

			for (int i = 0; i < p.size(); ++i) {
				const int edgeA = p[i];
				const int edgeB = p[(i + 1) % p.size()];

				if (edgeA < edgeB)
					m_edgeIndices << edgeA << edgeB;
			}

			for (int i = 0; i < 3; ++i)
				m_pointIndices << p[i];

			if (p.size() == 4)
				for (int i = 0; i < 3; ++i)
					m_pointIndices << p[(i + 2) % 4];
		}
	}

	bounds = boundsMax - boundsMin;
	modelBoundsMin = boundsMin;
	/*const Point3d bounds = boundsMax - boundsMin;
	const qreal scale = s / qMax(bounds.x, qMax(bounds.y, bounds.z));
	for (int i = 0; i < m_points.size(); ++i)
		m_points[i] = (m_points[i] + p - (boundsMin + bounds * 0.5)) * scale;*/

	m_normals.resize(m_points.size());
	for (int i = 0; i < m_pointIndices.size(); i += 3) {
		const Point3d a = m_points.at(m_pointIndices.at(i));
		const Point3d b = m_points.at(m_pointIndices.at(i+1));
		const Point3d c = m_points.at(m_pointIndices.at(i+2));

		const Point3d normal = cross(b - a, c - a).normalize();

		for (int j = 0; j < 3; ++j)
			m_normals[m_pointIndices.at(i + j)] += normal;
	}

	for (int i = 0; i < m_normals.size(); ++i)
		m_normals[i] = m_normals[i].normalize();

	origin = m_points;
}

void Model::render(bool wireframe, bool normals) const
{
	glEnable(GL_DEPTH_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);

	if (wireframe) {
		glVertexPointer(3, GL_FLOAT, 0, (float *)m_points.data());
		glDrawElements(GL_LINES, m_edgeIndices.size(), GL_UNSIGNED_INT, m_edgeIndices.data());
	} else {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_COLOR_MATERIAL);
		glShadeModel(GL_SMOOTH);

		glEnableClientState(GL_NORMAL_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, (float *)m_points.data());
		glNormalPointer(GL_FLOAT, 0, (float *)m_normals.data());
		glDrawElements(GL_TRIANGLES, m_pointIndices.size(), GL_UNSIGNED_INT, m_pointIndices.data());

		glDisableClientState(GL_NORMAL_ARRAY);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
	}

	if (normals) {
		QVector<Point3d> normals;
		for (int i = 0; i < m_normals.size(); ++i)
			normals << m_points.at(i) << (m_points.at(i) + m_normals.at(i) * 0.02f);
		glVertexPointer(3, GL_FLOAT, 0, (float *)normals.data());
		glDrawArrays(GL_LINES, 0, normals.size());
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_DEPTH_TEST);
}

void Model::changePos(Point3d p, Point3d dir, Point3d ori){
	pos = p, front = dir.normalize(), orient = ori;

	rotateX = - PI / 2 - acos(ori.y) * (ori.z>0 ? 1 : -1);
	float rX[3][3] = { {1,0,0}, {0,cos(rotateX), -sin(rotateX)}, {0, sin(rotateX), cos(rotateX)} };
	for (int i = 0; i < origin.size(); ++i)
		m_points[i] = origin[i].matmul(rX);

	rotateY = acos(dir.x) * (dir.z>0 ? -1 : 1);
	float rY[3][3] = { {cos(rotateY),0,sin(rotateY)}, {0,1, 0}, {-sin(rotateY), 0, cos(rotateY)} };
	for (int i = 0; i < origin.size(); ++i)
		m_points[i] = m_points[i].matmul(rY);

	/*rotateZ = acos(ori.y) * (ori.x > 0 ? 1 : -1);
	float rZ[3][3] = { {cos(rotateZ),-sin(rotateZ), 0}, {sin(rotateZ), cos(rotateZ), 0}, {0, 0, 1} };
	for (int i = 0; i < origin.size(); ++i)
		m_points[i] = m_points[i].matmul(rZ);*/
	//Reference: https://zh.wikipedia.org/zh-tw/%E6%97%8B%E8%BD%AC%E7%9F%A9%E9%98%B5

	p *= (1.0 / modelscale);
	p += Point3d{0, 0, 4.0};
	
	const qreal scale = modelscale / qMax(bounds.x, qMax(bounds.y, bounds.z));
	for (int i = 0; i < m_points.size(); ++i)
		m_points[i] = (m_points[i] - (modelBoundsMin + bounds * 0.5)) * scale + p;

}


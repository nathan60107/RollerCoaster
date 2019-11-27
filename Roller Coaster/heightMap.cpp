#include "heightMap.h"

heightMap::heightMap(QString path, int size, int baseHeight){
	heightMapImage = QImage("../../Textures/heightmap.jpg");

	for (int i = 0; i <= size; i++){
		for (int j = 0; j <= size; j++) {
			int i_t = (i + mapSize) / 2.0 / mapSize * (heightMapImage.width() - 1),
				j_t = (j + mapSize) / 2.0 / mapSize * (heightMapImage.height() - 1);
			int height = qGray(heightMapImage.pixel(i_t, j_t));

			vertices << QVector3D(i - size / 2, baseHeight * height / 25.5, j - size / 2);//dimension = (size+1)^2
		}
	}
	printf("ver size = %d\n", vertices.size());

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			triangle << QVector3D(i * (size + 1) + j, (i + 1) * (size + 1) + j, i * (size + 1) + j + 1);
			triangle << QVector3D((i + 1) * (size + 1) + j + 1, (i + 1) * (size + 1) + j, i * (size + 1) + j + 1);
		}
	}
	printf("tri size = %d\n", triangle.size());
	
	normal.resize(vertices.size());
	for (int i = 0; i < triangle.size(); i++) {
		QVector3D tri[3] = { vertices[triangle[i][0]], vertices[triangle[i][1]], vertices[triangle[i][2]] };
		QVector3D vec[2] = { tri[0] - tri[1], tri[2] - tri[1] };
		QVector3D cross = QVector3D::crossProduct(vec[0], vec[2]).normalized();
		for (int j = 0; j < 3; j++) {
			normal[triangle[i][j]] += cross;
		}
	}

	for (int i = 0; i < normal.size(); i++) {
		normal[i].normalize();
	}

	for (int i = 0; i < triangle.size(); i++) {
		int index[3] = { triangle[i][0], triangle[i][1], triangle[i][2] };
		verticesVbo << vertices[index[0]] << vertices[index[1]] << vertices[index[2]];
		normalVbo << normal[index[0]] << normal[index[1]] << normal[index[2]];
	}
}
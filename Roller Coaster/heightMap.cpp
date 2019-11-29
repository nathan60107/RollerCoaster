#include "heightMap.h"

heightMap::heightMap(QString path, int size, int baseHeight, float waterH):waterHeight(waterH){
	heightMapImage = QImage("../../Textures/heightmap.jpg");

	for (int i = 0; i <= size; i++){
		for (int j = 0; j <= size; j++) {
			int i_t = i / 2.0 / size * (heightMapImage.width() - 1),
				j_t = j / 2.0 / size * (heightMapImage.height() - 1);
			int height = qGray(heightMapImage.pixel(i_t, j_t));

			vertices << QVector3D(i - size / 2, baseHeight * height / 255.0 * 2, j - size / 2);//dimension = (size+1)^2
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
		for (int j = 0; j < 3; j++) {
			verticesVbo << vertices[index[j]];
			textureCordVbo << QVector2D(vertices[index[j]].x() / 2.0, vertices[index[j]].z() / 2.0);
			normalVbo << normal[index[j]];

			//printf("%f %f\n", vertices[index[j]].x(), vertices[index[j]].z());
			//printf("%f %f\n", textureCordVbo[textureCordVbo.size() - 1].x(), textureCordVbo[textureCordVbo.size() - 1].y());
		}
	}

	/*for (int i = 0; i < verticesVbo.size(); i++) {
		printf("%f %f %f\n", verticesVbo[i][0], verticesVbo[i][1], verticesVbo[i][2]);
	}*/
}

void heightMap::generateWater()
{
	for (int i = 0; i < triangle.size(); i+=2) {
		//if square lower than water than add a water square on the ground
		QVector3D tri[4] = { vertices[triangle[i][0]], vertices[triangle[i][1]], vertices[triangle[i][2]], vertices[triangle[i+1][0]]};

		for (int j = 0; j < 4; j++) {
			if (tri[j].y() >= waterHeight)goto NEXT;
		}

		water_verticesVbo << tri[0] + QVector3D(0, 1, 0) << tri[1] + QVector3D(0, 1, 0) << tri[2] + QVector3D(0, 1, 0)
						<< tri[3] + QVector3D(0, 1, 0) << tri[1] + QVector3D(0, 1, 0) << tri[2] + QVector3D(0, 1, 0);

		NEXT:;
	}
	printf("water ver size = %d\n", water_verticesVbo.size()/3);
}

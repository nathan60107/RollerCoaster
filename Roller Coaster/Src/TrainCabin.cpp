#include "TrainCabin.h"

TrainCabin::TrainCabin(const QString & filePath, const QString &vsPath, const QString &fsPath)
{
	//initial shader
	InitShader(vsPath, fsPath);

	//read obj file
	QFile file(filePath);

	QFileInfo info(filePath);//�B�z�ɮפ��s�b�����p
	if (!info.exists())
	{
		qDebug() << info.filePath() << " can't be found";
	}

	if (!file.open(QIODevice::ReadOnly))
		return;

	QTextStream in(&file);
	float texMax[2] = { 1.484000, 1.905700 }, texMin[2] = { -0.006100, -0.164200 };
	while (!in.atEnd()) {
		QString input = in.readLine();//input=�ɮפ����䤤�@��
		if (input.isEmpty() || input[0] == '#')
			continue;

		if (input[0] == 'g')//�C�J��@��g�N���U�@�ӹs��
			objCounter++;

		QTextStream ts(&input);
		QString id;
		ts >> id;//id=�C�@��}�Y�Ĥ@�ӵ�

		if (id == "v") {//v��vertex��²��
			Point3d p;
			for (int i = 0; i < 3; ++i) {
				ts >> p[i];
			}
			vertex << p;
			/*auto aa = vertex[g][vertex[g].size() - 1];
			printf("v  %f %f %f\n", aa.x, aa.y, aa.z);*/
		}
		else if (id == "vn") {//vertex normal
			Point3d p;
			for (int i = 0; i < 3; ++i) {
				ts >> p[i];
			}
			normal << p;
		}
		else if (id == "vt") {//vertex texture coordinate
			Point3d p;
			for (int i = 0; i < 2; ++i) {
				ts >> p[i];
				/*if (p[i] > 1)(2-p[i]);
				else if(p[i] < 0)p[i] *= -1;*/
				//p[i] = (p[i] - texMin[i]) / (texMax[i] - texMin[i]);
				//p[i] = 0;
			}
			//printf("%f %f\n", p[0], p[1]);
			for (int i = 0; i < 2; i++) {
				texMax[i] = texMax[i] > p[i] ? texMax[i] : p[i];
				texMin[i] = texMin[i] < p[i] ? texMin[i] : p[i];
			}
			texCoord << p;
		}
		else if (id == "f") {//face, �榡��:f vertex1/texcoodr1/normal1 vertex2.... vertex3... vertex4...
			while (!ts.atEnd()) {
				QString str;
				ts >> str;
				if (str.isEmpty()) {
					break;
				}
				//printf("\"%s\"\n", str.toStdString());

				for (int i = 0; i < 3; i++) {
					const int index = str.split('/').value(i).toInt();//�H/�@�����j��T�ӼƦr���}�Өä��}�x�s i=1�Overtex 2�Otexcoord 3�Onormal

					//printf("%s ", str.split('/').value(i));
					face[objCounter][i] << (index - 1);//�w�g�T�{�L�S���tindex
				}
			}
		}
	}
	printf("%f %f %f %f\n", texMax[0], texMax[1], texMin[0], texMin[1]);

	//build buffer
	for (int i = 0; i <= objCounter; i++) {
		for (int j = 0; j < face[i][0].size(); j += 4) {
			for (int k = 0; k < 3; k++) {//�̷�face����index��F���i�h
				//printf("%d %d\n", face[i][1][j + k], texCoord.size());
				trainPart[i].vertexs << QVector3D(vertex[face[i][0][j + k]].x, vertex[face[i][0][j + k]].z, vertex[face[i][0][j + k]].y);
				trainPart[i].texCoords << QVector2D(texCoord[face[i][1][j + k]].x, texCoord[face[i][1][j + k]].y);
				trainPart[i].normals << QVector3D(normal[face[i][2][j + k]].x, normal[face[i][2][j + k]].y, normal[face[i][2][j + k]].z);
			}
			for (int k = 2; k != 1; k = (k+1)%4) {
				trainPart[i].vertexs << QVector3D(vertex[face[i][0][j + k]].x, vertex[face[i][0][j + k]].z, vertex[face[i][0][j + k]].y);
				trainPart[i].texCoords << QVector2D(texCoord[face[i][1][j + k]].x, texCoord[face[i][1][j + k]].y);
				trainPart[i].normals << QVector3D(normal[face[i][2][j + k]].x, normal[face[i][2][j + k]].y, normal[face[i][2][j + k]].z);
			}
		}

		//�Ы�vbo��
		trainPart[i].vvbo.create();
		trainPart[i].vvbo.bind();
		trainPart[i].vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
		trainPart[i].vvbo.allocate(trainPart[i].vertexs.constData(), trainPart[i].vertexs.size() * sizeof(QVector3D));

		trainPart[i].tvbo.create();
		trainPart[i].tvbo.bind();
		trainPart[i].tvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
		trainPart[i].tvbo.allocate(trainPart[i].texCoords.constData(), trainPart[i].texCoords.size() * sizeof(QVector2D));

		trainPart[i].nvbo.create();
		trainPart[i].nvbo.bind();
		trainPart[i].nvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
		trainPart[i].nvbo.allocate(trainPart[i].normals.constData(), trainPart[i].normals.size() * sizeof(QVector3D));

		//�B�z��L�Ѽ�
		if (i == 0 || i==1) {
			trainPart[i].textureNumber = 2;
		}
		else if(i == 2) {
			trainPart[i].textureNumber = 3;
		}
		else if(i == 3 || i==6 || i==7) {
			trainPart[i].textureNumber = 4;
		}
		else if(i == 4 || i==5) {
			trainPart[i].textureNumber = 5;
		}
	}
	scale = 0.001;
}

void TrainCabin::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
{
	printf("Loading shader \"%s\" & \"%s\"...\n", vertexShaderPath.toStdString().c_str(), fragmentShaderPath.toStdString().c_str());

	// Create Shader
	shaderProgram = new QOpenGLShaderProgram();
	QFileInfo  vertexShaderFile(vertexShaderPath);
	if (vertexShaderFile.exists())
	{
		vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
		if (vertexShader->compileSourceFile(vertexShaderPath))
			shaderProgram->addShader(vertexShader);
		else
			qWarning() << "Vertex Shader Error " << vertexShader->log();
	}
	else
		qDebug() << vertexShaderFile.filePath() << " can't be found";

	QFileInfo  fragmentShaderFile(fragmentShaderPath);
	if (fragmentShaderFile.exists())
	{
		fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
		if (fragmentShader->compileSourceFile(fragmentShaderPath))
			shaderProgram->addShader(fragmentShader);
		else
			qWarning() << "fragment Shader Error " << fragmentShader->log();
	}
	else
		qDebug() << fragmentShaderFile.filePath() << " can't be found";
	shaderProgram->link();
}

void TrainCabin::DimensionTransformation(GLfloat source[], GLfloat target[][4])
{
	//for uniform value, transfer 1 dimension to 2 dimension
	int i = 0;
	for (int j = 0; j < 4; j++)
		for (int k = 0; k < 4; k++)
		{
			target[j][k] = source[i];
			i++;
		}
}

void TrainCabin::drawTrain(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QVector3D pos, float rX, float rY) {
	shaderProgram->bind();
	
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);
	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ModelViewMatrix", MV);

	shaderProgram->setUniformValue("trainPos", pos);
	shaderProgram->setUniformValue("rX", rX);
	shaderProgram->setUniformValue("rY", rY);
	shaderProgram->setUniformValue("scale", scale);

	for (int i = 0; i <= objCounter; i++) {
		//printf("%d \n", objCounter);
		shaderProgram->setUniformValue("Texture", 5);
		shaderProgram->setUniformValue("center", trainPart[i].center);
		shaderProgram->setUniformValue("isWheel", trainPart[i].isWheel);
		shaderProgram->setUniformValue("time", -wheelTime);

		trainPart[i].vvbo.bind();
		shaderProgram->enableAttributeArray(0);
		shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
		trainPart[i].vvbo.release();

		trainPart[i].tvbo.bind();
		shaderProgram->enableAttributeArray(1);
		shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 2, NULL);
		trainPart[i].tvbo.release();

		trainPart[i].nvbo.bind();
		shaderProgram->enableAttributeArray(2);
		shaderProgram->setAttributeArray(2, GL_FLOAT, 0, 3, NULL);
		trainPart[i].nvbo.release();

		//Draw triangles with 4 indices starting from the 0th index
		glDrawArrays(GL_TRIANGLES, 0, trainPart[i].vertexs.size());
	}

	wheelTime += 0.1;

	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);
	shaderProgram->disableAttributeArray(2);

	shaderProgram->release();
}

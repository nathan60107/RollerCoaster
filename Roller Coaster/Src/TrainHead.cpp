#include "TrainHead.h"

TrainHead::TrainHead(const QString & filePath, const QString &vsPath, const QString &fsPath)
{
	//initial shader
	InitShader(vsPath, fsPath);

	//read obj file
	QFile file(filePath);

	QFileInfo info(filePath);
	if (!info.exists())
	{
		qDebug() << info.filePath() << " can't be found";
	}

	if (!file.open(QIODevice::ReadOnly))
		return;

	QTextStream in(&file);
	while (!in.atEnd()) {
		QString input = in.readLine();
		if (input.isEmpty() || input[0] == '#')
			continue;

		if (input[0] == 'g')
			objCounter++;

		QTextStream ts(&input);
		QString id;
		ts >> id;

		if (id == "v") {//42194
			Point3d p;
			for (int i = 0; i < 3; ++i) {
				ts >> p[i];
			}
			vertex << p;
			/*auto aa = vertex[g][vertex[g].size() - 1];
			printf("v  %f %f %f\n", aa.x, aa.y, aa.z);*/
		}
		else if (id == "vn") {
			Point3d p;
			for (int i = 0; i < 3; ++i) {
				ts >> p[i];
			}
			normal << p;
		}
		else if (id == "vt") {//46072
			Point3d p;
			for (int i = 0; i < 2; ++i) {
				ts >> p[i];
			}
			texCoord << p;
		}
		else if (id == "f") {
			QVarLengthArray<int, 4> p;

			while (!ts.atEnd()) {
				QString str;
				ts >> str;
				if (str.isEmpty()) {
					break;
				}
				//printf("\"%s\"\n", str.toStdString());

				for (int i = 0; i < 3; i++) {
					const int index = str.split('/').value(i).toInt();

					//printf("%s ", str.split('/').value(i));
					face[objCounter][i] << (index - 1);//已經確認過沒有負index
				}
			}
		}
	}

	//build buffer
	for (int i = 0; i <= objCounter; i++) {
		for (int j = 0; j < face[i][0].size(); j += 4) {
			for (int k = 0; k < 3; k++) {
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

		if (i == 0 || i == 1 || i==3 || i == 7 || i == 8 || i == 10 || i == 12) {
			trainPart[i].textureNumber = 3;//wood
		}
		else if (i == 2 || i == 4) {
			trainPart[i].textureNumber = 4;//blue
		}
		else {
			trainPart[i].textureNumber = 5;//black
		}

		if (i == 12 || i == 10 || i == 8 || i == 7) {
			trainPart[i].isWheel = 1;
			trainPart[i].center.setX(0);
			trainPart[i].center.setY(0);
			trainPart[i].center.setZ(0);
			for (int j = 0; j < trainPart[i].vertexs.size(); j++) {
				trainPart[i].center += trainPart[i].vertexs[j] / trainPart[i].vertexs.size();
			}
		}
		else {
			trainPart[i].isWheel = 0;
		}
	}
}

void TrainHead::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
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

void TrainHead::DimensionTransformation(GLfloat source[], GLfloat target[][4])
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

void TrainHead::drawTrain(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QVector3D pos, float rX, float rY) {
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

	for (int i = 0; i <= objCounter; i++) {
		shaderProgram->setUniformValue("Texture", trainPart[i].textureNumber);
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

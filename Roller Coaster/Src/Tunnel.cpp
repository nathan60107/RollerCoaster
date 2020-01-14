#include "Tunnel.h"

Tunnel::Tunnel(const QString & filePath, const QString &vsPath, const QString &fsPath)
{
	//initial shader
	InitShader(vsPath, fsPath);

	//read obj file
	QFile file(filePath);

	QFileInfo info(filePath);//處理檔案不存在的狀況
	if (!info.exists())
	{
		qDebug() << info.filePath() << " can't be found";
	}

	if (!file.open(QIODevice::ReadOnly))
		return;

	//objCounter = 0;

	QTextStream in(&file);
	while (!in.atEnd()) {
		QString input = in.readLine();//input=檔案中的其中一行

		if (input.isEmpty() || input[0] == '#')
			continue;

		if (input[0] == 'f')//每遇到一個g就換下一個零件
			objCounter++;

		QTextStream ts(&input);
		QString id;
		ts >> id;//id=每一行開頭第一個詞

		if (id == "v") {//v為vertex的簡稱
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
			}
			texCoord << p;
		}
		else if (id == "f") {//face, 格式為:f vertex1/texcoodr1/normal1 vertex2.... vertex3... vertex4...
			while (!ts.atEnd()) {
				QString str;
				ts >> str;
				//printf("%s ", str.toStdString().c_str());
				if (str.isEmpty()) {
					break;
				}
				//printf("\"%s\"\n", str.toStdString());

				for (int i = 0; i < 3; i++) {
					const int index = str.split('/').value(i).toInt();//以/作為分隔把三個數字分開來並分開儲存 i=1是vertex 2是texcoord 3是normal

					//printf("%s ", str.split('/').value(i));
					face[objCounter][i] << (index - 1);//已經確認過沒有負index
				}
			}
		}
	}

	//build buffer
	int tc = 0;
	for (int i = 0; i <= objCounter; i++) {
		for (int j = 0; j < 40 /*face[i][0].size()*/; j += 40) {
			for (int b = 0; b < 40; b +=1) {
				for (int k = 0; k <1; k++) {//依照face給的index把東西丟進去
					//printf("%d %d\n", face[i][1][j + k], texCoord.size());
					trainPart[i].vertexs << QVector3D(vertex[face[i][0][j+(b+k)%40]].x, vertex[face[i][0][j+(b+k)%40]].y, vertex[face[i][0][j+(b+k)%40]].z);
					trainPart[i].texCoords << QVector2D(texCoord[face[i][1][j+(b+k)%40]].x, texCoord[face[i][1][j+(b+k)%40]].y);
					trainPart[i].normals << QVector3D(normal[face[i][2][j+(b+k)%40]].x, normal[face[i][2][j+(b+k)%40]].y, normal[face[i][2][j+(b+k)%40]].z);
				}
			}
		}

		//創建vbo們
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
		
	}
}

void Tunnel::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
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

void Tunnel::DimensionTransformation(GLfloat source[], GLfloat target[][4])
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

void Tunnel::drawTrain(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QVector3D trainPos_) {
	trainPos = trainPos_;

	shaderProgram->bind();
	
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);
	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ModelViewMatrix", MV);

	shaderProgram->setUniformValue("trainPos", trainPos);
	shaderProgram->setUniformValue("rX", 0);
	shaderProgram->setUniformValue("rY", float(45.0 / 180 * PI));
	shaderProgram->setUniformValue("rZ", 0);
	shaderProgram->setUniformValue("scale", scale);

	for (int i = 0; i <= objCounter; i++) {
		shaderProgram->setUniformValue("Texture", 3);
		shaderProgram->setUniformValue("center", QVector3D(0, 0, 0));
		shaderProgram->setUniformValue("isWheel", false);
		shaderProgram->setUniformValue("time", (float)0);

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
		glDrawArrays(GL_POLYGON, 0, trainPart[i].vertexs.size());
	}

	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);
	shaderProgram->disableAttributeArray(2);

	shaderProgram->release();
}

void Tunnel::drawTrainShadow(QOpenGLShaderProgram *sp)
{
	sp->setUniformValue("trainPos", trainPos);
	sp->setUniformValue("rX", 0);
	sp->setUniformValue("rY", float(45.0 / 180 * PI));
	sp->setUniformValue("rZ", 0);
	//printf("%f %f %f\n", rX, rY, rZ);

	for (int i = 0; i <= objCounter; i++) {
		trainPart[i].vvbo.bind();
		shaderProgram->enableAttributeArray(0);
		shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
		trainPart[i].vvbo.release();

		//Draw triangles with 4 indices starting from the 0th index
		glDrawArrays(GL_POLYGON, 0, trainPart[i].vertexs.size());
	}

	sp->disableAttributeArray(0);
}
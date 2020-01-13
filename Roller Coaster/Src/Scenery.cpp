#include "Scenery.h"
#include "heightMap.h"
#define M_PI 3.14159

Scenery::Scenery()
{
}
void Scenery::matrixFormat(GLfloat source[], GLfloat target[][4])
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
void Scenery::matrixFormat(QMatrix4x4 source, GLfloat target[][4])
{
	//for uniform value, transfer 1 dimension to 2 dimension
	int i = 0;
	for (int j = 0; j < 4; j++)
		for (int k = 0; k < 4; k++)
		{
			target[j][k] = source.data()[k*4+j];
			//printf("%f %f\n", target[j][k], source.data()[i]);
			i++;
		}//printf("\n");
}
void Scenery::Begin(int mode)
{
	/*if (mode == 2) {
		depthShaderProgram->bind();
		depthVao.bind();
	}else */if (mode==1) {
		waterShaderProgram->bind();
		waterVao.bind();
	} else{
		mountainShaderProgram->bind();
		mountainVao.bind();
	}
}

std::string format(const std::string fmt, ...) {
	va_list args;

	va_start(args, fmt);
	char *buffer = NULL;
	int size = vsnprintf(buffer, 0, fmt.c_str(), args);
	va_end(args);
	buffer = new char[size + 1];

	va_start(args, fmt);
	vsnprintf(buffer, size + 1, fmt.c_str(), args);
	va_end(args);
	std::string str(buffer);
	delete[] buffer;

	va_end(args);
	return str;
}

float uniformRandomInRange(float min, float max) {
	double n = (double)rand() / (double)RAND_MAX;
	double v = min + n * (max - min);
	return v;
}

void Scenery::PaintMountain(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QVector3D eyePos)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	matrixFormat(ProjectionMatrix, P);
	matrixFormat(ModelViewMatrix, MV);

	//pass projection matrix to shader
	mountainShaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	mountainShaderProgram->setUniformValue("ModelViewMatrix", MV);
	//mountainShaderProgram->setUniformValue("eye_direction", eyePos);

	// Bind the buffer so that it is the current active buffer.
	vvbo.bind();
	// Enable Attribute 0
	mountainShaderProgram->enableAttributeArray(0);
	// Set Attribute 0 to be position
	mountainShaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	//unbind buffer
	vvbo.release();

	//printf("%d\n", waterVbo.size());
	// Bind the buffer so that it is the current active buffer.
	tvbo.bind();
	// Enable Attribute 1
	mountainShaderProgram->enableAttributeArray(1);
	// Set Attribute 0 to be position
	mountainShaderProgram->setAttributeArray(1, GL_FLOAT, 0, 2, NULL);
	//unbind buffer
	tvbo.release();

	nvbo.bind();
	mountainShaderProgram->enableAttributeArray(2);
	mountainShaderProgram->setAttributeArray(2, GL_FLOAT, 0, 3, NULL);
	nvbo.release();

	//Draw a myTriangle with 3 indices starting from the 0th index
	glDrawArrays(GL_TRIANGLES, 0, triangleCount);
}
void Scenery::PaintMountainShadow(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QOpenGLShaderProgram *sp)
{
	//sp->bind();

	depthVao.bind();

	// Bind the buffer so that it is the current active buffer.
	vvbo.bind();
	// Enable Attribute 0
	sp->enableAttributeArray(0);
	// Set Attribute 0 to be position
	sp->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	//unbind buffer
	vvbo.release();

	//Draw a myTriangle with 3 indices starting from the 0th index
	glDrawArrays(GL_TRIANGLES, 0, triangleCount);

	sp->disableAttributeArray(0);

	depthVao.release();

	sp->release();
}
void Scenery::PaintWater(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QVector3D eyeDir)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	matrixFormat(ProjectionMatrix, P);
	matrixFormat(ModelViewMatrix, MV);

	//pass projection matrix to shader
	waterShaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	waterShaderProgram->setUniformValue("ModelViewMatrix", MV);

	waterShaderProgram->setUniformValue("waterHeight", (float)waterHeight);
	waterShaderProgram->setUniformValue("time", (float)(clock() - t));

	waterShaderProgram->setUniformValue("eyePos", eyeDir);

	waterShaderProgram->setUniformValue("numWaves", 4);
	//waterShaderProgram->setUniformValue("envMap", 0);
	for (int i = 0; i < 4; ++i) {
		GLfloat amplitude = 0.5f / (i + 1);
		waterShaderProgram->setUniformValue(format("amplitude[%d]", i).c_str(), amplitude);

		float wavelength = 8 * 3.14159 / (i + 1);
		waterShaderProgram->setUniformValue(format("wavelength[%d]", i).c_str(), wavelength);

		float speed = 1.0f + 2 * i;
		waterShaderProgram->setUniformValue(format("speed[%d]", i).c_str(), speed);

		float angle = uniformRandomInRange(-M_PI / 3, M_PI / 3);
		waterShaderProgram->setUniformValue(format("direction[%d]", i).c_str(), cos(angle), sin(angle));
	}

	//printf("%d\n", waterVbo.size());
	// Bind the buffer so that it is the current active buffer.
	waterVbo.bind();
	// Enable Attribute 0
	waterShaderProgram->enableAttributeArray(0);
	// Set Attribute 0 to be position
	waterShaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	//unbind buffer
	waterVbo.release();

	//Draw a myTriangle with 3 indices starting from the 0th index
	glDrawArrays(GL_TRIANGLES, 0, waterCount);
}
void Scenery::End(int mode)
{
	/*if (mode == 2) {
		depthShaderProgram->disableAttributeArray(0);
		depthVao.release();
		depthShaderProgram->release();
	}else */if (mode == 1) {
		waterShaderProgram->disableAttributeArray(0);
		waterVao.release();
		waterShaderProgram->release();
	}
	else {
		mountainShaderProgram->disableAttributeArray(0);
		mountainShaderProgram->disableAttributeArray(1);
		mountainVao.release();
		mountainShaderProgram->release();
	}
}
void Scenery::InitVAO()
{
	mountainVao.create();
	mountainVao.bind();

	waterVao.create();
	waterVao.bind();

	depthVao.create();
	depthVao.bind();
}
void Scenery::InitVBO()
{
	waterHeight = 10;
	heightMap hm(QString("../../Textures/heightmap.jpg"), 200, 30, waterHeight);
	hm.generateWater();

	triangleCount = hm.verticesVbo.size();
	waterCount = hm.water_verticesVbo.size();
	textureCount = hm.textureCordVbo.size();
	t = clock();

	// Create Buffer for position
	vvbo.create();
	// Bind the buffer so that it is the current active buffer.
	vvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	vvbo.allocate(hm.verticesVbo.constData(), hm.verticesVbo.size() * sizeof(QVector3D));

	tvbo.create();
	tvbo.bind();
	tvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	tvbo.allocate(hm.textureCordVbo.constData(), hm.textureCordVbo.size() * sizeof(QVector2D));

	nvbo.create();
	nvbo.bind();
	nvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	nvbo.allocate(hm.normalVbo.constData(), hm.normalVbo.size() * sizeof(QVector3D));

	//printf("%d\n", hm.water_verticesVbo.size());
	waterVbo.create();
	waterVbo.bind();
	waterVbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	waterVbo.allocate(hm.water_verticesVbo.constData(), hm.water_verticesVbo.size() * sizeof(QVector3D));
}
void Scenery::InitShader(QString vertexShaderPath, QString fragmentShaderPath, int mode)
{
	//mode 2: shadow, mode 1: water, mode 0: mountain
	printf("Loading shader \"%s\" & \"%s\"...\n", vertexShaderPath.toStdString().c_str(), fragmentShaderPath.toStdString().c_str());
	auto &shaderP = (mode == 2 ? depthShaderProgram : (mode == 1 ? waterShaderProgram : mountainShaderProgram));
	auto &vertexS = (mode == 2 ? depthVertexShader : (mode == 1 ? waterVertexShader : mountainVertexShader)),
		&fragmentS = (mode == 2 ? depthFragmentShader : (mode == 1 ? waterFragmentShader : mountainFragmentShader));

	// Create Shader
	shaderP = new QOpenGLShaderProgram();
	QFileInfo  vertexShaderFile(vertexShaderPath);
	if (vertexShaderFile.exists())
	{
		vertexS = new QOpenGLShader(QOpenGLShader::Vertex);
		if (vertexS->compileSourceFile(vertexShaderPath))
			shaderP->addShader(vertexS);
		else
			qWarning() << "Vertex Shader Error " << vertexS->log();
	}
	else
		qDebug() << vertexShaderFile.filePath() << " can't be found";

	QFileInfo  fragmentShaderFile(fragmentShaderPath);
	if (fragmentShaderFile.exists())
	{
		fragmentS = new QOpenGLShader(QOpenGLShader::Fragment);
		if (fragmentS->compileSourceFile(fragmentShaderPath))
			shaderP->addShader(fragmentS);
		else
			qWarning() << "fragment Shader Error " << fragmentS->log();
	}
	else
		qDebug() << fragmentShaderFile.filePath() << " can't be found";
	shaderP->link();
}
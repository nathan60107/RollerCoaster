#include "myTriangle.h"
#include "heightMap.h"
#define M_PI 3.14159
//#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
//#include <QHeightMapSurfaceDataProxy>

myTriangle::myTriangle()
{
}
void myTriangle::DimensionTransformation(GLfloat source[], GLfloat target[][4])
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
void myTriangle::Begin()
{
	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();
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

void myTriangle::Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QVector3D eyeDir)
{
	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);
	
	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ModelViewMatrix", MV);

	shaderProgram->setUniformValue("waterHeight", 50);
	shaderProgram->setUniformValue("time", t); t++;
	
	shaderProgram->setUniformValue("eyePos", eyeDir);

	shaderProgram->setUniformValue("numWaves", 4);
	//shaderProgram->setUniformValue("envMap", 0);
	for (int i = 0; i < 4; ++i) {
		GLfloat amplitude = 0.5f / (i + 1);
		shaderProgram->setUniformValue(format("amplitude[%d]", i).c_str(), amplitude);

		float wavelength = 8 * 3.14159 / (i + 1);
		shaderProgram->setUniformValue(format("wavelength[%d]", i).c_str(), wavelength);

		float speed = 1.0f + 2 * i;
		shaderProgram->setUniformValue(format("speed[%d]", i).c_str(), speed);

		float angle = uniformRandomInRange(-M_PI / 3, M_PI / 3);
		shaderProgram->setUniformValue(format("direction[%d]", i).c_str(), cos(angle), sin(angle));
	}
	
	// Bind the buffer so that it is the current active buffer.
	vvbo.bind();
	// Enable Attribute 0
	shaderProgram->enableAttributeArray(0);
	// Set Attribute 0 to be position
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	//unbind buffer
	vvbo.release();

	/*// Bind the buffer so that it is the current active buffer
	cvbo.bind();
	// Enable Attribute 1
	shaderProgram->enableAttributeArray(1);
	// Set Attribute 1 to be color
	shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 1, NULL);
	//unbind buffer
	cvbo.release();*/

	// Bind the buffer so that it is the current active buffer
	/*tvbo.bind();
	// Enable Attribute 2
	shaderProgram->enableAttributeArray(2);
	// Set Attribute 2 to be color
	shaderProgram->setAttributeArray(2, GL_FLOAT, 0, 2, NULL);
	//unbind buffer
	tvbo.release();*/

	//Draw a myTriangle with 3 indices starting from the 0th index
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}
void myTriangle::End()
{
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	//shaderProgram->disableAttributeArray(1);
	//shaderProgram->disableAttributeArray(2);

	vao.release();
	shaderProgram->release();
}
void myTriangle::Init()
{
	InitShader("../../Shader/Triangle.vs", "../../Shader/Triangle.fs");
	InitVAO();
	InitVBO();
}
void myTriangle::InitVAO()
{
	// Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}
void myTriangle::InitVBO()
{
	heightMap hm(QString("../../Textures/heightmap.jpg"), 200, 40);

	// Create Buffer for position
	vvbo.create();
	// Bind the buffer so that it is the current active buffer.
	vvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	vvbo.allocate(hm.verticesVbo.constData(), hm.verticesVbo.size() * sizeof(QVector3D));
}
void myTriangle::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
{
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
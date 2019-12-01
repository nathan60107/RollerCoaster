#include "Skybox.h"

Skybox::Skybox(int tIndex) :textureIndex(tIndex)
{
}	
void Skybox::DimensionTransformation(GLfloat source[], GLfloat target[][4])
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
void Skybox::Paint(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix, QVector3D eyePos)
{
	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();

	texture->bind();
	shaderProgram->setUniformValue("uTexture", textureIndex);

	GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, MV);
	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ModelViewMatrix", MV);

	shaderProgram->setUniformValue("eyePos", eyePos);

	// Bind the buffer so that it is the current active buffer
	vvbo.bind();
	// Enable Attribute 0
	shaderProgram->enableAttributeArray(0);
	// Set Attribute 0 to be position
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	//unbind buffer
	vvbo.release();

	//Draw triangles with 4 indices starting from the 0th index
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);

	vao.release();
	shaderProgram->release();
}
void Skybox::Init()
{
	InitShader("../../Shader/skybox.vs", "../../Shader/skybox.fs");
	InitVAO();
	InitVBO();
	InitTexture();
}
void Skybox::InitVAO()
{
	// Create Vertex Array Object
	vao.create();
	// Bind the VAO so it is the current active VAO
	vao.bind();
}
void Skybox::InitVBO()
{
	//Set each vertex's position
	float Size = 325;
	vertices << QVector3D(-Size, Size, -Size)
		<< QVector3D(-Size, -Size, -Size)
		<< QVector3D(+Size, -Size, -Size)
		<< QVector3D(+Size, -Size, -Size)
		<< QVector3D(+Size, +Size, -Size)
		<< QVector3D(-Size, +Size, -Size)

		<< QVector3D(-Size, -Size, +Size)
		<< QVector3D(-Size, -Size, -Size)
		<< QVector3D(-Size, +Size, -Size)
		<< QVector3D(-Size, +Size, -Size)
		<< QVector3D(-Size, +Size, +Size)
		<< QVector3D(-Size, -Size, +Size)

		<< QVector3D(+Size, -Size, -Size)
		<< QVector3D(+Size, -Size, +Size)
		<< QVector3D(+Size, +Size, +Size)
		<< QVector3D(+Size, +Size, +Size)
		<< QVector3D(+Size, +Size, -Size)
		<< QVector3D(+Size, -Size, -Size)

		<< QVector3D(-Size, -Size, +Size)
		<< QVector3D(-Size, +Size, +Size)
		<< QVector3D(+Size, +Size, +Size)
		<< QVector3D(+Size, +Size, +Size)
		<< QVector3D(+Size, -Size, +Size)
		<< QVector3D(-Size, -Size, +Size)

		<< QVector3D(-Size, +Size, -Size)
		<< QVector3D(+Size, +Size, -Size)
		<< QVector3D(+Size, +Size, +Size)
		<< QVector3D(+Size, +Size, +Size)
		<< QVector3D(-Size, +Size, +Size)
		<< QVector3D(-Size, +Size, -Size)

		<< QVector3D(-Size, -Size, -Size)
		<< QVector3D(-Size, -Size, +Size)
		<< QVector3D(+Size, -Size, -Size)
		<< QVector3D(+Size, -Size, -Size)
		<< QVector3D(-Size, -Size, +Size)
		<< QVector3D(+Size, -Size, +Size);
	// Create Buffer for position
	vvbo.create();
	// Bind the buffer so that it is the current active buffer
	vvbo.bind();
	// Since we will never change the data that we are about to pass the Buffer, we will say that the Usage Pattern is StaticDraw
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Allocate and initialize the information
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));
}
void Skybox::InitShader(QString vertexShaderPath, QString fragmentShaderPath)
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
void Skybox::InitTexture() {
	QImage posx = QImage("../../Textures/lostvalley_west.bmp").convertToFormat(QImage::Format_RGBA8888);;
	QImage posy = QImage("../../Textures/lostvalley_up.bmp").convertToFormat(QImage::Format_RGBA8888);;
	QImage posz = QImage("../../Textures/lostvalley_south.bmp").convertToFormat(QImage::Format_RGBA8888);;
	QImage negx = QImage("../../Textures/lostvalley_east.bmp").convertToFormat(QImage::Format_RGBA8888);;
	QImage negy = QImage("../../Textures/lostvalley_down.bmp").convertToFormat(QImage::Format_RGBA8888);;
	QImage negz = QImage("../../Textures/lostvalley_north.bmp").convertToFormat(QImage::Format_RGBA8888);;

	texture = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
	texture->create();
	texture->setSize(posx.width(), posx.height(), posx.depth());
	texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
	texture->allocateStorage();
	texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveX,
		QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
		(const void*)posx.constBits(), 0);
	texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveY,
		QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
		(const void*)posy.constBits(), 0);
	texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveZ,
		QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
		(const void*)posz.constBits(), 0);
	texture->setData(0, 0, QOpenGLTexture::CubeMapNegativeX,
		QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
		(const void*)negx.constBits(), 0);
	texture->setData(0, 0, QOpenGLTexture::CubeMapNegativeY,
		QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
		(const void*)negy.constBits(), 0);
	texture->setData(0, 0, QOpenGLTexture::CubeMapNegativeZ,
		QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
		(const void*)negz.constBits(), 0);
	texture->setWrapMode(QOpenGLTexture::ClampToEdge);
	texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	texture->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
}

#include "TrainView.h"  

TrainView::TrainView(QWidget *parent) :  
QGLWidget(parent)  
{  
	resetArcball();
	
}  
TrainView::~TrainView()  
{} 

void TrainView::initializeGL()
{
	initializeOpenGLFunctions();

	const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
	const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* shading = glGetString(GL_SHADING_LANGUAGE_VERSION);

	printf("=======================================\n");
	printf("=======================================\n");
	printf("Hardware Information:\n");
	printf("vendor         = %s\n", vendor);
	printf("GPU version    = %s\n", renderer);
	printf("OpenGL version = %s\n", version);
	printf("GLSL version   = %s\n", shading);
	printf("=======================================\n");
	printf("=======================================\n");

	//Create a triangle object
	triangle = new Triangle();
	//Initialize the triangle object
	triangle->Init();
	//Create a square object
	square = new Square();
	//Initialize the square object
	square->Init();

	scenery = new Scenery();
	scenery->InitShader("../../Shader/mountain with shadow.vs", "../../Shader/mountain with shadow.fs", 0);
	scenery->InitShader("../../Shader/testSq.vs", "../../Shader/testSq.fs", 1);
	scenery->InitVAO();
	scenery->InitVBO();

	skybox = new Skybox(2);
	skybox->Init();

	for (int i = 0; i < 3; i++){
		trainHead[i] = new TrainHead("../../Model/train/11709_train_v1_L3.obj", "../../Shader/train head.vs", "../../Shader/train head.fs");
	}
	//Initialize texture 
	initializeTexture();
}

void TrainView::initializeTexture()
{
	//Load and create a texture for square;'stexture
	QOpenGLTexture* texture = new QOpenGLTexture(QImage("../../Textures/Tupi.bmp"));
	QOpenGLTexture* heightMapTexture = new QOpenGLTexture(QImage("../../Textures/sand.png"));
	Textures.push_back(texture);
	Textures.push_back(heightMapTexture);
	//train = new Model("../../Model/train/11709_train_v1_L3.obj", 10, Point3d(0, 0, 0));
	
	Textures.push_back(skybox->texture);

	Textures.push_back(new QOpenGLTexture(QImage("../../Model/train/11709_train_wood_diff.jpg")));
	Textures.push_back(new QOpenGLTexture(QImage("../../Model/train/11709_train_wood_blue_diff.jpg")));
	Textures.push_back(new QOpenGLTexture(QImage("../../Model/train/11709_train_wood_black_diff.jpg")));

	for (int i = 0; i < 6; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		Textures[i]->bind();
	}

	initDepth();
}

void TrainView:: resetArcball()
	//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

void TrainView::paintGL()
{
	//drawing shadow frame buffer
	//main reference: "https://learnopengl-cn.readthedocs.io/zh/latest/05 Advanced Lighting/03 Shadows/01 Shadow Mapping/"
	//sub reference: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
	//https://blog.csdn.net/gamesdev/article/details/45969675
	//https://stackoverflow.com/questions/35134270/how-to-use-qopenglframebufferobject-for-shadow-mapping

	int depthWidth = width(), depthHeight = height();
	frameBuffer->bind();
	glViewport(0, 0, depthWidth, depthHeight);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	QMatrix4x4  depthProjectionMatrix, depthViewMatrix, depthModelMatrix, depthMVP;
	double aspect = depthWidth / depthHeight;

	//printf("%f %f %f\n", arcball.eyeX, arcball.eyeY, arcball.eyeZ);
	/*QVector3D lightInvDir(0, 250, 0); //gg += 0.01;
	float oo = 120;
	depthProjectionMatrix.setToIdentity();
	depthProjectionMatrix.ortho(-oo, oo, -oo, oo, 0.1, 1000);
	depthViewMatrix.setToIdentity();
	depthViewMatrix.lookAt(lightInvDir, QVector3D(0, 0, 0), QVector3D(1, 0, 0));
	depthModelMatrix.setToIdentity();*/

	QVector3D LID(0,0,0), lightPos(0, 50, 250);
	depthProjectionMatrix.setToIdentity();
	depthProjectionMatrix.perspective(45, aspect, 130, 370);
	depthViewMatrix.setToIdentity();
	depthViewMatrix.lookAt(lightPos, LID, QVector3D(0, 1, 0));
	depthModelMatrix.setToIdentity();
	
	depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
	//depthMVP = depthProjectionMatrix * depthViewMatrix;

	depthShaderProgram->bind();
		//depthShaderProgram->setUniformValue("depthMVP", depthMVP);
		depthShaderProgram->setUniformValue("ProjectionMatrix",depthProjectionMatrix);
		depthShaderProgram->setUniformValue("ModelViewMatrix", depthViewMatrix);
		//test->PaintMountainShadow(depthMVP.transposed(), depthShaderProgram);
		scenery->PaintMountainShadow(depthShaderProgram);
		for (int i = 0; i < trainNumber; i++) {
			trainHead[i]->drawTrainShadow(depthShaderProgram);
		}

	depthShaderProgram->release();

	auto FBOtext = frameBuffer->texture();

	if (save) {
		QImage FBOimg = frameBuffer->toImage();
		FBOimg.save("FBO.BMP");
		//save = 0;
	}
	frameBuffer->release();
	glActiveTexture(GL_TEXTURE0 + FBOtext);
	glBindTexture(GL_TEXTURE_2D, FBOtext);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{
	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// Set up the view port
	glViewport(0, 0, width(), height());
	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0, 0, 0.3f, 0);

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0 + skybox->textureIndex);
	skybox->Paint(ProjectionMatrex, ModelViewMatrex, QVector3D(arcball.eyeX, arcball.eyeY, arcball.eyeZ));
	glEnable(GL_DEPTH_TEST);

	// top view only needs one light
	if (this->camera == 1) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}
	else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[] = { 0,1,1,0 }; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[] = { 1, 0, 0, 0 };
	GLfloat lightPosition3[] = { 0, -1, 0, 0 };
	GLfloat yellowLight[] = { 0.5f, 0.5f, .1f, 1.0 };
	GLfloat whiteLight[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	GLfloat blueLight[] = { .1f,.1f,.3f,1.0 };
	GLfloat grayLight[] = { .3f, .3f, .3f, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);

	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	/*setupFloor();
	glDisable(GL_LIGHTING);
	drawFloor(200, 10);*/


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	if (this->camera != 1) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}

	//Get modelview matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	//Get projection matrix
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);

	//QMatrix4x4(ModelViewMatrex);

	

	/*//Call triangle's render function, pass ModelViewMatrex and ProjectionMatrex
	triangle->Paint(ProjectionMatrex, ModelViewMatrex);

	//we manage textures by Trainview class, so we modify square's render function
	square->Begin();
	//Active Texture
	glActiveTexture(GL_TEXTURE0);
	//Bind square's texture
	Textures[0]->bind();
	//pass texture to shader
	square->shaderProgram->setUniformValue("Texture", 0);
	//Call square's render function, pass ModelViewMatrex and ProjectionMatrex
	square->Paint(ProjectionMatrex, ModelViewMatrex);
	square->End();*/

	}
	QMatrix4x4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);
	//QMatrix4x4 depthBiasMVP = /*biasMatrix **/ depthMVP;
	
	scenery->Begin(false);
		//Active Texture
		//glActiveTexture(GL_TEXTURE1);
		//Bind square's texture
		//Textures[1]->bind();
		//pass texture to shader
		scenery->mountainShaderProgram->setUniformValue("Texture", 1);
		//test->mountainShaderProgram->setUniformValue("DepthBiasMVP", depthMVP);
		scenery->mountainShaderProgram->setUniformValue("depthP", depthProjectionMatrix);
		scenery->mountainShaderProgram->setUniformValue("depthMV", depthViewMatrix);
		scenery->mountainShaderProgram->setUniformValue("shadowMap", frameBuffer->texture());

		scenery->PaintMountain(ProjectionMatrex, ModelViewMatrex, QVector3D(-arcball.eyeX, -arcball.eyeY, -arcball.eyeZ));
	scenery->End(false);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	scenery->Begin(true);
		//Active Texture
		//glActiveTexture(GL_TEXTURE2);
		//Bind square's texture
		//Textures[2]->bind();
		//pass texture to shader
		scenery->waterShaderProgram->setUniformValue("envMap", 2);

		scenery->PaintWater(ProjectionMatrex, ModelViewMatrex, QVector3D(arcball.eyeX, arcball.eyeY, arcball.eyeZ));
	scenery->End(true);
	glDisable(GL_BLEND);
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(width()) / static_cast<float>(height());

	// Check whether we use the world camp
	if (this->camera == 0){
		arcball.setProjection(false);
		update();
	// Or we use the top cam
	}else if (this->camera == 1) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		} 
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90,1,0,0);
		update();
	}else if (this->camera == 2) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(arcball.fieldOfView, aspect, .1, 1000);
		gluLookAt(trainHead[0]->pos.x/10 + trainHead[0]->front.x * 6, trainHead[0]->pos.y/10 + trainHead[0]->front.y * 6 + 3, trainHead[0]->pos.z/10 + trainHead[0]->front.z * 6,
			trainHead[0]->pos.x/10 + trainHead[0]->front.x*7, trainHead[0]->pos.y/10 + trainHead[0]->front.y*7 + 3, trainHead[0]->pos.z/10 + trainHead[0]->front.z*7,
			trainHead[0]->orient.x, trainHead[0]->orient.y, trainHead[0]->orient.z);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		update();
	}
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else {
#ifdef EXAMPLE_SOLUTION
		trainCamView(this,aspect);
#endif
		update();
	}
}

float TrainView::calcNik(int i, int k, float u, vector<float> T)
{
	if (k == 0) {
		//printf("%d\n", i);
		return (T[i] <= u && u < T[i + 1])?1.0f:0.0f;
	}

	return (u - T[i]) / (T[i + k] - T[i]) * calcNik(i, k - 1, u, T) +
		(T[i + 1 + k] - u) / (T[i + 1 + k] - T[i + 1])*calcNik(i + 1, k - 1, u, T);
}

void TrainView::drawWood(Pnt3f rail, Pnt3f side, Pnt3f pos, bool doingShadows) {
	if (!doingShadows)glColor3ub(255 - 173, 255 - 179, 255 - 195);
	glBegin(GL_QUADS);
	Pnt3f high = (rail * side).normalize(), wood;
	rail.normalize(), side.normalize();

	for (int k = 1; k != 4; (k == 1) ? k = 0 : (k == 0) ? k = 2 : k++) {
		wood = pos;
		wood += side * (k % 2 ? 4 : -4);
		wood += rail * (k / 2 % 2 ? 1 : -1);
		wood += high * 0.1;
		glVertex3f(wood.x, wood.y, wood.z);
	}
	glEnd();
}

void TrainView::initDepth()
{
	//frameBuffer = new QOpenGLFramebufferObject(1024, 1024, QOpenGLFramebufferObject::Depth);
	frameBuffer = new QOpenGLFramebufferObject(width(), height(), QOpenGLFramebufferObject::Depth);
	frameBuffer->bind();
	//glDrawBuffer(GL_NONE);//開了會爆
	glReadBuffer(GL_NONE);
	frameBuffer->release();

	/*depthVao.create();
	depthVao.bind();*/

	QString vertexShaderPath("../../Shader/depth.vs"),
			fragmentShaderPath("../../Shader/depth.fs");

	printf("Loading shader \"%s\" & \"%s\"...\n", vertexShaderPath.toStdString().c_str(), fragmentShaderPath.toStdString().c_str());

	// Create Shader
	depthShaderProgram = new QOpenGLShaderProgram();
	QFileInfo  vertexShaderFile(vertexShaderPath);
	if (vertexShaderFile.exists())
	{
		depthVertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
		if (depthVertexShader->compileSourceFile(vertexShaderPath))
			depthShaderProgram->addShader(depthVertexShader);
		else
			qWarning() << "Vertex Shader Error " << depthVertexShader->log();
	}
	else
		qDebug() << vertexShaderFile.filePath() << " can't be found";

	QFileInfo  fragmentShaderFile(fragmentShaderPath);
	if (fragmentShaderFile.exists())
	{
		depthFragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
		if (depthFragmentShader->compileSourceFile(fragmentShaderPath))
			depthShaderProgram->addShader(depthFragmentShader);
		else
			qWarning() << "fragment Shader Error " << depthFragmentShader->log();
	}
	else
		qDebug() << fragmentShaderFile.filePath() << " can't be found";
	depthShaderProgram->link();
}

//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows)
{
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (this->camera != 2) {
		for(size_t i = 0; i < this->m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if ( ((int) i) != selectedCube)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			this->m_pTrack->points[i].draw();
		}
		update();
	}
	// draw the track
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################

	//draw rail
	int N = m_pTrack->points.size();
	int worldPos = 0;
	vector<Pnt3f> veco, vecp, pToDraw;

	glLineWidth(4);
	glEnable(GL_LINE_SMOOTH);
	glBegin(GL_LINES);

	//find knot position and orient.
	if (curve == 0) {//spline_Linear
		for (int i = 0; i < N; i++) {
			Pnt3f p1 = m_pTrack->points[i % N].pos, p2 = m_pTrack->points[(i + 1) % N].pos,
				o1 = m_pTrack->points[i % N].orient, o2 = m_pTrack->points[(i + 1) % N].orient;
			
			DIVIDE_LINE = (p2 - p1).len() / 14 * 4;
			percent = 1.0 / DIVIDE_LINE;

			for (int j = 0; j < DIVIDE_LINE; j++) {
				Pnt3f o = percent * (DIVIDE_LINE - j) * o1 + percent * j * o2,
					p = percent * (DIVIDE_LINE - j) * p1 + percent * j * p2;

				vecp.push_back(p);
				veco.push_back(o);
			}
		}
	}
	else if (curve == 1) {//spline_CardinalCubic
		float tau = 0.5, m[16];
		
		for (int i = 0; i < N; i++) {
			
			Pnt3f p1 = m_pTrack->points[i].pos, p2 = m_pTrack->points[(i + 1) % N].pos,
				p3 = m_pTrack->points[(i + 2) % N].pos, p4 = m_pTrack->points[(i + 3) % N].pos,
				o1 = m_pTrack->points[i].orient, o2 = m_pTrack->points[(i + 1) % N].orient,
				o3 = m_pTrack->points[(i + 2)].orient, o4 = m_pTrack->points[(i + 3) % N].orient;

			QMatrix4x4 matM(-tau, 2 - tau, tau - 2, tau,
							2 * tau, tau - 3, 3 - 2 * tau, -tau,
							-tau, 0, tau, 0,
							0, 1, 0, 0);
			QMatrix4x4 matGP(p1.x, p1.y, p1.z, 0,
							p2.x, p2.y, p2.z, 0,
							p3.x, p3.y, p3.z, 0,
							p4.x, p4.y, p4.z, 0);
			QMatrix4x4 matGO(o1.x, o1.y, o1.z, 0,
							o2.x, o2.y, o2.z, 0,
							o3.x, o3.y, o3.z, 0,
							o4.x, o4.y, o4.z, 0);

			DIVIDE_LINE = (p3 - p2).len() / 14 * 4;
			percent = 1.0 / DIVIDE_LINE;
			for (int j = 0; j < DIVIDE_LINE; j++) {
				float u = 1.0 * j / DIVIDE_LINE;
				QMatrix4x4 matT(u*u*u, u*u, u, 1,
								0, 0, 0, 0,
								0, 0, 0, 0,
								0, 0, 0, 0);
				auto matP = matT * matM * matGP,matO = matT * matM * matGO;

				vecp.push_back(Pnt3f(matP.data()[0], matP.data()[4], matP.data()[8]));
				veco.push_back(Pnt3f(matO.data()[0], matO.data()[4], matO.data()[8]));
			}
		}
		//Reference: https://blog.csdn.net/ZJU_fish1996/article/details/52735947
	}
	else if (curve == 2) {//spline_CubicB_Spline
		int K = 3;
		float totalLen = 0.0;
		vector<float> T(N + K + K + 1);

		for (int i = 0; i <= N + K + K; i++) {
			T[i] = i;
		}

		for (int j = 0; j < N + K - 1; j++) {
			totalLen += (m_pTrack->points[(j + 1) % N].pos - m_pTrack->points[j % N].pos).len();
		}

		DIVIDE_LINE = totalLen / 37.5;

		for (float i = DIVIDE_LINE * K; i < DIVIDE_LINE * (N+K); i+=0.5) {
			float u = 1.0 * i / DIVIDE_LINE;
			Pnt3f p(0, 0, 0), o(0,0,0);
			for (int j = 0; j < N + K; j++) {
				float Nik = calcNik(j, K, u, T);
				p += m_pTrack->points[(j-K+N)%N].pos * Nik;
				o += m_pTrack->points[(j-K+N)%N].orient * Nik;
			}
			if (vecp.empty() || (p - vecp[vecp.size() - 1]).len() >= 4) {
				vecp.push_back(p);
				veco.push_back(o);
			}
		}

		/*Reference: https://www.itread01.com/content/1548858635.html, 
		https://darkblack01.blogspot.com/2013/11/b-spline-curve.html,
		https://blog.csdn.net/tuqu/article/details/5177405 */
	}
	glEnd();
	
	int M = vecp.size();
	for (int i = 0; i < M; i++) {//using knot to calculate position of rail, sleeper and train.
		Pnt3f v1 = vecp[i], v2 = vecp[(i + 1) % M], v3 = vecp[(i + 2) % M],
			o1 = veco[i], o2 = veco[(i + 1) % M],
			side1 = ((v2 - v1) * o1).normalize() * 2.5,
			side2 = ((v2 - v1) * o2).normalize() * 2.5;

		pToDraw.push_back(v1 + side1);
		pToDraw.push_back(v2 + side2);
		pToDraw.push_back(v1 - side1);
		pToDraw.push_back(v2 - side2);
		drawWood(v2 - v1, side1, v1, doingShadows);

		for (int j = 0; j < trainNumber; j++) {
			if (worldPos <= t_time[j] && t_time[j] < worldPos + 10) {
				Pnt3f trainPos = (worldPos + 10 - t_time[j]) * v1 +
					(t_time[j] - worldPos) * v2,
					trainDir = (worldPos + 10 - t_time[j]) * (v2 - v1).normalize() +
					(t_time[j] - worldPos) * (v3 - v2).normalize(),
					ori = (side1 * (v2 - v1)).normalize(),//指向火車頭頂
					dir = trainDir.normalize(),
					dir_noY = trainDir,
					ori_noX = ori,
					ori_noZ = ori;
				
				dir_noY.y = 0;
				dir_noY = dir_noY.normalize();
				ori_noX.x = 0;
				ori_noX = ori_noX.normalize();
				ori_noZ.z = 0;
				ori_noZ = ori_noZ.normalize();

				float rX = acos(ori_noX.y) * (ori_noX.z > 0 ? 1 : -1),
					rY = acos(dir_noY.z) * (dir_noY.x > 0 ? +1 : -1) - PI / 2,
					rZ = acos(ori_noZ.y) * (ori_noZ.x > 0 ? -1 : 1);

				if (ori.y < 0.1) {//解決over the top的軌道問題
					rX += PI;
				}
			
				trainHead[j]->drawTrain(ProjectionMatrex, ModelViewMatrex, 
					QVector3D(trainPos.x, trainPos.y, trainPos.z), Point3d(trainPos), dir, ori,
					rX, rY, rZ);

				if (j == 0) {//對車頭取行進角度
					trainAngle = -dir.y;
				}
			}
		}
		
		worldPos += 10;
	}

	glLineWidth(3);
	glBegin(GL_LINES);
	if (!doingShadows) {
		glColor3ub(255 - 170, 255 - 168, 255 - 169);
	}
	for (auto a : pToDraw) {//draw rail
		glVertex3f(a.x, a.y, a.z);
	}
	glEnd();

	for (int i = 0; i < 3; i++) {
		if (trainAngle < -0.3)trainAngle = -0.3;
		t_time[i] += (isrun ? speed * (1 + trainAngle) : 0);
	}
	for (int i = 0; i < 3; i++) {
		if (t_time[i] > worldPos) t_time[i] -= worldPos;
	}
	


#ifdef EXAMPLE_SOLUTION
	drawTrack(this, doingShadows);
#endif

	// draw the train
	//####################################################################
	// TODO: 
	//	call your own train drawing code
	//####################################################################
#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
#endif
}

void TrainView::
	doPick(int mx, int my)
	//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	makeCurrent();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

	gluPickMatrix((double)mx, (double)(viewport[3]-my), 
		5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);


	// draw the cubes, loading the names as we go
	for(size_t i=0; i<m_pTrack->points.size(); ++i) {
		glLoadName((GLuint) (i+1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;
}

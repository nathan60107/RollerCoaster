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
	//Create a triangle object
	triangle = new Triangle();
	//Initialize the triangle object
	triangle->Init();
	//Create a square object
	square = new Square();
	//Initialize the square object
	square->Init();
	//Initialize texture 
	initializeTexture();
	
}
void TrainView::initializeTexture()
{
	//Load and create a texture for square;'stexture
	QOpenGLTexture* texture = new QOpenGLTexture(QImage("../../Textures/Tupi.bmp"));
	Textures.push_back(texture);
	train = new Model("../../Model/train/11709_train_v1_L3.obj", 10, Point3d(0, 0, 0));
	//train->changePos({ 0,0,0 });
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

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// Set up the view port
	glViewport(0,0,width(),height());
	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0,0,0.3f,0);
	
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

	// top view only needs one light
	if (this->camera == 1) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	} else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[]	= {0,1,1,0}; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[]	= {1, 0, 0, 0};
	GLfloat lightPosition3[]	= {0, -1, 0, 0};
	GLfloat yellowLight[]		= {0.5f, 0.5f, .1f, 1.0};
	GLfloat whiteLight[]		= {1.0f, 1.0f, 1.0f, 1.0};
	GLfloat blueLight[]			= {.1f,.1f,.3f,1.0};
	GLfloat grayLight[]			= {.3f, .3f, .3f, 1.0};

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
	setupFloor();
	glDisable(GL_LIGHTING);
	drawFloor(200,10);


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
 	glGetFloatv(GL_MODELVIEW_MATRIX,ModelViewMatrex);
	//Get projection matrix
 	glGetFloatv(GL_PROJECTION_MATRIX,ProjectionMatrex);

	//Call triangle's render function, pass ModelViewMatrex and ProjectionMatrex
 	triangle->Paint(ProjectionMatrex,ModelViewMatrex);
    
	//we manage textures by Trainview class, so we modify square's render function
	square->Begin();
		//Active Texture
		glActiveTexture(GL_TEXTURE0);
		//Bind square's texture
		Textures[0]->bind();
		//pass texture to shader
		square->shaderProgram->setUniformValue("Texture",0);
		//Call square's render function, pass ModelViewMatrex and ProjectionMatrex
		square->Paint(ProjectionMatrex,ModelViewMatrex);
	square->End();
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
		/*glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(train->pos.x, train->pos.y, train->pos.z,
			train->front.x, train->front.y, train->front.z,
			train->orient.x, train->orient.y, train->orient.z);
		update();*/

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//gluPerspective(120, 1, 1, 200);
		gluPerspective(arcball.fieldOfView, aspect, .1, 1000);
		gluLookAt(train->pos.x/10 + train->front.x * 6, train->pos.y/10 + train->front.y * 6 + 3, train->pos.z/10 + train->front.z * 6,
			train->pos.x/10 + train->front.x*7, train->pos.y/10 + train->front.y*7 + 3, train->pos.z/10 + train->front.z*7,
			train->orient.x, train->orient.y, train->orient.z);
		/*gluLookAt(train->pos.x, train->pos.y, train->pos.z,
			train->front.x, train->front.y, train->front.z,
			train->orient.x, train->orient.y, train->orient.z);*/
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
	Pnt3f high = (rail * side).normalize(), wood[4];
	rail.normalize(), side.normalize();

	for (int k = 1; k != 4; (k == 1) ? k = 0 : (k == 0) ? k = 2 : k++) {
		wood[k] = pos;
		wood[k] += side * (k % 2 ? 4 : -4);
		wood[k] += rail * (k / 2 % 2 ? 1 : -1);
		wood[k] += high * 0.1;
		glVertex3f(wood[k].x, wood[k].y, wood[k].z);
	}
	glEnd();
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
	t_time += (isrun?speed:0);
	//t_time += (isrun?arcSpeed:0);
	int worldPos = 0;
	glLineWidth(4);
	glEnable(GL_LINE_SMOOTH);
	glBegin(GL_LINES);

	vector<Pnt3f> pToDraw, pToDraw2;
	
	if (curve == 0) {//spline_Linear
		for (int i = 0; i < N; i++) {
			Pnt3f p1 = m_pTrack->points[i % N].pos, p2 = m_pTrack->points[(i + 1) % N].pos,
				o1 = m_pTrack->points[i % N].orient, o2 = m_pTrack->points[(i + 1) % N].orient;
			
			DIVIDE_LINE = (p2 - p1).len() / 14 * 4;
			percent = 1.0 / DIVIDE_LINE;

			for (int j = 0; j < DIVIDE_LINE; j++) {
				Pnt3f orient_t1 = percent * (DIVIDE_LINE - j) * o1 + percent * j * o2,
					orient_t2 = percent * (DIVIDE_LINE - j - 1 ) * o1 + percent * ( j + 1 ) * o2,
					point1 = percent * (DIVIDE_LINE - j) * p1 + percent * j * p2,
					point2 = percent * (DIVIDE_LINE - j - 1) * p1 + percent * ( j + 1 ) * p2;
				orient_t1 = (p2 - p1)*orient_t1;
				orient_t1.normalize();
				orient_t1 = orient_t1 * 2.5f;
				orient_t2 = (p2 - p1)*orient_t2;
				orient_t2.normalize();
				orient_t2 = orient_t2 * 2.5f;

				pToDraw.push_back(point1 + orient_t1);
				pToDraw.push_back(point2 + orient_t2);
				pToDraw.push_back(point1 - orient_t1);
				pToDraw.push_back(point2 - orient_t2);

				if(j%2)drawWood(p2 - p1, orient_t1, point1, doingShadows);
				if (worldPos <= t_time && t_time < worldPos + 10) {
					Pnt3f trainPos = (worldPos + 10 - t_time) * point1 +
						(t_time - worldPos) * point2;
					train->changePos(Point3d(trainPos), Point3d(point2-point1).normalize(), (orient_t1*(point2 - point1)).normalize());
					arcSpeed = speed / (point2 - point1).len()*5;
				}
				worldPos += 10;
			}
		}
		
	}
	else if (curve == 1) {//spline_CardinalCubic
		float Ax, Ay, Az, Bx, By, Bz, Cx, Cy, Cz, Dx, Dy, Dz,
			Ex, Ey, Ez, Fx, Fy, Fz, Gx, Gy, Gz, Hx, Hy, Hz, tau = 0.5, m[16];
		

		for (int i = 0; i < N; i++) {
			vector<Pnt3f> veco, vecp;
			Pnt3f p1 = m_pTrack->points[i].pos, p2 = m_pTrack->points[(i + 1) % N].pos,
				p3 = m_pTrack->points[(i + 2) % N].pos, p4 = m_pTrack->points[(i + 3) % N].pos,
				o1 = m_pTrack->points[i].orient, o2 = m_pTrack->points[(i + 1) % N].orient,
				o3 = m_pTrack->points[(i + 2)].orient, o4 = m_pTrack->points[(i + 3) % N].orient;
			m[0] = -tau, m[1] = 2 - tau, m[2] = tau - 2, m[3] = tau;
			m[4] = 2 * tau, m[5] = tau - 3, m[6] = 3 - 2 * tau, m[7] = -tau;
			m[8] = -tau, m[9] = 0, m[10] = tau, m[11] = 0;
			m[12] = 0, m[13] = 1, m[14] = 0, m[15] = 0;

			Ax = m[0] * p1.x + m[1] * p2.x + m[2] * p3.x + m[3] * p4.x;
			Bx = m[4] * p1.x + m[5] * p2.x + m[6] * p3.x + m[7] * p4.x;
			Cx = m[8] * p1.x + m[9] * p2.x + m[10] * p3.x + m[11] * p4.x;
			Dx = m[12] * p1.x + m[13] * p2.x + m[14] * p3.x + m[15] * p4.x;
			Ay = m[0] * p1.y + m[1] * p2.y + m[2] * p3.y + m[3] * p4.y;
			By = m[4] * p1.y + m[5] * p2.y + m[6] * p3.y + m[7] * p4.y;
			Cy = m[8] * p1.y + m[9] * p2.y + m[10] * p3.y + m[11] * p4.y;
			Dy = m[12] * p1.y + m[13] * p2.y + m[14] * p3.y + m[15] * p4.y;
			Az = m[0] * p1.z + m[1] * p2.z + m[2] * p3.z + m[3] * p4.z;
			Bz = m[4] * p1.z + m[5] * p2.z + m[6] * p3.z + m[7] * p4.z;
			Cz = m[8] * p1.z + m[9] * p2.z + m[10] * p3.z + m[11] * p4.z;
			Dz = m[12] * p1.z + m[13] * p2.z + m[14] * p3.z + m[15] * p4.z;

			Ex = m[0] * o1.x + m[1] * o2.x + m[2] * o3.x + m[3] * o4.x;
			Fx = m[4] * o1.x + m[5] * o2.x + m[6] * o3.x + m[7] * o4.x;
			Gx = m[8] * o1.x + m[9] * o2.x + m[10] * o3.x + m[11] * o4.x;
			Hx = m[12] * o1.x + m[13] * o2.x + m[14] * o3.x + m[15] * o4.x;
			Ey = m[0] * o1.y + m[1] * o2.y + m[2] * o3.y + m[3] * o4.y;
			Fy = m[4] * o1.y + m[5] * o2.y + m[6] * o3.y + m[7] * o4.y;
			Gy = m[8] * o1.y + m[9] * o2.y + m[10] * o3.y + m[11] * o4.y;
			Hy = m[12] * o1.y + m[13] * o2.y + m[14] * o3.y + m[15] * o4.y;
			Ez = m[0] * o1.z + m[1] * o2.z + m[2] * o3.z + m[3] * o4.z;
			Fz = m[4] * o1.z + m[5] * o2.z + m[6] * o3.z + m[7] * o4.z;
			Gz = m[8] * o1.z + m[9] * o2.z + m[10] * o3.z + m[11] * o4.z;
			Hz = m[12] * o1.z + m[13] * o2.z + m[14] * o3.z + m[15] * o4.z;

			DIVIDE_LINE = (p3 - p2).len() / 14 * 4;
			percent = 1.0 / DIVIDE_LINE;
			for (int j = 0; j <= DIVIDE_LINE; j++) {
				float u = 1.0 * j / DIVIDE_LINE;
				Pnt3f p = Pnt3f(Ax * u*u*u + Bx * u*u + Cx * u + Dx,
					Ay * u*u*u + By * u*u + Cy * u + Dy,
					Az * u*u*u + Bz * u*u + Cz * u + Dz),
					o = Pnt3f(Ex * u*u*u + Fx * u*u + Gx * u + Hx,
						Ey * u*u*u + Fy * u*u + Gy * u + Hy,
						Ez * u*u*u + Fz * u*u + Gz * u + Hz);
				o = (p3 - p2)*o;
				o.normalize();
				o = o * 2.5f;

				vecp.push_back(p);
				veco.push_back(o);
			}
			for (int j = 0; j < DIVIDE_LINE; j++) {
				pToDraw.push_back(vecp[j] + veco[j]);
				pToDraw.push_back(vecp[j+1] + veco[j+1]);
				pToDraw2.push_back(vecp[j] - veco[j]);
				pToDraw2.push_back(vecp[j+1] - veco[j+1]);
			

				drawWood(vecp[j+1] - vecp[j], veco[j], vecp[j], doingShadows);
				if (worldPos <= t_time && t_time < worldPos + 10) {
					Pnt3f trainPos = (worldPos + 10 - t_time) * vecp[j] +
						(t_time - worldPos) * vecp[j + 1],
						trainDir = (worldPos + 10 - t_time) * (vecp[j + 1] - vecp[j]).normalize() +
						(t_time - worldPos) * (vecp[(j + 2)% (DIVIDE_LINE+1)] - vecp[j + 1]).normalize();
					train->changePos(Point3d(trainPos), trainDir.normalize(), (veco[j]* (vecp[j + 1] - vecp[j])).normalize());
					arcSpeed = speed / (vecp[j + 1] - vecp[j]).len() * 5;
				}
				worldPos += 10;
			}
		}
		//Reference: https://blog.csdn.net/ZJU_fish1996/article/details/52735947
	}
	else if (curve == 2) {//spline_CubicB_Spline
		int K = 3;
		vector<Pnt3f> veco, vecp;

		vector<float> T(N + K + K + 1);
		for (int i = 0; i <= N + K + K; i++) {
			T[i] = i;
		}

		float totalLen = 0.0, realLen;
		for (int j = 0; j < N + K - 1; j++)totalLen += (m_pTrack->points[(j + 1) % N].pos - m_pTrack->points[j % N].pos).len();
		DIVIDE_LINE = totalLen / 37.5;
		
		vector<float> trackLen(N);
		for (int j = 0; j < N; j++) {
			trackLen[j] = (m_pTrack->points[(j + 1) % N].pos - m_pTrack->points[j % N].pos).len();
			realLen += trackLen[j];
		}

		for (float i = DIVIDE_LINE * K; i <= DIVIDE_LINE * (N+K); i+=0.5) {
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

		/*for(float u = K; u<=(N+K); u += realLen / trackLen[((int)u - 1 + N) % N] / sqrt(trackLen[((int)u - 1 + N) % N]) / 5.0){{
			
		//for (int q = K; q < (N + K); q++) {
			//for (float u = q; u < q + 1; u += realLen / trackLen[(q - 1 + N) % N] / sqrt(trackLen[(q - 1 + N) % N]) / 5.0) {
				//printf("%f\n", u);
				//float u = 1.0 * i / DIVIDE_LINE;
				//int cur = i / DIVIDE_LINE - K;
				Pnt3f p(0, 0, 0), o(0, 0, 0);
				for (int j = 0; j < N + K; j++) {
					float Nik = calcNik(j, K, u, T);
					p += m_pTrack->points[(j - K + N) % N].pos * Nik;
					o += m_pTrack->points[(j - K + N) % N].orient * Nik;
				}
				//if (vecp.empty() || (p - vecp[vecp.size() - 1]).len() >= 4) {
					vecp.push_back(p);
					veco.push_back(o);
				//}
			}
		}*/

		for (int i = 0; i < vecp.size()-1; i++) {
			Pnt3f orient1 = veco[i], orient2 = veco[i+1];
			orient1 = (vecp[i + 1] - vecp[i])*orient1;
			orient1.normalize();
			orient1 = orient1 * 2.5f;
			orient2 = (vecp[i + 1] - vecp[i])*orient2;
			orient2.normalize();
			orient2 = orient2 * 2.5f;

			pToDraw.push_back(vecp[i] + orient1);
			pToDraw.push_back(vecp[i + 1] + orient2);
			pToDraw.push_back(vecp[i] - orient1);
			pToDraw.push_back(vecp[i + 1] - orient2);

			drawWood(vecp[i+1]-vecp[i], orient1, vecp[i], doingShadows);
			if (worldPos <= t_time && t_time < worldPos + 10) {
				Pnt3f trainPos = (worldPos + 10 - t_time) * vecp[i] +
					(t_time - worldPos) * vecp[i+1];
				//(veco[i] * (vecp[i + 1] - vecp[i])).normalize().printself();
				train->changePos(Point3d(trainPos), Point3d(vecp[i+1] - vecp[i]).normalize(), (orient1* (vecp[i + 1] - vecp[i])).normalize());
				arcSpeed = speed / (vecp[i + 1] - vecp[i]).len() * 5;
			}
			worldPos += 10;
		}

		/*Reference: https://www.itread01.com/content/1548858635.html, 
		https://darkblack01.blogspot.com/2013/11/b-spline-curve.html,
		https://blog.csdn.net/tuqu/article/details/5177405 */
	}

	glLineWidth(3);
	glBegin(GL_LINES);
	if (!doingShadows) {
		glColor3ub(255 - 170, 255 - 168, 255 - 169);
	}
	for (auto a : pToDraw) {
		glVertex3f(a.x, a.y, a.z);
	}
	for (auto a : pToDraw2) {
		glVertex3f(a.x, a.y, a.z);
	}
	glEnd();

	//draw train?
	train->render(0,0);
	if (t_time > worldPos) t_time -= worldPos;
	


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

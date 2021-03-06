#ifndef TRAINVIEW_H  
#define TRAINVIEW_H  
#include <QGLWidget> 
#include <QtGui>  
#include <QtOpenGL>  
#include <GL/GLU.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib") 
#include "Utilities/ArcBallCam.H"
#include "Utilities/3DUtils.H"
#include "Track.H"
#include "Triangle.h"
#include "Square.h"
#include "Scenery.h"
#include "Skybox.h"
#include "TrainHead.h"
#include "Tunnel.h"

class AppMain;
class CTrack;

//#######################################################################
// TODO
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//#######################################################################


class TrainView : public QGLWidget, protected QOpenGLFunctions_4_3_Core
{  
	Q_OBJECT  
public:  
	explicit TrainView(QWidget *parent = 0);  
	~TrainView();  

public:
	// overrides of important window things
	//virtual int handle(int);
	virtual void paintGL();

	// all of the actual drawing happens in this routine
	// it has to be encapsulated, since we draw differently if
	// we're drawing shadows (no colors, for example)
	void drawStuff(bool doingShadows=false);

	// setup the projection - assuming that the projection stack has been
	// cleared for you
	void setProjection();

	// Reset the Arc ball control
	void resetArcball();

	// pick a point (for when the mouse goes down)
	void doPick(int mx, int my);

	void initializeGL();
	void initializeTexture();

	float calcNik(int i, int k, float u, vector<float> T);
	void drawTrain(float);
	void drawWood(Pnt3f rail, Pnt3f side, Pnt3f pos, bool doingShadows);
	void initDepth();

public:
	ArcBallCam		arcball;			// keep an ArcBall for the UI
	int				selectedCube;  // simple - just remember which cube is selected

	CTrack*			m_pTrack;		// The track of the entire scene

	int camera;
	int curve;
	int track;
	bool isrun;
	Triangle* triangle;
	Square* square;
	Scenery *scenery;
	TrainHead *trainHead[3];
	int trainNumber = 3;
	Tunnel *tunnel;

	GLfloat ProjectionMatrex[16];
	GLfloat ModelViewMatrex[16];
	QVector<QOpenGLTexture*> Textures;
	float t_time[3] = { 50,25,0 };

	Skybox *skybox;

	//shadow
	QOpenGLFramebufferObject *frameBuffer;
	QOpenGLShaderProgram* depthShaderProgram;
	QOpenGLShader* depthVertexShader;
	QOpenGLShader* depthFragmentShader;
	QOpenGLVertexArrayObject depthVao;
	bool save = true;

	unsigned int DIVIDE_LINE = 20;
	float percent;
	float speed = 0.5;
	float trainAngle = 0;
	bool tunnelOn = 0;

};  
#endif // TRAINVIEW_H  
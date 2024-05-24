/*
 *  clip.cpp
 *  This program clips the line with the given window.you can define your own rectangle and lines.
 * 
 *  press:
 *  key"1" to (re)draw a rectangle and then draw the line
 *  key"2" to the lines be wiped and the rectangle remains
 */

#include <iostream>
#include <windows.h>
#include <GL/glut.h>
#include <math.h>

/* Gloable Variety*/
int width = 500, height = 500;	//preset window size 
int n;	// count the number of lines that has been drawed
int **line;	// original lines
int **clip; //clip lines
int rect[4]; // the edge of the rectangle

bool lineStart; //state of line has defined the start point but with undifined end point
bool lineEnd; //state of that the current line draw process has complete

bool RectExist; //state of undefined viewport(rectangle/box)


// allocate or reallocate memory for lines and clip lines
void ReallocateLine() {
	line = new int* [50]; 
	for (int i = 0; i < 50; ++i) {
		line[i] = new int[4]; // x0, y0 for the start point and x1, y1 for the end point respectively
	}

	clip = new int* [50]; 
	for (int i = 0; i < 50; ++i) {
		clip[i] = new int[4]; //same as above
	}
}

//init function
void myinit(void)
{
	glShadeModel(GL_FLAT);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	//set the initial state 
	lineStart = false;
	lineEnd = false;
	RectExist = false;
	n = 0;
	ReallocateLine();
}

//keyboard function
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case'1':RectExist = false; n = 0; ReallocateLine();  break;
	case'2':n = 0; ReallocateLine(); ; break;
	case 27:exit(0);
	default:return;
	}
	glutPostRedisplay();
}

//mouse click function
void mymouse(int button, int state, int x, int y)
{
	//if the left button is press down
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		//if HAVE NOT define a rectangle, define the left&bottom point(can be seen as left and bottom bondary)
		if (!RectExist) {
			rect[0] = x;
			rect[1] = height - y;
		}
		//if the rectangle exists, define the start point of original line
		else {
			n++;
			line[n][0] = x;
			line[n][1] = height - y;
			lineEnd = false;
			lineStart = true;	//set the state
		}
	}
	//if the left button is lossen up
	if (state == GLUT_UP && button == GLUT_LEFT_BUTTON)
	{
		//if HAVE NOT define a rectangle, define the right&top point(can be seen as right and top bondary)
		if (!RectExist) {
			rect[2] = x;
			rect[3] = height - y;
			RectExist = true;	//change the state
		}
		//if the rectangle exists, define the end point of original line
		else {
			line[n][2] = x;
			line[n][3] = height - y;
			lineStart = false;
			lineEnd = true;		//change the state

			//print info of this line
			std::cout << "original: x0: " << line[n][0] << " y0: " << line[n][1] << " x1: " << line[n][2] << " y1: " << line[n][3] << std::endl;
		}
	}
	glutPostRedisplay();
}

//set mouse-move interaction 
void mymotion(int x, int y)
{
	if (!RectExist) {
		rect[2] = x;
		rect[3] = height - y;
	}
	else {
		line[n][2] = x;
		line[n][3] = height - y;
	}
	glutPostRedisplay();
}

//reshape function
void myReshape(int w, int h)
{
	 glViewport(0,0,w,h); 
	 glMatrixMode(GL_PROJECTION); 
	 glLoadIdentity(); 
	 gluOrtho2D(0.0,w,0.0,h); 
	 glMatrixMode(GL_MODELVIEW);
	 width=w; 
	 height=h; 
}


//-------------------------------
// your task!!!
//-------------------------------

bool myclip(int* rect, int* line, int index)
// line clipping algorithm. if the line is out of the rectangle return false, otherwise return true.
{
	//the viewport edge
	float edgeLeft = rect[0] > rect[2] ? rect[2] : rect[0];
	float edgeBottom = rect[1] > rect[3] ? rect[3] : rect[1];
	float edgeRight = rect[0] > rect[2] ? rect[0] : rect[2];
	float edgeTop = rect[1] > rect[3] ? rect[1] : rect[3];

	//the start and end point of the original line 
	float x0 = line[0];
	float y0 = line[1];
	float x1 = line[2];
	float y1 = line[3];

	float t0 = 0.0f, t1 = 1.0f;
	float deltaX = x1 - x0;
	float deltaY = y1 - y0;
	float p, q, r;

	for (int edge = 0; edge < 4; edge++)
	{
		if (edge == 0) {	p = -deltaX;	q = -(edgeLeft - x0); } //left clip
		else if (edge == 1) {	p = deltaX;	q = (edgeRight - x0); }//right clip
		else if (edge == 2) {	p = -deltaY;	q = -(edgeBottom - y0); }//bottom clip
		else if (edge == 3) {	p = deltaY;	q = (edgeTop - y0); }//top clip
		r = q / p;
		if (p == 0 && q < 0) {
			return false;
		}
		else if (p < 0) {
			if (r > t1) {
				return false;
			}
			else if (r > t0)	
				t0 = r; //replace t0 with r
		}
		else if (p > 0) {
			if (r < t0) {
				return false;
			}
			else if (r < t1)
				t1 = r;//replace t1 with r
		}
	}

	clip[index][0] = x0 + t0 * deltaX;	 //new x0
	clip[index][1] = y0 + t0 * deltaY;	//new y0
	clip[index][2] = x0 + t1 * deltaX;	//new x1
	clip[index][3] = y0 + t1 * deltaY;	//new y1
	return true;
}

void drawlines()
{
	// ------------------------------------
	//	please define your own line segment and draw 
	//	it here with different color and line width
	// ------------------------------------
	if (lineStart) {
		glColor4f(1.0, 1.0, 1.0, 0.75);
		glLineWidth(2);
		glBegin(GL_LINES);
		glVertex2f(line[n][0], line[n][1]);
		glVertex2f(line[n][2], line[n][3]);
		glEnd();
	}

	//-------------------------------
	//do the clipping in myclip() funtion 
	//-------------------------------
	if (lineEnd) {
		bool isClip = myclip(rect, line[n], n);
		if (!isClip) clip[n][0] = -1;
		else {
			std::cout << "clip: x0: " << clip[n][0] << " y0: " << clip[n][1] << " x1: " << clip[n][2] << " y1: " << clip[n][3] << std::endl;
		}
	}

	// ------------------------------------
	//	please draw clipped line here with another 
	//  color and line width
	// ------------------------------------   
	for (int k = 1; k <= n; k++)
	{
		if (clip[k][0] != -1) {
			glColor4f(1.0, 0.0, 0.0, 1);
			glLineWidth(4);
			glBegin(GL_LINES);
			glVertex2f(clip[k][0], clip[k][1]);
			glVertex2f(clip[k][2], clip[k][3]);
			glEnd();
		}
	}
}

void drawbox()
{
	glColor4f(0.0, 1.0, 1.0, 0.75);
	glBegin(GL_POLYGON);
	glVertex2f(rect[0], rect[1]);
	glVertex2f(rect[2], rect[1]);
	glVertex2f(rect[2], rect[3]);
	glVertex2f(rect[0], rect[3]);
	glEnd();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	std::string message1;
	std::string message2;

	drawbox();
	if (!RectExist) {
		message1 = "click and drag to define a box";
	}
	else{
		message1 = "click and drag to define a line.";
		message2 = "press 1 on keyboard to redraw a box, press 2 on keyboard to delete all the lines";
		drawlines();
	}

	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(40, 100);
	for (char c : message1) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c); 
	}
	glRasterPos2f(40, 85);
	for (char c : message2) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
	}
	glutSwapBuffers();
}


/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	//define size and the relative positon of the applicaiton window on the display
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	//init the defined window with "argv[1]" as topic showed on the top the window
	glutCreateWindow(argv[0]);
	// opengl setup
	myinit();

	//define callbacks
	glutDisplayFunc(display);
	glutMouseFunc(mymouse);
	glutMotionFunc(mymotion);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(myReshape);

	//enter the loop for display
	glutMainLoop();

	return 1;
}

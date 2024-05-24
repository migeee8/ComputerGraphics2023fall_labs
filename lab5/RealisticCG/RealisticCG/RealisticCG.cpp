
/* Copyright (c) Mark J. Kilgard, 1994. */


#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <string>

#include <GL/glut.h>
#include <GL/glu.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TORUS 0
#define NOTHING 1
#define DOD 2
#define TET 3
#define ISO 4
#define CHANGE_LIGHT 5
#define CHANGE_MATERIAL 6
#define ADD_TEXTURE 7
#define DELETE_TEXTURE 8
#define TEAPOT 9
#define DEPTTEST 10
#define QUIT 11
#define QUADS 12


/*光源*/
static int spinVertical = 0;
static int spinHorizontal = 0;
static int beginX;
static int beginY;
GLfloat position[] ={ 0.0, 0.0, 1.5, 1.0 };

GLfloat light_diffuse_orange[] = { 1.0, 0.5, 0.0, 1.0 };
GLfloat light_diffuse_blue[] = { 0.0, 0.5, 1.0, 1.0 };
GLfloat light_diffuse_white[] = { 1.0, 1.0, 1.0, 1.0 };


/*物体*/
static int obj = TORUS;
int rotationV = 0; 
int rotationH = 0; 
static int objRotateBeginX;
static int objRotateBeginY;
static float translateX;
static float translateY;
static float scaleValue = 1.0f;
float minScale = 0.5f; // min scale
float maxScale = 2.0f; // max scale

GLfloat obj_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat obj_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
GLfloat obj_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat obj_shininess = 50.0;

unsigned char* imageData;
unsigned int texture;
bool EnableTex = false;


/*state*/
int mouseBt;

// 菜单函数
void menu_select(int item)
{
	if (item == QUIT)
		exit(0);
	else if (item == CHANGE_LIGHT) {
		GLfloat light_diffuse[] = { 1.0, 0.5, 0.0, 1.0 };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	}
	else if (item >= 100 && item < 103) {
		switch (item) {
		case 100: // Red light
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_orange);
			break;
		case 101: // Blue light
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_blue);
			break;
		case 102: // White light
			glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_white);
			break;
		}
	}
	else if (item == CHANGE_MATERIAL) {
		srand((unsigned int)time(NULL)); 

		// random ambient、diffuse、specular
		obj_ambient[0] = (GLfloat)rand() / RAND_MAX; 
		obj_ambient[1] = (GLfloat)rand() / RAND_MAX;
		obj_ambient[2] = (GLfloat)rand() / RAND_MAX;
		obj_ambient[3] = 1.0; 

		obj_diffuse[0] = (GLfloat)rand() / RAND_MAX;
		obj_diffuse[1] = (GLfloat)rand() / RAND_MAX;
		obj_diffuse[2] = (GLfloat)rand() / RAND_MAX;
		obj_diffuse[3] = 1.0;

		obj_specular[0] = (GLfloat)rand() / RAND_MAX;
		obj_specular[1] = (GLfloat)rand() / RAND_MAX;
		obj_specular[2] = (GLfloat)rand() / RAND_MAX;
		obj_specular[3] = 1.0;

		obj_shininess = (GLfloat)(rand() % 128); 

	}
	else if (item == ADD_TEXTURE) {
		EnableTex = true;
	}
	else if (item == DELETE_TEXTURE) {
		EnableTex = false;
	}
	else {
		obj = item;
		glutPostRedisplay();
	}
}

/* ARGSUSED2 */
void movelight(int button, int state, int x, int y)
{
	// 光源
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		beginX = x;
		beginY = y;
		mouseBt = 0;
	}
	// 几何物体
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		objRotateBeginX = x;
		objRotateBeginY = y;
		mouseBt = 1;
	}
}

/* ARGSUSED1 */
void motion(int x, int y)
{
	// 光源的旋转
	if (mouseBt == 0) {
		spinHorizontal = (spinHorizontal + (x - beginX)) % 360;
		spinVertical = (spinVertical + (y - beginY)) % 360;
		beginX = x;
		beginY = y;
	}
	// 几何物体的旋转
	if (mouseBt == 1){
		rotationV += ((y - objRotateBeginY) % 360);
		objRotateBeginY = y;
		rotationH += ((x - objRotateBeginX) % 360);
		objRotateBeginX = x;
	}
	
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case'w':
		translateY += 0.1f;
		break;
	case's':
		translateY -= 0.1f;
		break;
	case'a':
		translateX -= 0.1f;
		break;
	case'd':
		translateX += 0.1f;
		break;
	case '-':
		if(scaleValue>minScale)
			scaleValue -= 0.1f; // 按下 '-' 键，缩小物体
		break;
	case '+':
		if(scaleValue<maxScale)
			scaleValue += 0.1f; // 按下 'shift 和 +' 键，放大物体
		break;
	}
	glutPostRedisplay();
}

void myinit(void)
{
	// 加载纹理
	glGenTextures(1, &texture);
	int width, height, channels;
	imageData = stbi_load("tex.png", &width, &height, &channels, STBI_rgb);
	if (imageData == NULL) {
		fprintf(stderr, "Error loading texture.\n");
		return;
	}
	glBindTexture(GL_TEXTURE_2D, texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 启用光照
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// 打开深度测试
	glDepthFunc(GL_LESS);
	// glDepthFunc(GL_GREATER);
	// glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
}

/*  Here is where the light position is reset after the modeling
 *  transformation (glRotated) is called.  This places the
 *  light at a new position in world coordinates.  The cube
 *  represents the position of the light.
 */
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
		glTranslatef(0.0, 0.0, -5.0);

		// 设置几何物体材质
		glMaterialfv(GL_FRONT, GL_AMBIENT, obj_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, obj_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, obj_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, &obj_shininess);

		glPushMatrix();
			// 绘制光源以及光源的移动
			glRotated((GLdouble)spinVertical, 1.0, 0.0, 0.0);
			glRotated((GLdouble)spinHorizontal, 0.0,1.0, 0.0);
			glLightfv(GL_LIGHT0, GL_POSITION, position);

			glTranslated(0.0, 0.0, 1.5);
			glDisable(GL_LIGHTING);
			glColor3f(0.0, 1.0, 1.0);
			glutWireCube(0.1);
			glEnable(GL_LIGHTING);
		glPopMatrix();

		// 几何物体的平移旋转缩放 
		glScalef(scaleValue, scaleValue, scaleValue);
		glTranslatef(translateX, translateY, 0.0);
		glRotatef((GLdouble)rotationV, 1.0f, 0.0f, 0.0f);
		glRotatef((GLdouble)rotationH, 0.0f, 1.0f, 0.0f);
	
		// 打开纹理贴图
		if(EnableTex)
			glEnable(GL_TEXTURE_2D);
		
		// 绘制几何物体
		switch (obj) {
		case TORUS:
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glutSolidTorus(0.275, 0.85, 20, 20);
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			break;
		case NOTHING:
			break;
		case DOD:
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glPushMatrix();
			glScalef(.5, .5, .5);
			glutSolidDodecahedron();
			glPopMatrix();
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			break;
		case TET:
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glutSolidTetrahedron();
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			break;
		case ISO:
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glutSolidIcosahedron();
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			break;
		case QUADS:
			// 设置纹理坐标并绘制物体
			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 0.0);
			glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 0.0);
			glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
			glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 0.0);
			glEnd();
			break;
		case TEAPOT:	
			glutSolidTeapot(1.0);
			break;
		case DEPTTEST:
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			// 第一个对象Cube
			glColor3f(1.0f, 0.0f, 0.0f);
			glutSolidCube(1.0);

			// 深度缓冲区清除，深度缓冲区被重置
			//glClear(GL_DEPTH_BUFFER_BIT);

			// 第二个对象sphere
			glPushMatrix();
				glTranslatef(0.0f, 0.0f, -2.0f);
				glColor3f(0.0f, 1.0f, 0.0f);
				glutSolidSphere(1.0, 20, 20);
			glPopMatrix();

			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
		}
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glutSwapBuffers();
}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
}


/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow(argv[0]);
	myinit();
	glutMouseFunc(movelight);
	glutMotionFunc(motion);
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	// 创建光颜色子菜单
	int lightColorMenu = glutCreateMenu(menu_select);
	glutAddMenuEntry("Orange Light", 100);
	glutAddMenuEntry("Blue Light", 101);
	glutAddMenuEntry("White Light", 102);

	// 创建主菜单
	glutCreateMenu(menu_select);
	glutAddMenuEntry("Apply Random Material", CHANGE_MATERIAL);
	glutAddMenuEntry("Add Texture", ADD_TEXTURE);
	glutAddMenuEntry("Remove Texture", DELETE_TEXTURE);
	glutAddSubMenu("Change Light Color", lightColorMenu);
	glutAddMenuEntry(" ", NOTHING);
	glutAddMenuEntry("Torus", TORUS);
	glutAddMenuEntry("Tetrahedron", TET);
	glutAddMenuEntry("Dodecahedron", DOD);
	glutAddMenuEntry("Icosahedron", ISO);
	glutAddMenuEntry("Teapot", TEAPOT);
	glutAddMenuEntry("Quad", QUADS);
	glutAddMenuEntry("DeptTest", DEPTTEST);
	glutAddMenuEntry("Quit", QUIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();
	return 0;             /* ANSI C requires main to return int. */
}

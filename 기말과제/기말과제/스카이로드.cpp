/*
��¥ : 2017/05/17 ~ 06/12
�ۼ��� : ��ȿ��
���ϸ� : ��ī�̷ε�.cpp
���� : ���ּ� ����
*/

#include <glut.h>
#include <glu.h>
#include <gl.h>
#include <iostream>
#include <Windows.h>
#include <glaux.h>
#include <glext.h>
#include <cmath>
#include <mmsystem.h>
#include "fmod.hpp"

#pragma comment (lib, "fmodex_vc.lib")
#pragma comment (lib, "legacy_stdio_definitions.lib")
using namespace std;
using namespace FMOD;

System* pSystem; //����� ������ ��������
Sound* pSound[2];
Channel* pChannel[2];
bool IsBGPlaying = false;
float fVolume = 0.6;

int MenuChk;

GLint width = 800, height = 600;
GLfloat look1 = 0.0; //���ּ��� �Բ� �ü��� �̵��ϵ��� ���ִ� ��������
unsigned int speed = 5; //���ּ��� �ӵ��� �����ϴ� ��������
float g_lightPosition[] = { 0.0f, 4.0f, 16.0f }; //���� �� ���ּ��� ��ǥ X, Y, Z
bool jump = false, over = false, go = false; //���� ��������, ���ӿ��� ����, ����Ⱑ ������ ���� �������� �����̴�.

GLfloat g_light[] = { 0.0, 0.0, 1.5, 1.0 };
GLfloat light0_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
GLfloat light0_diffuse[] = { 0.8, 0.7, 0.6, 1.0 };
GLfloat light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };

GLuint tex[6]; //������ �ؽ�ó �̹����� ����

AUX_RGBImageRec *LoadBMP(char *Filename) {  // Bitmap �̹����� ȣ���Ѵ�.
	FILE *File = NULL;

	if (!Filename) return NULL;
	File = fopen(Filename, "r");
	if (File) {
		fclose(File);
		return auxDIBImageLoad(Filename);
	}

	return NULL;
}

void LoadGLTextures() {  // Bitmap �̹��� 6���� ȣ���Ͽ� Texture �̹����� ��ȯ�Ѵ�.
	AUX_RGBImageRec *texRec[6];
	memset(texRec, 0, sizeof(void *) * 6);

	if ((texRec[0] = LoadBMP("cosmo.bmp")) &&
		(texRec[1] = LoadBMP("cosmo.bmp")) &&
		(texRec[2] = LoadBMP("cosmo.bmp")) &&
		(texRec[3] = LoadBMP("cosmo.bmp")) &&
		(texRec[4] = LoadBMP("cosmo.bmp")) &&
		(texRec[5] = LoadBMP("cosmo.bmp"))) {

		for (int i = 0; i<6; i++) {
			glGenTextures(1, &tex[i]);
			glBindTexture(GL_TEXTURE_2D, tex[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, texRec[i]->sizeX, texRec[i]->sizeY, 0,
				GL_RGB, GL_UNSIGNED_BYTE, texRec[i]->data);
		}
	}

	for (int i = 0; i<6; i++) {
		if (texRec[i]) {
			if (texRec[i]->data) free(texRec[i]->data);
			free(texRec[i]);
		}
	}

	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void MyInit(void) {  //��ü����� �������� 0 ����
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
}

void PolarView(GLfloat radius, GLfloat elevation, GLfloat azimuth, GLfloat twist) {
	glTranslatef(0.0, 0.0, -radius); //�� ��ǥ�� ����
	glRotatef(-elevation, 1.0, 0.0, 0.0);
	glRotatef(azimuth, 0.0, 0.0, 1.0);
	glRotatef(-twist, 0.0, 0.0, 1.0);
}

void board() { //���ּ� ���
	glFrontFace(GL_CW);
	GLUquadricObj *obj;
	obj = gluNewQuadric();
	gluQuadricDrawStyle(obj, GLU_FILL);
	glPushMatrix();
		glDisable(GL_LIGHTING);
		glColor3f(0.0, 0.5, 0.0);
		glRotatef(180.0, 0.0, 0.0, 0.0);
		glutSolidTeapot(0.4);
		glEnable(GL_LIGHTING);

		glDisable(GL_LIGHTING);
		glRotatef(180.0, 0.0, 180.0, 0.0);
		glutSolidTeapot(0.4);
		glEnable(GL_LIGHTING);

		glDisable(GL_LIGHTING);
		glColor3f(0.0, 0.0, 0.7);
		glTranslatef(0.0, 0.0, -0.2);
		gluCylinder(obj, 0.38, 0.38, 0.6, 12, 10);
		glEnable(GL_LIGHTING);

		glDisable(GL_LIGHTING);
		glColor3f(0.5f, 0.5f, 0.5f);
		glutWireCube(1.0);
		glEnable(GL_LIGHTING);
		
		glDisable(GL_LIGHTING);
		glTranslatef(0.0, 0.0, -0.7);
		gluCylinder(obj, 0.0, 0.38, 0.7, 12, 10);
		glEnable(GL_LIGHTING);

		glDisable(GL_LIGHTING);
		glColor3f(0.8, 0.0, 0.0);
		glTranslatef(0.0, 0.0, 1.3);
		glutSolidSphere(0.3, 60, 60);
		glEnable(GL_LIGHTING);

		glColor3f(1.0, 1.0, 1.0);
	glPopMatrix();
}
void ground(float w, float h) { //����� �⺻�� �Ǵ� �Լ� (w = ���� ��, h = ���� ����)
	glColor3f(0.5f, 0.5f, 0.5f);
	glBegin(GL_QUADS);
		glVertex3f(-w, w, 0.0);
		glVertex3f(w, w, 0.0);
		glVertex3f(w, -h, 0.0);
		glVertex3f(-w, -h, 0.0);
	glEnd();
	glColor3f(1.0, 1.0, 1.0);
}
void map() { //ground�Լ��� �̿��Ͽ� ���� ���� ������
	glTranslatef(0.0f, 3.5f, 16.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	ground(1.0, 7.0); //1
	glTranslatef(4.0f, -10.0f, 0.0f);
	ground(1.0, 7.0); //2
	glTranslatef(-4.0f, -10.0f, 0.0f);
	ground(1.0, 7.0); //3
	glTranslatef(-4.0f, -10.0f, 0.0f);
	ground(1.0, 7.0); //4
	glTranslatef(4.0f, -10.0f, 0.0f);
	ground(1.0, 7.0); //5
	glTranslatef(4.0f, -10.0f, 0.0f);
	ground(1.0, 6.0); //6
	glTranslatef(-8.0f, -2.0f, 0.0f);
	ground(1.0, 20.0); //7 - 1
	glTranslatef(3.0f, -8.0f, 0.0f);
	ground(2.0, 12.0); //7 - 2
	glColor3f(0.5f, 0.5f, 0.5f);
	glTranslatef(1.0f, -6.0f, -1.0f);
	glutSolidCube(2.0); //��ֹ�
	glTranslatef(0.0f, -9.0f, 1.0f);
	ground(1.0, 12.0); //8
}

void MyDisplay() {
	GLfloat LightPosition[] = { 0.0, 0.0, 1.5, 1.0 };
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);

	float r = 999.9f;
	glPushMatrix();
		// Skybox�� �ո��� Rendering �Ѵ�.
		glBindTexture(GL_TEXTURE_2D, tex[0]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(-r, -r / 2, -r);
			glTexCoord2f(1, 0); glVertex3f(r, -r / 2, -r);
			glTexCoord2f(1, 1); glVertex3f(r, r, -r);
			glTexCoord2f(0, 1); glVertex3f(-r, r, -r);
		glEnd();

		// Skybox�� �޸��� Rendering �Ѵ�.
		glBindTexture(GL_TEXTURE_2D, tex[1]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(r, -r / 2, r);
			glTexCoord2f(1, 0); glVertex3f(-r, -r / 2, r);
			glTexCoord2f(1, 1); glVertex3f(-r, r, r);
			glTexCoord2f(0, 1); glVertex3f(r, r, r);
		glEnd();

		// Skybox�� ������ ���� Rendering �Ѵ�.
		glBindTexture(GL_TEXTURE_2D, tex[2]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(r, -r / 2, -r);
			glTexCoord2f(1, 0); glVertex3f(r, -r / 2, r);
			glTexCoord2f(1, 1); glVertex3f(r, r, r);
			glTexCoord2f(0, 1); glVertex3f(r, r, -r);
		glEnd();

		// Skybox�� ���� ���� Rendering �Ѵ�.
		glBindTexture(GL_TEXTURE_2D, tex[3]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(-r, -r / 2, r);
			glTexCoord2f(1, 0); glVertex3f(-r, -r / 2, -r);
			glTexCoord2f(1, 1); glVertex3f(-r, r, -r);
			glTexCoord2f(0, 1); glVertex3f(-r, r, r);
		glEnd();

		// Skybox�� ������ Rendering �Ѵ�.
		glBindTexture(GL_TEXTURE_2D, tex[4]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(-r, r, -r);
			glTexCoord2f(1, 0); glVertex3f(r, r, -r);
			glTexCoord2f(1, 1); glVertex3f(r, r, r);
			glTexCoord2f(0, 1); glVertex3f(-r, r, r);
		glEnd();

		// Skybox�� �Ʒ����� Rendering �Ѵ�.
		glBindTexture(GL_TEXTURE_2D, tex[5]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(-r, -r / 2, r);
			glTexCoord2f(1, 0); glVertex3f(r, -r / 2, r);
			glTexCoord2f(1, 1); glVertex3f(r, -r / 2, -r);
			glTexCoord2f(0, 1); glVertex3f(-r, -r / 2, -r);
		glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);

	glDisable(GL_TEXTURE_2D);

	PolarView(27.0, -20.0, 0.0, 0.0); //�� ��ǥ�� ����
	glTranslatef(0.0, 0.0, look1); //������ ���ּ��� �Բ� �����̵��� ���� 
	glFrustum(-1.0, 1.0, -1.0, 1.0, 0.0, 5.0); //���� ����
	glPushMatrix(); //�������� ����
		glTranslatef(0.0, 15.0, look1);
		glLightfv(GL_LIGHT0, GL_POSITION, g_light);
		glDisable(GL_LIGHTING);
		glColor3f(0.9, 0.9, 0.9);
		glutWireSphere(0.06, 10, 10);
		glEnable(GL_LIGHTING);
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(g_lightPosition[0], g_lightPosition[1], g_lightPosition[2]); //���ּ��� ��ġ ����
		board();
	glPopMatrix();

	glDisable(GL_LIGHTING);
	map(); //�� ����
	glEnable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);

	glutSwapBuffers();
}

void MyReshape(int NewWidth, int NewHeight) {
	glViewport(0, 0, NewWidth, NewHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLfloat)NewWidth / (GLfloat)NewHeight, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void reset() { //�޴����� reset�׸��� �����ϰų� r ��ư�� ���� �� ����Ǵ� �Լ�
	cout << "������ ���� �����մϴ�." << endl;
	g_lightPosition[0] = 0.0f;
	g_lightPosition[1] = 4.0f;
	g_lightPosition[2] = 16.0f;
	look1 = 0.0f;
	over = false;
	go = false;
	speed = 5;
}

void jumpTimer(int Value) { //���ּ��� ������Ű�� �Լ�
	if (jump) { 
		g_lightPosition[1] = 6.0f;
		jump = false;
	}
	else { //jump�� true�� �ƴϸ� ������ ���� �ʴ´�.
		g_lightPosition[1] = 4.0f;
	}
	glutTimerFunc(2000, jumpTimer, 1);
	glutPostRedisplay();
}

void MyKeyboard(unsigned char KeyPressed, int X, int Y) { //Ű���� �ݹ�
	switch (KeyPressed) {
	case 'r':
		reset(); //����
		break;
	case 's':
		if (IsBGPlaying) { //���� ������ ����ǰ� ���� ��� ������ �����Ѵ�.
			cout << "��������� �����մϴ�." << endl;
			pChannel[0]->stop();
			IsBGPlaying = false;
		}
		else { //���� ������ ���� ���� ��� ������ ����Ѵ�.
			cout << "��������� ����մϴ�." << endl;
			pSystem->playSound(FMOD_CHANNEL_REUSE, pSound[0], false, &pChannel[0]);
			pChannel[0]->setVolume(fVolume);
			IsBGPlaying = true;
		}
		break;
	case 32: //�����̽���(return 32) �Է�
		jump = true; //���� jumpTimer�Լ����� ������ �� �� �ֵ��� �ϴ� Ʈ����
		break;
	case 27:
		printf("���α׷��� �����մϴ�.\n");
		exit(0); //EscŰ(return 27) �Է½� �ܼ�â�� "���α׷��� �����մϴ�."�� ����� �� ���α׷� ����
		break;
	default:
		break; //�Ϲ����� Ű�� ��� ������ ���� x
	}
	glutPostRedisplay();
}

void MySpecial(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		go = true; //���ּ��� ������ �����̰� �Ѵٴ� Ʈ���� ����
		if (speed >= 140) { //���ǵ��� ���� 140�� ���� ��� ���̻� �������� ����
			cout << "�� �̻� ������ų �� �����ϴ�." << endl;
			speed = 140;
		}
		else {
			speed += 5; //���ǵ� ����
		}
	}
	if (key == GLUT_KEY_DOWN) {
		if (speed <= 5) { //���ǵ��� ���� 5���� ���� ��� ���̻� ���ҵ��� �ʰ� ���ּ��� ���ߴ� Ʈ���� ����
			cout << "�� �̻� ���ҽ�ų �� �����ϴ�." << endl;
			speed = 5;
			go = false;
		}
		else {
			speed -= 5; //���ǵ� ����
		}
	}
	if (key == GLUT_KEY_LEFT) {
		g_lightPosition[0] -= 0.1f; //���ּ��� �������� �̵���Ŵ
	}
	else if (key == GLUT_KEY_RIGHT) {
		g_lightPosition[0] += 0.1f; //���ּ��� ���������� �̵���Ŵ
	}

	glutPostRedisplay();
}

void MyTimer(int Value) {
	if (go) { //go �� ���� ��쿡�� ����
		if (g_lightPosition[2] > -74.7) {
			g_lightPosition[2] -= 0.1f; //���ּ��� Z�� �������� �̵���Ŵ
			look1 += 0.1; //������ ���ּ��� �Բ� �̵�
		}
		else {
			g_lightPosition[2] = -74.7; //���� ������ �����ϸ� ���ڸ����� ����
			cout << ">>> Game End <<<" << endl;
			over = true; 
		}
	}
	glutTimerFunc(150 - speed, MyTimer, 1);
	glutPostRedisplay();
}

void MenuProc(int entryID) {
	MenuChk = entryID;

	//glutAddMenuEntry("Reset", 1);
	if (entryID == 1) {
		reset(); //����
	}

	//glutAddMenuEntry("������� ���/����", 21);
	if (entryID == 21) {
		if (IsBGPlaying) {
			cout << "��������� �����մϴ�." << endl;
			pChannel[0]->stop();
			IsBGPlaying = false;
		}
		else {
			cout << "��������� ����մϴ�." << endl;
			pSystem->playSound(FMOD_CHANNEL_REUSE, pSound[0], false, &pChannel[0]);
			pChannel[0]->setVolume(fVolume);
			IsBGPlaying = true;
		}
	}
	//glutAddMenuEntry("���� ����", 22);
	if (entryID == 22) {
		pChannel[0]->getVolume(&fVolume); //���� ���� ������
		pChannel[0]->setVolume(fVolume + 0.2f); //���� ���� ������Ŵ
		cout << "������ ������ŵ�ϴ�." << endl;
	}
	//glutAddMenuEntry("���� ����", 23);
	if (entryID == 23) {
		if (fVolume < 0.2) {
			cout << "���̻� ���ҽ�ų �� �����ϴ�." << endl;
		}
		else {
			pChannel[0]->getVolume(&fVolume); //���� ���� ������
			pChannel[0]->setVolume(fVolume - 0.2f); //���� ���� ���ҽ�Ŵ
			cout << "������ ���ҽ�ŵ�ϴ�."<< endl;
		}
	}

	//glutAddMenuEntry("Exit", 4);
	if (entryID == 4) {
		cout << "���α׷��� �����մϴ�." << endl;
		exit(0); //�޴����� Exit ���ý� �ܼ�â�� "���α׷��� �����մϴ�."�� ����� �� ���α׷� ����
	}
	glutPostRedisplay();
}

void MenuFunc() {
	GLint subMenu2 = glutCreateMenu(MenuProc); //BGM�� �����޴�
	glutAddMenuEntry("������� ���/����", 21);
	glutAddMenuEntry("���� ����", 22);
	glutAddMenuEntry("���� ����", 23);

	GLint mainMenu = glutCreateMenu(MenuProc); //���� �޴�
	glutAddMenuEntry("Reset", 1);
	glutAddSubMenu("BGM", subMenu2);
	glutAddMenuEntry("Exit", 4);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void SoundIn() {
	System_Create(&pSystem); //���� �ý����� ����
	pSystem->init(2, FMOD_INIT_NORMAL, NULL); //�ý��� �⺻ ����
	pSystem->createSound("SkyroadsRoad08B.wav", FMOD_HARDWARE, NULL, &pSound[0]); //������� ����
}
void SoundOut() {
	pSound[0]->release(); //������� ���� ����
	pSystem->release(); //���� �ý��� ����
	pSystem->close(); //���� �ý��� �ݱ�
}

void help() { //�ܼ�â�� ��µ� ���۹�
	cout << "----------------------------------------------" << endl;
	cout << " �� & �� : ���ּ��� ��, ��� �̵���ŵ�ϴ�." << endl;
	cout << " �� & �� : ���ּ��� �ӵ��� �����մϴ�." << endl;
	cout << " space	: ���ּ��� ������ŵ�ϴ�." << endl;
	cout << "----------------------------------------------" << endl;
	cout << " s	: ������� �Ѱų� �� �� �ֽ��ϴ�." << endl;
	cout << " r	: ������ �����մϴ�." << endl;
	cout << " esc	: ������ �����մϴ�." << endl;
	cout << "----------------------------------------------" << endl;
}

int main(int argc, char** argv) {
	help();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Skyroads");
	SoundIn();
	LoadGLTextures();
	MyInit();

	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);

	glutKeyboardFunc(MyKeyboard);
	glutSpecialFunc(MySpecial);
	glutTimerFunc(150 - speed, MyTimer, 1);
	glutTimerFunc(2000, jumpTimer, 1);
	MenuFunc();

	glutMainLoop();
	SoundOut();

	return 0;
}
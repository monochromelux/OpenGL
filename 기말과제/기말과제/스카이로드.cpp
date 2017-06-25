/*
날짜 : 2017/05/17 ~ 06/12
작성자 : 이효은
파일명 : 스카이로드.cpp
주제 : 우주선 게임
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

System* pSystem; //배경음 설정용 전역변수
Sound* pSound[2];
Channel* pChannel[2];
bool IsBGPlaying = false;
float fVolume = 0.6;

int MenuChk;

GLint width = 800, height = 600;
GLfloat look1 = 0.0; //우주선과 함께 시선이 이동하도록 해주는 전역변수
unsigned int speed = 5; //우주선의 속도를 조절하는 전역변수
float g_lightPosition[] = { 0.0f, 4.0f, 16.0f }; //시작 시 우주선의 좌표 X, Y, Z
bool jump = false, over = false, go = false; //각각 점프여부, 게임오버 여부, 비행기가 앞으로 가는 움직임의 여부이다.

GLfloat g_light[] = { 0.0, 0.0, 1.5, 1.0 };
GLfloat light0_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
GLfloat light0_diffuse[] = { 0.8, 0.7, 0.6, 1.0 };
GLfloat light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };

GLuint tex[6]; //설정할 텍스처 이미지의 갯수

AUX_RGBImageRec *LoadBMP(char *Filename) {  // Bitmap 이미지를 호출한다.
	FILE *File = NULL;

	if (!Filename) return NULL;
	File = fopen(Filename, "r");
	if (File) {
		fclose(File);
		return auxDIBImageLoad(Filename);
	}

	return NULL;
}

void LoadGLTextures() {  // Bitmap 이미지 6개를 호출하여 Texture 이미지로 변환한다.
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

void MyInit(void) {  //전체조명과 지역조명 0 설정
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
}

void PolarView(GLfloat radius, GLfloat elevation, GLfloat azimuth, GLfloat twist) {
	glTranslatef(0.0, 0.0, -radius); //극 좌표계 설정
	glRotatef(-elevation, 1.0, 0.0, 0.0);
	glRotatef(azimuth, 0.0, 0.0, 1.0);
	glRotatef(-twist, 0.0, 0.0, 1.0);
}

void board() { //우주선 출력
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
void ground(float w, float h) { //길들의 기본이 되는 함수 (w = 길의 폭, h = 길의 길이)
	glColor3f(0.5f, 0.5f, 0.5f);
	glBegin(GL_QUADS);
		glVertex3f(-w, w, 0.0);
		glVertex3f(w, w, 0.0);
		glVertex3f(w, -h, 0.0);
		glVertex3f(-w, -h, 0.0);
	glEnd();
	glColor3f(1.0, 1.0, 1.0);
}
void map() { //ground함수를 이용하여 게임 맵을 설정함
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
	glutSolidCube(2.0); //장애물
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
		// Skybox의 앞면을 Rendering 한다.
		glBindTexture(GL_TEXTURE_2D, tex[0]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(-r, -r / 2, -r);
			glTexCoord2f(1, 0); glVertex3f(r, -r / 2, -r);
			glTexCoord2f(1, 1); glVertex3f(r, r, -r);
			glTexCoord2f(0, 1); glVertex3f(-r, r, -r);
		glEnd();

		// Skybox의 뒷면을 Rendering 한다.
		glBindTexture(GL_TEXTURE_2D, tex[1]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(r, -r / 2, r);
			glTexCoord2f(1, 0); glVertex3f(-r, -r / 2, r);
			glTexCoord2f(1, 1); glVertex3f(-r, r, r);
			glTexCoord2f(0, 1); glVertex3f(r, r, r);
		glEnd();

		// Skybox의 오른쪽 면을 Rendering 한다.
		glBindTexture(GL_TEXTURE_2D, tex[2]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(r, -r / 2, -r);
			glTexCoord2f(1, 0); glVertex3f(r, -r / 2, r);
			glTexCoord2f(1, 1); glVertex3f(r, r, r);
			glTexCoord2f(0, 1); glVertex3f(r, r, -r);
		glEnd();

		// Skybox의 왼쪽 면을 Rendering 한다.
		glBindTexture(GL_TEXTURE_2D, tex[3]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(-r, -r / 2, r);
			glTexCoord2f(1, 0); glVertex3f(-r, -r / 2, -r);
			glTexCoord2f(1, 1); glVertex3f(-r, r, -r);
			glTexCoord2f(0, 1); glVertex3f(-r, r, r);
		glEnd();

		// Skybox의 윗면을 Rendering 한다.
		glBindTexture(GL_TEXTURE_2D, tex[4]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(-r, r, -r);
			glTexCoord2f(1, 0); glVertex3f(r, r, -r);
			glTexCoord2f(1, 1); glVertex3f(r, r, r);
			glTexCoord2f(0, 1); glVertex3f(-r, r, r);
		glEnd();

		// Skybox의 아랫면을 Rendering 한다.
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

	PolarView(27.0, -20.0, 0.0, 0.0); //극 좌표계 설정
	glTranslatef(0.0, 0.0, look1); //시점이 우주선과 함께 움직이도록 설정 
	glFrustum(-1.0, 1.0, -1.0, 1.0, 0.0, 5.0); //원근 투영
	glPushMatrix(); //지역조명 설정
		glTranslatef(0.0, 15.0, look1);
		glLightfv(GL_LIGHT0, GL_POSITION, g_light);
		glDisable(GL_LIGHTING);
		glColor3f(0.9, 0.9, 0.9);
		glutWireSphere(0.06, 10, 10);
		glEnable(GL_LIGHTING);
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(g_lightPosition[0], g_lightPosition[1], g_lightPosition[2]); //우주선의 위치 설정
		board();
	glPopMatrix();

	glDisable(GL_LIGHTING);
	map(); //맵 설정
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

void reset() { //메뉴에서 reset항목을 선택하거나 r 버튼을 누를 시 실행되는 함수
	cout << "게임을 새로 시작합니다." << endl;
	g_lightPosition[0] = 0.0f;
	g_lightPosition[1] = 4.0f;
	g_lightPosition[2] = 16.0f;
	look1 = 0.0f;
	over = false;
	go = false;
	speed = 5;
}

void jumpTimer(int Value) { //우주선을 점프시키는 함수
	if (jump) { 
		g_lightPosition[1] = 6.0f;
		jump = false;
	}
	else { //jump가 true가 아니면 점프를 하지 않는다.
		g_lightPosition[1] = 4.0f;
	}
	glutTimerFunc(2000, jumpTimer, 1);
	glutPostRedisplay();
}

void MyKeyboard(unsigned char KeyPressed, int X, int Y) { //키보드 콜백
	switch (KeyPressed) {
	case 'r':
		reset(); //리셋
		break;
	case 's':
		if (IsBGPlaying) { //현재 음악이 실행되고 있을 경우 음악을 정지한다.
			cout << "배경음악을 정지합니다." << endl;
			pChannel[0]->stop();
			IsBGPlaying = false;
		}
		else { //현재 음악이 멈춰 있을 경우 음악을 재생한다.
			cout << "배경음악을 재생합니다." << endl;
			pSystem->playSound(FMOD_CHANNEL_REUSE, pSound[0], false, &pChannel[0]);
			pChannel[0]->setVolume(fVolume);
			IsBGPlaying = true;
		}
		break;
	case 32: //스페이스바(return 32) 입력
		jump = true; //위의 jumpTimer함수에서 점프를 할 수 있도록 하는 트리거
		break;
	case 27:
		printf("프로그램을 종료합니다.\n");
		exit(0); //Esc키(return 27) 입력시 콘솔창에 "프로그램을 종료합니다."를 출력한 뒤 프로그램 종료
		break;
	default:
		break; //일반적인 키의 경우 눌러도 반응 x
	}
	glutPostRedisplay();
}

void MySpecial(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		go = true; //우주선을 앞으로 움직이게 한다는 트리거 실행
		if (speed >= 140) { //스피드의 값이 140이 넘을 경우 더이상 증가되지 않음
			cout << "더 이상 증가시킬 수 없습니다." << endl;
			speed = 140;
		}
		else {
			speed += 5; //스피드 증가
		}
	}
	if (key == GLUT_KEY_DOWN) {
		if (speed <= 5) { //스피드의 값이 5보다 낮을 경우 더이상 감소되지 않고 우주선을 멈추는 트리거 실행
			cout << "더 이상 감소시킬 수 없습니다." << endl;
			speed = 5;
			go = false;
		}
		else {
			speed -= 5; //스피드 감소
		}
	}
	if (key == GLUT_KEY_LEFT) {
		g_lightPosition[0] -= 0.1f; //우주선을 왼쪽으로 이동시킴
	}
	else if (key == GLUT_KEY_RIGHT) {
		g_lightPosition[0] += 0.1f; //우주선을 오른쪽으로 이동시킴
	}

	glutPostRedisplay();
}

void MyTimer(int Value) {
	if (go) { //go 가 참일 경우에만 실행
		if (g_lightPosition[2] > -74.7) {
			g_lightPosition[2] -= 0.1f; //우주선을 Z축 방향으로 이동시킴
			look1 += 0.1; //시점도 우주선과 함께 이동
		}
		else {
			g_lightPosition[2] = -74.7; //맵의 끝까지 도달하면 그자리에서 멈춤
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
		reset(); //리셋
	}

	//glutAddMenuEntry("배경음악 재생/정지", 21);
	if (entryID == 21) {
		if (IsBGPlaying) {
			cout << "배경음악을 정지합니다." << endl;
			pChannel[0]->stop();
			IsBGPlaying = false;
		}
		else {
			cout << "배경음악을 재생합니다." << endl;
			pSystem->playSound(FMOD_CHANNEL_REUSE, pSound[0], false, &pChannel[0]);
			pChannel[0]->setVolume(fVolume);
			IsBGPlaying = true;
		}
	}
	//glutAddMenuEntry("음량 증가", 22);
	if (entryID == 22) {
		pChannel[0]->getVolume(&fVolume); //음량 값을 가져옴
		pChannel[0]->setVolume(fVolume + 0.2f); //음량 값을 증가시킴
		cout << "음량을 증가시킵니다." << endl;
	}
	//glutAddMenuEntry("음량 감소", 23);
	if (entryID == 23) {
		if (fVolume < 0.2) {
			cout << "더이상 감소시킬 수 없습니다." << endl;
		}
		else {
			pChannel[0]->getVolume(&fVolume); //음량 값을 가져옴
			pChannel[0]->setVolume(fVolume - 0.2f); //음량 값을 감소시킴
			cout << "음량을 감소시킵니다."<< endl;
		}
	}

	//glutAddMenuEntry("Exit", 4);
	if (entryID == 4) {
		cout << "프로그램을 종료합니다." << endl;
		exit(0); //메뉴에서 Exit 선택시 콘솔창에 "프로그램을 종료합니다."를 출력한 뒤 프로그램 종료
	}
	glutPostRedisplay();
}

void MenuFunc() {
	GLint subMenu2 = glutCreateMenu(MenuProc); //BGM의 하위메뉴
	glutAddMenuEntry("배경음악 재생/정지", 21);
	glutAddMenuEntry("음량 증가", 22);
	glutAddMenuEntry("음량 감소", 23);

	GLint mainMenu = glutCreateMenu(MenuProc); //메인 메뉴
	glutAddMenuEntry("Reset", 1);
	glutAddSubMenu("BGM", subMenu2);
	glutAddMenuEntry("Exit", 4);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void SoundIn() {
	System_Create(&pSystem); //사운드 시스템을 생성
	pSystem->init(2, FMOD_INIT_NORMAL, NULL); //시스템 기본 세팅
	pSystem->createSound("SkyroadsRoad08B.wav", FMOD_HARDWARE, NULL, &pSound[0]); //배경음악 셋팅
}
void SoundOut() {
	pSound[0]->release(); //배경음악 셋팅 해제
	pSystem->release(); //사운드 시스템 해제
	pSystem->close(); //사운드 시스템 닫기
}

void help() { //콘솔창에 출력될 조작법
	cout << "----------------------------------------------" << endl;
	cout << " → & ← : 우주선을 좌, 우로 이동시킵니다." << endl;
	cout << " ↑ & ↓ : 우주선의 속도를 조절합니다." << endl;
	cout << " space	: 우주선을 점프시킵니다." << endl;
	cout << "----------------------------------------------" << endl;
	cout << " s	: 배경음을 켜거나 끌 수 있습니다." << endl;
	cout << " r	: 게임을 리셋합니다." << endl;
	cout << " esc	: 게임을 종료합니다." << endl;
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
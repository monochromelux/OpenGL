/*
날짜 : 2017/04/25
작성자 : 이효은
파일명 : 중간과제.cpp
주제 : 태양 주위를 공전하는 지구와 달(190p Solar System 예제를 수정 + 콜백함수를 추가하여 응용)
*/

#include <glut.h>
#include <glu.h>
#include <gl.h>
#include <stdio.h>

int MenuChk;
static int Day = 0, Time = 0, Draw = 0, Turn = 0, Speed = 5;
GLint TopX, TopY, BottomX, BottomY;

void MyDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST); //깊이 버퍼를 설정하여 바깥으로도 돌 수 있게 함
	//태양
	glColor3f(1.0, 0.0, 0.3);
	if (Draw == 0)
		glutSolidSphere(0.36, 60, 16); //꽉 찬 형태의 구(Draw가 0일 경우 설정됨)
	else
		glutWireSphere(0.36, 60, 16); //뼈대 형태의 구(Draw가 0이 아닐 경우 설정됨)
	glPushMatrix();
		//지구
		glRotatef((GLfloat)Day, 0.0, 1.0, 0.0);
		glTranslatef(0.75, 0.0, 0.0);
		glRotatef((GLfloat)Time, 0.0, 1.0, 0.0);
		glColor3f(0.3, 1.0, 0.9);
		if (Draw == 0)
			glutSolidSphere(0.16, 60, 8); //꽉 찬 형태의 구(Draw가 0일 경우 설정됨)
		else
			glutWireSphere(0.16, 60, 8); //뼈대 형태의 구(Draw가 0이 아닐 경우 설정됨)
		glPushMatrix();
			//달
			glRotatef((GLfloat)Time, 0.0, 1.0, 0.0);
			glTranslatef(0.25, 0.0, 0.0);
			glColor3f(0.8, 0.5, 0.9);
			if (Draw == 0)
				glutSolidSphere(0.06, 60, 8); //꽉 찬 형태의 구(Draw가 0일 경우 설정됨)
			else
				glutWireSphere(0.06, 60, 8); //뼈대 형태의 구(Draw가 0이 아닐 경우 설정됨)
		glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();
}

void MyReshape(int NewWidth, int NewHeight) {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	GLfloat nRange = 1.0f;
	if (NewHeight == 0)
		NewHeight = 1;
	glViewport(0, 0, NewWidth, NewHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (NewWidth <= NewHeight)
		glOrtho(-nRange, nRange, -nRange*NewHeight / NewWidth, nRange*NewHeight / NewWidth, -nRange, nRange);
	else
		glOrtho(-nRange*NewWidth / NewHeight, nRange*NewWidth / NewHeight, -nRange, nRange, -nRange, nRange);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MyKeyboard(unsigned char KeyPressed, int X, int Y) {
	switch (KeyPressed) {
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
	if (key == GLUT_KEY_UP)
		Time = (Time + 5) % 360;
		//위쪽 방향키를 누를 때 마다 Time의 값이 5씩 증가, %360을 해줘서 값이 너무 커지는 것을 방지
	if (key == GLUT_KEY_DOWN)
		Time = (Time - 5) % 360;
		//아래쪽 방향키를 누를 때 마다 Time의 값이 5씩 감소, %360을 해줘서 값이 음수로 내려가는 것을 방지
	if (key == GLUT_KEY_LEFT)
		Day = (Day - 10) % 360;
		//왼쪽 방향키를 누를 때 마다 Day의 값이 10씩 감소, %360을 해줘서 값이 음수로 내려가는 것을 방지
	if (key == GLUT_KEY_RIGHT)
		Day = (Day + 10) % 360;
		//오른쪽 방향키를 누를 때 마다 Day의 값이 10씩 증가, %360을 해줘서 값이 너무 커지는 것을 방지
	glutPostRedisplay();
}

void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y) {
	if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) {
		TopX = X;
		TopY = Y;
	} //마우스 왼쪽 버튼을 눌렀을때의 좌표
}

void MyMouseMove(GLint X, GLint Y) {
	BottomX = X;
	BottomY = Y; //마우스가 움직인 뒤의 좌표
	if (Turn == 1 || Turn == 2) { //자동 회전 모드일때만 작동
		if (TopX < BottomX || TopY > BottomY)
			Speed = 15; //왼쪽->오른쪽 혹은 아래->위로 드래그할 경우 최대 속도로 전환
		if (TopX > BottomX || TopY < BottomY)
			Speed = 1; //오른쪽->왼쪽 혹은 위->아래로 드래그할 경우 최저 속도로 전환
	}
	glutPostRedisplay();
}

void MyTimer(int Value) {
	if (Turn == 1)
		Day = (Day + Speed) % 360;
		//Turn이 1일 경우 Day에 Speed만큼 주기적으로 더해서 지구 부분을 회전, %360을 해줘서 값이 너무 커지는 것을 방지
	else if (Turn == 2)
		Time = (Time + Speed) % 360;
		//Turn이 2일 경우 Time에 Speed만큼 주기적으로 더해서 달 부분을 회전, %360을 해줘서 값이 너무 커지는 것을 방지
	else {
		Day += 0;
		Time += 0;
	} //Turn이 1도 2도 아닐 경우 Day와 Time에 0을 더하도록 해서 타이머 무력화
	glutPostRedisplay();
	glutTimerFunc(40, MyTimer, 1);
}

void MenuProc(int entryID) {
	MenuChk = entryID;
	//glutAddSubMenu("Auto Turn", subMenu1);
	if (entryID == 11) {
		printf("자동으로 지구를 공전시킵니다. 현재 속도는 %d\n", Speed);
		Turn = 1;
		//메뉴에서 Auto Turn->지구 회전 선택시 콘솔창에 "자동으로 지구를 공전시킵니다."와 현재 속도를 출력한 뒤 Turn을 1로 변경
	}
	if (entryID == 12) {
		printf("자동으로 지구를 자전, 달을 공전시킵니다. 현재 속도는 %d\n", Speed);
		Turn = 2;
		//메뉴에서 Auto Turn->달 회전 선택시 콘솔창에 "자동으로 지구를 자전, 달을 공전시킵니다."와 현재 속도를 출력한 뒤 Turn을 2로 변경
	}
		if (entryID == 131) {
			if (Speed == 15)
				printf("최대 속도입니다.\n");
			else {
				Speed += 1;
				printf("속도를 증가시킵니다. 현재 속도는 %d\n", Speed);
			}
			//메뉴에서 Auto Turn->회전 속도->빠르게 선택시 콘솔창에 "속도를 증가시킵니다."와 현재 속도를 출력한 뒤 Speed를 1증가시킴
			//단 Speed가 15일 경우 콘솔창에 "최대 속도입니다."가 출력되며 Speed가 더이상 증가되지 않음
		}
		if (entryID == 132) {
			if (Speed == 1)
				printf("최저 속도입니다.\n");
			else {
				Speed -= 1;
				printf("속도를 감소시킵니다. 현재 속도는 %d\n", Speed);
			}
			//메뉴에서 Auto Turn->회전 속도->느리게 선택시 콘솔창에 "속도를 감소시킵니다."와 현재 속도를 출력한 뒤 Speed를 1감소시킴
			//단 Speed가 1일 경우 콘솔창에 "최저 속도입니다."가 출력되며 Speed가 더이상 감소되지 않음
		}
	if (entryID == 14) {
		printf("회전을 멈춥니다.\n");
		Turn = 0;
		//메뉴에서 Auto Turn->Stop 선택시 콘솔창에 "회전을 멈춥니다."를 띄운 뒤 Turn을 0으로 변경
	}

	//glutAddSubMenu("Draw", subMenu2);
	if (entryID == 21) {
		printf("뼈대 형태로 변경합니다.\n");
		Draw = 1;
		//메뉴에서 Draw->Draw Wire 선택시 콘솔창에 "뼈대 형태로 변경합니다."를 출력한 뒤 Draw를 1로 변경
	}
	if (entryID == 22) {
		printf("꽉 찬 형태로 변경합니다.\n");
		Draw = 0;
		//메뉴에서 Draw->Draw Soild 선택시 콘솔창에 "꽉 찬 형태로 변경합니다."를 출력한 뒤 Draw를 0으로 변경
	}

	//glutAddMenuEntry("Exit", 4);
	if (entryID == 4) {
		printf("프로그램을 종료합니다.\n");
		exit(0); //메뉴에서 Exit 선택시 콘솔창에 "프로그램을 종료합니다."를 출력한 뒤 프로그램 종료
	}
	glutPostRedisplay();
}

void MenuFunc() {
	GLint subMenu13 = glutCreateMenu(MenuProc); //회전 속도 의 하위 메뉴
	glutAddMenuEntry("빠르게", 131);
	glutAddMenuEntry("느리게", 132);

	GLint subMenu1 = glutCreateMenu(MenuProc); //Auto Turn 의 하위 메뉴
	glutAddMenuEntry("지구 공전", 11);
	glutAddMenuEntry("지구 자전 + 달 공전", 12);
	glutAddSubMenu("회전 속도", subMenu13);
	glutAddMenuEntry("Stop", 14);

	GLint subMenu2 = glutCreateMenu(MenuProc); //Draw 의 하위메뉴
	glutAddMenuEntry("Draw Wire", 21);
	glutAddMenuEntry("Draw Soild", 22);

	GLint mainMenu = glutCreateMenu(MenuProc); //메인 메뉴
	glutAddSubMenu("Auto Turn", subMenu1);
	glutAddSubMenu("Draw", subMenu2);
	glutAddMenuEntry("Exit", 4);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 400);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Solar System");
	printf(" * 방향키의 오른쪽&왼쪽 버튼을 누르면 지구가, 위쪽&아래쪽 버튼을 누르면 달이 공전합니다.\n");
	printf(" * 자동 회전 모드에서 왼쪽->오른쪽 혹은 아래->위로 드래그할 경우 최대 속도,\n");
	printf("   오른쪽->왼쪽 혹은 위->아래로 드래그할 경우 최저 속도로 전환됩니다.\n");
	printf(" * 종료는 마우스 오른쪽 버튼을 누른 후 Exit항목을 선택하거나 Esc버튼을 눌러주세요.\n\n");

	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);

	glutKeyboardFunc(MyKeyboard);
	glutSpecialFunc(MySpecial);
	glutMouseFunc(MyMouseClick);
	glutMotionFunc(MyMouseMove);
	glutTimerFunc(40, MyTimer, 1);
	MenuFunc();

	glutMainLoop();
	return 0;
}
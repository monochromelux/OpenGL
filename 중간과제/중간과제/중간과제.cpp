/*
��¥ : 2017/04/25
�ۼ��� : ��ȿ��
���ϸ� : �߰�����.cpp
���� : �¾� ������ �����ϴ� ������ ��(190p Solar System ������ ���� + �ݹ��Լ��� �߰��Ͽ� ����)
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
	glEnable(GL_DEPTH_TEST); //���� ���۸� �����Ͽ� �ٱ����ε� �� �� �ְ� ��
	//�¾�
	glColor3f(1.0, 0.0, 0.3);
	if (Draw == 0)
		glutSolidSphere(0.36, 60, 16); //�� �� ������ ��(Draw�� 0�� ��� ������)
	else
		glutWireSphere(0.36, 60, 16); //���� ������ ��(Draw�� 0�� �ƴ� ��� ������)
	glPushMatrix();
		//����
		glRotatef((GLfloat)Day, 0.0, 1.0, 0.0);
		glTranslatef(0.75, 0.0, 0.0);
		glRotatef((GLfloat)Time, 0.0, 1.0, 0.0);
		glColor3f(0.3, 1.0, 0.9);
		if (Draw == 0)
			glutSolidSphere(0.16, 60, 8); //�� �� ������ ��(Draw�� 0�� ��� ������)
		else
			glutWireSphere(0.16, 60, 8); //���� ������ ��(Draw�� 0�� �ƴ� ��� ������)
		glPushMatrix();
			//��
			glRotatef((GLfloat)Time, 0.0, 1.0, 0.0);
			glTranslatef(0.25, 0.0, 0.0);
			glColor3f(0.8, 0.5, 0.9);
			if (Draw == 0)
				glutSolidSphere(0.06, 60, 8); //�� �� ������ ��(Draw�� 0�� ��� ������)
			else
				glutWireSphere(0.06, 60, 8); //���� ������ ��(Draw�� 0�� �ƴ� ��� ������)
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
		printf("���α׷��� �����մϴ�.\n");
		exit(0); //EscŰ(return 27) �Է½� �ܼ�â�� "���α׷��� �����մϴ�."�� ����� �� ���α׷� ����
		break;
	default:
		break; //�Ϲ����� Ű�� ��� ������ ���� x
	}
	glutPostRedisplay();
}

void MySpecial(int key, int x, int y) {
	if (key == GLUT_KEY_UP)
		Time = (Time + 5) % 360;
		//���� ����Ű�� ���� �� ���� Time�� ���� 5�� ����, %360�� ���༭ ���� �ʹ� Ŀ���� ���� ����
	if (key == GLUT_KEY_DOWN)
		Time = (Time - 5) % 360;
		//�Ʒ��� ����Ű�� ���� �� ���� Time�� ���� 5�� ����, %360�� ���༭ ���� ������ �������� ���� ����
	if (key == GLUT_KEY_LEFT)
		Day = (Day - 10) % 360;
		//���� ����Ű�� ���� �� ���� Day�� ���� 10�� ����, %360�� ���༭ ���� ������ �������� ���� ����
	if (key == GLUT_KEY_RIGHT)
		Day = (Day + 10) % 360;
		//������ ����Ű�� ���� �� ���� Day�� ���� 10�� ����, %360�� ���༭ ���� �ʹ� Ŀ���� ���� ����
	glutPostRedisplay();
}

void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y) {
	if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) {
		TopX = X;
		TopY = Y;
	} //���콺 ���� ��ư�� ���������� ��ǥ
}

void MyMouseMove(GLint X, GLint Y) {
	BottomX = X;
	BottomY = Y; //���콺�� ������ ���� ��ǥ
	if (Turn == 1 || Turn == 2) { //�ڵ� ȸ�� ����϶��� �۵�
		if (TopX < BottomX || TopY > BottomY)
			Speed = 15; //����->������ Ȥ�� �Ʒ�->���� �巡���� ��� �ִ� �ӵ��� ��ȯ
		if (TopX > BottomX || TopY < BottomY)
			Speed = 1; //������->���� Ȥ�� ��->�Ʒ��� �巡���� ��� ���� �ӵ��� ��ȯ
	}
	glutPostRedisplay();
}

void MyTimer(int Value) {
	if (Turn == 1)
		Day = (Day + Speed) % 360;
		//Turn�� 1�� ��� Day�� Speed��ŭ �ֱ������� ���ؼ� ���� �κ��� ȸ��, %360�� ���༭ ���� �ʹ� Ŀ���� ���� ����
	else if (Turn == 2)
		Time = (Time + Speed) % 360;
		//Turn�� 2�� ��� Time�� Speed��ŭ �ֱ������� ���ؼ� �� �κ��� ȸ��, %360�� ���༭ ���� �ʹ� Ŀ���� ���� ����
	else {
		Day += 0;
		Time += 0;
	} //Turn�� 1�� 2�� �ƴ� ��� Day�� Time�� 0�� ���ϵ��� �ؼ� Ÿ�̸� ����ȭ
	glutPostRedisplay();
	glutTimerFunc(40, MyTimer, 1);
}

void MenuProc(int entryID) {
	MenuChk = entryID;
	//glutAddSubMenu("Auto Turn", subMenu1);
	if (entryID == 11) {
		printf("�ڵ����� ������ ������ŵ�ϴ�. ���� �ӵ��� %d\n", Speed);
		Turn = 1;
		//�޴����� Auto Turn->���� ȸ�� ���ý� �ܼ�â�� "�ڵ����� ������ ������ŵ�ϴ�."�� ���� �ӵ��� ����� �� Turn�� 1�� ����
	}
	if (entryID == 12) {
		printf("�ڵ����� ������ ����, ���� ������ŵ�ϴ�. ���� �ӵ��� %d\n", Speed);
		Turn = 2;
		//�޴����� Auto Turn->�� ȸ�� ���ý� �ܼ�â�� "�ڵ����� ������ ����, ���� ������ŵ�ϴ�."�� ���� �ӵ��� ����� �� Turn�� 2�� ����
	}
		if (entryID == 131) {
			if (Speed == 15)
				printf("�ִ� �ӵ��Դϴ�.\n");
			else {
				Speed += 1;
				printf("�ӵ��� ������ŵ�ϴ�. ���� �ӵ��� %d\n", Speed);
			}
			//�޴����� Auto Turn->ȸ�� �ӵ�->������ ���ý� �ܼ�â�� "�ӵ��� ������ŵ�ϴ�."�� ���� �ӵ��� ����� �� Speed�� 1������Ŵ
			//�� Speed�� 15�� ��� �ܼ�â�� "�ִ� �ӵ��Դϴ�."�� ��µǸ� Speed�� ���̻� �������� ����
		}
		if (entryID == 132) {
			if (Speed == 1)
				printf("���� �ӵ��Դϴ�.\n");
			else {
				Speed -= 1;
				printf("�ӵ��� ���ҽ�ŵ�ϴ�. ���� �ӵ��� %d\n", Speed);
			}
			//�޴����� Auto Turn->ȸ�� �ӵ�->������ ���ý� �ܼ�â�� "�ӵ��� ���ҽ�ŵ�ϴ�."�� ���� �ӵ��� ����� �� Speed�� 1���ҽ�Ŵ
			//�� Speed�� 1�� ��� �ܼ�â�� "���� �ӵ��Դϴ�."�� ��µǸ� Speed�� ���̻� ���ҵ��� ����
		}
	if (entryID == 14) {
		printf("ȸ���� ����ϴ�.\n");
		Turn = 0;
		//�޴����� Auto Turn->Stop ���ý� �ܼ�â�� "ȸ���� ����ϴ�."�� ��� �� Turn�� 0���� ����
	}

	//glutAddSubMenu("Draw", subMenu2);
	if (entryID == 21) {
		printf("���� ���·� �����մϴ�.\n");
		Draw = 1;
		//�޴����� Draw->Draw Wire ���ý� �ܼ�â�� "���� ���·� �����մϴ�."�� ����� �� Draw�� 1�� ����
	}
	if (entryID == 22) {
		printf("�� �� ���·� �����մϴ�.\n");
		Draw = 0;
		//�޴����� Draw->Draw Soild ���ý� �ܼ�â�� "�� �� ���·� �����մϴ�."�� ����� �� Draw�� 0���� ����
	}

	//glutAddMenuEntry("Exit", 4);
	if (entryID == 4) {
		printf("���α׷��� �����մϴ�.\n");
		exit(0); //�޴����� Exit ���ý� �ܼ�â�� "���α׷��� �����մϴ�."�� ����� �� ���α׷� ����
	}
	glutPostRedisplay();
}

void MenuFunc() {
	GLint subMenu13 = glutCreateMenu(MenuProc); //ȸ�� �ӵ� �� ���� �޴�
	glutAddMenuEntry("������", 131);
	glutAddMenuEntry("������", 132);

	GLint subMenu1 = glutCreateMenu(MenuProc); //Auto Turn �� ���� �޴�
	glutAddMenuEntry("���� ����", 11);
	glutAddMenuEntry("���� ���� + �� ����", 12);
	glutAddSubMenu("ȸ�� �ӵ�", subMenu13);
	glutAddMenuEntry("Stop", 14);

	GLint subMenu2 = glutCreateMenu(MenuProc); //Draw �� �����޴�
	glutAddMenuEntry("Draw Wire", 21);
	glutAddMenuEntry("Draw Soild", 22);

	GLint mainMenu = glutCreateMenu(MenuProc); //���� �޴�
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
	printf(" * ����Ű�� ������&���� ��ư�� ������ ������, ����&�Ʒ��� ��ư�� ������ ���� �����մϴ�.\n");
	printf(" * �ڵ� ȸ�� ��忡�� ����->������ Ȥ�� �Ʒ�->���� �巡���� ��� �ִ� �ӵ�,\n");
	printf("   ������->���� Ȥ�� ��->�Ʒ��� �巡���� ��� ���� �ӵ��� ��ȯ�˴ϴ�.\n");
	printf(" * ����� ���콺 ������ ��ư�� ���� �� Exit�׸��� �����ϰų� Esc��ư�� �����ּ���.\n\n");

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
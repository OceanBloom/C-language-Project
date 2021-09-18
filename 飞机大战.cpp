#define _CRT_SECURE_NO_WARNINGS//��ֹ��strcat()��ȫ����
#include <graphics.h>
#include <stdio.h>
#include <stdlib.h> //  exit() ����
#include <time.h>
#include <math.h>
#include <mmsystem.h>//��ý�岥�Žӿ�ͷ�ļ�
#pragma comment (lib,"winmm.lib")//���ؾ�̬�⣨���ڲ������֣�

#define MYPLANEBLOOD 10
#define STARTDELAY 2000//���ֵл���ûǰ���ӳ٣���λ��ms��
#define RULEMAX 500//�����ı��������
#define BULLETNUM 100//һ���ӵ�������
#define ENEMYNUM 30//һ���л�������
#define EDGE 2//���ڵ����ӵ����ел�����Ч��Ե��Χ����λ�����أ�

//���ֵл�Ѫ���궨��
#define ENEMY0HP 2
#define ENEMY1HP 3
#define ENEMY2HP 4
#define ENEMY3HP 5

#pragma region ͼƬ��Դ�ߴ�
/************����ͼƬ��Դ�ĳߴ�************/
#define WIDTH 1522//���ڿ�
#define HEIGHT 787//���ڸ�

#define PLAYERWIDTH 97//��ҷɻ�ͼƬ��
#define PLAYERHEIGHT 75//��ҷɻ�ͼƬ��

#define BLOODWIDTH 39//����ֵͼƬ��͸�
#define BLOODHEIGHT 39

#define BULLETWIDTH 30//����ӵ�ͼƬ��
#define BULLETHEIGHT 60//����ӵ�ͼƬ��

#define EWIDTH0 59//0�ŵл�ͼƬ��
#define EHEIGHT0 42//0�ŵл�ͼƬ��

#define EWIDTH1 80//1�ŵл�ͼƬ��
#define EHEIGHT1 70//1�ŵл�ͼƬ��

#define EWIDTH2 99//2�ŵл�ͼƬ��
#define EHEIGHT2 75//2�ŵл�ͼƬ��

#define EWIDTH3 125//3�ŵл�ͼƬ��
#define EHEIGHT3 81//3�ŵл�ͼƬ��
/****************************************/
#pragma endregion

#pragma region ��ť��Ϣ
/**************��ť��Ϣ************/
#define BUTTONNUM 3
//��ť˳��{��ʼ,�뿪,�淨���ر�}
 int buttonX[BUTTONNUM] = { 1042,1335,785 };
int buttonY[BUTTONNUM] = { 563,648,679};
int buttonR[BUTTONNUM] = { 145,93,85 };
int buttonTextSize[BUTTONNUM] = { 155,70,60 };
COLORREF buttonFillColor[BUTTONNUM] = { RGB(243, 113, 141) ,RGB(243, 113, 141) ,RGB(243, 113, 141) };
COLORREF buttonLineColor[BUTTONNUM] = { RGB(255, 225, 0) ,RGB(255, 225, 0) ,RGB(255, 225, 0) };
COLORREF buttonTextColor[BUTTONNUM] = { RGB(255, 225, 0) ,RGB(255, 225, 0) ,RGB(255, 225, 0) };
double buttonLineRate[BUTTONNUM] = { 0.1,0.1,0.1};
/***********************************/
#pragma endregion

struct Plane {
	int x; //������
	int y; //������
	bool live; //�Ƿ���
	int type; //�ɻ������ͣ��˴�ָ���ŵл�
	int hp; //Ѫ����Ѫ��Ϊ0������
	bool enemyDone;//��¼�õл��Ƿ��Ѿ���ʹ��ҿ�Ѫ�Է��ظ���Ѫ
	int enemyWidth; //�л�ͼƬ��
	int enemyHeight; //�л�ͼƬ��
}myPlane,bullet[BULLETNUM],enemy[ENEMYNUM];
//��ҷɻ�������ӵ����ݣ���ŵл�����

struct CircleButton {
	int x;//Բ������
	int y;
	int r;//�뾶
	COLORREF fillColor;
	COLORREF lineColor;
	COLORREF textColor;
	int textSize;//�����С
	double rate;//�����ߴ�ϸռ�뾶�ı���
}buttons[BUTTONNUM];

int playerBlood = MYPLANEBLOOD;//���Ѫ��
ExMessage mouse;//��¼�����Ϣ
IMAGE menuBackground;//�����Ϸ�˵����汳��ͼ
IMAGE playingBackground;//�����Ϸ�б���ͼ
IMAGE playerImg[2];//�����ҷɻ���ͼƬ
IMAGE playerBloodImg[2];//����������ͼƬ
IMAGE bulletImg[2];//�������ӵ�ͼƬ
IMAGE enemyImg[4][2];//��ŵл�ͼƬ��Դ

/***********************
*���룺��ťԲ�ĺ����꣬�����꣬�뾶�����ɫ������ɫ���ı����ݣ��ı���С�������ߴ�ϸռ�뾶����
*�������
*���ã���������λ�á���С�������ɫ��������ɫ���ı���Բ�ΰ�ť
************************/
void SingleButton(int x, int y,int radius , COLORREF fillColor, COLORREF lineColor, COLORREF textColor, const char* text,int textSize,double rate) {
	setbkmode(TRANSPARENT);//����͸��ʹ�ı����������Ǻ�ɫ
	//���û�����ʽΪ����ǰ뾶��0.1�������ߣ��˵�ΪԲ��
	setlinestyle(PS_DASH | PS_ENDCAP_ROUND, (int) (rate*(double)radius) );
	setfillcolor(fillColor);//���ɫ
	setlinecolor(lineColor);//�����ߵ���ɫ
	fillcircle(x, y, radius);//��Բ
	char word[50] = "";//���ڽ���������ı�
	strcpy_s(word, text);//��������ı����Ƶ�Word��
	settextstyle(textSize, 0, "����");//�����ʽ
	int textX = x - textwidth(text) / 2;//λ�þ���
	int textY = y - textheight(text) / 2;
	settextcolor(textColor);//������ɫ
	outtextxy(textX, textY, text);//��ʾ�ı�
}

/***********************
*���룺��
*�������
*���ã���ʼ���а�ť��Ϣ
************************/
void ButtonInit() {
	for (int i = 0; i < BUTTONNUM; i++)
	{
		buttons[i].x = buttonX[i];
		buttons[i].y = buttonY[i];
		buttons[i].r = buttonR[i];
		buttons[i].textSize = buttonTextSize[i];
		buttons[i].fillColor = buttonFillColor[i];
		buttons[i].lineColor = buttonLineColor[i];
		buttons[i].textColor = buttonTextColor[i];
		buttons[i].rate = buttonLineRate[i];
	}
}

/***********************
*���룺��
*�������
*���ã����Ƴ��˵����������а�ť
************************/
void DrawMenuButtons() {
	SingleButton(buttons[0].x, buttons[0].y, buttons[0].r, buttons[0].fillColor, buttons[0].lineColor,
		buttons[0].textColor, " GO!", buttons[0].textSize,buttons[0].rate);//���ƿ�ʼ��Ϸ��ť
	SingleButton(buttons[1].x, buttons[1].y, buttons[1].r, buttons[1].fillColor, buttons[1].lineColor,
		buttons[1].textColor, "�뿪", buttons[1].textSize, buttons[1].rate);//�����˳���Ϸ��ť
	SingleButton(buttons[2].x, buttons[2].y, buttons[2].r, buttons[1].fillColor, buttons[2].lineColor,
		buttons[2].textColor, "�淨", buttons[2].textSize, buttons[2].rate);//�����淨���ܰ�ť
}

/***********************
*���룺��������ɫ������������ɫ���ı�������ɫ
*�������
*���ã������ָ���˵���ťʱ��ť���б�ɫ�����û�����
************************/
void MouseOnMenuButtons(COLORREF newFillColor, COLORREF newLineColor, COLORREF newTextColor) {
	if (sqrt(pow((double)mouse.x - buttons[0].x, 2.0) + pow((double)mouse.y - buttons[0].y, 2.0)) <= buttons[0].r)
		SingleButton(buttons[0].x, buttons[0].y, buttons[0].r, newFillColor, newLineColor,
			newTextColor, " GO!", buttons[0].textSize, buttons[0].rate);//���ָ��ʼ��ťʱ�ı�ɫ
	else if (sqrt(pow((double)mouse.x - buttons[1].x, 2.0) + pow((double)mouse.y - buttons[1].y, 2.0)) <= buttons[1].r)
		SingleButton(buttons[1].x, buttons[1].y, buttons[1].r, newFillColor, newLineColor,
			newTextColor, "�뿪", buttons[1].textSize, buttons[1].rate);//���ָ�뿪��ťʱ�ı�ɫ
	else if (sqrt(pow((double)mouse.x - buttons[2].x, 2.0) + pow((double)mouse.y - buttons[1].y, 2.0)) <= buttons[2].r)
		SingleButton(buttons[2].x, buttons[2].y, buttons[2].r, newFillColor, newLineColor,
			newTextColor, "�淨", buttons[2].textSize, buttons[2].rate);//���ָ�淨��ťʱ�ı�ɫ
	else
		DrawMenuButtons();
}

/***********************
*���룺��
*�������
*���ã�����ͼƬ��Դ
************************/
void Loading() {
	//���ر���ͼ
	loadimage(&playingBackground, "./��Դ/����.png");
	//�����������ͼ+ԭͼ
	loadimage(&playerImg[1], "./��Դ/���.png");
	loadimage(&playerImg[0], "./��Դ/��ң�����ͼ��.png");
	//�����������ͼƬ
	loadimage(&playerBloodImg[1], "./��Դ/������ԭͼ��.png");
	loadimage(&playerBloodImg[0], "./��Դ/����������ͼ��.png");
	//�����ӵ�����ͼ+ԭͼ
	loadimage(&bulletImg[0], "./��Դ/�ӵ�1������ͼ��.png");
	loadimage(&bulletImg[1], "./��Դ/�ӵ�1��ԭͼ��.png");
	//���صл�����ͼ+ԭͼ
	loadimage(&enemyImg[0][0], "./��Դ/�л�0������ͼ��.png");
	loadimage(&enemyImg[0][1], "./��Դ/�л�0��ԭͼ��.png");
	loadimage(&enemyImg[1][0], "./��Դ/�л�1������ͼ��.png");
	loadimage(&enemyImg[1][1], "./��Դ/�л�1��ԭͼ��.png");
	loadimage(&enemyImg[2][0], "./��Դ/�л�2������ͼ��.png");
	loadimage(&enemyImg[2][1], "./��Դ/�л�2��ԭͼ��.png");
	loadimage(&enemyImg[3][0], "./��Դ/�л�3������ͼ��.png");
	loadimage(&enemyImg[3][1], "./��Դ/�л�3��ԭͼ��.png");
}

/***********************
*���룺��
*�������
*���ã���ʼ���л����ݣ��ɻ������Ͱ��ȶ������������
************************/
void EnemyInit() {
	int ranNum;//���������
	for (int i = 0; i < ENEMYNUM; i++)//�л�����
	{
		ranNum = rand() % 10;//0-9�����
		if (ranNum <= 2) {//�����Ϊ0��1��2ʱ����ʼΪ0�ŵл�
			enemy[i].hp = ENEMY0HP;//0�ŵл�Ѫ��
			enemy[i].type = 0;//0�ŵл�
			//0�ŵл��Ŀ�͸�
			enemy[i].enemyWidth = EWIDTH0;
			enemy[i].enemyHeight = EHEIGHT0;
		}
		else if (ranNum <= 5) {//�����Ϊ3��4��5ʱ����ʼΪ1�ŵл�
			enemy[i].hp = ENEMY1HP;//1�ŵл�Ѫ��
			enemy[i].type = 1;//1�ŵл�
			//1�ŵл��Ŀ�͸�
			enemy[i].enemyWidth = EWIDTH1;
			enemy[i].enemyHeight = EHEIGHT1;
		}
		else if (ranNum <= 7) {//�����Ϊ6��7ʱ����ʼΪ2�ŵл�
			enemy[i].hp = ENEMY2HP;//2�ŵл�Ѫ��
			enemy[i].type = 2;//2�ŵл�
			//2�ŵл��Ŀ�͸�
			enemy[i].enemyWidth = EWIDTH2;
			enemy[i].enemyHeight = EHEIGHT2;
		}
		else if (ranNum <= 9) {//�����Ϊ8��9ʱ����ʼΪ3�ŵл�
			enemy[i].hp = ENEMY3HP;//3�ŵл�Ѫ��
			enemy[i].type = 3;//3�ŵл�
			//3�ŵл��Ŀ�͸�
			enemy[i].enemyWidth = EWIDTH3;
			enemy[i].enemyHeight = EHEIGHT3;
		}
	}
}

/***********************
*���룺��
*�������
*���ã���Ϸ��ʼ������
************************/
void GameInit() {
	//��ҷɻ���ʼλ��Ϊ��Ϸ���ڵײ�����
	myPlane.x = (WIDTH - PLAYERWIDTH) / 2;
	myPlane.y = HEIGHT - PLAYERHEIGHT;
	myPlane.live = true;//���״̬��true
	playerBlood = MYPLANEBLOOD;
	myPlane.hp = playerBlood;
	//��ʼ�ӵ�
	for (int i = 0; i < BULLETNUM; i++)
	{
		bullet[i].live = false;
		bullet[i].x = 0;
		bullet[i].y = 0;
	}
	for (int i = 0; i < ENEMYNUM; i++)
	{
		//��ʼ״̬�����ел���δ������������
		enemy[i].live = false;
		enemy[i].enemyDone = false;//��ʼʱ���зɻ���û��ʹ��Ҽ�Ѫ
	}
	EnemyInit();//��ʼ�л�����
}

/***********************
*���룺��
*�������
*���ã����������л�
************************/
void CreatEnemy() {
	//�л�����
	for (int i = 0; i < ENEMYNUM; i++)
	{
		if (!enemy[i].live) {//������һ���л��Ĵ��״̬Ϊfalse�����ɸõ����л�
			switch (enemy[i].type) {//���ݵл����;���Ѫ��
			case 0: enemy[i].hp = ENEMY0HP; break;
			case 1: enemy[i].hp = ENEMY1HP; break;
			case 2: enemy[i].hp = ENEMY2HP; break;
			case 3: enemy[i].hp = ENEMY3HP; break;
			}
			enemy[i].live = true;//��Ϊ���
			//���ɷɻ�λ���ں������������Χ��[0,WIDTH - enemy[i].enemyWidth]��֤��ʾ�������ķɻ���
			enemy[i].x = rand() % (WIDTH - enemy[i].enemyWidth);
			enemy[i].y = 0;//�������������
			break;//���ɵ����ɻ�������ѭ��
		}
	}
}

/***********************
*���룺��
*�������
*���ã����������ӵ�
************************/
void CreatBullet() {
	for (int i = 0; i < BULLETNUM; i++)//����һ���ӵ�
	{
		if (!bullet[i].live) {//������һ���ӵ��Ĵ��״̬Ϊfalse�����ɸõ����ӵ�
			bullet[i].live = true;//��Ϊ���
			//������λ������ҷɻ������м�
			bullet[i].x = myPlane.x + PLAYERWIDTH / 2 - BULLETWIDTH / 2;
			bullet[i].y = myPlane.y - BULLETHEIGHT; 
			break;//���ɵ����ӵ�������ѭ��
		}
	}
}

/***********************
*���룺��
*�������
*���ã�������Ϸͼ��
************************/
void GameDraw() {
	int bloodX = 0;
	Loading();//����ͼƬ��Դ
	//������ͼ
	putimage(0, 0, &playingBackground);
	//������ֵͼƬ
	for (int i = 0; i < playerBlood; i++)
	{
		putimage(bloodX, 0, &playerBloodImg[0], NOTSRCERASE);
		putimage(bloodX, 0, &playerBloodImg[1], SRCINVERT);
		bloodX += BLOODWIDTH+4;
	}
	//���������ͼ+ԭͼ
	if (myPlane.hp > 0) {
		putimage(myPlane.x, myPlane.y, &playerImg[0], NOTSRCERASE);
		putimage(myPlane.x, myPlane.y, &playerImg[1], SRCINVERT);
	}
	//�������ӵ���ͼƬ
	for (int i = 0; i < BULLETNUM; i++)
	{
		if (bullet[i].live) {
			putimage(bullet[i].x, bullet[i].y, &bulletImg[0], NOTSRCERASE);
			putimage(bullet[i].x, bullet[i].y, &bulletImg[1], SRCINVERT);
		}
	}
	//�����ɵл���ͼƬ
	for (int i = 0; i < ENEMYNUM; i++)
	{
		if (enemy[i].live == true) {
			switch (enemy[i].type) {
			case 0:
				putimage(enemy[i].x, enemy[i].y, &enemyImg[0][0], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &enemyImg[0][1], SRCINVERT); break;
			case 1:
				putimage(enemy[i].x, enemy[i].y, &enemyImg[1][0], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &enemyImg[1][1], SRCINVERT); break;
			case 2:
				putimage(enemy[i].x, enemy[i].y, &enemyImg[2][0], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &enemyImg[2][1], SRCINVERT); break;
			case 3:
				putimage(enemy[i].x, enemy[i].y, &enemyImg[3][0], NOTSRCERASE);
				putimage(enemy[i].x, enemy[i].y, &enemyImg[3][1], SRCINVERT); break;
				}
			}
		}
	}

/***********************
*���룺��int���ͣ��ӳٵ�ʱ�䣬��λ��ms
*�������bool���ͣ�ʱ�䵽->true; ����->false
*���ã���ʱ��
************************/
bool Timer(int delay) {
	static DWORD t1, t2;
	if (unsigned(t2 - t1) > unsigned(delay)) {
		t1 = t2;
		return true;
	}
	t2 = clock();
	return false;
}

/***********************
*���룺��int���ͣ�������ҷɻ��ƶ����ٶ�
*�������
*���ã�ʹ��ҷɻ��ƶ�
************************/
void MyPlaneMove(int speed) {
	//GetAsyncKeyState(_In_ int vKey);�������ڼ�ⰴ��
	//���ƶ�������������б���ƶ�
	if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W')) {//��дW��ͬʱ��ʾW��w
		if(myPlane.y>0)
			myPlane.y -= speed;//����
	}
	if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S')) {
		if(myPlane.y+ PLAYERHEIGHT<HEIGHT)
			myPlane.y += speed;//����
	}
	if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A')) {
		if(myPlane.x+ PLAYERWIDTH /2>0)
			myPlane.x -= speed;//����
	}
	if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D')) {
		if(myPlane.x+ PLAYERWIDTH / 2<WIDTH)
			myPlane.x += speed;//����
	}
	//�ո������ӵ�
	//����һ���ӳٷ�ֹ��һ�¿ո��������ӵ���ͬʱ���Կ��������ӵ����ܶ�
	if (GetAsyncKeyState(VK_SPACE) && Timer(150)) 
		CreatBullet();
}

/***********************
*���룺��int���ͣ������ӵ��ƶ����ٶ�
*�������
*���ã�ʹ�ӵ��ƶ�
************************/
void BulletMove(int speed) {
	for (int i = 0; i < BULLETNUM; i++)
	{
		if (bullet[i].live) {//����ӵ�Ҫ�Զ������ƶ�
			bullet[i].y -= speed;//����
			if (bullet[i].y + BULLETHEIGHT < 0)//�ӵ���ȫ�Ƴ����ں�ָ�false���״̬���Ա��������ӵ�
				bullet[i].live = false;
		}
	}
}

/***********************
*���룺��int���ͣ���ʾ�л������ƶ����ٶȣ���Ϊ��ͬ���͵л����ٲ�ͬ��
*�������
*���ã�ʹ�л��ƶ�
************************/
void EnemyMove(int speed) {
	for (int i = 0; i < ENEMYNUM; i++)
	{
		if (enemy[i].live) {//�л�������Ҫ�Զ������ƶ�
			//�������ٷ���
#if 1
			//�ֲ�ȡ�ķ�������ͬ���͵л��ò�ͬ������speed�����ֳ��ٶ�
			switch (enemy[i].type)
			{//  0-->3  ��-->��  
			case 0:enemy[i].y += 5 * speed; break;
			case 1:enemy[i].y += 4 * speed; break;
			case 2:enemy[i].y += 3 * speed; break;
			case 3:enemy[i].y += 2 * speed; break;
			}
#elif 0
			//�����������ел��ٶ���������ڴ˷����ƶ���̫������δ���ã�
			enemy[i].y += (rand() % 10 + 1) * speed;
#endif
			//�л��������뿪���ں�live�ָ�false�Ա�֤�����ел�����
			if (enemy[i].y - enemy[i].enemyHeight > HEIGHT)
			{
				enemy[i].live = false;
				enemy[i].enemyDone = false;
			}
		}
	}
}

/***********************
*���룺��
*�������
*���ã���ҷɻ�����ϵͳ
************************/
void Attack() {
	for (int i = 0; i < ENEMYNUM; i++)//�����л�
	{
		if (!enemy[i].live)
			continue;//���������л�
		for (int j = 0; j < BULLETNUM; j++)
		{//�����ӵ�
			if (!bullet[j].live)
				continue;//�����������ӵ�
			//�ӵ���л�һ�����غ���������Ϊ������Ч������EDGE������Ч��Ե��
			if ((bullet[j].x + BULLETWIDTH >= enemy[i].x -EDGE && bullet[j].x <= enemy[i].x + enemy[i].enemyWidth + EDGE)
				&& (bullet[j].y >= enemy[j].y -EDGE && bullet[j].y <= enemy[i].y + enemy[i].enemyHeight + EDGE)) {
				bullet[j].live = false;//�������ӵ�����
				enemy[i].hp--;//�л�����һ��Ѫ��
			}
		}
		if (enemy[i].hp <= 0)//�л�Ѫ��<=0������
			enemy[i].live = false;
	}
}

/***********************
*���룺int����  �Ѿ�������Ϸ��ʱ��
*�����unsigned int����  �����Ϸ����������ҶԵ�����ѡ���������������
*���ã��������ʲôʱ���Ѫ�������Ϸ
************************/
UINT PlayerBlood(int gameTime) {
	HWND h = GetHWnd();//��ȡ���ھ��
	for (int i = 0; i < ENEMYNUM; i++)//�����л�
	{  //���ĳ�л�����δ��ʹ��ҵ�Ѫ
		if (enemy[i].live && !enemy[i].enemyDone) {
			//��Ѫ���һ���л������ҷ��ڲ�
			if (enemy[i].y >= HEIGHT)
			{
				myPlane.hp--;
				playerBlood--;
				enemy[i].enemyDone = true;
			}
			//��Ѫ���������ҷɻ���л�ֱ�ӽӴ�
			if ((myPlane.x < enemy[i].x + enemy[i].enemyWidth) && (myPlane.x > enemy[i].x - PLAYERWIDTH)
				&& (myPlane.y < enemy[i].y + enemy[i].enemyHeight) && (myPlane.y + PLAYERHEIGHT > enemy[i].y))
			{
				myPlane.hp--;
				playerBlood--;
				enemy[i].enemyDone = true;
			}
		}
	}
	char chTime[15] = "";//����ת�����ַ������ͺ����Ϸʱ��
	_itoa_s(gameTime,chTime,15,10);//_itoa_s������int����ת�����ַ�������
	//ƴ�ӳ�һ���ַ���
	char string1[100] = "��Ϸ������̫�����ˣ����������Ѿ������";
	char string2[] = "�룡�Ƿ�����һ�֣�";
	strcat(string1, chTime);
	strcat(string1, string2);
	if (myPlane.hp <= 0)//Ѫ�������������Ϸ��������
	{
		mciSendStringA("close ./��Դ/ս��BGM.mp3", 0, 0, 0);
		mciSendStringA("open ./��Դ/��Ϸ����BGM.mp3", 0, 0, 0);
		mciSendStringA("play ./��Դ/��Ϸ����BGM.mp3", 0, 0, 0);
		UINT choice = MessageBoxA(h, string1, "��Ϸ����", MB_YESNO);
		return choice;
	}
	return 1;//����·���з���һ����Ӱ��YES/NO��ֵ
}

/***********************
*���룺int����ˮӡ�����ꡢint����ˮӡ�����ꡢint����ˮӡ����ߴ硢ˮӡ��ɫ
*�������
*���ã�������λ�����������С����ɫ���ı��䵱ˮӡ
************************/
void WaterMark(int textX,int textY, int textSize,COLORREF textColor) {
	setbkmode(TRANSPARENT);//����͸��ʹ�ı����������Ǻ�ɫ
	settextstyle(textSize, 0, "����");//�����ʽ
	settextcolor(textColor);//������ɫ
	outtextxy(textX, textY, "By ��ı��");//��ʾ�ı�
}

/***********************
*���룺��
*�������
*���ã���Ϸ�˵�����
************************/
void Menu() {
	ButtonInit();
	mciSendStringA("open ./��Դ/�˵�BGM.mp3", 0, 0, 0);//�򿪲˵�����BGM
	loadimage(&menuBackground, "./��Դ/menuBackground.png");
	putimage(0, 0, &menuBackground);
	DrawMenuButtons();
	mciSendStringA("play ./��Դ/�˵�BGM.mp3 repeat", 0, 0, 0);//��������
	WaterMark(2, 763, 25, RGB(0, 0, 0));//�����½���ʾˮӡ
}

/***********************
*���룺��
*�������
*���ã�����Ϸ�е�ȫ����
************************/
void PlayingGame() {
	mciSendStringA("close ./��Դ/�˵�BGM.mp3", 0, 0, 0);//�رղ˵�����BGM
	mciSendStringA("open ./��Դ/ս��BGM.mp3", 0, 0, 0);//����Ϸ����BGM
L0:GameInit();//��ʼ����Ϸ
	mciSendStringA("play ./��Դ/ս��BGM.mp3 repeat", 0, 0, 0);//������Ϸ����BGM
	BeginBatchDraw();//�����������ƣ�ʹѭ����ͼ����ʾ����
	int playTime = 0;//���ڼ�¼�Ѿ�������Ϸ��ʱ�䣬ͬʱҲ�ǵ÷�
	UINT endChoice;//��¼���������а�ť��ѡ��
	DWORD startTime=clock(), endTime=clock();
	while (1) {
		//���������ӳ�ʱ�������л������������л�֮����0.65��
		if (Timer(650) && unsigned(endTime - startTime) > STARTDELAY) {
			CreatEnemy();
		}
		GameDraw();//��ͼ
		FlushBatchDraw();//ˢ��
		MyPlaneMove(22);//��ҷɻ��ƶ�
		endChoice = PlayerBlood(playTime);
		if (endChoice == IDYES)
		{
			mciSendStringA("close ./��Դ/��Ϸ����BGM.mp3", 0, 0, 0);
			goto L0;//����һ�ֺ����¿�ʼ
		}
		if (endChoice == IDNO)
		{
			mciSendStringA("close ./��Դ/ս��BGM.mp3", 0, 0, 0);//�ر���Ϸ����BGM
			mciSendStringA("close ./��Դ/��Ϸ����BGM.mp3", 0, 0, 0);
			exit(1);//������Ϸ��ֹ����
		}
		BulletMove(12);//�ӵ��ƶ�
		EnemyMove(2);//�л��ƶ�
		Attack();//����
		endTime = clock();
		playTime = ((int)endTime-(int)startTime) / 1000;
	}
	EndBatchDraw();//������������
}

/***********************
*���룺��
*�������
*���ã�����һ���йع�����ܵĵ���
************************/
void RulesWindow() {
	HWND h = GetHWnd();//��ȡ���ھ��
	char ruleText[RULEMAX];
	FILE* fp; int k = 0;
	fopen_s(&fp,"./��Դ/Rules.txt","r");//�򿪴�Ź����ı���txt�ļ�
	if (fp == NULL)
		exit(1);//�򲻿��ļ�ʱֱ��ֹͣ����
	else {
		if(fgets(ruleText,RULEMAX,fp)!=NULL)
			MessageBoxA(h, ruleText, "�淨���", MB_OK);
	} 
	fclose(fp);
}

//������
int main() {
	initgraph(WIDTH, HEIGHT);//���ƴ���
	Menu();
	BeginBatchDraw();//�����������ƣ�ʹѭ����ͼ����ʾ����
	while (1) {
		FlushBatchDraw();//ˢ��
		if (peekmessage(&mouse, EM_MOUSE)) {//��ȡ�����Ϣ
			//�˵������У������ָ���ƶ�����ť��ʱ���ɫ�Է����û�
			MouseOnMenuButtons(RGB(56, 199, 170), RGB(216, 120, 147), RGB(216, 120, 147));
			if (mouse.message == WM_LBUTTONDOWN) {//���°�ťʱ
				//���"Go��"��ť
				if (sqrt(pow((double)mouse.x - buttons[0].x, 2.0) + pow((double)mouse.y - buttons[0].y, 2.0)) <= buttons[0].r)
					PlayingGame();
				//���"�뿪"��ť
				if (sqrt(pow((double)mouse.x - buttons[1].x, 2.0) + pow((double)mouse.y - buttons[1].y, 2.0)) <= buttons[1].r)
				{
					mciSendStringA("close ./��Դ/�˵�BGM.mp3", 0, 0, 0);//�رղ˵�����BGM
					return 0;
				}
				//���"�淨"��ť
				if (sqrt(pow((double)mouse.x - buttons[2].x, 2.0) + pow((double)mouse.y - buttons[2].y, 2.0)) <= buttons[2].r)
					RulesWindow();
			}
		}
	}
	EndBatchDraw();//������������
	return 0;
}
#define _CRT_SECURE_NO_WARNINGS//防止对strcat()安全警告
#include <graphics.h>
#include <stdio.h>
#include <stdlib.h> //  exit() 函数
#include <time.h>
#include <math.h>
#include <mmsystem.h>//多媒体播放接口头文件
#pragma comment (lib,"winmm.lib")//加载静态库（用于播放音乐）

#define MYPLANEBLOOD 10
#define STARTDELAY 2000//开局敌机出没前的延迟（单位：ms）
#define RULEMAX 500//规则文本最大字数
#define BULLETNUM 100//一梭子弹的数量
#define ENEMYNUM 30//一波敌机的数量
#define EDGE 2//用于调整子弹命中敌机的有效边缘范围（单位：像素）

//四种敌机血量宏定义
#define ENEMY0HP 2
#define ENEMY1HP 3
#define ENEMY2HP 4
#define ENEMY3HP 5

#pragma region 图片资源尺寸
/************所有图片资源的尺寸************/
#define WIDTH 1522//窗口宽
#define HEIGHT 787//窗口高

#define PLAYERWIDTH 97//玩家飞机图片宽
#define PLAYERHEIGHT 75//玩家飞机图片高

#define BLOODWIDTH 39//生命值图片宽和高
#define BLOODHEIGHT 39

#define BULLETWIDTH 30//玩家子弹图片宽
#define BULLETHEIGHT 60//玩家子弹图片高

#define EWIDTH0 59//0号敌机图片宽
#define EHEIGHT0 42//0号敌机图片高

#define EWIDTH1 80//1号敌机图片宽
#define EHEIGHT1 70//1号敌机图片高

#define EWIDTH2 99//2号敌机图片宽
#define EHEIGHT2 75//2号敌机图片高

#define EWIDTH3 125//3号敌机图片宽
#define EHEIGHT3 81//3号敌机图片高
/****************************************/
#pragma endregion

#pragma region 按钮信息
/**************按钮信息************/
#define BUTTONNUM 3
//按钮顺序：{开始,离开,玩法，关闭}
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
	int x; //横坐标
	int y; //纵坐标
	bool live; //是否存活
	int type; //飞机的类型，此处指几号敌机
	int hp; //血量，血量为0后死亡
	bool enemyDone;//记录该敌机是否已经致使玩家扣血以防重复减血
	int enemyWidth; //敌机图片宽
	int enemyHeight; //敌机图片高
}myPlane,bullet[BULLETNUM],enemy[ENEMYNUM];
//玩家飞机，存放子弹数据，存放敌机数据

struct CircleButton {
	int x;//圆心坐标
	int y;
	int r;//半径
	COLORREF fillColor;
	COLORREF lineColor;
	COLORREF textColor;
	int textSize;//字体大小
	double rate;//轮廓线粗细占半径的比例
}buttons[BUTTONNUM];

int playerBlood = MYPLANEBLOOD;//玩家血量
ExMessage mouse;//记录鼠标消息
IMAGE menuBackground;//存放游戏菜单界面背景图
IMAGE playingBackground;//存放游戏中背景图
IMAGE playerImg[2];//存放玩家飞机的图片
IMAGE playerBloodImg[2];//存放玩家生命图片
IMAGE bulletImg[2];//存放玩家子弹图片
IMAGE enemyImg[4][2];//存放敌机图片资源

/***********************
*输入：按钮圆心横坐标，纵坐标，半径，填充色，轮廓色，文本内容，文本大小，轮廓线粗细占半径比例
*输出：空
*作用：产生任意位置、大小、填充颜色、轮廓颜色、文本的圆形按钮
************************/
void SingleButton(int x, int y,int radius , COLORREF fillColor, COLORREF lineColor, COLORREF textColor, const char* text,int textSize,double rate) {
	setbkmode(TRANSPARENT);//背景透明使文本背景不再是黑色
	//设置画线样式为宽度是半径的0.1倍的虚线，端点为圆形
	setlinestyle(PS_DASH | PS_ENDCAP_ROUND, (int) (rate*(double)radius) );
	setfillcolor(fillColor);//填充色
	setlinecolor(lineColor);//轮廓线的颜色
	fillcircle(x, y, radius);//画圆
	char word[50] = "";//用于接收输入的文本
	strcpy_s(word, text);//将输入的文本复制到Word中
	settextstyle(textSize, 0, "黑体");//字体格式
	int textX = x - textwidth(text) / 2;//位置居中
	int textY = y - textheight(text) / 2;
	settextcolor(textColor);//字体颜色
	outtextxy(textX, textY, text);//显示文本
}

/***********************
*输入：空
*输出：空
*作用：初始所有按钮信息
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
*输入：空
*输出：空
*作用：绘制出菜单界面中所有按钮
************************/
void DrawMenuButtons() {
	SingleButton(buttons[0].x, buttons[0].y, buttons[0].r, buttons[0].fillColor, buttons[0].lineColor,
		buttons[0].textColor, " GO!", buttons[0].textSize,buttons[0].rate);//绘制开始游戏按钮
	SingleButton(buttons[1].x, buttons[1].y, buttons[1].r, buttons[1].fillColor, buttons[1].lineColor,
		buttons[1].textColor, "离开", buttons[1].textSize, buttons[1].rate);//绘制退出游戏按钮
	SingleButton(buttons[2].x, buttons[2].y, buttons[2].r, buttons[1].fillColor, buttons[2].lineColor,
		buttons[2].textColor, "玩法", buttons[2].textSize, buttons[2].rate);//绘制玩法介绍按钮
}

/***********************
*输入：填充的新颜色，轮廓的新颜色，文本的新颜色
*输出：空
*作用：当鼠标指到菜单按钮时按钮进行变色以向用户反馈
************************/
void MouseOnMenuButtons(COLORREF newFillColor, COLORREF newLineColor, COLORREF newTextColor) {
	if (sqrt(pow((double)mouse.x - buttons[0].x, 2.0) + pow((double)mouse.y - buttons[0].y, 2.0)) <= buttons[0].r)
		SingleButton(buttons[0].x, buttons[0].y, buttons[0].r, newFillColor, newLineColor,
			newTextColor, " GO!", buttons[0].textSize, buttons[0].rate);//鼠标指开始按钮时的变色
	else if (sqrt(pow((double)mouse.x - buttons[1].x, 2.0) + pow((double)mouse.y - buttons[1].y, 2.0)) <= buttons[1].r)
		SingleButton(buttons[1].x, buttons[1].y, buttons[1].r, newFillColor, newLineColor,
			newTextColor, "离开", buttons[1].textSize, buttons[1].rate);//鼠标指离开按钮时的变色
	else if (sqrt(pow((double)mouse.x - buttons[2].x, 2.0) + pow((double)mouse.y - buttons[1].y, 2.0)) <= buttons[2].r)
		SingleButton(buttons[2].x, buttons[2].y, buttons[2].r, newFillColor, newLineColor,
			newTextColor, "玩法", buttons[2].textSize, buttons[2].rate);//鼠标指玩法按钮时的变色
	else
		DrawMenuButtons();
}

/***********************
*输入：空
*输出：空
*作用：加载图片资源
************************/
void Loading() {
	//加载背景图
	loadimage(&playingBackground, "./资源/背景.png");
	//加载玩家掩码图+原图
	loadimage(&playerImg[1], "./资源/玩家.png");
	loadimage(&playerImg[0], "./资源/玩家（掩码图）.png");
	//加载玩家生命图片
	loadimage(&playerBloodImg[1], "./资源/生命（原图）.png");
	loadimage(&playerBloodImg[0], "./资源/生命（掩码图）.png");
	//加载子弹掩码图+原图
	loadimage(&bulletImg[0], "./资源/子弹1（掩码图）.png");
	loadimage(&bulletImg[1], "./资源/子弹1（原图）.png");
	//加载敌机掩码图+原图
	loadimage(&enemyImg[0][0], "./资源/敌机0（掩码图）.png");
	loadimage(&enemyImg[0][1], "./资源/敌机0（原图）.png");
	loadimage(&enemyImg[1][0], "./资源/敌机1（掩码图）.png");
	loadimage(&enemyImg[1][1], "./资源/敌机1（原图）.png");
	loadimage(&enemyImg[2][0], "./资源/敌机2（掩码图）.png");
	loadimage(&enemyImg[2][1], "./资源/敌机2（原图）.png");
	loadimage(&enemyImg[3][0], "./资源/敌机3（掩码图）.png");
	loadimage(&enemyImg[3][1], "./资源/敌机3（原图）.png");
}

/***********************
*输入：空
*输出：空
*作用：初始化敌机数据，飞机的类型按既定比率随机生成
************************/
void EnemyInit() {
	int ranNum;//随机数声明
	for (int i = 0; i < ENEMYNUM; i++)//敌机遍历
	{
		ranNum = rand() % 10;//0-9随机数
		if (ranNum <= 2) {//随机数为0、1、2时，初始为0号敌机
			enemy[i].hp = ENEMY0HP;//0号敌机血量
			enemy[i].type = 0;//0号敌机
			//0号敌机的宽和高
			enemy[i].enemyWidth = EWIDTH0;
			enemy[i].enemyHeight = EHEIGHT0;
		}
		else if (ranNum <= 5) {//随机数为3、4、5时，初始为1号敌机
			enemy[i].hp = ENEMY1HP;//1号敌机血量
			enemy[i].type = 1;//1号敌机
			//1号敌机的宽和高
			enemy[i].enemyWidth = EWIDTH1;
			enemy[i].enemyHeight = EHEIGHT1;
		}
		else if (ranNum <= 7) {//随机数为6、7时，初始为2号敌机
			enemy[i].hp = ENEMY2HP;//2号敌机血量
			enemy[i].type = 2;//2号敌机
			//2号敌机的宽和高
			enemy[i].enemyWidth = EWIDTH2;
			enemy[i].enemyHeight = EHEIGHT2;
		}
		else if (ranNum <= 9) {//随机数为8、9时，初始为3号敌机
			enemy[i].hp = ENEMY3HP;//3号敌机血量
			enemy[i].type = 3;//3号敌机
			//3号敌机的宽和高
			enemy[i].enemyWidth = EWIDTH3;
			enemy[i].enemyHeight = EHEIGHT3;
		}
	}
}

/***********************
*输入：空
*输出：空
*作用：游戏初始化函数
************************/
void GameInit() {
	//玩家飞机初始位置为游戏窗口底部居中
	myPlane.x = (WIDTH - PLAYERWIDTH) / 2;
	myPlane.y = HEIGHT - PLAYERHEIGHT;
	myPlane.live = true;//存活状态：true
	playerBlood = MYPLANEBLOOD;
	myPlane.hp = playerBlood;
	//初始子弹
	for (int i = 0; i < BULLETNUM; i++)
	{
		bullet[i].live = false;
		bullet[i].x = 0;
		bullet[i].y = 0;
	}
	for (int i = 0; i < ENEMYNUM; i++)
	{
		//初始状态，所有敌机均未存活。随后逐个生成
		enemy[i].live = false;
		enemy[i].enemyDone = false;//初始时所有飞机都没有使玩家减血
	}
	EnemyInit();//初始敌机数据
}

/***********************
*输入：空
*输出：空
*作用：产生单个敌机
************************/
void CreatEnemy() {
	//敌机遍历
	for (int i = 0; i < ENEMYNUM; i++)
	{
		if (!enemy[i].live) {//遍历到一个敌机的存活状态为false，生成该单个敌机
			switch (enemy[i].type) {//根据敌机类型决定血量
			case 0: enemy[i].hp = ENEMY0HP; break;
			case 1: enemy[i].hp = ENEMY1HP; break;
			case 2: enemy[i].hp = ENEMY2HP; break;
			case 3: enemy[i].hp = ENEMY3HP; break;
			}
			enemy[i].live = true;//改为存活
			//生成飞机位置在横轴上随机（范围：[0,WIDTH - enemy[i].enemyWidth]保证显示出完整的飞机）
			enemy[i].x = rand() % (WIDTH - enemy[i].enemyWidth);
			enemy[i].y = 0;//窗口最顶端上生成
			break;//生成单个飞机后跳出循环
		}
	}
}

/***********************
*输入：空
*输出：空
*作用：产生单个子弹
************************/
void CreatBullet() {
	for (int i = 0; i < BULLETNUM; i++)//遍历一梭子弹
	{
		if (!bullet[i].live) {//遍历到一个子弹的存活状态为false，生成该单个子弹
			bullet[i].live = true;//改为存活
			//产生的位置是玩家飞机顶部中间
			bullet[i].x = myPlane.x + PLAYERWIDTH / 2 - BULLETWIDTH / 2;
			bullet[i].y = myPlane.y - BULLETHEIGHT; 
			break;//生成单个子弹后跳出循环
		}
	}
}

/***********************
*输入：空
*输出：空
*作用：绘制游戏图像
************************/
void GameDraw() {
	int bloodX = 0;
	Loading();//加载图片资源
	//贴背景图
	putimage(0, 0, &playingBackground);
	//贴生命值图片
	for (int i = 0; i < playerBlood; i++)
	{
		putimage(bloodX, 0, &playerBloodImg[0], NOTSRCERASE);
		putimage(bloodX, 0, &playerBloodImg[1], SRCINVERT);
		bloodX += BLOODWIDTH+4;
	}
	//贴玩家掩码图+原图
	if (myPlane.hp > 0) {
		putimage(myPlane.x, myPlane.y, &playerImg[0], NOTSRCERASE);
		putimage(myPlane.x, myPlane.y, &playerImg[1], SRCINVERT);
	}
	//贴生成子弹的图片
	for (int i = 0; i < BULLETNUM; i++)
	{
		if (bullet[i].live) {
			putimage(bullet[i].x, bullet[i].y, &bulletImg[0], NOTSRCERASE);
			putimage(bullet[i].x, bullet[i].y, &bulletImg[1], SRCINVERT);
		}
	}
	//贴生成敌机的图片
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
*输入：（int类型）延迟的时间，单位：ms
*输出：（bool类型）时间到->true; 否则->false
*作用：计时器
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
*输入：（int类型）代表玩家飞机移动的速度
*输出：空
*作用：使玩家飞机移动
************************/
void MyPlaneMove(int speed) {
	//GetAsyncKeyState(_In_ int vKey);函数用于检测按键
	//且移动更加流畅，可斜着移动
	if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W')) {//大写W可同时表示W和w
		if(myPlane.y>0)
			myPlane.y -= speed;//上移
	}
	if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S')) {
		if(myPlane.y+ PLAYERHEIGHT<HEIGHT)
			myPlane.y += speed;//下移
	}
	if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A')) {
		if(myPlane.x+ PLAYERWIDTH /2>0)
			myPlane.x -= speed;//左移
	}
	if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D')) {
		if(myPlane.x+ PLAYERWIDTH / 2<WIDTH)
			myPlane.x += speed;//右移
	}
	//空格生成子弹
	//引入一定延迟防止按一下空格产生多个子弹，同时可以控制相邻子弹的密度
	if (GetAsyncKeyState(VK_SPACE) && Timer(150)) 
		CreatBullet();
}

/***********************
*输入：（int类型）代表子弹移动的速度
*输出：空
*作用：使子弹移动
************************/
void BulletMove(int speed) {
	for (int i = 0; i < BULLETNUM; i++)
	{
		if (bullet[i].live) {//存活子弹要自动向上移动
			bullet[i].y -= speed;//上移
			if (bullet[i].y + BULLETHEIGHT < 0)//子弹完全移出窗口后恢复false存活状态，以保持无限子弹
				bullet[i].live = false;
		}
	}
}

/***********************
*输入：（int类型）表示敌机整体移动的速度（因为不同类型敌机移速不同）
*输出：空
*作用：使敌机移动
************************/
void EnemyMove(int speed) {
	for (int i = 0; i < ENEMYNUM; i++)
	{
		if (enemy[i].live) {//敌机产生后要自动向下移动
			//两种移速方案
#if 1
			//现采取的方案：不同类型敌机用不同常数乘speed以区分出速度
			switch (enemy[i].type)
			{//  0-->3  快-->慢  
			case 0:enemy[i].y += 5 * speed; break;
			case 1:enemy[i].y += 4 * speed; break;
			case 2:enemy[i].y += 3 * speed; break;
			case 3:enemy[i].y += 2 * speed; break;
			}
#elif 0
			//方案二：所有敌机速度随机（由于此方案移动不太流畅，未采用）
			enemy[i].y += (rand() % 10 + 1) * speed;
#endif
			//敌机完整地离开窗口后live恢复false以保证不断有敌机产生
			if (enemy[i].y - enemy[i].enemyHeight > HEIGHT)
			{
				enemy[i].live = false;
				enemy[i].enemyDone = false;
			}
		}
	}
}

/***********************
*输入：空
*输出：空
*作用：玩家飞机攻击系统
************************/
void Attack() {
	for (int i = 0; i < ENEMYNUM; i++)//遍历敌机
	{
		if (!enemy[i].live)
			continue;//跳过死亡敌机
		for (int j = 0; j < BULLETNUM; j++)
		{//遍历子弹
			if (!bullet[j].live)
				continue;//跳过死亡的子弹
			//子弹与敌机一旦有重合区域则视为攻击有效（可用EDGE调整有效边缘）
			if ((bullet[j].x + BULLETWIDTH >= enemy[i].x -EDGE && bullet[j].x <= enemy[i].x + enemy[i].enemyWidth + EDGE)
				&& (bullet[j].y >= enemy[j].y -EDGE && bullet[j].y <= enemy[i].y + enemy[i].enemyHeight + EDGE)) {
				bullet[j].live = false;//攻击后子弹死亡
				enemy[i].hp--;//敌机减少一点血量
			}
		}
		if (enemy[i].hp <= 0)//敌机血量<=0后死亡
			enemy[i].live = false;
	}
}

/***********************
*输入：int类型  已经进行游戏的时间
*输出：unsigned int类型  如果游戏结束返回玩家对弹窗的选择，其他情况无意义
*作用：控制玩家什么时候减血或结束游戏
************************/
UINT PlayerBlood(int gameTime) {
	HWND h = GetHWnd();//获取窗口句柄
	for (int i = 0; i < ENEMYNUM; i++)//遍历敌机
	{  //如果某敌机存活并尚未致使玩家掉血
		if (enemy[i].live && !enemy[i].enemyDone) {
			//减血情况一：敌机深入我方内部
			if (enemy[i].y >= HEIGHT)
			{
				myPlane.hp--;
				playerBlood--;
				enemy[i].enemyDone = true;
			}
			//减血情况二：玩家飞机与敌机直接接触
			if ((myPlane.x < enemy[i].x + enemy[i].enemyWidth) && (myPlane.x > enemy[i].x - PLAYERWIDTH)
				&& (myPlane.y < enemy[i].y + enemy[i].enemyHeight) && (myPlane.y + PLAYERHEIGHT > enemy[i].y))
			{
				myPlane.hp--;
				playerBlood--;
				enemy[i].enemyDone = true;
			}
		}
	}
	char chTime[15] = "";//接收转换成字符串类型后的游戏时间
	_itoa_s(gameTime,chTime,15,10);//_itoa_s函数将int类型转换成字符串类型
	//拼接成一个字符串
	char string1[100] = "游戏结束！太厉害了！本局中您已经坚持了";
	char string2[] = "秒！是否再来一局？";
	strcat(string1, chTime);
	strcat(string1, string2);
	if (myPlane.hp <= 0)//血量掉完后跳出游戏结束弹窗
	{
		mciSendStringA("close ./资源/战斗BGM.mp3", 0, 0, 0);
		mciSendStringA("open ./资源/游戏结束BGM.mp3", 0, 0, 0);
		mciSendStringA("play ./资源/游戏结束BGM.mp3", 0, 0, 0);
		UINT choice = MessageBoxA(h, string1, "游戏结束", MB_YESNO);
		return choice;
	}
	return 1;//其他路径中返回一个不影响YES/NO的值
}

/***********************
*输入：int类型水印横坐标、int类型水印纵坐标、int类型水印字体尺寸、水印颜色
*输出：空
*作用：在任意位置生成任意大小、颜色的文本充当水印
************************/
void WaterMark(int textX,int textY, int textSize,COLORREF textColor) {
	setbkmode(TRANSPARENT);//背景透明使文本背景不再是黑色
	settextstyle(textSize, 0, "隶书");//字体格式
	settextcolor(textColor);//字体颜色
	outtextxy(textX, textY, "By 曹谋仁");//显示文本
}

/***********************
*输入：空
*输出：空
*作用：游戏菜单界面
************************/
void Menu() {
	ButtonInit();
	mciSendStringA("open ./资源/菜单BGM.mp3", 0, 0, 0);//打开菜单界面BGM
	loadimage(&menuBackground, "./资源/menuBackground.png");
	putimage(0, 0, &menuBackground);
	DrawMenuButtons();
	mciSendStringA("play ./资源/菜单BGM.mp3 repeat", 0, 0, 0);//播放音乐
	WaterMark(2, 763, 25, RGB(0, 0, 0));//在左下角显示水印
}

/***********************
*输入：空
*输出：空
*作用：玩游戏中的全过程
************************/
void PlayingGame() {
	mciSendStringA("close ./资源/菜单BGM.mp3", 0, 0, 0);//关闭菜单界面BGM
	mciSendStringA("open ./资源/战斗BGM.mp3", 0, 0, 0);//打开游戏界面BGM
L0:GameInit();//初始化游戏
	mciSendStringA("play ./资源/战斗BGM.mp3 repeat", 0, 0, 0);//播放游戏界面BGM
	BeginBatchDraw();//开启批量绘制，使循环中图像显示流畅
	int playTime = 0;//用于记录已经进行游戏的时间，同时也是得分
	UINT endChoice;//记录结束窗口中按钮的选择
	DWORD startTime=clock(), endTime=clock();
	while (1) {
		//经过开局延迟时间后产生敌机，产生两个敌机之间间隔0.65秒
		if (Timer(650) && unsigned(endTime - startTime) > STARTDELAY) {
			CreatEnemy();
		}
		GameDraw();//绘图
		FlushBatchDraw();//刷新
		MyPlaneMove(22);//玩家飞机移动
		endChoice = PlayerBlood(playTime);
		if (endChoice == IDYES)
		{
			mciSendStringA("close ./资源/游戏结束BGM.mp3", 0, 0, 0);
			goto L0;//再来一局后重新开始
		}
		if (endChoice == IDNO)
		{
			mciSendStringA("close ./资源/战斗BGM.mp3", 0, 0, 0);//关闭游戏界面BGM
			mciSendStringA("close ./资源/游戏结束BGM.mp3", 0, 0, 0);
			exit(1);//结束游戏终止程序
		}
		BulletMove(12);//子弹移动
		EnemyMove(2);//敌机移动
		Attack();//攻击
		endTime = clock();
		playTime = ((int)endTime-(int)startTime) / 1000;
	}
	EndBatchDraw();//结束批量绘制
}

/***********************
*输入：空
*输出：空
*作用：产生一个有关规则介绍的弹窗
************************/
void RulesWindow() {
	HWND h = GetHWnd();//获取窗口句柄
	char ruleText[RULEMAX];
	FILE* fp; int k = 0;
	fopen_s(&fp,"./资源/Rules.txt","r");//打开存放规则文本的txt文件
	if (fp == NULL)
		exit(1);//打不开文件时直接停止运行
	else {
		if(fgets(ruleText,RULEMAX,fp)!=NULL)
			MessageBoxA(h, ruleText, "玩法简介", MB_OK);
	} 
	fclose(fp);
}

//主函数
int main() {
	initgraph(WIDTH, HEIGHT);//绘制窗口
	Menu();
	BeginBatchDraw();//开启批量绘制，使循环中图像显示流畅
	while (1) {
		FlushBatchDraw();//刷新
		if (peekmessage(&mouse, EM_MOUSE)) {//获取鼠标信息
			//菜单界面中，当鼠标指针移动到按钮上时会变色以反馈用户
			MouseOnMenuButtons(RGB(56, 199, 170), RGB(216, 120, 147), RGB(216, 120, 147));
			if (mouse.message == WM_LBUTTONDOWN) {//按下按钮时
				//点击"Go！"按钮
				if (sqrt(pow((double)mouse.x - buttons[0].x, 2.0) + pow((double)mouse.y - buttons[0].y, 2.0)) <= buttons[0].r)
					PlayingGame();
				//点击"离开"按钮
				if (sqrt(pow((double)mouse.x - buttons[1].x, 2.0) + pow((double)mouse.y - buttons[1].y, 2.0)) <= buttons[1].r)
				{
					mciSendStringA("close ./资源/菜单BGM.mp3", 0, 0, 0);//关闭菜单界面BGM
					return 0;
				}
				//点击"玩法"按钮
				if (sqrt(pow((double)mouse.x - buttons[2].x, 2.0) + pow((double)mouse.y - buttons[2].y, 2.0)) <= buttons[2].r)
					RulesWindow();
			}
		}
	}
	EndBatchDraw();//结束批量绘制
	return 0;
}
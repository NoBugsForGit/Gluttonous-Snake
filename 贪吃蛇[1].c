//蛇身模块有恶性bug
/*
大版本更新计划：
增加"围死障碍物"功能
修复某些情况下刚开始游戏蛇就死亡√
正式实现🈳和🚨的功能
不再能够逆方向移动√
*/
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#define FP1 "./text/score.txt"
#define FP2 "./text/name.txt"
#define FP3 "./text/CHECK.txt"
#define NOW time(NULL)
#define CL clrscr()
#define qq printf("%d\n",snake.longth);
#define CHresult entity[coor[0]/100][coor[0]%100]
#define IMAX 24
#define JMAX 30
//PMAX参数上限
#define PMAX 10
//ID最大值
#define IDMAX 15
#define IJM IMAX*JMAX
#define PRPA nothing
//设定nothing无参数器，设定PRINTPA出现参数显示器
FILE*fp;
int ca=0;//控制蛇长方向
int fre;//控制实体生成频率
int checkwhile=0,sp=0;
int checkelseif=0;
int allentitynum=0;//除蛇和墙外的实体总数
void EC()
{
	checkwhile++;
	fp=fopen(FP3,"w");
	fprintf(fp,"%d",checkwhile);
	fclose(fp);
	}//检查循环次数
int direction(int dir)
//方向反转处理
{
	int return_num;
	switch (dir)
	{
		case 2:return_num=8;break;
		case 4:return_num=6;break;
		case 6:return_num=4;break;
		case 8:return_num=2;break;
		default:break;
		}
	return return_num;	
	}	
int speed;
int rom,ti;
int check_num;
int coor[IJM];
void PRINTPA();
void nothing(){}
typedef enum {false=0,true=1}bool;
typedef struct//定义实体
{
	int id;//实体ID
	/*实体ID列表
	0:空ID
	1:蛇头💚
	2:蛇身🈯
	3:墙体🅾️
	4:障碍物⚠️
	5:一分1⃣
	6:二分2⃣
	7:五分5⃣
	8:减长🈹
	9:加命❤️
	10:警报🚨
	11:列车🚆
	12:清空🈳
	13:药品💊
	14:炸弹💣
	*/
	long int other[PMAX];//实体其他参数
	int eni;
	int enj;//eni,enj实体坐标
	}Entity;	
Entity ia;//交互用的暂存变量	
Entity AUTO;//运动用的暂存变量
Entity nullentity;//空实体
struct Snake
{
	Entity head;
	Entity tail;//尾巴
	Entity body[IJM];
	//other[0]:方向
	/*
	0初始化
	2上 
	4左 
	6下
	8右
	*/
	int length;//长度
	int life;//命数，0死
	int car;//🚆
	int medicine;//💊
    int warn;//🚨
	}snake;
bool warning;	
int ennum[IDMAX];	
struct ennoplace//禁止生成实体的区块
{
	int ei;
	int ej;
	bool a;
	}EP[IJM*2],he[9];int epnum=0;	
Entity entity[IMAX][JMAX];//i,j
int hc;//最高分
int nc;//现在分数
bool lonc;//长度判定分数
char hname[50];
char nname[50];
int key;//各种指令码
long int opentime,starttime,nowtime;
int openperiod,gameperiod;
int i,j,ij;	


void msleep(long int sleeptime)
{
	usleep(sleeptime*1000);
	}
void FWRITE()
{	
	fp=fopen(FP1,"w");
	fprintf(fp,"%d",hc);
	fclose(fp);
	fp=fopen(FP2,"w");
	fprintf(fp,"%s",hname);
	fclose(fp);
	}
void FREAD()
{
	fp=fopen(FP1,"r");	
	if (fp==NULL)
	{
    	hc=0;
        strcpy(hname,"无");
        fclose(fp);
    	exit(0);
    	}
    fscanf(fp,"%d",&hc);	
	fclose(fp);
	fp=fopen(FP2,"r");
	fread(hname,50,1,fp);
	fclose(fp);
	}
int entityinit()
{
	nullentity.id=0;
	for (i=0;i<PMAX;i++)
	{
		nullentity.other[i]=0;
		}
	nullentity.eni=25;
	nullentity.enj=31;	
	for (i=0;i<IMAX;i++)
	{
		for (j=0;j<JMAX;j++)
		{
			entity[i][j].id=0;
			entity[i][j].eni=i;
			entity[i][j].enj=j;
			}
		}
	}	
int welcome()//返回0退出,否则正常
{WELCOME:
	printf("欢迎！输入1获取帮助，"
	       "\n输入2获取更新日志与版本信息\n"
	       "输入0退出"
	       "\n输入其它开始游戏\n");
	key=getch()-48;
	switch (key)
	{
		case 0:return 0;
		case 1:Help();goto WELCOME;
		case 2:updatef();goto WELCOME;
		default:return 1;
		}       
	}
int updatef()//输出更新日志
{
	CL;
	printf("-----------公测版本-----------\n"
	       "————感谢您参与本游戏测试!————\n"
	       "版本号:beta 0.5.0\n"
	       "本次更新内容:\n"
	       "移除了Herobrine\n"
	       "\n");
    return 0;	
    }	
int Help()
{
	CL;
	printf("💚蛇头\n"
	       "🈯:蛇\n"
	       "🅾️:墙壁\n"
	       "⚠️:️障碍物\n"
	       "1⃣:1分食物\n"
	       "2⃣:2分食物\n"
	       "5⃣:5分食物\n"
	       "🈹:减去2长度(暂未实现)\n"
	       "❤️:加一条命并减1分\n"
	       "🚨:5秒内不生成"
	       "⚠️、5⃣、❤️和🈳\n"
	       "🚆:可撞破一个障碍物\n"
	       "🈳:清空分数和所有"
	       "道具,该物品生成"
	       "3秒后自动消失\n"
	       "💊:可以咬自己一次(暂未实现)\n"
	       "💣:清除所有道具和buff并"
	       "使长度变为3\n"
	       "每得2分+1长度\n"
	       "每减去2分-1长度\n\n");
	}
int gameover()
{
if (snake.life!=0)
{return 2;}
else
{
	if (nc>hc)
	{
		printf("恭喜打破记录！"
		       "请输入您的名字！\n");
		scanf("%s",&nname);
		hc=nc;
		strcpy(hname,nname);
		FWRITE();       
		}
	else 
	{
		PRPA();
		printf("遗憾！\n");		
		}
	printf("0:退出游戏\n"
	       "其它:继续游戏\n");
	key=getch()-48;}
	if (key==0)
	{
		return 0;
		} 
	else 
	{
		return 1;
		}	      		
	}			

int enwall()	
{
	for (i=0;i<JMAX;i++)
	{
		entity[0][i].id=3;
	    entity[IMAX-1][i].id=3;	    	
		}
	for (i=0;i<IMAX;i++)
	{
		entity[i][0].id=3;
		entity[i][JMAX-1].id=3;
		}	
	ennum[3]=(IMAX+JMAX)*2-4;	
	}		
int entools()
{
	int m,n,k,l,num,idcard;
	//k:控制循环
	//l:判断本次实体生成数量
	num=abs(rom%10);
	switch (num)
	{
		case 0:l=1;break;
		case 1:l=1;break;
		case 2:l=1;break;
		case 3:l=1;break;
		case 4:l=2;break;
		case 5:l=2;break;
		case 6:l=4;break;
		case 7:l=3;break;
		case 8:l=2;break;
		case 9:l=2;break;
		}
		if (l==0){return 0;}
		for (k=0;k<l;k++){
		num=abs(rom%100);
switch (num)
{
		    case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			idcard=4;
			break;
			case 16:			
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
			case 22:
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
			case 28:
			case 29:
			case 30:
			idcard=6;
			break;
			case 31:
			case 32:
			case 33:
			idcard=7;
			break;
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
			idcard=8;
			break;
			case 39:
			case 40:
			idcard=9;
			break;
			case 41:
			idcard=10;
			break;
			case 42:
			case 43:
			case 44:
			case 45:
			idcard=11;
			break;
			case 46:
			idcard=12;
			break;
			case 47:
			case 48:
			case 49:
			idcard=13;
			break;
			case 50:
			idcard=14;
			break;
			default:
			idcard=5;
			break;
	}
ENTITYCREATE:			
		m=abs(rom%IMAX);
		n=abs(rom%JMAX);
		for (i=0;i<IJM*2+1;i++)
	{
		if ((EP[i].ei==m)&&
		(EP[i].ej==n))
		{ECHECK:
			return 0;
			}}
		for (i=0;i<9;i++)
		{
		    	if ((he[i].ei==m)&&
	         	(he[i].ej==n))
	    	{
			goto ECHECK;
		   	}
			}				
NUMCHECK:
if (warning)
{
	switch (idcard)
	{
		case 4:case 7:case 9:case 12:
		idcard=((abs(rom)+idcard)
		%(IDMAX-1));
		goto NUMCHECK;
		default:break;
		}
	}
	switch (idcard)
	{
		case 4:
		case 5:
		if (ennum[idcard]>8)
		{
			idcard+=2;
			goto NUMCHECK;
			}
		break;
		case 6:
		if (ennum[idcard]>4)
		{
			idcard+=2;
			goto NUMCHECK;
			}			
		break;
		case 7:
		if (ennum[idcard]>2)
		{
			idcard+=2;
			goto NUMCHECK;
			}	
		break;
		case 8:
		case 11:
		case 13:
		if (ennum[idcard]>2)
		{
			idcard-=2;
			goto NUMCHECK;
			}
		break;
		case 9:
		if (ennum[idcard]>1)
		{
			idcard+=2;
			goto NUMCHECK;
			}
		break;
		case 10:
		case 14:
		if (ennum[idcard]>1)
		{
			idcard-=2;
			goto NUMCHECK;
			}
		break;
		case 12:
		if (ennum[idcard]>1)
		{
			idcard-=2;
			goto NUMCHECK;
			}
	   entity[m][n].other[1]=3000;			
		break;
		}
		ennum[idcard]++;
		allentitynum++;
		entity[m][n].id=idcard;
		entity[m][n].eni=m;
		entity[m][n].enj=n;
		EP[epnum].ei=m;
		EP[epnum].ej=n;
		epnum++;		
		}
	return 0;	
	}
	
void ennuminit()
{
	for (i=0;i<IDMAX;i++)
	{
		ennum[i]=0;
		}
	}	
int Snakeinit()
{
	int k=0,m,n;
SNAKEINIT:	
	m=rom%IMAX;n=rom%JMAX;
	for (i=0;i<IJM*2;i++)
	{
		if ((EP[i].ei==m)&&
		(EP[i].ej==n))
		{
			rom++;
			goto SNAKEINIT;
			}
		}
	entity[m][n].id=1;
	entity[m][n].eni=m;
	entity[m][n].enj=n;
	ennum[1]=1;
	snake.head.id=1;
	snake.head.eni=m;
	snake.head.enj=n;
	for (i=0,j=-2;i<9;i++)
	{
		if (i%3==0){j++;}
		he[i].ei=m+j;
		he[i].ej=n-1+(i%3);
		}
	snake.length=1;
	snake.life=1;
	snake.car=0;
	snake.medicine=0;
	snake.warn=0;
	snake.head.other[0]=0;
	EP[epnum].ei=m;
	EP[epnum].ej=n;
	epnum++;	
	ca=0;
	snake.body[0]=snake.head;
	for (i=1;i<IJM;i++)
	{
		snake.body[i].id=2;
		snake.body[i].eni=-1;
		snake.body[i].enj=-1;
		}
	}		
int Enplaceinit()
{
	for (i=0;i<JMAX;i++)
	{
		EP[epnum].ei=0;
		EP[epnum].ej=i;
		epnum++;	    	
		}
	for (i=0;i<JMAX;i++)
	{
		EP[epnum].ei=23;
		EP[epnum].ej=i;
		epnum++;	    	
		}
	for (i=0;i<IMAX;i++)
	{
		EP[epnum].ei=i;
		EP[epnum].ej=0;
        epnum++;
		}		
	for (i=0;i<IMAX;i++)
	{
		EP[epnum].ei=i;
		EP[epnum].ej=29;
        epnum++;
		}			
	}
//蛇移动,包含掉实体交互,蛇头附近的禁生成区(不含初始化)
int SNAKEAUTO()
{
    int au,IN,m,n;
    Entity temauto[IJM];
    Entity th;
    snake.body[0]=snake.head;
    goto SAO;
AUTOSWITCH:
    au=abs(rom%9);
    if (((au%2)!=0)||(au==0))
    {rom++;goto AUTOSWITCH;}
    snake.head.other[0]=au;
SAO:    
	switch (snake.head.other[0])
	{
		case 0:IN=0;
		break;
		case 2:
        ia=entity[snake.head.eni-1]
		[snake.head.enj];
		IN=Interactive();
		if ((IN==1)&&(snake.life!=0))
		{goto SAO;}
		entity[snake.head.eni-1]
		[snake.head.enj].id=1;
		entity[snake.head.eni-1]
		[snake.head.enj].other[0]=2;
		entity[snake.head.eni]
		[snake.head.enj].id=0;
		snake.head=entity
		[snake.head.eni-1]
		[snake.head.enj];
		break;
		case 4:
		ia=entity[snake.head.eni]
		[snake.head.enj-1];
		IN=Interactive();
		if ((IN==1)&&(snake.life!=0))
		{goto SAO;}
		entity[snake.head.eni]
		[snake.head.enj-1].id=1;
		entity[snake.head.eni]
		[snake.head.enj-1].other[0]=4;
		entity[snake.head.eni]
		[snake.head.enj].id=0;
		snake.head=entity
		[snake.head.eni]
		[snake.head.enj-1];
		
		break;
		case 6:
		ia=entity[snake.head.eni]
		[snake.head.enj+1];
		IN=Interactive();
		if ((IN==1)&&(snake.life!=0))
		{goto SAO;}
		entity[snake.head.eni]
		[snake.head.enj+1].id=1;
		entity[snake.head.eni]
		[snake.head.enj+1].other[0]=6;
		entity[snake.head.eni]
		[snake.head.enj].id=0;
		snake.head=entity
		[snake.head.eni]
		[snake.head.enj+1];
		
		break;
		case 8:
		ia=entity[snake.head.eni+1]
		[snake.head.enj];
		IN=Interactive();
		if ((IN==1)&&(snake.life!=0))
		{goto SAO;}
		entity[snake.head.eni+1]
		[snake.head.enj].id=1;
		entity[snake.head.eni+1]
		[snake.head.enj].other[0]=8;
		entity[snake.head.eni]
		[snake.head.enj].id=0;
		snake.head=entity
		[snake.head.eni+1]
		[snake.head.enj];
		
		break;
		default:
		goto AUTOSWITCH;
		}//前面检验完毕
		
		
		for (j=0;j<snake.length;j++)
		{temauto[j]=snake.body[j];}
		temauto[0].id=2;
		if (snake.length==1)
		{snake.tail=snake.head;}
		checkelseif=0;
		for (i=1;i<snake.length;i++)
		{
			//重写区段开始
			snake.body[i]=temauto[i-1];
			entity[snake.body[i].eni]
			[snake.body[i].enj]=snake.body[i];
			entity[temauto[i].eni]
			[temauto[i].enj].id=0;			
		    //重写区段结束	
		    	
			}
         snake.body[0]=snake.head;
         snake.tail=snake.body[snake.length-1];

				
								
												
																
//调用蛇增长的函数
//已确认与蛇体bug无关			
	if (IN>=2)
		{Snakelong(IN-2);}
		else if (IN<0)
		{Snakelong(IN);}	
		
//设置禁止实体生成的区域			
	for (i=0;i<snake.length;i++)
	{
		EP[IJM*2-i-1].ei=snake.body[i].eni;
		EP[IJM*2-i-1].ej=snake.body[i].enj;
		}		
		
		
		
	//重置蛇头禁生成区			
	m=snake.head.eni;
	n=snake.head.enj;
		for (i=0,j=-2;i<9;i++)
	{
		if (i%3==0){j++;}
		he[i].ei=m+j;
		he[i].ej=n-1+(i%3);
		}
	return 1;
	}														
int SNAKECON()
{
	int con; 
	if (kbhit())
	{
		con=getch();
		con-=48;
		switch (con)
		{
			case 0:
			return 0;
			case 5:case 55:case 555:
			while (1)
			{ 
			    clrscr();
				msleep(100);
				printf("暂停中\n"
				       "按任意键解除暂停\n");
				PRPA();COUT();
				if (kbhit()){break;}
				}
			break;
			case 2:case 22:case 222:
			if ((snake.head.other[0]==8)
			&&(snake.length!=1))
			{break;}
			snake.head.other[0]=2;
			break;
			case 4:case 44:case 444:
			if ((snake.head.other[0]==6)
			&&(snake.length!=1))
			{break;}
			snake.head.other[0]=4;
			break;
			case 6:case 66:case 666:
			if ((snake.head.other[0]==4)
			&&(snake.length!=1))
			{break;}
			snake.head.other[0]=6;
			break;
			case 8:case 88:case 888:
			if ((snake.head.other[0]==2)
			&&(snake.length!=1))
			{break;}
			snake.head.other[0]=8;
			break;
			default:break;			
			}		
		return 1;
		}
	else
	{
		return 2;
		}	
	}
int Interactive()//交互函数
//在snakeauto中间嵌入
//如果强制转换了方向，返回1
{
	ennum[ia.id]--;
	allentitynum--;
	switch (ia.id)
	{
		case 0:return 0;
	    case 2:
	    if (snake.medicine==0)
	    {goto CASE_LIFE;}
	    else 
	    {
	    	return -1;
	    	}
	    case 3:
CASE_LIFE:	    
	    snake.life--;
	    if (snake.head.other[0]==2)
	    {
	    	if (ia.enj<3)
	    	{snake.head.other[0]=6;}
	    	else if (ia.enj>JMAX-3)
	    	{snake.head.other[0]=4;}
	    	else
	    	{
	    		if (abs(rom%2)==0)
	    		{snake.head.other[0]=4;}
	    		else
	    		{snake.head.other[0]=6;}
	    		}
	    	}
	    	
	    else if (snake.head.other[0]==8)
	   {
	   	if (ia.enj<3)
	    	{snake.head.other[0]=6;}
	    	else if (ia.enj>JMAX-3)
	    	{snake.head.other[0]=4;}
	    	else
	    	{
	    		if (abs(rom%2)==0)
	    		{snake.head.other[0]=4;}
	    		else
	    		{snake.head.other[0]=6;}
	    		}
	   	}
	   	
	    else if (snake.head.other[0]==4)
	    {
	    	if (ia.eni<3)
	    	{snake.head.other[0]=8;}
	    	else if (ia.eni>IMAX-3)
	    	{snake.head.other[0]=2;}
	    	else
	    	{
	    		if (abs(rom%2)==0)
	    		{snake.head.other[0]=2;}
	    		else
	    		{snake.head.other[0]=8;}
	    		}
	    	}
	    
	    else if (snake.head.other[0]==6)
	    {
	    	if (ia.eni<3)
	    	{snake.head.other[0]=8;}
	    	else if (ia.eni>IMAX-3)
	    	{snake.head.other[0]=2;}
	    	else
	    	{
	    		if (abs(rom%2)==0)
	    		{snake.head.other[0]=2;}
	    		else
	    		{snake.head.other[0]=8;}
	    		}
	    	}
	    
	    else {if (snake.life!=0)
	    {snake.life--;}break;}
	    return 1;
	    case 4:
        if (snake.car==0)
        {goto CASE_LIFE;}
		else
		{
			snake.car--;
			nc+=5;
			break;
			}    
	    case 5:
	    nc++;
	    if (lonc)
	    {lonc=false;return 3;}
	    else
	    {lonc=true;}
	    break;
	    case 6:
	    nc+=2;
	    return 3;
	    case 7:
	    nc+=5;
	    if (lonc)
	    {lonc=false;return 5;}
	    else
	    {lonc=true;return 4;}
	    break;
	    case 8:
	    return -2;
	    case 9:
	    snake.life++;
	    nc--;
	    if (lonc)
	    {lonc=false;}
	    else
	    {lonc=true;return -1;}
	    break;
	    case 10:
	    snake.warn=5000;
	    break;
	    case 11:
	    snake.car++;
	    break;
	    case 12:
	    nc=0;
	    for (i=0;i<IMAX;i++)
	    {
	    	for (j=0;j<JMAX;j++)
	    	{
	    		entity[i][j].id=0;
	    		for (ij=0;ij<10;ij++)
	    		{entity[i][j].other[ij]=0;}
	    		}
	    	}
	    ennuminit();
        Enplaceinit();
        Snakeinit();
    	enwall();
	    break;
	    case 13:
	    snake.medicine++;
	    break;
	    case 14:
	    for (i=0;i<IMAX;i++)
	    {
	    	for (j=0;j<JMAX;j++)
	    	{
	    		entity[i][j].id=0;
	    		for (ij=0;ij<10;ij++)
	    		{entity[i][j].other[ij]=0;}
	    		}
	    	}
	    ennuminit();
        Enplaceinit();
        Snakeinit();
    	enwall();	
	    break;
    	default:return 0;
	    }
	    
	return 0;    
	}
int Snakelong(int growth)
//0减长度，非0表示加长度
//蛇长与蛇身处理
{
	if (snake.length==1)
	{
		snake.tail=snake.head;
		snake.tail.other[1]=
		direction(snake.head.other[0]);	
		}
	if (growth==0){return 0;}	
	if (growth<0)
	{
		
		return -1;
		}
	for (i=0;i<growth;i++)
	{
		switch (snake.tail.other[1])
		{
			case 2:
			entity[snake.tail.eni-1]
			[snake.tail.enj].id=2;
			entity[snake.tail.eni-1]
			[snake.tail.enj].other[0]=8;
			entity[snake.tail.eni-1]
			[snake.tail.enj].other[1]=2;
			entity[snake.tail.eni-1]
			[snake.tail.enj].other[2]=
			snake.length;
			snake.tail=entity
			[snake.tail.eni-1][snake.tail.enj];
			break;
			
			case 4:
			entity[snake.tail.eni]
			[snake.tail.enj-1].id=2;
			entity[snake.tail.eni]
			[snake.tail.enj-1].other[0]=6;
			entity[snake.tail.eni]
			[snake.tail.enj-1].other[1]=4;
			entity[snake.tail.eni]
			[snake.tail.enj-1].other[2]=
			snake.length;
			snake.tail=entity
			[snake.tail.eni][snake.tail.enj-1];
			break;
							
			case 6:
			entity[snake.tail.eni]
			[snake.tail.enj+1].id=2;
			entity[snake.tail.eni]
			[snake.tail.enj+1].other[0]=4;
			entity[snake.tail.eni]
			[snake.tail.enj+1].other[1]=6;
			entity[snake.tail.eni]
			[snake.tail.enj+1].other[2]=
			snake.length;
			snake.tail=entity
			[snake.tail.eni]
			[snake.tail.enj+1];
			break;
			
			case 8:
			entity[snake.tail.eni+1]
			[snake.tail.enj].id=2;
			entity[snake.tail.eni+1]
			[snake.tail.enj].other[0]=2;
			entity[snake.tail.eni+1]
			[snake.tail.enj].other[1]=8;
			entity[snake.tail.eni+1]
			[snake.tail.enj].other[2]=
			snake.length;
			snake.tail=entity
			[snake.tail.eni+1]
			[snake.tail.enj];
			break;
			
			default:break;
			}
		snake.body[snake.length]=snake.tail;		
		snake.length++;	
		ennum[2]++;
		}
	  						
	}	
int Special()//特殊实体和功能处理
//纳入ENTITY函数
{
	int check;
	Entity temen;
	check=CHECK(12);
	if (check>0)
	{
		CHresult.other[1]-=speed;
		if (CHresult.other[1]<=0)
		{
			CHresult.id=0;
			CHresult.other[1]=0;
			allentitynum--;
			}
		}
		
	//以下是warn函数	
	if (snake.warn>0)
	{
		snake.warn-=speed;
		warning=true;
		}
	else
	{
		snake.warn=0;
		warning=false;
		}	
	return 0;	
	}			
int CHECK(int ID)
{
	check_num=0;
	for (i=0;i<IMAX;i++)
	{
		for (j=0;j<JMAX;j++)
		{
			if (entity[i][j].id==ID)
			{
				coor[check_num]=i*100+j;
				check_num++;
				}
			}
		}
	return check_num;
	}
				
int ENTITY()//实体函数归纳
{
	if (((rom%fre)==0)&&(allentitynum<31))
	{entools();}
	Special();
	}	
	
																							
int COUT()//显示
{
	int m,n;
	n=0;
	for (i=0;i<IMAX;i++)
	{
		for (j=0;j<JMAX;j++)
		{
			switch (entity[i][j].id){
			case 1:
			printf("💚");
			break;
			case 2:
			printf("🈯");
			break;
			case 3:
			printf("🅾️");
			break;
			case 4:
			printf("⚠️");
			break;
			case 5:
			printf("1⃣");
			break;
			case 6:
			printf("2⃣");
			break;
			case 7:
			printf("5⃣");
			break;
			case 8:
			printf("🈹");
			break;
			case 9:
			printf("❤️");
			break;
			case 10:
			printf("🚨");
			break;
			case 11:
			printf("🚆");
			break;
			case 12:
			printf("🈳");
			break;
			case 13:
			printf("💊");
			break;
			case 14:
			printf("💣");
			break;
			default:
			printf("  ");
			break;}
			}
		printf("\n");	
		}
		CHECK(1);
	printf("⬆️2\t⬅️4\t➡️6\t"
	       "⬇️8\t⏸️5\t⏹0\n");	
	printf("当前得分:%d\t"
	      "最高分记录:%d\t"
	      "游戏时间:%ds\n"
	       ,nc,hc,gameperiod);
	printf("❤️×%d\t🚆×%d\t"
	       "💊×%d\t去速度:%d"
	       "\t蛇头方向:%d\n",snake.life,
	       snake.car,snake.medicine,sp,
	       snake.head.other[0]);
    printf("纪录保持者:%s\t\t",hname);   
    printf("蛇头坐标:%d,%d\n"  
    ,snake.head.eni,snake.head.enj);    
	m=snake.warn;
    if ((m>0)&&(m<=5000))
    {
    	printf("🚨:%ds\t",
    	snake.warn);
    	}
    m=CHECK(12);
    if (m>0)
    {
    	printf("🈳:%ds\t",
    	CHresult.other[1]);
    	n=1;
    	}		
	}
void PRINTPA()//参数显示器
{
	printf("\n蛇长:%d\t\t"
	       "种子:%d\n"
	       "ti:%d\t\t"
	       "rom:%d\t%d\n"
	       ,snake.length,NOW,ti,rom,
	       checkelseif);
	for (i=0;i<15;i++)
	{
		printf("%d==%d\t",i,ennum[i]);
		if ((i%5==0)&&(i!=0))
		{printf("\n");}
		}       
	printf("\n禁");
	for (i=0;i<9;i++)
	{
		printf("%d==%d,,%d\t",i,he[i].ei,
		he[i].ej);
		if ((i%3==0)&&(i!=0))
		{printf("\n");}
		}
	printf("禁数::%d",epnum);	   
	printf("头%d\t坐标：%d--%d\tid=%d\n"
	,CHECK(1),snake.head.eni,
	snake.head.enj,
	entity[snake.head.eni][snake.head.enj].id);
	printf("尾巴  坐标%d--%d\tID%d\n",
	snake.tail.eni,snake.tail.enj
	,snake.tail.id);
	printf("%d\n",ennum[2]);
	}
int Speed()
{
	printf("请选择速度\n"
	       "1-6档，档位越高越快"
	       "\n输入65535调为超快"
	       "\t-1为超慢\n"
	       "输入完毕后回车\n");
	/*sp=getch()-48;
	 (int)(sp);*/
	scanf("%d",&sp);
	switch (sp)
	{
		case 1:speed=1500;return 3;
		case 2:speed=1000;return 3;
		case 3:speed=750;return 4;
		case 4:speed=500;return 4;
		case 5:speed=300;return 3;
		case 6:speed=200;return 3;
		case -1:speed=2000;return 3;
		case 65535:speed=100;return 6;
		default:fre=Speed();return fre;
		}
	}							
		
int main()
{
	int gameover_num,wm;
	int c;	
	entityinit();
	FREAD();
	opentime=NOW;
START:
    entityinit();
    wm=welcome();
    if (wm==0)
    {goto END;}
    fre=Speed();
    CL;
    starttime=NOW;
    srand(NOW);
    rom=rand();
    ennuminit();
    Enplaceinit();
    Snakeinit();
	enwall();
	ti=rand();	
while (1)
{EC();	
    srand(NOW+ti);
    rom=rand()+ti++;
    ti*=14;
    nowtime=NOW;
	openperiod=nowtime-opentime;
	gameperiod=nowtime-starttime;
    ENTITY();
    c=SNAKECON();
    if (c==0){snake.life=0;}
    SNAKEAUTO();
 /*printf("%d\t%d\n",snake.head.eni,
    snake.head.enj);*/
    //printf("%d\n",snake.length);
    PRPA();
    COUT();
    CL;
	gameover_num=gameover();
	if (gameover_num==0)
	{END:
		clrscr();
		printf("谢谢使用，再见！\n");
		return 0;
		}
	else if (gameover_num==1)
	{
		goto START;
		}
	else
	{
		msleep(speed);
		}	
	}
	return 0;
	}				
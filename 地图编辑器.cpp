/*=========================================
--- 项目名称: 冒险之路 之 地图编辑器 ------
--- 当前项目版本: 0.2 ---------------------
--- 项目环境: windows C++11 ---------------
--- 编译软件: Dev-C++ ---------------------
-------------------------------------------
--- 项目作者: 平凡0_0 ---------------------
--- 哔哩哔哩: https://b23.tv/mGuvqJi ------
--- 项目初创时间: 2026/2/13 ---------------
--- 项目更新时间: 2026/2/19 ---------------
--- 项目类型: 开源 ------------------------
=========================================*/


/*===============声明=========================================================================================
--------------------------------------------------------------------------------------------------------------
--- 本项目由 [平凡0_0] 原创制作 版权所有 ---------------------------------------------https://b23.tv/mGuvqJi--
-------------------------------------------------------------------------------------------------------^^^----
--- 任何个人或机构"可以"对其 复制 修改 用于商业盈利或通过信息网络进行传播 但使用时需完整注明作者及出处链接 ---
--- 此项目为开源项目 任何组织和个人不可将其占为己有 ----------------------------------------------------------
--- 此项目主要用于代码的交流与学习 --------------------------------------------------------------------------- 
--- 此项目为 "冒险之路" 的辅助项目 --------------------------------------------------------------------------- 
--------------------------------------------------------------------------------------------------------------
============================================================================================================*/

//部分代码 来自 主项目[冒险之路] 

/*------ 0.2 概述 --------------------------------------------------------
体验层面
	1.新增: 对话编辑功能 但未完工 
	2.导出的地图可以选择 是否 容易阅读 
代码层面
	1.地图数据结构改变
	2.代码长度增加     <---废话 
	3.添加部分注释 
其他层面
	1.地图编辑器(未完工) 
------------------------------------------------------------------------*/





#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<ctime>
#include<conio.h>
#include<windows.h>


#pragma warning(disable:4996)


int BoolMapDataEasyRead = 1;//导出的地图 是否 容易阅读 

int cz = 0;

std::wstring DataWayParent = L"MapData";//地图路径 "MapData" 
std::string DebugWay = "Debug.txt";

const int MapMaxSize = 55;//定义地图内存的大小 


struct SP{//StructPlayer 的缩写   玩家信息  
	int myx,myy;//-----------玩家位置 
};
SP Player;

struct SM{//StructMap 的缩写 地图信息 
	int maxx,maxy;//-------地图大小 
	std::string wjname;//--文件名字 
	std::string name;//----地图名字 
	
	int Data[MapMaxSize][MapMaxSize];
//	int SJData[20][40];
	struct SJData{//事件数据
		int SJNum;//特殊事件数量 
		int CSNum;//传送事件数量 
		int DHNum;//对话事件数量 
		struct Data{//=====事件参数 
			int X,Y;//触发位置 
			int Type;//事件类型 
			int Hand;//事件序号 
		}Data[10];
		struct CSData{//=====传送数据 
			int X,Y;//传送目标位置
			std::string MapName;//目标名字
		}CSData[30];
		struct DHData{//=====对话数据 
			std::string ObjectName;//对话者名称 
			int DHNum;//对话数量 
			struct DH{//对话 
				std::string Text;//对话内容
				int HDNum;//回答数量 
				struct HD{//回答 
					std::string Text;//回答内容
					int JG;//回答结果 
				}HD[10];
			}DH[20];
		}DHData[20];	
	}SJData;
	
	int textnum;
	std::string Text[101];
	
	
}Map;


//0."  "空气   1."##"墙   2."门"门   9."%%"陷阱   10."敌"敌人   11."人"一个人 
const int wps = 20;
struct SI{//StructItem 的缩写
	//                       0    1    2    3    4    5    6 
	int wpnum[wps]=         {0   ,1   ,2   ,9   ,10  ,11  ,-1};
	std::string wpstr[wps]= {"  ","##","门","%%","敌","人"};
	std::string wpname[wps]={"空气","墙","门","陷阱","敌人","NPC"}; 
	int Hand;
	int Num = 6;
};
SI Item;

	


std::string ClsText;//清屏str 

std::string MapName[21];//地图列表  暂时作用不大 
int MapNum;//地图列表 的地图数量 




int ct(int x, int y, std::string str){//<-------------------------------------------------------指定打印 增加FPS 
    COORD zb;
    zb.X = x;
    zb.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), zb);
    std::cout << str;
    std::cout.flush(); 
    return 0;
}

std::string wstring_string(std::wstring wstr){
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);//   来自AI 将 wstring 转换为 string 
    std::string strr(len, 0);//                                                               来自AI 将 wstring 转换为 string 
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &strr[0], len, NULL, NULL);//       来自AI 将 wstring 转换为 string 
	strr.pop_back();//去除最后的一个字符   在此是 ' '<---空格 
	return strr;
}

int Debug(std::string str){//<---------------------------------------------------------日志
	
	if(str == "sss"){//清空日志 
    	std::ofstream outfile(DebugWay);
		outfile<<"==================================日志==================================";
		return 0;
	}
	
	
	std::string strr; 
	strr+="\n";
    
    
    //写入程序运行时间 
    strr+="[";
	double NowTime = clock();
	strr += std::to_string(NowTime/1000); 
    strr+="] ";
    
    //写入Debug 
    strr+=str;
    std::ofstream outfile(DebugWay,std::ios::app);
    outfile<<strr;//写入文件
}

void cls(){//<-------------------------------------------------------------------------------清屏函数  "极大"的减少了屏幕刷新时的闪烁 
	ct(0,0,ClsText);
}

void CDW(const std::wstring& str){//<-------------------------------------------------------创建文件夹
    CreateDirectoryW(str.c_str(),NULL);
}

void FFFW(const std::wstring& str){//<-------------------------------------------------------获取文件 
	Debug("搜索地图文件==="); 
	WIN32_FIND_DATAW num;//创建结构体以暂时保存地图名称 
	HANDLE Name = FindFirstFileW((str + L"\\*").c_str(), &num);// 来自AI 获取名称
	int i = 0;//定义记号 
	while(FindNextFileW(Name,&num)){//是否还有文件 
		std::string strr = wstring_string(num.cFileName);//获取名称
		MapName[i] = strr;//将地图名称写入数组
		Debug("第"+std::to_string(i)+"个地图文件:"+strr);
		i++;//记号加一 
	}
    MapNum = i-1;//记录地图数量 
    
    FindClose(Name);//结束寻找 
    Debug("搜索了"+std::to_string(MapNum)+"个文件");
    Debug("===搜索地图文件");
}

//void IfTextNull(){//<-------------------------------------------------------检测是否有  "无/Null"
//	if(Map.Text[0] != "无/Null"){
//		for(int i = Map.textnum;i>0;i--){
//			Map.Text[i] = Map.Text[i-1];
//		}
//		Map.Text[0] = "无/Null";
//		Map.textnum++;
//		int num = Map.SJData.SJNum;//获取地图特殊事件数量 
//		for(int i=0;i<num;i++){
//			if(Map.SJData.Data[i].Type == 1){//1===传送 
//				Map.SJData[i][3]++;
//			}else if(Map.SJData.Data[i].Type == 2){//2===对话 
//				Map.SJData[i][3]++;//对话人的名字
//				int dhnum;//对话数量
//				dhnum = Map.SJData[i][4];//对话数量
//				int Hand = 5;
//				for(int k=1;k<=dhnum;k++){
//					Map.SJData[i][Hand]++;//对话内容 
//					Hand++;
//					int hdnum;//回答数量 
//					hdnum = Map.SJData[i][Hand];//回答数量
//					Hand++;
//					for(int j=1;j<=hdnum;j++){
//						Map.SJData[i][Hand]++;//回答内容
//						Hand+=2;
//					}
//				}
//			}
//		}
//	}
//}

int AddText(std::string Text){//<----------------------------------------------------------------------添加文本信息 
	bool FindT = 0;
	for(int i=0;i<Map.textnum;i++){
		if(Map.Text[i] == Text){
			FindT = 1;
			return i; 
		}
	}
	if(!FindT){
		Map.Text[Map.textnum] = Text;
		Map.textnum++;
		return Map.textnum-1;
	}
}

void GameMap(std::string mapname){
	
	std::string mapway = wstring_string(DataWayParent)+ "/" + mapname;//计算地图路径 
//	Debug("地图路径:"+mapway);
	std::ifstream PFMapData(mapway);
	
	std::string a;
	PFMapData>>a;//读取地图名字
	
	PFMapData>>Map.maxx>>Map.maxy;//读取地图大小 
	for(int i=0;i<Map.maxy;i++){//读取地图数据 
		for(int j=0;j<Map.maxx;j++){
			PFMapData>>Map.Data[i][j];
		}
	}
}

std::string MapDataEasyRead(){
	if(BoolMapDataEasyRead)return "\n";
	else return " ";
}

void GameMapData(std::string cs,std::string mapname){//<---------------------------------------------读取地/写入图文件
	if(cs == "MDR"){//MDR MapDataRead ===========的缩写
		Debug("读取地图=====");
		Debug("文件名称:"+mapname);
		
		std::string mapway = wstring_string(DataWayParent)+ "/" + mapname;//计算地图路径 
		Debug("地图路径:"+mapway);
		std::ifstream PFMapData(mapway);
		
		PFMapData>>Map.name;//读取地图名字
		Debug("地图名称:"+Map.name);
		
		PFMapData>>Map.maxx>>Map.maxy;//读取地图大小 
		for(int i=0;i<Map.maxy;i++){//读取地图数据 
			for(int j=0;j<Map.maxx;j++){
				PFMapData>>Map.Data[i][j];
			}
		}
		
		int sjnum;
		PFMapData>>sjnum;//读取地图特殊事件数量 
		Map.SJData.SJNum = sjnum;//读取地图特殊事件数量
		Debug("特殊事件个数:"+std::to_string(Map.SJData.SJNum)); 
		for(int i=0;i<sjnum;i++){
			PFMapData>>Map.SJData.Data[i].X>>Map.SJData.Data[i].Y;//X Y
			PFMapData>>Map.SJData.Data[i].Type;//事件类型
			if(Map.SJData.Data[i].Type == 1){//1===传送 
				PFMapData>>Map.SJData.CSData[Map.SJData.CSNum].MapName;//目标地图
				PFMapData>>Map.SJData.CSData[Map.SJData.CSNum].X>>Map.SJData.CSData[Map.SJData.CSNum].Y; //目标位置 
				Map.SJData.Data[i].Hand = Map.SJData.CSNum;
				Map.SJData.CSNum ++;
			}else if(Map.SJData.Data[i].Type == 2){//2===对话 
				PFMapData>>Map.SJData.DHData[Map.SJData.DHNum].ObjectName;//对话人的名字
				int dhnum;//对话数量
				PFMapData>>dhnum;//对话数量
				Map.SJData.DHData[Map.SJData.DHNum].DHNum = dhnum;//对话数量
				for(int j=0;j<dhnum;j++){
					PFMapData>>Map.SJData.DHData[Map.SJData.DHNum].DH[j].Text;//对话内容 
					int hdnum;//回答数量 
					PFMapData>>hdnum;//回答数量 
					Map.SJData.DHData[Map.SJData.DHNum].DH[j].HDNum = hdnum;//回答数量 
					for(int k=0;k<hdnum;k++){
						PFMapData>>Map.SJData.DHData[Map.SJData.DHNum].DH[j].HD[k].Text;//回答内容
						PFMapData>>Map.SJData.DHData[Map.SJData.DHNum].DH[j].HD[k].JG;//此回答结果 
					}
				}
				Map.SJData.Data[i].Hand = Map.SJData.DHNum;
				Map.SJData.DHNum++;
			}
		}
		
		int strnum;
		PFMapData>>strnum;//获取地图文本数量
		Map.textnum = strnum;
		for(int i=0;i<strnum;i++){//读取地图文本
			PFMapData>>Map.Text[i];
		}
		
		for(int i=0;i<Map.SJData.SJNum;i++){//写入文本信息 
			//在此之前 变量为 文本地址
			if(Map.SJData.Data[i].Type == 1){//传送事件 
				Map.SJData.CSData[Map.SJData.Data[i].Hand].MapName = Map.Text[std::stoi(Map.SJData.CSData[Map.SJData.Data[i].Hand].MapName)];// 目标地图 通过地址获取文本
			}else if(Map.SJData.Data[i].Type == 2){//对话事件 
				Map.SJData.DHData[Map.SJData.Data[i].Hand].ObjectName = Map.Text[std::stoi(Map.SJData.DHData[Map.SJData.Data[i].Hand].ObjectName)];// 对话者 通过地址获取文本
				for(int j=0;j<Map.SJData.DHData[Map.SJData.Data[i].Hand].DHNum;j++){
					Map.SJData.DHData[Map.SJData.Data[i].Hand].DH[j].Text = Map.Text[std::stoi(Map.SJData.DHData[Map.SJData.Data[i].Hand].DH[j].Text)];// 对话内容 通过地址获取文本
					for(int k=0;k<Map.SJData.DHData[Map.SJData.Data[i].Hand].DH[j].HDNum;k++){
						Map.SJData.DHData[Map.SJData.Data[i].Hand].DH[j].HD[k].Text = Map.Text[std::stoi(Map.SJData.DHData[Map.SJData.Data[i].Hand].DH[j].HD[k].Text)];// 回答内容 通过地址获取文本
					}
				}
			}
			//在此之后 变量为 文本信息 
		}
		Map.textnum = 1;
		Map.Text[0] = "无/Null";
		Debug("=====读取地图");
		cls();
//		IfTextNull();//如果当前地图第0个 文本 内容不是 无/Null 
	}else if(cs == "MDW"){//MDR MapDataWrite ===========的缩写
		Debug("保存地图=====");
		Debug("文件名称:"+mapname); 
		
		Map.textnum = 1;
		Map.Text[0] = "无/Null";
		
		std::string str;
		str+=Map.name+MapDataEasyRead();//地图名称 
		str+=std::to_string(Map.maxx)+" "+std::to_string(Map.maxy)+MapDataEasyRead();//地图尺寸 
		for(int i=0;i<Map.maxy;i++){//地图数据 
			for(int j=0;j<Map.maxx;j++){
				str+=std::to_string(Map.Data[i][j])+" ";
			}
			str+=MapDataEasyRead();
		}
		Debug("特殊事件:"+std::to_string(Map.SJData.SJNum));
		str+=std::to_string(Map.SJData.SJNum)+MapDataEasyRead();//特殊事件数量
		for(int i=0;i<Map.SJData.SJNum;i++){
			str+=std::to_string(Map.SJData.Data[i].X)+" "+std::to_string(Map.SJData.Data[i].Y)+" ";//事件触发位置XY 
			str+=std::to_string(Map.SJData.Data[i].Type)+" ";//事件类型
			Debug("第"+std::to_string(i+1)+"个 特殊事件: X Y 类型"+std::to_string(Map.SJData.Data[i].X)+" "+std::to_string(Map.SJData.Data[i].Y)+" "+std::to_string(Map.SJData.Data[i].Type));
			if(Map.SJData.Data[i].Type == 1){//传送事件 
				str+=std::to_string(AddText(Map.SJData.CSData[Map.SJData.Data[i].Hand].MapName))+" ";//目标地图 
				str+=std::to_string(Map.SJData.CSData[Map.SJData.Data[i].Hand].X)+" "+std::to_string(Map.SJData.CSData[Map.SJData.Data[i].Hand].Y);//目标位置XY 
				str+=MapDataEasyRead();
			}else if(Map.SJData.Data[i].Type == 2){//对话事件 
				str+=std::to_string(AddText(Map.SJData.DHData[Map.SJData.Data[i].Hand].ObjectName))+" ";//对话人
				str+=std::to_string(Map.SJData.DHData[Map.SJData.Data[i].Hand].DHNum)+" ";//对话数量
				for(int j=0;j<Map.SJData.DHData[Map.SJData.Data[i].Hand].DHNum;j++){//对话 
					str+=std::to_string(AddText(Map.SJData.DHData[Map.SJData.Data[i].Hand].DH[j].Text))+" ";//当前对话内容
					int hdnum = Map.SJData.DHData[Map.SJData.Data[i].Hand].DH[j].HDNum;//回答数量 
					str+=std::to_string(hdnum)+" ";//回答数量 
					for(int k=0;k<hdnum;k++){//回答 
						str+=std::to_string(AddText(Map.SJData.DHData[Map.SJData.Data[i].Hand].DH[j].HD[k].Text))+" ";//回答内容
						str+=std::to_string(Map.SJData.DHData[Map.SJData.Data[i].Hand].DH[j].HD[k].JG)+" ";//此回答结果 
					}
				}
				str+=MapDataEasyRead();
			} 
		}
		str+=std::to_string(Map.textnum)+MapDataEasyRead();
		for(int i=0;i<Map.textnum;i++){
			str+=Map.Text[i]+MapDataEasyRead();
		}
		std::string mapway = wstring_string(DataWayParent)+ "/" + mapname;//计算地图路径 
		Debug("地图路径:"+mapway);
		std::ofstream outfile(mapway);
		outfile<<str;
	}
	
}

void pmsx(){//<---------------------------------------------------------------屏幕刷新
	
//	cls();
	std::string str;
	
	str += "=====冒险之路 地图编辑器=====\n\n";
	 
	//=====指针坐标显示 
	str +="位置:"+std::to_string(Player.myx)+" "+std::to_string(Player.myy)+"               \n"; 
	
	//=====地图文件信息显示 
	str+="文件名:"+Map.wjname+" 大小XY:"+std::to_string(Map.maxx)+"*"+std::to_string(Map.maxy)+" 事件:"+std::to_string(Map.SJData.SJNum);
	str+="\n";
	
	str+="地图名称: "+Map.name;//地图名称 
	str+="\n";
	str+="警告当前地图未保存  <------\n";
	str+="\n";
	for(int i=0;i<Map.maxy;i++){//地图显示 
		for(int j=0;j<Map.maxx;j++){
			int num = Map.Data[i][j];
			
			if(num ==  0) str+="  ";
			else if(num ==  1) str+="##"; 
			else if(num ==  2) str+="门"; 
			else if(num ==  9) str+="%%"; 
			else if(num == 10) str+="敌"; 
			else if(num == 11) str+="人"; 	
		}
		str+="  ";
		str+="\n"; 
	}
	
	str+="\n";
	str+="  ";
	if(Item.Hand == 0)str+="   \"";
	else str+=Item.wpstr[Item.Hand-1]+" \"";
	str+=Item.wpstr[Item.Hand]+"\" ";
	if(Item.Hand != Item.Num)str+=Item.wpstr[Item.Hand+1];
	else str+="  ";
	str+="    \n";
	str+="      ^^---------当前选择\n";
	str+="当前选择的物品名称: "+Item.wpname[Item.Hand]+"           \n";
	
	for(int i=0;i<=Map.maxx;i++) str+="  ";
	str+="\n";
	
	
	ct(0,0,str);
	
    ct(Player.myx*2,Player.myy+7,"");
}

void PlayerInput(char input){//<---------------------------------------------------玩家输入 
	
	if(input == 'r' || input == 'R'){
		GameMapData("MDW",Map.wjname);
		cz=1;
	}
	
	//移动-----------------------------
	int xx=0,yy=0;
	
	
	if(input == 'w' || input == 'W'){
		yy--;
	}else if(input == 'a' || input == 'A'){
		xx--;
	}else if(input == 's' || input == 'S'){
		yy++;
	}else if(input == 'd' || input == 'D'){
		xx++;
	}
	
	
	if(Player.myx+xx >= 0 && Player.myx+xx < Map.maxx && Player.myy+yy >= 0 && Player.myy+yy < Map.maxy){
		Player.myx += xx;
		Player.myy += yy;
	}
	//-----------------------------移动
	//操作-----------------------------
	if(input == 'o' || input == 'O'){
		if(Item.Hand>0)Item.Hand--;
	}else if(input == 'p' || input == 'P'){
		if(Item.Hand<Item.Num-1)Item.Hand++;
	}else if(input == ' '){
		if(Map.Data[Player.myy][Player.myx] == Item.wpnum[Item.Hand])Map.Data[Player.myy][Player.myx] = 0;
		else Map.Data[Player.myy][Player.myx] = Item.wpnum[Item.Hand];
	}else if(input == 'q' || input == 'Q'){
		cz=4;
	}else if(input == 'e' || input == 'E'){
		cz=5;
	}
	//-----------------------------操作
}

void GameRun(){
	pmsx();//屏幕刷新 
	PlayerInput(_getch());//玩家输入 
}



void BJSJCFWZ(const int Hand){//<----------------------------------------------------------------------编辑特殊事件触发位置
	Player.myx = Map.SJData.Data[Hand].X;//获取触发的位置 
	Player.myy = Map.SJData.Data[Hand].Y;//获取触发的位置 
	while(1){
		cls();
		pmsx();
		for(int i=1;i<=6;i++){
			ct(0,i,"                                                                              ");
		}
		for(int i=1;i<=6;i++){
			ct(0,i+Map.maxy+7,"                                                                              ");
		} 
		std::string str;
		str+="=====冒险之路 地图编辑器=====\n\n";
		str+="当前事件序号"+std::to_string(Hand);
		if(Map.SJData.Data[Hand].Type == 1)     str+=" 传送事件\n";
		else if(Map.SJData.Data[Hand].Type == 2)str+=" 对话事件\n";
		str+="====================================================\n";
		str+="当前位置 XY  "+std::to_string(Player.myx)+" "+std::to_string(Player.myy)+"          \n"; 
		str+="WASD移动 到触发位置 按空格确定\n";
		str+="r.返回\n";
		
		ct(0,0,str);
		ct(Player.myx*2,Player.myy+7,"");
		char input=_getch();
		if(input =='r'||input =='R')break;
		else if(input == 'w'||input == 'W'||input == 'a'||input == 'A'||input == 's'||input == 'S'||input == 'd'||input == 'D')PlayerInput(input);
		else if(input ==' '){
			Map.SJData.Data[Hand].X = Player.myx;
			Map.SJData.Data[Hand].Y = Player.myy;
			break;
		}
	}
}

void BJSJYM(const int Hand){//<----------------------------------------------------------------------编辑特殊事件页面
	int type = Map.SJData.Data[Hand].Type;
	if(type == 1){//传送 
		int MyHand = 1; 
		while(1){
			std::string str;
			str+="=====冒险之路 地图编辑器=====\n\n";
			str+="当前事件序号"+std::to_string(Hand)+" 传送事件     \n";
			str+="====================================================\n";
			str+="\n";
			str+="当前触发位置XY   "+std::to_string(Map.SJData.Data[Hand].X)+" "+std::to_string(Map.SJData.Data[Hand].Y);                                                 if(MyHand == 1)str+="   <----------\n";else str+="            \n" ;
			str+="传送的目标地图:  "+Map.SJData.CSData[Map.SJData.Data[Hand].Hand].MapName;                                                                               if(MyHand == 2)str+="   <----------\n";else str+="            \n" ;
			str+="传送的目标位置:  "+std::to_string(Map.SJData.CSData[Map.SJData.Data[Hand].Hand].X)+" "+std::to_string(Map.SJData.CSData[Map.SJData.Data[Hand].Hand].Y); if(MyHand == 3)str+="   <----------\n";else str+="            \n" ;
			
			str+="\n\n";
			if(MyHand == 1)str+="当前选择 编辑 触发位置XY         \n";
			if(MyHand == 2)str+="当前选择 编辑 传送的目标地图     \n";
			if(MyHand == 3)str+="当前选择 编辑 传送的目标位置     \n";
			if(MyHand == 3)str+="选择此项 将自动保存<=======";
			
			str+="\n\n\n";
			str+="WS.上下   空格.编辑\n";
			str+="r.返回\n";
			
			cls();
			ct(0,0,str);
			char input=_getch();
			if(input =='r' || input =='R')break;
			else if(input == 'w' || input == 'W'){
				if(MyHand>1)MyHand--;
			}else if(input == 's' || input == 'S'){
				if(MyHand<3)MyHand++;
			}else if(input == ' '){
				if(MyHand == 1){//==========编辑 触发位置 
					BJSJCFWZ(Hand); 
				}else if(MyHand == 2){//==========编辑 传送的目标地图
					FFFW(DataWayParent);//获取地图名字
					int TwoHand = 1;
					while(1){
						std::string str;
						str+="=====冒险之路 地图编辑器=====\n\n";
						str+="当前事件序号"+std::to_string(Hand);
						if(Map.SJData.Data[Hand].Type == 1)     str+=" 传送事件\n";
						else if(Map.SJData.Data[Hand].Type == 2)str+=" 对话事件\n";
						str+="====================================================\n";
						str+="\n";
						str+="发现地图 "+std::to_string(MapNum)+"\n";
						str+="地图列表: 请选择地图列表当中的地图\n";
						for(int i=1;i<=MapNum;i++){
							str+=std::to_string(i)+"."+MapName[i];
							if(TwoHand == i)str+="   <---\n";							
							else str+="\n";
						}
						
						str+="\n\n";
						str+="当前选择: "+MapName[TwoHand]+"\n";
						
						str+="\n\n\n\n";
						str+="ws.上下 空格.选择";
						str+="\n";
						str+="r.返回\n";
						
						cls();
						ct(0,0,str);
						char input=_getch();
						if(input == 'w' || input == 'W'){
							if(TwoHand>1)TwoHand--;
						}else if(input == 's' || input == 'S'){
							if(TwoHand<MapNum)TwoHand++;
						}else if(input == 'r' || input == 'R'){
							break;
						}
						else if(input == ' '){
							Map.SJData.CSData[Map.SJData.Data[Hand].Hand].MapName = Map.Text[AddText(MapName[TwoHand])];//添加文本信息 ---文本内容
							break;
						}
					}
				}else if(MyHand == 3){//==========编辑 传送的目标位置
					if(Map.SJData.CSData[Map.SJData.Data[Hand].Hand].MapName == "无/Null"){
						std::string str;
						str+="=====冒险之路 地图编辑器=====\n\n";
						str+="当前事件序号"+std::to_string(Hand);
						if(Map.SJData.Data[Hand].Type == 1)     str+=" 传送事件\n";
						else if(Map.SJData.Data[Hand].Type == 2)str+=" 对话事件\n";
						str+="====================================================\n\n";
						str+="请选择地图在设置此参数!\n\n";
						str+="按任意键返回\n";
						
						cls();
						ct(0,0,str);
						char input=_getch();
					}else{
						std::string MainMapName = Map.wjname;
						Player.myx = Map.SJData.CSData[Map.SJData.Data[Hand].Hand].X;//将指针设置为 传送位置 
						Player.myy = Map.SJData.CSData[Map.SJData.Data[Hand].Hand].Y;//将指针设置为 传送位置 
						GameMapData("MDW",MainMapName);//保存文件 
						GameMap(Map.SJData.CSData[Map.SJData.Data[Hand].Hand].MapName);//只读取目标地图信息 
						while(1){
							cls();
							pmsx();
							for(int i=1;i<=6;i++){
								ct(0,i,"                                                                              ");
							}
							for(int i=1;i<=6;i++){
								ct(0,i+Map.maxy+7,"                                                                              ");
							} 
							std::string str;
							str+="=====冒险之路 地图编辑器=====\n\n";
							str+="当前事件序号"+std::to_string(Hand);
							str+="\n";
							str+="====================================================\n";
							str+="当前位置 XY  "+std::to_string(Player.myx)+" "+std::to_string(Player.myy)+"          \n"; 
							str+="WASD移动 到触发位置 按空格确定\n";
							str+="r.返回\n";
							
							ct(0,0,str);
							ct(Player.myx*2,Player.myy+7,"");
							char input=_getch();
							if(input =='r'||input =='R')break;
							else if(input == 'w'||input == 'W'||input == 'a'||input == 'A'||input == 's'||input == 'S'||input == 'd'||input == 'D')PlayerInput(input);
							else if(input ==' '){
								Map.SJData.CSData[Map.SJData.Data[Hand].Hand].X = Player.myx;
								Map.SJData.CSData[Map.SJData.Data[Hand].Hand].Y = Player.myy;
								break;
							}
						}
						GameMap(MainMapName);//只读取原地图信息
						Player.myx = Map.SJData.Data[Hand].X;//将指针设置为 触发位置 
						Player.myy = Map.SJData.Data[Hand].Y;//将指针设置为 触发位置 
					}
				}
			}
		}
	}else if(type == 2){//对话  
		int MyHand = 1; 
		while(1){
			std::string str;
			str+="=====冒险之路 地图编辑器=====\n\n";
			str+="当前事件序号"+std::to_string(Hand)+" 对话事件     \n";
			str+="====================================================\n";
			str+="\n";
			str+="当前触发位置XY   "+std::to_string(Map.SJData.Data[Hand].X)+" "+std::to_string(Map.SJData.Data[Hand].Y); if(MyHand == 1)str+="   <----------\n";else str+="            \n" ;
			str+="对话者名称:      "+Map.SJData.DHData[Map.SJData.Data[Hand].Hand].ObjectName;                            if(MyHand == 2)str+="   <----------\n";else str+="            \n" ;
			str+="对话的数量:      "+std::to_string(Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DHNum);                 if(MyHand == 3)str+="   <----------\n";else str+="            \n" ;
			
			str+="\n\n\n";
			str+="AD.上下一个对话\n";
			str+="空格.编辑 查看超详细内容\n";
			str+="r.返回\n";
			cls();
			ct(0,0,str);
			char input=_getch();
			if(input =='r' || input =='R')break;
			else if(input == 'w' || input == 'W'){
				if(MyHand>1)MyHand--;
			}else if(input == 's' || input == 'S'){
				if(MyHand<3)MyHand++;
			}else if(input == ' '){
				if(MyHand == 1){//==========编辑 触发位置 
					BJSJCFWZ(Hand);
				}else if(MyHand == 2){//==========编辑 对话者名称 
					std::string name = Map.SJData.DHData[Map.SJData.Data[Hand].Hand].ObjectName;
					std::string str;
					str+="=====冒险之路 地图编辑器=====\n\n";
					str+="当前事件序号"+std::to_string(Hand);
					if(Map.SJData.Data[Hand].Type == 1)     str+=" 传送事件\n";
					else if(Map.SJData.Data[Hand].Type == 2)str+=" 对话事件\n";
					str+="====================================================\n\n";
					str+="请输入对话者名称(string):\n";
					str+=name+"\n\n";
					str+="目前警告禁止使用空格\n";
					str+="\n\n\n\n";
					str+="回车.确定\n";
					cls();
					ct(0,0,str);
					ct(0,7,"");
					std::cin>>name;
					Map.SJData.DHData[Map.SJData.Data[Hand].Hand].ObjectName = Map.Text[AddText(name)];
				}else if(MyHand == 3){//==========编辑 对话
//					int Num = Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DHNum;//获取对话数量
					int ADHand = 0;
					int WSHand = 1;
					while(1){
						std::string str;
						str+="=====冒险之路 地图编辑器=====\n\n";
						str+="当前事件序号"+std::to_string(Hand);
						if(Map.SJData.Data[Hand].Type == 1)     str+=" 传送事件\n";
						else if(Map.SJData.Data[Hand].Type == 2)str+=" 对话事件\n";
						str+="====================================================\n\n";
						str+="当前对话序号("+std::to_string(ADHand)+")    当前对话数量("+std::to_string(Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DHNum)+")          \n\n";
						if(ADHand != Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DHNum)str+="编辑对话内容";else  str+="请新建对话  ";if(WSHand == 1)str+="   <----------\n";else str+="               \n";
						if(ADHand != Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DHNum)str+="编辑回答    ";else  str+="请新建对话  ";if(WSHand == 2)str+="   <----------\n";else str+="               \n";
						str+="\n";
						str+="删除此对话  ";   if(WSHand == 3)str+="   <----------\n";else str+="               \n";
						str+="新建对话    ";   if(WSHand == 4)str+="   <----------\n";else str+="               \n";
						str+="\n\n";
						str+="对话"+std::to_string(ADHand)+"的 当前效果:   \n";
						str+="|  "+Map.SJData.DHData[Map.SJData.Data[Hand].Hand].ObjectName+":\n";
						str+="|      "+Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].Text+"\n";
						if(Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HDNum > 0){
							str+="|  你的回答:\n";
							for(int i=0;i<Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HDNum;i++){
								str+="|  "+std::to_string(i+1)+". "+Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HD[i].Text+"\n";
							}
						}
						
						str+="\n\n\n";
						str+="WS.上下选择  AD.左右切换对话\n";
						str+="空格.确定\n";
						str+="r.返回\n";
						cls();
						ct(0,0,str);
						char input=_getch();
						if(input =='r'||input =='R')break;
						else if(input == 'a' || input == 'A'){//左 
							if(ADHand>0)ADHand--;
						}else if(input == 'd' || input == 'D'){//右
						    if(ADHand < Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DHNum-1)ADHand++;
						}else if(input == 'w' || input == 'W'){//上 
							if(WSHand>1)WSHand--;
						}else if(input == 's' || input == 'S'){//下 
							if(WSHand<4)WSHand++;
						}else if(input == ' '){
							if(WSHand == 1){//编辑对话内容
								if(ADHand != Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DHNum){
									std::string text = Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].Text;
									std::string str;
									str+="=====冒险之路 地图编辑器=====\n\n";
									str+="当前事件序号"+std::to_string(Hand);
									if(Map.SJData.Data[Hand].Type == 1)     str+=" 传送事件\n";
									else if(Map.SJData.Data[Hand].Type == 2)str+=" 对话事件\n";
									str+="====================================================\n\n";
									str+="当前对话序号("+std::to_string(ADHand)+")\n\n";
									str+="输入对话内容 建议粘贴\n";
									str+=text+"                                                \n\n";
									str+="目前警告禁止使用空格\n";
									str+="\n\n";
									str+="回车.确定\n";
									cls();
									ct(0,0,str);
									ct(0,9,"");
									std::cin>>text;
									Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].Text = Map.Text[AddText(text)];
								}
							}else if(WSHand == 2){//编辑回答
								if(ADHand != Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DHNum){
									int HDADHand = 0;
									int HDWSHand = 1;
									while(1){
										std::string str;
										str+="=====冒险之路 地图编辑器=====\n\n";
										str+="当前事件序号"+std::to_string(Hand);
										if(Map.SJData.Data[Hand].Type == 1)     str+=" 传送事件\n";
										else if(Map.SJData.Data[Hand].Type == 2)str+=" 对话事件\n";
										str+="====================================================\n";
										str+="当前对话序号("+std::to_string(ADHand)+")\n";
										str+="当前回答序号("+std::to_string(HDADHand)+")    当前对话数量("+std::to_string(Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HDNum)+")          \n\n";
										if(HDADHand != Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HDNum)str+="编辑回答内容";else  str+="请新建回答  ";if(HDWSHand == 1)str+="   <----------\n";else str+="               \n";
										if(HDADHand != Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HDNum)str+="编辑回答结果(未完工)";else  str+="请新建回答  ";if(HDWSHand == 2)str+="   <----------\n";else str+="               \n";
										str+="\n";
										str+="删除此回答  ";   if(HDWSHand == 3)str+="   <----------\n";else str+="               \n";
										str+="新建回答    ";   if(HDWSHand == 4)str+="   <----------\n";else str+="               \n";
										str+="\n\n";
										str+="对话"+std::to_string(ADHand)+"的 当前效果:   \n";
										str+="|  "+Map.SJData.DHData[Map.SJData.Data[Hand].Hand].ObjectName+":\n";
										str+="|      "+Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].Text+"\n";
										if(Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HDNum > 0){
											str+="|  你的回答:\n";
											for(int i=0;i<Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HDNum;i++){
												str+="|  "+std::to_string(i+1)+". "+Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HD[i].Text+"\n";
											}
										}
										
										str+="\n\n";
										str+="WS.上下选择  AD.左右切换对话\n";
										str+="空格.确定\n";
										str+="r.返回\n";
										cls();
										ct(0,0,str);
										char input=_getch();
										if(input =='r'||input =='R')break;
										else if(input == 'a' || input == 'A'){//左 
											if(HDADHand>0)HDADHand--;
										}else if(input == 'd' || input == 'D'){//右
										    if(HDADHand < Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HDNum-1)HDADHand++;
										}else if(input == 'w' || input == 'W'){//上 
											if(HDWSHand>1)HDWSHand--;
										}else if(input == 's' || input == 'S'){//下 
											if(HDWSHand<4)HDWSHand++;
										}else if(input == ' '){
											if(HDWSHand == 1){//编辑回答内容
												if(HDADHand != Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HDNum){
													std::string text = Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HD[HDADHand].Text;
													std::string str;
													str+="=====冒险之路 地图编辑器=====\n\n";
													str+="当前事件序号"+std::to_string(Hand);
													if(Map.SJData.Data[Hand].Type == 1)     str+=" 传送事件\n";
													else if(Map.SJData.Data[Hand].Type == 2)str+=" 对话事件\n";
													str+="====================================================\n\n";
													str+="当前对话序号("+std::to_string(ADHand)+")\n";
													str+="当前回答序号("+std::to_string(HDADHand)+")    当前对话数量("+std::to_string(Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HDNum)+")          \n\n";
													str+="输入回答内容 建议粘贴\n";
													str+=text+"                                                \n\n";
													str+="目前警告禁止使用空格\n";
													str+="\n\n";
													str+="回车.确定\n";
													cls();
													ct(0,0,str);
													ct(0,10,"");
													std::cin>>text;
													Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HD[HDADHand].Text = Map.Text[AddText(text)];
												}
											}else if(HDWSHand == 2){//编辑回答结果
												
											}else if(HDWSHand == 3){//删除此回答
												if(Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HDNum > 0) Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HDNum --;
												if(HDADHand == Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HDNum && HDADHand != 0)HDADHand--;
											}else if(HDWSHand == 4){//新建回答
												HDADHand = Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HDNum;
												Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HD[HDADHand].Text = Map.Text[0];
												Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].HDNum ++;
											}
											
										}
									}
								}
							}else if(WSHand == 3){//删除对话 
								if(Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DHNum > 0) Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DHNum --;
								if(ADHand == Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DHNum && ADHand != 0)ADHand--;
							}else if(WSHand == 4){//新建对话
								ADHand = Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DHNum;
								Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DH[ADHand].Text = Map.Text[0]; 
								Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DHNum ++;
							} 
						}
					}
				}
			}
		}
	} 
}

void ymxs(){//<----------------------------------------------------------------------页面显示
	if(cz == 0){
		GameRun();
	}else if(cz == 1){
		while(1){
			std::string str;
			str+="=====冒险之路 地图编辑器=====\n\n";
			str+="1.加载地图\n";
			str+="2.新建地图\n";
			str+="3.关于\n";
			
			str+="\n\n\n";
			str+="r.退出\n";
			
			cls();
			ct(0,0,str);
			char input=_getch(); 
			if(input == '1'){
				cz = 3;
				break;
			}else if(input == '2'){//新建地图
				std::string str;
				str+="=====冒险之路 地图编辑器=====\n\n";
				str+="新建地图\n";
				str+="输入文件名称(不需要加后缀 特殊事件定位的文件名称)(string)\n";
				cls();
				ct(0,0,str);
				std::string wjnamenum;
				std::cin>>wjnamenum;
				wjnamenum+=".txt";
				
				str="=====冒险之路 地图编辑器=====\n\n";
				str+="新建地图\n";
				str+="输入地图名称(string)\n";
				cls();
				ct(0,0,str);
				std::string mapnamenum;
				std::cin>>mapnamenum;
				
				str="=====冒险之路 地图编辑器=====\n\n";
				str+="地图名称: "+mapnamenum+"\n";
				str+="输入地图尺寸 X Y:(建议大于5 不建议超过50)(int)\n";
				int NumX,NumY;
				cls();
				ct(0,0,str);
				std::cin>>NumX>>NumY;
				Map.name = mapnamenum;
				Map.wjname = wjnamenum;
				Map.maxx = NumX;
				Map.maxy = NumY;
				GameMapData("MDW",Map.wjname);
				cz = 0;
				break;
			}else if(input == '3'){
				cz = 2;
				break;
			}else if(input == 'r' || input == 'R'){
				exit(0);
			}
		}
	}else if(cz == 2){
		while(1){
			std::string str;
			str+="=====冒险之路 地图编辑器=====\n\n";
			str+="  关于\n\n";
			str+="此项目为 \"冒险之路\" 的辅助项目\n";
			str+="wsad.上下左右\n";
			str+="r.返回 并 保存\n";
			str+="o.向左选择 p.向右选择\n";
			str+="q.查看 编辑特殊事件\n";
			str+="e.查看文本数组(不可更改)\n";
			str+="空格.放置 或 取消\n"; 
			 
			str+="\n\n\n";
			str+="r.返回\n";
			
			cls();
			ct(0,0,str);
			char input=_getch();
			if(input == 'r' || input == 'R'){
				cz = 1;
				break;
			}
		}
	}else if(cz == 3){
		FFFW(DataWayParent);//获取地图名字
		int Hand = 1;
		while(1){
			std::string str;
			str+="=====冒险之路 地图编辑器=====\n\n";
			str+="发现地图 "+std::to_string(MapNum)+"\n";
			str+="地图列表:\n";
			for(int i=1;i<=MapNum;i++){
				str+=std::to_string(i)+"."+MapName[i];
				if(Hand == i)str+="   <---\n";							
				else str+="\n";
			}
			
			str+="\n\n";
			str+="当前选择: "+MapName[Hand]+"\n";
			
			str+="\n\n\n\n";
			str+="ws.上下 空格.选择";
			str+="\n";
			str+="r.返回\n";
			
			cls();
			ct(0,0,str);
			char input=_getch();
			if(input == 'w' || input == 'W'){
				if(Hand>1)Hand--;
			}else if(input == 's' || input == 'S'){
				if(Hand<MapNum)Hand++;
			}else if(input == 'r' || input == 'R'){
				cz=1;
				break;
			}
			else if(input == ' '){
				cz=0;
				Debug("读取地图   "+MapName[Hand]);
				Map.wjname=MapName[Hand];
				GameMapData("MDR",MapName[Hand]);
				break;
			}
		}
	}else if(cz == 4){// 特殊事件 
		int Hand = 0;
		while(1){
			std::string str;
			str+="=====冒险之路 地图编辑器=====\n\n";
			str+=" 特殊事件\n";
			str+="当前事件数量: "+std::to_string(Map.SJData.SJNum)+"    \n";
			str+="====================================================\n";
			
			for(int i=0;i<Map.SJData.SJNum;i++){
				str+=std::to_string(i)+". "; 
				if(Map.SJData.Data[i].Type == 1)     str+="传送事件 ";
				else if(Map.SJData.Data[i].Type == 2)str+="对话事件 ";
				str+="触发位置XY:"+std::to_string(Map.SJData.Data[Map.SJData.Data[i].Hand].X)+" "+std::to_string(Map.SJData.Data[Map.SJData.Data[i].Hand].Y)+" "; 
				if(Map.SJData.Data[i].Type == 1)     str+="目标地图:"+Map.SJData.CSData[Map.SJData.Data[i].Hand].MapName+" ";
				else if(Map.SJData.Data[i].Type == 2)str+="对话者:"+Map.SJData.DHData[Map.SJData.Data[i].Hand].ObjectName+" ";
				if(Hand == i)str+="<--------";
				str+="            ";
				str+="\n";
			}
			
			str+="\n\n";
			str+="当前选择: 序号"+std::to_string(Hand)+"的 ";
			if(Map.SJData.Data[Hand].Type == 1)     str+="传送事件     ";
			else if(Map.SJData.Data[Hand].Type == 2)str+="对话事件     ";
			str+="\n";
			str+="详细信息:";
			if(Map.SJData.Data[Hand].Type == 1)str+="传送至目标地图("+Map.SJData.CSData[Map.SJData.Data[Hand].Hand].MapName+")的坐标XY("+std::to_string(Map.SJData.CSData[Map.SJData.Data[Hand].Hand].X)+" "+std::to_string(Map.SJData.CSData[Map.SJData.Data[Hand].Hand].Y)+")";
			else if(Map.SJData.Data[Hand].Type == 2)str+="与对话者("+Map.SJData.DHData[Map.SJData.Data[Hand].Hand].ObjectName+")对话了("+std::to_string(Map.SJData.DHData[Map.SJData.Data[Hand].Hand].DHNum)+")句话";
			str+="\n";
			
			str+="\n\n\n";
			str+="WS.上下   空格.编辑 查看超详细内容\n";
			str+="Q.新建特殊事件---\n";
			str+="X.删除此事件(警告)\n";
			str+="r.返回\n";
			
			cls();
			ct(0,0,str);
			char input=_getch();
			if(input == 'w' || input == 'W'){
				if(Hand>0)Hand--;
			}else if(input == 's' || input == 'S'){
				if(Hand<Map.SJData.SJNum-1)Hand++;
			}else if(input == 'r' || input == 'R'){
				cz=0;
				break;
			}else if(input == ' '){
				BJSJYM(Hand);//编辑特殊事件页面 
			}else if(input == 'q' || input == 'Q'){//新建事件 
				while(1){
					std::string str;
					str+="=====冒险之路 地图编辑器=====\n\n";
					str+=" 特殊事件\n";
					str+="当前事件数量: "+std::to_string(Map.SJData.SJNum)+"    \n";
					str+="====================================================\n";
					
					str+="\n";
					str+="选择事件类型: \n";
					str+="1.传送事件\n";
					str+="2.对话事件\n";
					
					
					str+="\n\n\n";
					str+="r.返回\n";
					
					cls();
					ct(0,0,str);
					char input=_getch();
					if(input == '1'){
						Map.SJData.Data[Map.SJData.SJNum].X = Player.myx;         //设置新事件触发位置
						Map.SJData.Data[Map.SJData.SJNum].Y = Player.myy;         //设置新事件触发位置
						Map.SJData.Data[Map.SJData.SJNum].Type = 1;               //设置新事件类型-----传送
						Map.SJData.Data[Map.SJData.SJNum].Hand = Map.SJData.CSNum;//设置新事件地址
						Map.SJData.CSNum ++;                                      //传送事件数量加一
						Map.SJData.CSData[Map.SJData.SJNum].MapName = Map.Text[0];//设置传送目标
						Map.SJData.CSData[Map.SJData.SJNum].X = 0;                //设置传送位置
						Map.SJData.CSData[Map.SJData.SJNum].Y = 0;                //设置传送位置
						
						Hand = Map.SJData.SJNum;
						Map.SJData.SJNum ++;
						break;
					}else if(input == '2'){
						Map.SJData.Data[Map.SJData.SJNum].X = Player.myx;            //设置新事件触发位置
						Map.SJData.Data[Map.SJData.SJNum].Y = Player.myy;            //设置新事件触发位置
						Map.SJData.Data[Map.SJData.SJNum].Type = 2;                  //设置新事件类型-----对话
						Map.SJData.Data[Map.SJData.SJNum].Hand = Map.SJData.CSNum;   //设置新事件地址
						Map.SJData.DHNum ++;                                         //对话事件数量加一
						Map.SJData.DHData[Map.SJData.SJNum].ObjectName = Map.Text[0];//设置对话者名称
						Map.SJData.DHData[Map.SJData.SJNum].DHNum = 0;               //设置对话数量 
						
						Hand = Map.SJData.SJNum;
						Map.SJData.SJNum ++;
						break;
					}else if(input == 'r' || input == 'R')break;
				}
			}else if((input == 'x'||input == 'X') && Map.SJData.SJNum > 0){
				while(1){
					std::string str;
					str+="=====冒险之路 地图编辑器=====\n\n";
					str+=" 特殊事件\n";
					str+="当前事件数量: "+std::to_string(Map.SJData.SJNum)+"    \n";
					str+="====================================================\n";
					
					str+="\n";
					str+="删除事件: \n\n";
					str+="1.      警告            警告        警告警告    \n";
					str+="2.      警告        警告警告    警告        警告\n";
					str+="3.      警告    警告    警告    警告        警告\n";
					str+="4.      警告警告        警告    警告        警告\n";
					str+="5.      警告            警告        警告警告    \n";
					
					
					str+="\n\n";
					str+="你确定要删除 事件序号"+std::to_string(Hand);
					if(Map.SJData.Data[Hand].Type == 1)     str+=" 传送事件\n";
					else if(Map.SJData.Data[Hand].Type == 2)str+=" 对话事件\n";
					str+="删除后无法测回\n"; 
					
					str+="\n\n\n";
					str+="N.确定删除\n";
					str+="r.返回\n";
					
					cls();
					ct(0,0,str);
					char input=_getch();
					if(input == 'r' || input == 'R')break;
					else if(input == 'n'||input == 'N'){
						if(Map.SJData.Data[Hand].Type == 1){//传送 
							for(int i = Map.SJData.Data[Hand].Hand+1;i < Map.SJData.CSNum;i ++){
								Map.SJData.CSData[i-1] = Map.SJData.CSData[i];
							}
						}else if(Map.SJData.Data[Hand].Type == 2){//对话 
							for(int i = Map.SJData.Data[Hand].Hand+1;i < Map.SJData.DHNum;i ++){
								Map.SJData.DHData[i-1] = Map.SJData.DHData[i];
							}
						}
						for(int i = Hand+1;i < Map.SJData.SJNum;i++){
							Map.SJData.Data[i-1] = Map.SJData.Data[i];
						}
						Map.SJData.SJNum --;
						break;
					}
				}
			}
		}
		cls();
	}else if(cz == 5){// 文本数组 
		GameMapData("MDW",Map.wjname);
		while(1){
			std::string str;
			str+="=====冒险之路 地图编辑器=====\n\n";
			str+=" 文本数组\n";
			str+="当前文本数量: "+std::to_string(Map.textnum)+"    \n";
			str+="====================================================\n\n";
			
			for(int i=0;i<Map.textnum;i++){
				str+=std::to_string(i)+". "+Map.Text[i]+"\n";
			}
			
			
			str+="\n\n\n";
			str+="r.返回\n";
			
			cls();
			ct(0,0,str);
			char input=_getch();
			if(input == 'r' || input == 'R'){
				cz=0;
				break;
			}
		}
	}
}

void Begin(){//----------------------------------------------------------------------初始化
	CDW(DataWayParent);//创建文件夹
	cz = 1;
	for(int i=1;i<=40;i++)ClsText+="                                                                                \n";//补充清屏 
}


int main(){
	
	Debug("sss");
	
	Begin();
	while(1){
		ymxs();
	}
}





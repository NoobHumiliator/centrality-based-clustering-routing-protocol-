// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<math.h>
#include<iostream>
#include<time.h>
#include<string.h>
#include<stdio.h>
#include<list>
#include <stdlib.h>
#include <algorithm>
#include <cstdio>  
#include <cstdlib>  
#include <fstream>
#include <vector>
#include <complex>
#include <cmath>
#include<conio.h> 

using namespace std;

#define M_A_X 9999

const double pi = 3.141592654;
int L = 12000;//模拟环境的大小
double turn_left_possibility = 0.3; //下个路口左转的几率30%
double turn_right_poosibility = 0.3; //下个路口右转的概率30%


class aodvjr_RREQ_packet {   //aodvjr 的请求包
public:
	int T;
	int scource;
	int destination;
	int current;
	int ID;  //序列号  一个包又三个要素确定， scource destination ID
};

class aodvjr_RREP_packet {   //aodvjr的返回包
public:
	int T;
	int scource;
	int destination;
	int current;
};

class packet {
public:
	int T;
	int scource;
	int destination;
	int current;
};
 
class cbc_packet {
public:
	int T;
	int scource;
	int destination;
	int next_gateway;  //经由哪个网关
	int next_head;     //转发给哪个簇头
	int current; //当前位置
	int already_in_this_cluster;
	int go_for_gateway;  //表示正在从簇头发向网关
	int ID;  //这个就是时间，调试用
};

class lca_packet {
public:
	int T;
	int ID; //调试用，这个就是时间
	int scource;
	int destination;
	int next_gateway;  //经由哪个网关
	int next_head;     //转发给哪个簇头
	int current; //当前位置
};

class cbc_head_annouce_packet
{
public:
	int T;
	int scource;
	int current;
	int degree; //本节点的度
	vector<int> cbc_announce_path;  //记录cbc簇头声明节点的路径
	vector<int> is_passed_by; //记录哪些节点曾经经过了，上面那个的伴生数组  is_passed_by[4]=0;表示4号节点没有经过 is_passed_by[5]=1 ;表示曾经经过5号节点
	int ID;  //序列号  一个包又三个要素确定， scource ID
};

class cbc_deny_packet  //cbc簇头请求的驳回包
{
public:
	int destination;  //驳回包的目的地址
	int current;
	vector<int> deny_path;  //驳回包的路径
};

class cbc_agree_packet  //cbc簇头请求的同意包
{
public:
	int scource;
	int destination;  //同意包的目的地址
	int depth;  //同意节点对源节点的深度，depth=1认为是他的邻居 depth=2为2阶邻居
	int current;
	vector<int> control_neighbor; //与reachable是一样的，源节点支配了哪些节点
	vector<int> agree_path;  //同意包的路径
	vector<int> report_path; //同意包返回的时候，把路径都记下来，用以回复簇头节点
};

class cbc_maintain_packet
{
public:
	int scource;
	int destination;  //同意包的目的地址
	int current;
	vector<int> path_control_neighbor; //与reachable是一样的，把整个路径上支配哪些节点全部告诉簇头，用来维护簇头自己的内容
};

class cbc_routing_table_element  //路由表的单个元素，我本人到特定节点的路由信息
{
public:
	int cluster_head; //通过这个簇头可以找到，目的地
	bool in_this_cluster;  //是否在本簇可以直接送达
	int gateway;  //通过几号网关可以找到这个簇  gateway=-1;表示我可以直接联系上簇头
	int gateway_mainstay_number;  //骨干网的编号，0是说明簇首本身就可以够到网关，1说明1号展臂可以够到 2说明2号展臂可以够到(不论是网关还是目的地) 
	int time_stamp;  //时间戳 这条信息的时间戳
	int length;     //距离，经过多少次网关才能到目的簇头
};

class lca_routing_table_element  //路由表的单个元素
{
public:
	int cluster_head; //通过这个簇头可以找到，目的地
	int gateway;  //通过几号网关可以找到这个簇  gateway=-1;表示我可以直接联系上簇头
	int length;  //距离 length=0表示就在本簇中
	int time_stamp; //时间戳
};

class cbc_inform_packet  //cbc簇建立后对全体簇头广播的名片
{
public:
	int	effective;    //名片的有效性effective=0表示无效，effective=1表示有效，脱离簇以后名片无效
	int near;  //这个名片是哪个节点传过来的
	int scource;
	vector<cbc_routing_table_element> cbc_routing_table_packeted;  //打成包的路由表
	int time_stamp;  //时间戳，时间戳太久自动作废
	list<int> mainstay;   //骨干网的拓扑信息
};

class max_announce_received  //暂存下收到的最大请求包的ID
{ 
public:
	int max_announce_node_ID;    //暂存的最佳节点的id
	int max_announce_node_degree;  //暂存ID的度
	int wait_time_window;  //时间窗口
	int is_hearing;   //是否在接听请求包
	vector<cbc_agree_packet> all_virtual_cbc_agree_packet;//对应节点的全部包，只要是他的我就返回，不是他的我就丢弃
};

class lca_routing_exchange_packet
{
public:
	vector<lca_routing_table_element> packeted_lca_routing_table;  //lca路由表
};

class node {
public:
	int ID;   //车的ID
	int v;
	int x;
	int y;
	double r;  //通信距离
	int direction;  //当前方向  1 上（北） 2 右（东） 3 下（南） 4 左（西）
	list <int> turn; //下个路口是否转弯  0直行 1左转 2 右转
	vector<int> reachable;//可到达的节点
	vector<int> is_reachable; //临接关系的另一种表示  is_reachable[5]=1;表示跟5号节点是邻居关系 is_reachable[6]=0 表示跟6号节点不是邻居关系
	double distance(node p);//传一个对象指针进去
	void move(int block_width, int N);  //block width 指街区宽度，如果能被整除则处在路口应该转弯了  
	int virtual_move(node p,int block_width, int N, int time);  //虚拟移动，用来预测节点的未来位置
	//////////////////////////////////////////////////////////////////////////////////////////////////////aodvjr的所需的节点储存
	list<packet> queue;//数据包的队列
	list<aodvjr_RREP_packet> RREP_queue;  //返回包的队列
	list<aodvjr_RREQ_packet> RREQ_queue;  //请求包的队列
	//////// 这仨是虚拟队列,新的包总是先进虚拟队列,避免同一个时间步对数据包重复处理，等到每步结束的时候用虚拟队列覆盖真实队列
	list<packet> virtual_queue;//虚拟队列  
	list<aodvjr_RREP_packet> virtual_RREP_queue;  //虚拟返回包的队列
	list<aodvjr_RREQ_packet> virtual_RREQ_queue;  //虚拟请求包的队列
	vector<int>  aodvjr_table;  //aodvjr的正向路由,aodvjr_table[3]=6 表示从当前位置到3号目的地 要走6号节点
	vector<int>  aodvjr_back_table;  //aodvjr 协议的反向路由  aodvjr_back_table[5]=7  表示从当前位置到5号源 要走7号节点     -1表示反向路由尚未建立
	vector<int>  aodvjr_quest_times;  //对某个节点的请求次数，即请求包的ID
	vector<vector<int>> aodvjr_saved_ID;  //表示每个节点存储过的请求包ID，即aodvjr_saved_ID[5][7]=4表示我存储着源为5目的端为7的请求包的最新ID为4
	vector<int>  aodvjr_table_expiry;  //aodvjr的正向链路维护时间，若时间结束，链路自动断开aodvjr_table_expiry[3]=158表示从当前位置到3号目的地 链路剩余时间为 158
	vector<int>  aodvjr_back_expiry;  //aodvjr的反向链路维护时间，若时间结束，链路自动断开aodvjr_back_expiry[5]=128 表示从当前位置到5号源节点 链路剩余时间为128
	///////////////////////////////////////////////////////////////////////////////////////////////////////cbc协议的
	int cbc_incluster;//是否在簇中的标志  cbc_incluster=0 不在簇中,cbc_incluster=1 在簇中
	int cbc_ishead;   //是否为簇头节点   cbc_ishead=1 是簇头   cbc_ishead=0 不是簇头
	int cbc_isbone;   //是否为骨干网  cbc_isbone=1 是骨干网   cbc_isbone=0  不是骨干网
	int cbc_near; //作为骨干网近端邻居是谁 
	int cbc_far;  //作为骨干网远端邻居是谁
	list<cbc_packet> cbc_queue;//cbc的数据包的队列
	list<cbc_packet> virtual_cbc_queue;//cbc的数据包的队列
	list<cbc_head_annouce_packet> head_announce_queue;  //返回包的队列
	list<cbc_head_annouce_packet> virtual_head_announce_queue;  //返回包的虚拟队列
	list<cbc_deny_packet> cbc_deny_packet_queue;      //驳回包的队列
	list<cbc_deny_packet> virtual_cbc_deny_packet_queue;  //驳回包的虚拟队列
	list<cbc_agree_packet> cbc_agree_packet_queue;      //同意包的队列
	list<cbc_agree_packet> virtual_cbc_agree_packet_queue;  //同意包的虚拟队列
	int stop_announce; //簇头声明的暂停时间，如果收到deny包这个暂停时间增加，用以等待邻居成簇把他加进去
	int wait_to_create_head;  //等待多久建立簇头
	vector<vector<int>> saved_path; //存着返回包的路径   saved_path[2]={6,8,10} 说明存着由第二个返回包来回来的一条路径，路径中包括节点（包括簇头，本节点）6（簇头）.8.10
	vector<vector<int>> node_control; //存着哪些节点被那个节点支配   node_control[6]={2,9,11} 表示6号节点支配着2，9，11三个节点
	list<int>	mainstay_1; //簇的骨干网
	list<int>	mainstay_2; //骨干网
	int inform_interval;// 通知间隔，满足这个间隔才会广播通知信息
	vector<int> cluster_control; //本簇控制多少个节点,包括了簇头跟骨干网
	vector<int> mainstay_1_control;  //一号展臂能控制的节点
	vector<int> mainstay_2_control;  //二号展臂控制的节点
	cbc_inform_packet only_inform_packet;      //通知包，用来广播路由信息，这个每个骨干节点只有一个，作为本簇的名片
	cbc_inform_packet virtual_only_inform_packet;      //通知包，用来广播路由信息，这个每个骨干节点只有一个，作为本簇的名片
	list<cbc_inform_packet> exchange_packet_information_queue;      //网关节点用以交换的路由信息的存储队列
	list<cbc_inform_packet> virtual_exchange_packet_information_queue;  //网关节点用以交换的路由信息的虚拟队列
	vector<int> cbc_information_saved_ID; //本节点储存的全套路由信息的ID   cbc_information_saved_ID[7]=564; 我储存着到7号簇头广播的最新的最新的ID为564
	int hello_interval;
	int cbc_tail_1_validity;  //是否还能收到来自骨干网1的维护信息
	int cbc_tail_2_validity;  //是否还能收到来自骨干网1的维护信息
	int cbc_lonely_validity;  //如果这个簇头一个展臂都没有，不能让他一直活着
	int bone_of_whom;         //我作为骨干网的簇首编号
	vector<cbc_routing_table_element> cbc_routing_table;  //路由表
	list<cbc_maintain_packet> cbc_maintain_packet_queue;//这是又臂展节点末尾发来的簇维护包，告知簇头现在的情况
	list<cbc_maintain_packet> virtual_cbc_maintain_packet_queue;//维护包的虚拟队列
	max_announce_received my_max_announce_received;   //记录下一段时间内收到的最佳的请求包发来的度，只对他负责
	///////////////////////////////////////////////////////////////////////////////////////////////////////////// LCA算法所需结构
	int lca_ishead;     // 是否为lca的簇头
	vector<lca_routing_table_element> lca_routing_table;  //lca路由表
	list<lca_routing_exchange_packet> lca_routing_exchange_packet_queue;  //接收到路由表信息的虚拟队列
	list<lca_routing_exchange_packet> virtual_lca_routing_exchange_packet_queue;  //接收到路由表信息的虚拟队列
	list<lca_packet>  lca_packet_queue;
	list<lca_packet>  virtual_lca_packet_queue;
	int in_lca_cluster;
};

double node::distance(node p)
{
	return sqrt((x - p.x)*(x - p.x) + (y - p.y)*(y - p.y));

}

void node::move(int block_width,int N)
{
	reachable.clear();//把临接关系清空
	is_reachable.clear();
	for (int i = 0; i < N; i++)
	{
	 is_reachable.push_back(0); //临接关系清空
	}
	///////////////////////////////行进
	switch (direction)
	{
	case 1:       //北行 左下角坐标0，0 
	{
		y = y + v;
		break;
	}
	case 2:      //东行
	{
		x = x + v;
		break;
	}
	case 3:      //南行
	{
		y = y - v;
		break;
	}
	case 4:      //西行
	{
		x = x - v;
		break;
	}
	}
	////////////////////////////////////////////边界处理
	if (x <=0 )  //贴左边了  模糊处理
	{
		x = 0;
		direction = 2;
	}
	if (x >= L)  //贴右边了
	{
		x = L;
		direction = 4;
	}
	if (y <= 0)  //贴下边了
	{
		y = 0;
		direction = 1;
	}
	if (y >= L)  //贴上边了
	{
		y = L;
		direction = 3;
	}
	if (x%block_width == 0 && y%block_width ==0 && x != 0 && y != 0 && x != L&&y != L)   //x y 都能被街区宽度整除 说明处于路口，改变方向
	{
		cout << "车ID：" << ID << "到达路口,路口坐标为" << x << "," << y << endl;
		switch (turn.front())
		{
		case 0:       //车辆直行不做操作
		{
			break;
		}
		case 1:      //左转
		{
			direction = direction - 1;
			if (direction == 0)
			{
				direction = 4;   //北往左转是西
			}
			break;
		}
		case 2:      //右转
		{
			direction = direction + 1;
			if (direction == 5)
			{
				direction = 1;   //西往右转是北
			}
			break;
		}
		}
		double rand_num = (double)rand() / RAND_MAX;
		if (rand_num < turn_left_possibility)
		{
			turn.push_back(1);
		}
		if (rand_num >= turn_left_possibility&&rand_num <= (turn_right_poosibility + turn_left_possibility))
		{
			turn.push_back(2);
		}
		if (rand_num > (turn_right_poosibility + turn_left_possibility))
		{
			turn.push_back(0);
		}
		turn.pop_front();   //转过弯了，第一个转弯预设抛弃
	}
}

int node::virtual_move(node p,int block_width, int N,int time)
{
	int is_keep_connected = 1;
	int time_last = time;
	int virtual_p_x = p.x;
	int virtual_p_y = p.y;
	int virtual_p_direction =p.direction;
	list <int> virtual_p_turn = p.turn;
	int virtual_x = x;
	int virtual_y = y;   
	int virtual_direction = direction; //复制当前坐标，方向
	list <int> virtual_turn=turn;
	while (time_last>1)
	{
		time_last--;
		///////////////////////////////行进
		switch (virtual_direction)
		{
		case 1:       //北行 左下角坐标0，0 
		{
			virtual_y = virtual_y + v;
			break;
		}
		case 2:      //东行
		{
			virtual_x = virtual_x + v;
			break;
		}
		case 3:      //南行
		{
			virtual_y = virtual_y - v;
			break;
		}
		case 4:      //西行
		{
			virtual_x = virtual_x - v;
			break;
		}
		}
		////////////////////////////////////////////边界处理
		if (virtual_x <= 0)  //贴左边了  模糊处理
		{
			virtual_x = 0;
			virtual_direction = 2;
		}
		if (virtual_x >= L)  //贴右边了
		{
			virtual_x = L;
			virtual_direction = 4;
		}
		if (virtual_y <= 0)  //贴下边了
		{
			virtual_y = 0;
			virtual_direction = 1;
		}
		if (virtual_y >= L)  //贴上边了
		{
			virtual_y = L;
			virtual_direction = 3;
		}
		if (virtual_x%block_width == 0 && virtual_y%block_width == 0 && virtual_x != 0 && virtual_y != 0 && virtual_x != L&&virtual_y != L)   //x y 都能被街区宽度整除 说明处于路口，改变方向
		{
			//cout << "虚拟车ID：" << ID << "到达虚拟路口,路口坐标为" << virtual_x << "," << virtual_y << endl;
			switch (virtual_turn.front())
			{
			case 0:       //车辆直行不做操作
			{
				break;
			}
			case 1:      //左转
			{
				virtual_direction = virtual_direction - 1;
				if (virtual_direction == 0)
				{
					virtual_direction = 4;   //北往左转是西
				}
				break;
			}
			case 2:      //右转
			{
				virtual_direction = virtual_direction + 1;
				if (virtual_direction == 5)
				{
					virtual_direction = 1;   //西往右转是北
				}
				break;
			}
			}
			virtual_turn.pop_front();   //转过弯了，第一个转弯预设抛弃
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////本节点运动完了，该p节点了
		switch (virtual_p_direction)
		{
		case 1:       //北行 左下角坐标0，0 
		{
			virtual_p_y = virtual_p_y + v;
			break;
		}
		case 2:      //东行
		{
			virtual_p_x = virtual_p_x + v;
			break;
		}
		case 3:      //南行
		{
			virtual_p_y = virtual_p_y - v;
			break;
		}
		case 4:      //西行
		{
			virtual_p_x = virtual_x - v;
			break;
		}
		}
		////////////////////////////////////////////边界处理
		if (virtual_p_x <= 0)  //贴左边了  模糊处理
		{
			virtual_p_x = 0;
			virtual_p_direction = 2;
		}
		if (virtual_x >= L)  //贴右边了
		{
			virtual_p_x = L;
			virtual_p_direction = 4;
		}
		if (virtual_p_y <= 0)  //贴下边了
		{
			virtual_p_y = 0;
			virtual_p_direction = 1;
		}
		if (virtual_p_y >= L)  //贴上边了
		{
			virtual_p_y = L;
			virtual_p_direction = 3;
		}
		if (virtual_p_x%block_width == 0 && virtual_p_y%block_width == 0 && virtual_p_x != 0 && virtual_p_y != 0 && virtual_p_x != L&&virtual_y != L)   //x y 都能被街区宽度整除 说明处于路口，改变方向
		{
			//cout << "虚拟p车ID：" << ID << "到达虚拟路口,路口坐标为" << virtual_p_x << "," << virtual_p_y << endl;
			switch (virtual_p_turn.front())
			{
			case 0:       //车辆直行不做操作
			{
				break;
			}
			case 1:      //左转
			{
				virtual_p_direction = virtual_p_direction - 1;
				if (virtual_p_direction == 0)
				{
					virtual_p_direction = 4;   //北往左转是西
				}
				break;
			}
			case 2:      //右转
			{
				virtual_p_direction = virtual_p_direction + 1;
				if (virtual_p_direction == 5)
				{
					virtual_p_direction = 1;   //西往右转是北
				}
				break;
			}
			}
			virtual_p_turn.pop_front();   //转过弯了，第一个转弯预设抛弃
		}
		if (sqrt((virtual_x - virtual_p_x)*(virtual_x - virtual_p_x) + (virtual_y - virtual_p_y)*(virtual_y - virtual_p_y))>r) // 距离比较长，已经链路断了
		{
			is_keep_connected = 0;  //链路断了
			break;
		}	
	}
	return is_keep_connected;
}

packet create_single_packet(int N)
{
		packet realpacket;
		realpacket.T = 0;
		while (1)
		{
			realpacket.current = rand()%N;
			realpacket.destination = rand()%N;	
			if (realpacket.destination != realpacket.current)
			{
				break;
			}
			else
			{
				continue;
			}
		}
		realpacket.scource = realpacket.current;  //数据包源地址
		return realpacket;
}

cbc_packet create_single_cbc_packet(int N)
{
	cbc_packet realpacket;
	realpacket.T = 0;
	realpacket.next_head = -1;
	realpacket.next_gateway = -1;
	realpacket.go_for_gateway = 0;
	realpacket.already_in_this_cluster = 0;
	while (1)
	{
		realpacket.current = rand() % N;
		realpacket.destination = rand() % N;
		if (realpacket.destination != realpacket.current)
		{
			break;
		}
		else
		{
			continue;
		}
	}
	realpacket.scource = realpacket.current;  //数据包源地址
	return realpacket;
}

void create_node(node** car,int block_width,int v,double r,int N)
{
	int pre_turn_time = 80;  //预测转弯多少次
	double rand_num = 0;  //0-1 之间的随机小数
	for (int start_node = 0; start_node < N; start_node++)
	{
		car[start_node] = new node();//node的实例初始化
		car[start_node]->ID = start_node;
		car[start_node]->v = v;
		car[start_node]->r = r;
		////////////////////////////////////////////////////以下是aodv协议所需的初始化
		car[start_node]->aodvjr_back_table.clear();
		car[start_node]->aodvjr_table.clear();
		car[start_node]->aodvjr_quest_times.clear();
		car[start_node]->aodvjr_back_expiry.clear();
		car[start_node]->aodvjr_table_expiry.clear();
		car[start_node]->is_reachable.clear();
		vector <int> all_zero_line;
		vector <int> nothing;
		for (int i = 0; i < N; i++)
		{
			all_zero_line.push_back(0);   //整个二维数组初始化为0
		}
		for (int i = 0; i < N; i++)
		{
			car[start_node]->aodvjr_back_table.push_back(-1);  //反向路由都初始化为-1
			car[start_node]->aodvjr_table.push_back(-1);
			car[start_node]->aodvjr_quest_times.push_back(0);
			car[start_node]->aodvjr_saved_ID.push_back(all_zero_line); //插入空列用来占座
			car[start_node]->node_control.push_back(nothing);     //插入空的来占位置
			car[start_node]->aodvjr_table_expiry.push_back(0);  //链路剩余时间的都置为0
			car[start_node]->aodvjr_back_expiry.push_back(0);  //置为0
			car[start_node]->is_reachable.push_back(0); //全部不是邻居
		}
		////////////////////////////////////////////////////////下面是运行cbc协议要做的一些初始化
		car[start_node]->cbc_incluster=0;//是否在簇中的标志  cbc_incluster=0 不在簇中,cbc_incluster=1 在簇中
		car[start_node]->cbc_ishead=0;   //是否为簇头节点   cbc_ishead=1 是簇头   cbc_ishead=0 不是簇头
		car[start_node]->cbc_isbone=0;   //是否为骨干网  cbc_isbone=1 是骨干网   cbc_isbone=0  不是骨干网
		car[start_node]->bone_of_whom = -1;  //他谁的簇头也不是
		car[start_node]->cbc_near = -1; //作为骨干网近端邻居是谁 
		car[start_node]->cbc_far = -1;  //作为骨干网远端邻居是谁
		car[start_node]->my_max_announce_received.max_announce_node_degree = -1;
		car[start_node]->my_max_announce_received.max_announce_node_ID = -1;
		car[start_node]->my_max_announce_received.wait_time_window = -1;
		car[start_node]->my_max_announce_received.is_hearing = 1;
		car[start_node]->only_inform_packet.effective = 0;
		for (int i = 0; i < N; i++)
		{
			car[start_node]->cbc_information_saved_ID.push_back(-1); //我储存着到所有簇头广播的最新的最新的ID为-1
		}
		car[start_node]->stop_announce=0; 
		car[start_node]->wait_to_create_head = M_A_X;
		car[start_node]->inform_interval=0;// 通知间隔，满足这个间隔才会广播通知信息
		/////////////////////////////////////////////////////////////////
		rand_num = (double)rand() / RAND_MAX;
		if (rand_num < 0.25)
		{
			car[start_node]->x = ((rand() % (L / block_width - 1)) + 1)*block_width;      //从1000到9000之间的一个随机数
			car[start_node]->y = ((rand() % (L / v - 1)) + 1)*v;
			car[start_node]->direction = 1;
		}
		if (rand_num >= 0.25&&rand_num < 0.5)
		{
			car[start_node]->y = ((rand() % (L / block_width - 1)) + 1)*block_width;
			car[start_node]->x = ((rand() % (L / v - 1)) + 1)*v;
			car[start_node]->direction = 2;
		}
		if (rand_num >= 0.5&&rand_num < 0.75)
		{
			car[start_node]->x = ((rand() % (L / block_width - 1)) + 1)*block_width;
			car[start_node]->y = ((rand() % (L / v - 1)) + 1)*v;
			car[start_node]->direction = 3;
		}
		if (rand_num >= 0.75)
		{
			car[start_node]->y = ((rand() % (L / block_width - 1)) + 1)*block_width;
			car[start_node]->x = ((rand() % (L / v - 1)) + 1)*v;
			car[start_node]->direction = 4;
		}
		for (int i = 0; i < pre_turn_time - 1; i++)        //将要拐弯的长度预测几次
		{
			rand_num = (double)rand() / RAND_MAX;
			if (rand_num < turn_left_possibility)
			{
				car[start_node]->turn.push_back(1);
			}
			if (rand_num >= turn_left_possibility&&rand_num <= (turn_right_poosibility + turn_left_possibility))
			{
				car[start_node]->turn.push_back(2);
			}
			if (rand_num >(turn_right_poosibility + turn_left_possibility))
			{
				car[start_node]->turn.push_back(0);
			}
		}
		//cout << start_node << "号节点初始分配的坐标是" << car[start_node]->x << "," << car[start_node]->y << "速度为" << car[start_node]->v << "方向为" << car[start_node]->direction << "下次转弯方向为" << car[start_node]->turn.front() << endl;
		//////////////////////////////////////////////////////////////下面是lca协议的一些初始化
		car[start_node]->lca_ishead = 0;
	}
}

void cover_queue(node** car,int N)
{
	for (int process_node = 0; process_node < N; process_node++)
	{
		car[process_node]->queue.clear();
		car[process_node]->queue = car[process_node]->virtual_queue;
		car[process_node]->virtual_queue.clear();
		car[process_node]->RREQ_queue.clear();
		car[process_node]->RREQ_queue = car[process_node]->virtual_RREQ_queue;
		car[process_node]->virtual_RREQ_queue.clear();
		car[process_node]->RREP_queue.clear();
		car[process_node]->RREP_queue = car[process_node]->virtual_RREP_queue;
		car[process_node]->virtual_RREP_queue.clear();
	}
}

void cbc_cover_queue(node** car, int N)
{
	for (int process_node = 0; process_node < N; process_node++)
	{
		car[process_node]->cbc_queue.clear();
		car[process_node]->cbc_queue = car[process_node]->virtual_cbc_queue;
		car[process_node]->virtual_cbc_queue.clear();

		car[process_node]->head_announce_queue.clear();
		car[process_node]->head_announce_queue = car[process_node]->virtual_head_announce_queue;
		car[process_node]->virtual_head_announce_queue.clear();

		car[process_node]->cbc_deny_packet_queue.clear();
		car[process_node]->cbc_deny_packet_queue = car[process_node]->virtual_cbc_deny_packet_queue;
		car[process_node]->virtual_cbc_deny_packet_queue.clear();

		car[process_node]->cbc_agree_packet_queue.clear();
		car[process_node]->cbc_agree_packet_queue = car[process_node]->virtual_cbc_agree_packet_queue;
		car[process_node]->virtual_cbc_agree_packet_queue.clear();

		car[process_node]->exchange_packet_information_queue.clear();
		car[process_node]->exchange_packet_information_queue = car[process_node]->virtual_exchange_packet_information_queue;
		car[process_node]->virtual_exchange_packet_information_queue.clear();

		car[process_node]->cbc_maintain_packet_queue.clear();
		car[process_node]->cbc_maintain_packet_queue = car[process_node]->virtual_cbc_maintain_packet_queue;
		car[process_node]->virtual_cbc_maintain_packet_queue.clear();

		car[process_node]->only_inform_packet = car[process_node]->virtual_only_inform_packet;
	}
}

void aodvjr_calculate_connection(node** car, double r, int N, int aodvjr_route_expiry)
{
	//////////////////////////////////////////////////////////////////计算临接关系
	for (int process_node = 0; process_node < N; process_node++)
	{
		for (int toreach = 0; toreach < process_node; toreach++)
		{
			if (process_node != toreach)
			{
				if (r >= car[process_node]->distance(*car[toreach]))
				{
					car[process_node]->reachable.push_back(toreach);
					car[process_node]->is_reachable[toreach] = 1;
					car[toreach]->reachable.push_back(process_node);
					car[toreach]->is_reachable[process_node] = 1;
					car[process_node]->aodvjr_table[toreach] = toreach;
					car[process_node]->aodvjr_table_expiry[toreach] = aodvjr_route_expiry;
					car[toreach]->aodvjr_table[process_node] = process_node;
					car[toreach]->aodvjr_table_expiry[process_node] = aodvjr_route_expiry;
					//cout << process_node << "号节点可到达邻居为" << toreach << endl;
				}
				else{}
			}
			else {}
		}
	}
}

void cbc_calculate_connection(node** car, double r, int N)
{
	//////////////////////////////////////////////////////////////////计算临接关系
	for (int process_node = 0; process_node < N; process_node++)
	{
		for (int toreach = 0; toreach < process_node; toreach++)
		{
			if (process_node != toreach)
			{
				if (r >= car[process_node]->distance(*car[toreach]))
				{
					car[process_node]->reachable.push_back(toreach);
					car[process_node]->is_reachable[toreach] = 1;
					car[toreach]->reachable.push_back(process_node);
					car[toreach]->is_reachable[process_node] = 1;
				}
				else{}
			}
			else {}
		}
	}
}

void net_work_draw(node** car, int N,int time)
{
	char filename[100];
	sprintf_s(filename, "%d_time_draw.net", time);
	ofstream ftest(filename, ios::app);
	ftest << "*Vertices " << N << endl;
	for (int i = 0; i < N; i++)
	{
		ftest << i + 1 << " " << "\"" << i << "\"" << " " << car[i]->x << " " << car[i]->y << " " << "0.5" << endl;
	}
	ftest << "*Edges" << endl;
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < car[i]->reachable.size(); j++)
		{
			ftest << i + 1 << " " << car[i]->reachable[j] + 1 << endl;
		}
	}
	ftest.close();
}

void net_work_situation(node** car, int N)
{
	int RREQ_sum = 0;
	int RREP_sum = 0;
	int queue_sum = 0;
	for (int i = 0; i < N; i++)
	{
		RREQ_sum = RREQ_sum + car[i]->RREQ_queue.size();
		RREP_sum = RREP_sum + car[i]->RREP_queue.size();
		queue_sum = queue_sum + car[i]->queue.size();
	}
	cout << "请求包总量" << RREQ_sum << "返回包总量" << RREP_sum << "数据包总量" << queue_sum << endl;
	system("pause");
}

void cbc_net_work_situation(node** car, int N)
{
	int head_sum = 0;
	int RREP_sum = 0;
	int queue_sum = 0;
	for (int i = 0; i < N; i++)
	{
		if (car[i]->cbc_ishead == 1)
		{
			head_sum++;
			//cout << i << "是簇头" << endl;
		}
		queue_sum = queue_sum + car[i]->cbc_queue.size();
	}
	cout << "簇头总量" << head_sum << "数据包总量" << queue_sum << endl;
	//system("pause");
}

void lca_net_work_situation(node** car, int N)
{
	int head_sum = 0;
	int queue_sum = 0;
	for (int i = 0; i < N; i++)
	{
		if (car[i]->lca_ishead == 1)
		{
			head_sum++;
			//cout << i << "是簇头" << endl;
		}
		queue_sum = queue_sum + car[i]->lca_packet_queue.size();
	}
	cout << "簇头总量" << head_sum << "数据包总量" << queue_sum << endl;
	//system("pause");
}

void create_packet(node** car,double packet_spawn_rate,int N)
{
	for (int start_packet = 0; start_packet<(int)(packet_spawn_rate*N); start_packet++)
	{
		packet new_packet = create_single_packet(N);
		//cout << new_packet.current << " " << new_packet.destination << endl;
		car[new_packet.current]->queue.push_back(new_packet);  //生成数据包
	}
}

void calculate_route_life(node** car, int N)
{
	for (int i = 0; i<N; i++)   //
	{
		for (int j = 0; j < N; j++)
		{
			if (i != j)
			{
				car[i]->aodvjr_table_expiry[j]--;    //又经过了一步，链路倒计时
				if (car[i]->aodvjr_table_expiry[j] <= 0)  //当前节点到目的节点已经很久没有数据包经过了
				{
					if (car[i]->aodvjr_table[j] != -1)
					{
						//cout << "从" << i << "到" << j << "经过" << car[i]->aodvjr_table[j] << "的正向路由失效" << endl;
						car[i]->aodvjr_table[j] = -1;  //链路失效
					}
				}
				car[i]->aodvjr_back_expiry[j]--;    //又经过了一步，反向链路倒计时
				if (car[i]->aodvjr_back_expiry[j] <= 0)  //当前节点到源节点已经很久没有数据包经过了
				{
					if (car[i]->aodvjr_back_table[j] != -1)
					{
						//cout << "从" << i << "到" << j << "经过" << car[i]->aodvjr_back_table[j] << "的反向路由失效" << endl;
						car[i]->aodvjr_back_table[j] = -1;  //链路失效				
					}			
				}
			}
		}
	}
}

void use_RREP_maintain(node** car, int N, int aodvjr_route_expiry,long long int &control_packet)
{
	for (int i = 0; i < N; i++)   
	{
		for (int j = 0; j < N; j++)
		{
			if (i != j)    //全部节点对，由i到j发送返回包，维持链路
			{
				aodvjr_RREP_packet   new_RREP_packet;
				new_RREP_packet.current = car[i]->aodvjr_back_table[j];
				new_RREP_packet.scource = i;
				new_RREP_packet.destination = j;
				new_RREP_packet.T = 0;
				if (new_RREP_packet.current!=-1)  //不存在反向路由也就不用维护
				{
					if (car[i]->is_reachable[new_RREP_packet.current] == 1)  //如果这条反向链路确实存在
					{
						control_packet++;
						//car[new_RREP_packet.current]->RREP_queue.push_back(new_RREP_packet);
						car[new_RREP_packet.current]->virtual_RREP_queue.push_back(new_RREP_packet);
						if (car[new_RREP_packet.current]->aodvjr_table[i] = i)
						{
							//cout << "正向路由(只一跳)：" << new_RREP_packet.current << "到目的地" << i << "的正向路由为" << car[new_RREP_packet.current]->aodvjr_table[i] << "匹配" << i << "更新存活时间" << endl;
							car[new_RREP_packet.current]->aodvjr_table_expiry[i] = aodvjr_route_expiry;
						}
					}
				}
			}
		}
	}
}

void handle_aodvjr_RREP_packet(node** car, int N, int aodvjr_route_expiry)
{
	for (int traversal_packet = 0; traversal_packet<N; traversal_packet++)   //反向数据包
	{
		for (list<aodvjr_RREP_packet>::iterator i = car[traversal_packet]->RREP_queue.begin(); i != car[traversal_packet]->RREP_queue.end();)  //这里i++ 写在下面,需要处理一些删除操作
		{
			//cout << traversal_packet << "号节点中有AODV返回包，源" << i->scource << "目的" << i->destination << "当前" << i->current << endl;
			if (i->destination == i->current)                  //如果目的已经是当前
			{
				//cout << traversal_packet << "返回包已经送达源节点，路由建立" << endl;
				car[traversal_packet]->RREP_queue.erase(i++); //返回包已经到达位置，删除
			}
			else
			{     //返回包继续前进
				int temp = car[i->current]->aodvjr_back_table[i->destination];
				if (temp != -1)   //不存在反向路由
				{
					aodvjr_RREP_packet new_RREP_packet = *i;
					new_RREP_packet.current = temp;
					if (car[traversal_packet]->is_reachable[temp]==1)  //确实存在这么一条链路，可以返回数据包
					{
						//car[temp]->RREP_queue.push_back(new_RREP_packet);
						car[temp]->virtual_RREP_queue.push_back(new_RREP_packet);
						if (car[temp]->aodvjr_table[i->scource] == traversal_packet)  //之前建立过的正向路由，由于反向数据包经过他了，更新一下存活时间
						{
							car[temp]->aodvjr_table_expiry[i->scource] = aodvjr_route_expiry;
							//cout << "正向路由：" << temp << "到目的地" << i->scource << "的正向路由为" << car[temp]->aodvjr_table[i->scource] << "匹配" << traversal_packet <<"更新存活时间"<< endl;
						}
						if (car[temp]->aodvjr_table[i->scource] == -1)
						{
							car[temp]->aodvjr_table[i->scource] = traversal_packet;
							//cout << "正向路由建立从当前位置：" << temp << "到目的地" << i->scource << "的正向路由为" << car[temp]->aodvjr_table[i->scource] << endl;
							car[temp]->aodvjr_table_expiry[i->scource] = aodvjr_route_expiry;  //新建立的路由，生存时间赋值
						}
					}
				}
				car[traversal_packet]->RREP_queue.erase(i++);  //不管能不能送，删
			}
		}
	}
}

void handle_aodvjr_RREQ_packet(node** car, int N, int aodv_RREQ_expiry,long int aodvjr_route_expiry,long long int &control_packet)
{
	for (int traversal_packet = 0; traversal_packet<N; traversal_packet++)    //请求
	{
		for (list<aodvjr_RREQ_packet>::iterator i = car[traversal_packet]->RREQ_queue.begin(); i != car[traversal_packet]->RREQ_queue.end();)  //这里i++ 写在下面,需要处理一些删除操作
		{
			//cout << traversal_packet << "号节点中有AODV请求包，源" << i->scource << "目的" << i->destination << "当前" << i->current << "  ID为" << i->ID << "时间为" << i->T << endl;
			if (i->T>aodv_RREQ_expiry)  //如果过期  
			{
				//cout << traversal_packet << "号节点中有AODV请求包失效，给予删除" << endl;
				car[traversal_packet]->RREQ_queue.erase(i++);
			}
			else
			{
				if (i->destination == i->current)                  //如果目的已经是当前
				{
					//cout << traversal_packet << "号节点中有AODV请求包到达目的，删除，发返回包" << endl;
					aodvjr_RREP_packet   new_RREP_packet;
					new_RREP_packet.current = car[i->destination]->aodvjr_back_table[i->scource];
					new_RREP_packet.scource = i->destination;
					new_RREP_packet.destination = i->scource;
					if (new_RREP_packet.current!=-1)  //不存在反向路由 无需发返回包
					{
						if (car[i->destination]->is_reachable[new_RREP_packet.current] == 1)  //如果这条反向链路确实存在
						{
							control_packet++;
							//car[new_RREP_packet.current]->RREP_queue.push_back(new_RREP_packet);
							car[new_RREP_packet.current]->virtual_RREP_queue.push_back(new_RREP_packet);
							if (car[new_RREP_packet.current]->aodvjr_table[i->destination] = i->destination)
							{
								car[new_RREP_packet.current]->aodvjr_table_expiry[i->destination] = aodvjr_route_expiry;
							}
						}
					}
					car[traversal_packet]->RREQ_queue.erase(i++); //返回包已发，删除请求包
				}
				else
				{     //再次广播请求包
					for (int j = 0; j < car[i->current]->reachable.size(); j++)
					{
						if (car[i->current]->reachable[j] != i->scource)
						{
							aodvjr_RREQ_packet quest_packet = *i;
							quest_packet.current = car[i->current]->reachable[j];
							quest_packet.T = quest_packet.T + 1;
							control_packet++;
							//cout << "转发出请求报文" << "其源节点为" << quest_packet.scource << "目的节点为" << quest_packet.destination << "当前位置为" << quest_packet.current << "时间为" << quest_packet.T << "ID为" << quest_packet.ID<< endl;
							if (car[quest_packet.current]->aodvjr_back_table[quest_packet.scource] == -1)
							{
								car[quest_packet.current]->aodvjr_back_table[quest_packet.scource] = traversal_packet;  //当前节点建立到源的反向路由
								//cout << "反向路由建立从当前位置：" << quest_packet.current << "到源" << quest_packet.scource << "的反向路由为" << car[quest_packet.current]->aodvjr_back_table[quest_packet.scource] << endl;
								car[quest_packet.current]->aodvjr_back_expiry[quest_packet.scource] = aodvjr_route_expiry;
							}
							if (car[car[i->current]->reachable[j]]->aodvjr_saved_ID[quest_packet.scource][quest_packet.destination] < quest_packet.ID)  //接收节点判断下，新的ID应该比旧的要大，否则不接收
							{
								//car[car[i->current]->reachable[j]]->RREQ_queue.push_back(quest_packet);
								car[car[i->current]->reachable[j]]->virtual_RREQ_queue.push_back(quest_packet);
								car[car[i->current]->reachable[j]]->aodvjr_saved_ID[quest_packet.scource][quest_packet.destination] = quest_packet.ID;
							}
						}
					}
					car[traversal_packet]->RREQ_queue.erase(i++);  //数据包已经转发 把原来的删了
				}
			}
		}
	}
}

void packet_delivery(node** car, int N, int aodvjr_route_expiry, long int &packet_arrive, long int &packet_drop, long int &packet_life_time,long long int &control_packet, int packet_expiry, int aodv_RREQ_expiry)
{
	for (int traversal_packet = 0; traversal_packet<N; traversal_packet++)   //数据包
	{
		for (list<packet>::iterator i = car[traversal_packet]->queue.begin(); i != car[traversal_packet]->queue.end();)
		{
			if (i->T>packet_expiry)
			{
				//cout << "数据包过期失效" << endl;
				packet_drop++;//计数器
				car[traversal_packet]->queue.erase(i++);
			}
			else
			{
				//cout << traversal_packet << "号节点数据包是从" << i->current << "到" << i->destination << endl;
				if (i->current == i->destination)  //已经送达
				{
					//cout << "数据包成功送达" << endl;
					packet_life_time = packet_life_time + i->T;
					packet_arrive++;//计数器
					car[traversal_packet]->queue.erase(i++);
				}
				else
				{
					if (car[i->current]->aodvjr_table[i->destination] == -1)   //不知道怎么发
					{
						car[i->current]->aodvjr_quest_times[i->destination] = car[i->current]->aodvjr_quest_times[i->destination] + 1;  //当前对目的的请求次数加了
						if (car[i->current]->aodvjr_quest_times[i->destination] % (aodv_RREQ_expiry*2)==1)   //按返回包最大生存寿命的两倍为周期，反复发请求
						{
							for (int j = 0; j < car[i->current]->reachable.size(); j++)
							{
								aodvjr_RREQ_packet quest_packet;
								quest_packet.ID = car[i->current]->aodvjr_quest_times[i->destination];
								quest_packet.current = car[i->current]->reachable[j];
								quest_packet.scource = i->current;
								quest_packet.destination = i->destination;
								quest_packet.T = 1;
								//cout << "发出请求报文" << "其源节点为" << quest_packet.scource << "目的节点为" << quest_packet.destination << "当前位置为" << quest_packet.current << "ID为" << quest_packet.ID<< endl;
								control_packet++;
								if (car[quest_packet.current]->aodvjr_back_table[quest_packet.scource] == -1)
								{
									car[quest_packet.current]->aodvjr_back_table[quest_packet.scource] = traversal_packet;  //当前节点建立到源的反向路由
									//cout << "反向路由建立从当前位置：" << quest_packet.current << "到源" << quest_packet.scource << "的反向路由为" << car[quest_packet.current]->aodvjr_back_table[quest_packet.scource] << endl;
									car[quest_packet.current]->aodvjr_back_expiry[quest_packet.scource] = aodvjr_route_expiry;
								}
								//car[car[i->current]->reachable[j]]->RREQ_queue.push_back(quest_packet);
								car[car[i->current]->reachable[j]]->virtual_RREQ_queue.push_back(quest_packet);
								car[car[i->current]->reachable[j]]->aodvjr_saved_ID[quest_packet.scource][quest_packet.destination] = quest_packet.ID;  //邻居内更新至最新的编号
							}
						}
						i->T++;
						packet  new_packet = *i;
						car[new_packet.current]->virtual_queue.push_back(new_packet);
						i++;   //数据包需要保留复制进虚拟队列，迭代器向前
					}
					else  //知道数据包怎么发
					{
						packet  new_packet = *i;
						int temp = car[i->current]->aodvjr_table[i->destination];
						new_packet.current = temp;
						new_packet.T = i->T + 1;
						if (car[i->current]->is_reachable[temp] == 1)   //实际上这个链路也是存在的
						{
							//car[temp]->queue.push_back(new_packet);
							car[temp]->virtual_queue.push_back(new_packet);
							if (car[temp]->aodvjr_back_table[i->scource] == i->current)   //如果反向路由表里面记录的正是送数据包过来的这个邻居，则反向路由保持有效
							{
								car[temp]->aodvjr_back_expiry[i->scource] = aodvjr_route_expiry;
								//cout << "源" << i->scource << "从" << i->current << "送到" << temp << "目的" << i->destination << "记录的反向路由为" << car[temp]->aodvjr_back_table[i->scource] << "匹配，反向路由更新存活时间" << endl;
							}
							else
							{
								//cout << "源" << i->scource << "从" << i->current << "送到" << temp << "目的" << i->destination << "记录的反向路由为" << car[temp]->aodvjr_back_table[i->scource] <<"并不匹配"<< endl;
							}
						}
						else
						{
							//cout << "源" << i->scource << "从" << i->current << "送到" << temp << "目的" << i->destination << "由于链路不通没有送达" << endl;
							packet_drop++;
						}
						car[traversal_packet]->queue.erase(i++);
					}
				}
			}
		}
	}
}

void check_in_cluster(node** car, int N, int cluster_radius)   //检查是否在簇中
{
	for (int i = 0; i < N; i++)
	{
		int in_or_not = 0;
		for (int j = 0; j < car[i]->reachable.size(); j++)
		{
			if (car[car[i]->reachable[j]]->cbc_isbone == 1 || car[car[i]->reachable[j]]->cbc_ishead == 1)
			{
				in_or_not = 1;  //如果有任何一个邻居是簇头或者骨干
			}
		}
		car[i]->cbc_incluster = in_or_not;
	}
}

void announce_cbchead(node** car, int N, int cluster_radius, long int &control_packet)
{
	for (int i = 0; i < N; i++)
	{
		if (car[i]->cbc_incluster==0)  //不在簇中
		{
			if (car[i]->stop_announce == 0)   //暂停期满，成功发送请求
			{
				for (int o = 0; o < N; o++)   //把自己知道的节点的控制关系清空
				{
					car[i]->node_control[o].clear();
				}
				car[i]->saved_path.clear();  //把返回包返回的路径清空
				car[i]->cluster_control.clear(); //把簇头控制的全部节点编号清空
				for (int j = 0; j < car[i]->reachable.size(); j++)  //向邻居广播簇头声明
				{
					if (car[car[i]->reachable[j]]->cbc_ishead == 0 && car[car[i]->reachable[j]]->cbc_isbone == 0)  //邻居既不是簇头也不是骨干
					{
						cbc_head_annouce_packet new_cbc_head_announce_packet;
						for (int k = 0; k < N; k++)
						{
							new_cbc_head_announce_packet.is_passed_by.push_back(0);
						}
						new_cbc_head_announce_packet.current = car[i]->reachable[j];
						new_cbc_head_announce_packet.scource = i;
						new_cbc_head_announce_packet.T = 1;
						new_cbc_head_announce_packet.degree = car[i]->reachable.size();  //把节点的度加入请求包
						new_cbc_head_announce_packet.cbc_announce_path.push_back(i);  //把源节点位置位置加入路径之中
						new_cbc_head_announce_packet.is_passed_by[i] = 1;  //源节点一定是经过了的
						control_packet++;
						car[new_cbc_head_announce_packet.current]->virtual_head_announce_queue.push_back(new_cbc_head_announce_packet);  //压入虚拟队列
					}
				}
				car[i]->stop_announce = car[i]->stop_announce + cluster_radius * 3 + 2;  //成功发送了请求后先暂停，避免数据包洪泛
				car[i]->wait_to_create_head = cluster_radius * 3 + 1;   //等待一段时间生成簇头
			}
			else
			{
				car[i]->stop_announce--;  //依旧处在暂停期，啥也不做
				car[i]->wait_to_create_head--;
			}	
		}
	}
}

void handle_cbchead_announce(node** car, int N, int cluster_radius, int block_width, int forecast_time,long int &control_packet)  //处理簇头声明
{
	for (int i = 0; i < N; i++)
	{
		for (list<cbc_head_annouce_packet>::iterator j = car[i]->head_announce_queue.begin(); j != car[i]->head_announce_queue.end();)  //这里i++ 写在下面,需要处理一些删除操作
		{
			if (car[i]->cbc_ishead == 0 || car[i]->cbc_isbone == 0)  //不在簇中
			{
				if (car[i]->virtual_move(*car[j->cbc_announce_path.back()], block_width, N, forecast_time) == 1)  //这是大前提，链路不断
				{
					if (car[i]->reachable.size() > j->degree || (car[i]->reachable.size() == j->degree&&i<j->scource))  //如果节点的度大于收到数据包的度,又或者度相等但是ID比较小
					{
						if (car[i]->cbc_incluster==0)   //并且跟请求节点一样,不在簇中，才发驳回包
						{
							cbc_deny_packet new_cbc_deny_packet;
							new_cbc_deny_packet.destination = j->scource; //驳回包的目的为请求包的源
							new_cbc_deny_packet.deny_path = j->cbc_announce_path;
							int temp = new_cbc_deny_packet.deny_path.back();  //驳回包将要去的下一步
							new_cbc_deny_packet.current = temp;
							new_cbc_deny_packet.deny_path.pop_back();
							if (car[i]->is_reachable[temp] == 1)  //如果这条链路确实存在
							{
								car[temp]->virtual_cbc_deny_packet_queue.push_back(new_cbc_deny_packet);  //将驳回包压入返回队列
								control_packet++;
							}
						}
					}
					else //如果节点的度小于等于收到的数据包的度
					{
						if (car[i]->my_max_announce_received.is_hearing==1)   //如果我还在接听
						{
							if (car[i]->my_max_announce_received.max_announce_node_ID == -1)    //一条内容都没
							{
								car[i]->my_max_announce_received.wait_time_window = cluster_radius;    //从我收到第一个请求包开始,过一会儿才确定到底当谁的簇头
								car[i]->my_max_announce_received.max_announce_node_ID = j->scource;
								car[i]->my_max_announce_received.max_announce_node_degree = j->degree;
								car[i]->my_max_announce_received.all_virtual_cbc_agree_packet.clear();
								cbc_agree_packet new_cbc_agree_packet;
								new_cbc_agree_packet.destination = j->scource; //同意包的目的为请求包的源
								new_cbc_agree_packet.scource = i;   //当前节点为同意包的源
								new_cbc_agree_packet.agree_path = j->cbc_announce_path; //同意包的路径为请求包的路径
								new_cbc_agree_packet.report_path = new_cbc_agree_packet.agree_path;  //汇报路径为返回路径
								new_cbc_agree_packet.report_path.push_back(i);//汇报路径把源也算进去 
								new_cbc_agree_packet.depth = j->T;    //数据包的生存时间为同意包的深度
								new_cbc_agree_packet.control_neighbor = car[i]->reachable;  //把控制哪些邻居写进包里
								new_cbc_agree_packet.current = new_cbc_agree_packet.agree_path.back();
								new_cbc_agree_packet.agree_path.pop_back();
								car[i]->my_max_announce_received.all_virtual_cbc_agree_packet.push_back(new_cbc_agree_packet);   //加入队列
							}
							else   //我还是有内容的，也就是之前收到过生命包
							{
								if (j->scource==car[i]->my_max_announce_received.max_announce_node_ID)    //如果跟我之前记录中的是同一个节点
								{
									cbc_agree_packet new_cbc_agree_packet;
									new_cbc_agree_packet.destination = j->scource; //同意包的目的为请求包的源
									new_cbc_agree_packet.scource = i;   //当前节点为同意包的源
									new_cbc_agree_packet.agree_path = j->cbc_announce_path; //同意包的路径为请求包的路径
									new_cbc_agree_packet.report_path = new_cbc_agree_packet.agree_path;  //汇报路径为返回路径
									new_cbc_agree_packet.report_path.push_back(i);//汇报路径把源也算进去 
									new_cbc_agree_packet.depth = j->T;    //数据包的生存时间为同意包的深度
									new_cbc_agree_packet.control_neighbor = car[i]->reachable;  //把控制哪些邻居写进包里
									new_cbc_agree_packet.current = new_cbc_agree_packet.agree_path.back();
									new_cbc_agree_packet.agree_path.pop_back();
									car[i]->my_max_announce_received.all_virtual_cbc_agree_packet.push_back(new_cbc_agree_packet);   //加入队列
								}
								else
								{
									if (j->degree > car[i]->my_max_announce_received.max_announce_node_degree || (j->degree == car[i]->my_max_announce_received.max_announce_node_degree&&j->scource < car[i]->my_max_announce_received.max_announce_node_ID))   //要么度大，要么度相同编号小,发现更好的了，变心了
									{
										car[i]->my_max_announce_received.max_announce_node_ID = j->scource;
										car[i]->my_max_announce_received.max_announce_node_degree = j->degree;
										car[i]->my_max_announce_received.all_virtual_cbc_agree_packet.clear();
										cbc_agree_packet new_cbc_agree_packet;
										new_cbc_agree_packet.destination = j->scource; //同意包的目的为请求包的源
										new_cbc_agree_packet.scource = i;   //当前节点为同意包的源
										new_cbc_agree_packet.agree_path = j->cbc_announce_path; //同意包的路径为请求包的路径
										new_cbc_agree_packet.report_path = new_cbc_agree_packet.agree_path;  //汇报路径为返回路径
										new_cbc_agree_packet.report_path.push_back(i);//汇报路径把源也算进去 
										new_cbc_agree_packet.depth = j->T;    //数据包的生存时间为同意包的深度
										new_cbc_agree_packet.control_neighbor = car[i]->reachable;  //把控制哪些邻居写进包里
										new_cbc_agree_packet.current = new_cbc_agree_packet.agree_path.back();
										new_cbc_agree_packet.agree_path.pop_back();
									}
								}
							}
						}
					}
					if (j->T < cluster_radius)  //没有到达最大搜索深度,可以把请求包转发一次
					{			
						for (int k = 0; k < car[i]->reachable.size(); k++)  //向邻居广播簇头声明
						{
						    cbc_head_annouce_packet new_head_annouce_packet = *j;  //新的请求转发给邻居
							new_head_annouce_packet.current = car[i]->reachable[k];
							new_head_annouce_packet.T = new_head_annouce_packet.T+1;
							new_head_annouce_packet.cbc_announce_path.push_back(i);  //把转发点位置加入路径之中
							new_head_annouce_packet.is_passed_by[i] = 1;  //转发节点也被经过了
							if (new_head_annouce_packet.is_passed_by[car[i]->reachable[k]] == 0 && car[car[i]->reachable[k]]->cbc_isbone == 0 && car[car[i]->reachable[k]]->cbc_ishead == 0)   //只有这个包没有经过邻居,并且邻居不是簇头也不是骨干
							{
								car[new_head_annouce_packet.current]->virtual_head_announce_queue.push_back(new_head_annouce_packet);  //压入虚拟队列
							}
						}
					}
				}
			}
			car[i]->head_announce_queue.erase(j++); //转发处理等都结束了，本簇头声明删除
		}
	}
}

void handle_deny_packet(node** car, int N, int cluster_radius, int stop_time_increase)
{
	for (int i = 0; i < N; i++)
	{
		for (list<cbc_deny_packet>::iterator j = car[i]->cbc_deny_packet_queue.begin(); j != car[i]->cbc_deny_packet_queue.end();)  //这里i++ 写在下面,需要处理一些删除操作
		{
			if (j->destination == j->current)   //已经送到目的地
			{
				car[i]->wait_to_create_head = stop_time_increase;  //收到了驳回包，暂停时间增加，暂停建立簇
			}
			else  //没送到
			{
				cbc_deny_packet new_deny_packet = *j;
				new_deny_packet.current = new_deny_packet.deny_path.back(); //驳回包将要去的下一步
				new_deny_packet.deny_path.pop_back();
				if (car[i]->is_reachable[new_deny_packet.current] == 1)  //如果这条链路确实存在
				{
					car[new_deny_packet.current]->virtual_cbc_deny_packet_queue.push_back(new_deny_packet);  //将驳回包压入返回队列
				}
			}
			car[i]->cbc_deny_packet_queue.erase(j++); //处理结束，删除驳回包
		}
	}
}

void check_agree_time_window(node** car, int N, int cluster_radius)
{
	for (int i = 0; i < N; i++)
	{
		if (car[i]->my_max_announce_received.wait_time_window<=0&&car[i]->my_max_announce_received.max_announce_node_ID!=-1)  //如果时间窗口为0 且我里面记录了一个人发来的东西
		{
			for (int j = 0; j < car[i]->my_max_announce_received.all_virtual_cbc_agree_packet.size(); j++)
			{
				int temp = car[i]->my_max_announce_received.all_virtual_cbc_agree_packet[j].current;
				car[temp]->virtual_cbc_agree_packet_queue.push_back(car[i]->my_max_announce_received.all_virtual_cbc_agree_packet[j]);  //把暂时包的全部内容往回发
			}
			car[i]->my_max_announce_received.max_announce_node_ID = -1;
			car[i]->my_max_announce_received.max_announce_node_degree = -1;
			car[i]->my_max_announce_received.is_hearing = 0;   //不再接听了
			car[i]->my_max_announce_received.all_virtual_cbc_agree_packet.clear();
			car[i]->my_max_announce_received.wait_time_window = 0; //同意包发完后等待一段时间（等待簇头建立） 决定是否要继续接听
		}
		if (car[i]->my_max_announce_received.wait_time_window <= 0 && car[i]->my_max_announce_received.max_announce_node_ID == -1)  //同意包发完过了一段时间了，自己没记录，可以继续接听
		{
			car[i]->my_max_announce_received.is_hearing = 1;
		}
		if (car[i]->my_max_announce_received.wait_time_window>0)
		{
			car[i]->my_max_announce_received.wait_time_window--;   //时间窗口自减
		}
	}
}

void handle_agree_packet(node** car, int N, int cluster_radius)
{
	for (int i = 0; i < N; i++)
	{
		for (list<cbc_agree_packet>::iterator j = car[i]->cbc_agree_packet_queue.begin(); j != car[i]->cbc_agree_packet_queue.end();)  //这里i++ 写在下面,需要处理一些删除操作
		{
			if (j->destination == j->current)   //收到了同意包，路径存储建立，准备建立簇，
			{
				car[i]->saved_path.push_back(j->report_path); //把汇报路径存到节点的内容之中去
				if (car[i]->node_control[j->scource].empty()==true)  //如果i对目标节点信息---支配集里面没内容
				{
					
					car[i]->node_control[j->scource] = j->control_neighbor;  //把控制的多少邻居的信息报告给簇头，据i簇头所知，j->scource控制了这些邻居
				}
			}
			else  //没送到
			{
				cbc_agree_packet new_agree_packet = *j;
				new_agree_packet.current = new_agree_packet.agree_path.back();  //驳回包将要去的下一步
				new_agree_packet.agree_path.pop_back();
				if (car[i]->is_reachable[new_agree_packet.current] == 1)  //如果这条链路确实存在
				{
					car[new_agree_packet.current]->virtual_cbc_agree_packet_queue.push_back(new_agree_packet);  //将驳回包压入返回队列
				}
			}
			car[i]->cbc_agree_packet_queue.erase(j++); //处理结束，删除驳回包
		}
	}
}

void create_cluster(node** car, int N, int cluster_radius,int tail_validity)   //生成簇
{
	for (int i = 0; i < N; i++)  //遍历全部节点
	{
		if (car[i]->cbc_ishead == 0 && car[i]->cbc_incluster == 0)   //既不是簇头也没有在簇中
		{
			if (car[i]->wait_to_create_head ==0)  //表示请求包发完过了一段时间了
			{
				car[i]->cbc_ishead = 1; //簇头建立了，我就是簇头
				car[i]->cbc_isbone = 0; 
				car[i]->cbc_incluster = 1;//我在簇里面了
				car[i]->inform_interval = 0;
				car[i]->mainstay_1.clear();
				car[i]->mainstay_2.clear();//骨干网清零
				car[i]->cbc_tail_1_validity = tail_validity;  //两个展臂还是有存活时间
				car[i]->cbc_tail_2_validity = tail_validity;
				car[i]->cbc_lonely_validity = tail_validity;
				car[i]->bone_of_whom = i;  //我作为簇头，自己是自己的骨干网
				if (car[i]->saved_path.size()>=2)    //至少收到两个同意包
				{
					int max_control = 0;
					int record_1 = 0;  //两个路径的编号
					int record_2 = 0;
					vector<int> path_control_max;
					vector<int> path_control_mainstay_1_max;
					vector<int> path_control_mainstay_2_max;
					for (int n = 0; n < car[i]->saved_path.size(); n++)    //遍历全部的路径对
					{
						for (int l = 0; l < n; l++)    //遍历全部的路径对
						{
							vector<int>   path_control_node;   //一个临时数组，把路径上全部的能控制的点加入这个数组
							vector<int> path_control_mainstay_1;
							vector<int> path_control_mainstay_2;
							for (int m = 0; m < car[i]->saved_path[n].size(); m++)
							{
								path_control_node.insert(path_control_node.end(), car[i]->node_control[car[i]->saved_path[n][m]].begin(), car[i]->node_control[car[i]->saved_path[n][m]].end());   //把路径上能控制的点全部压进去
								path_control_mainstay_1.insert(path_control_mainstay_1.end(), car[i]->node_control[car[i]->saved_path[n][m]].begin(), car[i]->node_control[car[i]->saved_path[n][m]].end());  //一号展臂的赋值给1
							}
							for (int o = 0; o < car[i]->saved_path[l].size(); o++)
							{
								path_control_node.insert(path_control_node.end(), car[i]->node_control[car[i]->saved_path[l][o]].begin(), car[i]->node_control[car[i]->saved_path[l][o]].end());   //把路径上能控制的点全部压进去								
								path_control_mainstay_2.insert(path_control_mainstay_2.end(), car[i]->node_control[car[i]->saved_path[l][o]].begin(), car[i]->node_control[car[i]->saved_path[l][o]].end());  //二号展臂的赋值给2
							}
							path_control_node.insert(path_control_node.end(), car[i]->reachable.begin(), car[i]->reachable.end());   //簇头自己的能控制的邻居也纳入计算
							sort(path_control_node.begin(), path_control_node.end());
							path_control_node.erase(unique(path_control_node.begin(), path_control_node.end()), path_control_node.end());  //先排序，再去重
							sort(path_control_mainstay_1.begin(), path_control_mainstay_1.end());
							path_control_mainstay_1.erase(unique(path_control_mainstay_1.begin(), path_control_mainstay_1.end()), path_control_mainstay_1.end());  //先排序，再去重	
							sort(path_control_mainstay_2.begin(), path_control_mainstay_2.end());
							path_control_mainstay_2.erase(unique(path_control_mainstay_2.begin(), path_control_mainstay_2.end()), path_control_mainstay_2.end());  //先排序，再去重	
							if (path_control_node.size()>max_control) //路径控制的节点数比记录的要大，比较出一个控制能力最强的路径
							{
								max_control = path_control_node.size();
								record_1 = n;   //记录下最大路径的对应编号
								record_2 = l;
								path_control_max.clear();
								path_control_max = path_control_node;
								path_control_mainstay_1_max.clear();
								path_control_mainstay_2_max.clear();
								path_control_mainstay_1_max = path_control_mainstay_1;
								path_control_mainstay_2_max = path_control_mainstay_2;
							}
							/*
							cout << "路径为：";
							for (int p = 0; p < car[i]->saved_path[n].size(); p++)
							{
								cout << car[i]->saved_path[n][p] << " ";
							}
							for (int p = 0; p < car[i]->saved_path[l].size(); p++)
							{
								cout << car[i]->saved_path[l][p] << " ";
							}
							cout << endl;
							cout << "路径控制的节点为：";
							for (int q = 0; q < path_control_node.size(); q++)
							{
								cout << path_control_node[q] << " ";
							}
							cout << endl;
							*/				
						}
					}
					car[i]->cluster_control = path_control_max;
					car[i]->mainstay_1_control = path_control_mainstay_1_max;
					car[i]->mainstay_2_control = path_control_mainstay_2_max;
					//cout <<i<< "为簇头，簇建立，第一条展臂";
					for (int u = 0; u < car[i]->saved_path[record_1].size(); u++)
					{	
						if (car[i]->saved_path[record_1][u]!=i)
						{		
							car[i]->mainstay_1.push_back(car[i]->saved_path[record_1][u]);
						//	cout << car[i]->saved_path[record_1][u] <<" ";
						}	
					}
				//	cout << "第二条展臂";
					for (int u = 0; u < car[i]->saved_path[record_2].size(); u++)
					{
						if (car[i]->saved_path[record_2][u] != i)
						{
							car[i]->mainstay_2.push_back(car[i]->saved_path[record_2][u]);
					//		cout << car[i]->saved_path[record_2][u] << " ";
						}
					}
				//	cout << endl;
				//	cout << "第一条展臂，控制的节点";
					for (int u = 0; u < car[i]->mainstay_1_control.size(); u++)
					{
				//		cout << car[i]->mainstay_1_control[u] << " ";
					}
				//	cout << "第二条展臂，控制的节点";
					for (int u = 0; u < car[i]->mainstay_2_control.size(); u++)
					{
				//		cout << car[i]->mainstay_2_control[u] << " ";
					}
				//	cout << endl;
				//	cout << "整个簇，控制的节点";
					for (int u = 0; u < car[i]->cluster_control.size(); u++)
					{
				//		cout << car[i]->cluster_control[u] << " ";
					}
				//	cout << endl;
				}

				if (car[i]->saved_path.size() ==1)    //只收到一个同意包，也就只有一条路径
				{
					vector<int>  path_control_node;   //一个临时数组，把路径上全部的能控制的点加入这个数组
					vector<int> path_control_mainstay_1;
					for (int k = 0; k < car[i]->saved_path[0].size(); k++)  //唯一的一个返回包
					{
						path_control_node.insert(path_control_node.end(), car[i]->node_control[car[i]->saved_path[0][k]].begin(), car[i]->node_control[car[i]->saved_path[0][k]].end());   //把路径上能控制的点全部压进去
						car[i]->mainstay_1.push_back(car[i]->saved_path[0][k]);
					}
					sort(path_control_node.begin(), path_control_node.end());
					path_control_node.erase(unique(path_control_node.begin(), path_control_node.end()), path_control_node.end());  //先排序，再去重
					path_control_mainstay_1 = path_control_node;
					path_control_node.insert(path_control_node.end(), car[i]->reachable.begin(), car[i]->reachable.end());   //簇头自己的能控制的邻居也纳入计算
					car[i]->cluster_control = path_control_node;
					car[i]->mainstay_1_control = path_control_mainstay_1;
				//	cout << "簇建立，只有一条展臂";
					for (list<int>::iterator q = car[i]->mainstay_1.begin(); q != car[i]->mainstay_1.end(); q++)  //唯一的一个返回包
					{
				//		cout << *q ;
					}
				//	cout << endl;
				}
				if (car[i]->saved_path.size() == 0)   //没收到数据包 独立成簇
				{
					car[i]->cluster_control = car[i]->reachable;   //自己控制自己的邻居
					car[i]->cbc_lonely_validity = tail_validity;
				}
			}
		}
	}
}

void create_routing_table(node** car, int N,int time)
{
	for (int i = 0; i < N; i++)  //遍历全部节点
	{
		if (car[i]->cbc_ishead == 1 && car[i]->cbc_routing_table.size() == 0)  //如果是簇头节点,并且路由表是空的，生成一份路由表
		{
			//cout << i << "开始建立路由表" << endl;
			for (int j = 0; j < N; j++)  //遍历全部节点，这个是目的地
			{
				if (i != j)
				{
					cbc_routing_table_element new_routing_table_element; //新的路由表元素
					int is_delivered = 0;
					if (car[i]->is_reachable[j] == 1)  //本簇头可以直接够到目的地
					{
						//cout << i << "到" << j << "可以直接送达" << endl;
						new_routing_table_element.cluster_head = i; //簇头就是本身
						new_routing_table_element.in_this_cluster = true;
						new_routing_table_element.gateway_mainstay_number = 0;
						new_routing_table_element.gateway = -1;
						new_routing_table_element.length = 0;
						new_routing_table_element.time_stamp = time;
						is_delivered = 1;
					}
					if (is_delivered == 0)  //还没送出去,试一试第一条展臂
					{
						for (int f = 0; f < car[i]->mainstay_1_control.size(); f++)
						{
							if (car[i]->mainstay_1_control[f] == j)  //第一条展臂里面找到了目的
							{
								new_routing_table_element.cluster_head = i; //簇头就是本身
								new_routing_table_element.in_this_cluster = true;  //在本簇中
								new_routing_table_element.gateway_mainstay_number = 1;  //第一条展臂可以送达
								new_routing_table_element.gateway = -1;
								new_routing_table_element.length = 0;
								new_routing_table_element.time_stamp = time;
								is_delivered = 1;
								//cout << i << "到" << j << "可以通过第一条展臂送达" << endl;
								break;
							}
						}
					}
					if (is_delivered == 0)  //还没送出去，试一试第二条展臂
					{
						for (int g = 0; g < car[i]->mainstay_2_control.size(); g++)
						{
							if (car[i]->mainstay_2_control[g] == j)
							{
								new_routing_table_element.cluster_head = i; //簇头就是本身
								new_routing_table_element.in_this_cluster = true;  //在本簇中
								new_routing_table_element.gateway_mainstay_number = 2;//第二条展臂可以送达
								new_routing_table_element.gateway = -1;
								new_routing_table_element.length = 0;
								new_routing_table_element.time_stamp = time;
								is_delivered = 1;
								//cout << i << "到" << j << "可以通过第二条展臂送达" << endl;
								break;
							}
						}
					}
					if (is_delivered == 0)   //两条展臂都没有信息
					{
						new_routing_table_element.cluster_head = -1;  //不知道目的地是哪个簇下面的
						new_routing_table_element.in_this_cluster = false;  //不在本簇中
						new_routing_table_element.gateway_mainstay_number = -1;
						new_routing_table_element.gateway = -1;
						new_routing_table_element.length = M_A_X;      //距离最大
						new_routing_table_element.time_stamp = time;
						//cout << i << "到" << j << "在本簇内没有可用的路由信息" << endl;
					}
					car[i]->cbc_routing_table.push_back(new_routing_table_element);  //不管什么情况，坑还是要填满的
				}
				else   //自己到自己，也生成个路由表
				{
					cbc_routing_table_element empty_routing_table_element; //新的路由表元素
					empty_routing_table_element.cluster_head = j;
					empty_routing_table_element.in_this_cluster = 1;
					empty_routing_table_element.gateway_mainstay_number = 0;
					empty_routing_table_element.length = 0;
					empty_routing_table_element.time_stamp = time;
					car[i]->cbc_routing_table.push_back(empty_routing_table_element);  //不管什么情况，坑还是要填满的
				}
			}
		}
	}
}

void arrange_routing_table(node** car, int N, int time)
{
	for (int i = 0; i < N; i++)  //遍历全部节点
	{
		if (car[i]->cbc_ishead == 1 && car[i]->cbc_routing_table.size() != 0)  //如果是簇头节点,并且路由表不是空的，整理路由表
		{
			//cout << i << "开始整理路由表" << endl;
			for (int j = 0; j < N; j++)  //遍历全部节点，这个是目的地
			{
				if (i != j)
				{
					int is_delivered = 0;
					if (car[i]->is_reachable[j] == 1)  //本簇头可以直接够到目的地
					{
					//	cout << i << "到" << j << "可以直接送达" << endl;
						car[i]->cbc_routing_table[j].cluster_head = i; //簇头就是本身
						car[i]->cbc_routing_table[j].in_this_cluster = true;
						car[i]->cbc_routing_table[j].gateway_mainstay_number = 0;
						car[i]->cbc_routing_table[j].gateway = -1;
						car[i]->cbc_routing_table[j].length = 0;
						car[i]->cbc_routing_table[j].time_stamp = time;
						is_delivered = 1;
					}
					if (is_delivered == 0)  //还没送出去,试一试第一条展臂
					{
						for (int f = 0; f < car[i]->mainstay_1_control.size(); f++)
						{
							if (car[i]->mainstay_1_control[f] == j)  //第一条展臂里面找到了目的
							{
								car[i]->cbc_routing_table[j].cluster_head = i; //簇头就是本身
								car[i]->cbc_routing_table[j].in_this_cluster = true;  //在本簇中
								car[i]->cbc_routing_table[j].gateway_mainstay_number = 1;  //第一条展臂可以送达
								car[i]->cbc_routing_table[j].gateway = -1;
								car[i]->cbc_routing_table[j].length = 0;
								car[i]->cbc_routing_table[j].time_stamp = time;
								is_delivered = 1;
						//		cout << i << "到" << j << "可以通过第一条展臂送达" << endl;
								break;
							}
						}
					}
					if (is_delivered == 0)  //还没送出去，试一试第二条展臂
					{
						for (int g = 0; g < car[i]->mainstay_2_control.size(); g++)
						{
							if (car[i]->mainstay_2_control[g] == j)
							{
								car[i]->cbc_routing_table[j].cluster_head = i; //簇头就是本身
								car[i]->cbc_routing_table[j].in_this_cluster = true;  //在本簇中
								car[i]->cbc_routing_table[j].gateway_mainstay_number = 2;//第二条展臂可以送达
								car[i]->cbc_routing_table[j].gateway = -1;
								car[i]->cbc_routing_table[j].length = 0;
								car[i]->cbc_routing_table[j].time_stamp = time;
								is_delivered = 1;
						//		cout << i << "到" << j << "可以通过第二条展臂送达" << endl;
								break;
							}
						}
					}
					if (is_delivered == 0)   //两条展臂都没有信息
					{
						if (car[i]->cbc_routing_table[j].cluster_head!=-1)
						{
					//		cout << i << "到" << j << "可以通过" << car[i]->cbc_routing_table[j].cluster_head << "簇头,通过" << car[i]->cbc_routing_table[j].gateway_mainstay_number << "号展臂的" << car[i]->cbc_routing_table[j].gateway << "网关,距离为" << car[i]->cbc_routing_table[j].length << "时间戳为" << car[i]->cbc_routing_table[j].time_stamp << endl;
						}
						else
						{
					//		cout << i << "到" << j << "没有可用的路径" << endl;
						}
						
					}
				}
				else   //自己到自己，也生成个路由表
				{
					car[i]->cbc_routing_table[j].cluster_head = j;
					car[i]->cbc_routing_table[j].in_this_cluster = 1;
					car[i]->cbc_routing_table[j].gateway_mainstay_number = 0;
					car[i]->cbc_routing_table[j].length = 0;
					car[i]->cbc_routing_table[j].time_stamp = time;
				}
			}
		}
	}
}

void inform_cluster(node** car, int N, int cluster_radius, int inform_interval, int time, long int &control_packet)   //通知骨干网,本身自己的信息带时间戳
{
	for (int i = 0; i < N; i++)  //遍历全部节点
	{
		if (car[i]->cbc_ishead==1)  //如果是簇头
		{
			if (car[i]->inform_interval == 0)   //满足周期
			{
				cbc_inform_packet new_inform_packet;   //新的通知数据包
				new_inform_packet.scource = i;
				new_inform_packet.cbc_routing_table_packeted = car[i]->cbc_routing_table;   //把路由表打包放进名片
				new_inform_packet.effective = 1; //通知包有效的
				new_inform_packet.near = i;  //近端是自己
				new_inform_packet.time_stamp = time;  //数据包也带上时间戳
				car[i]->only_inform_packet = new_inform_packet;//自己本身的名片也是他
				if (car[i]->mainstay_1.empty() == false) //第一条展臂不是空的
				{
					new_inform_packet.mainstay = car[i]->mainstay_1; //第一条展臂的拓扑复制过来
					new_inform_packet.mainstay.pop_front();  //删掉队列第一位的元素，展臂只剩下后面的拓扑内容
					if (car[i]->is_reachable[car[i]->mainstay_1.front()]) //这条边真实存在
					{
						//cout << i << "作为簇头，向第一条展臂的" << car[i]->mainstay_1.front() << "骨干转发通知包" << endl;
						car[car[i]->mainstay_1.front()]->virtual_only_inform_packet = new_inform_packet;  //把簇信息赋值给虚拟的名片
						control_packet++;
					}
				}
				if (car[i]->mainstay_2.empty() == false) //第二条展臂不是空的
				{
					
					new_inform_packet.mainstay.clear();
					new_inform_packet.mainstay = car[i]->mainstay_2;  //第二条展臂的拓扑信息复制过来
					new_inform_packet.mainstay.pop_front();
					if (car[i]->is_reachable[car[i]->mainstay_2.front()])  //这条边真实存在
					{
						//cout << i << "作为簇头，向第二条展臂的" << car[i]->mainstay_2.front() << "骨干转发通知包" << endl;
						car[car[i]->mainstay_2.front()]->virtual_only_inform_packet = new_inform_packet;
						control_packet++;
					}
				}
				car[i]->inform_interval = inform_interval;  //满足周期
			}
			else
			{
				car[i]->inform_interval--;
			}
		}
	}
}

void handle_inform_packet(node** car, int N, int cluster_radius)   //通知信息复制给骨干网
{
	for (int i = 0; i < N; i++)  //遍历全部节点
	{
		if (car[i]->only_inform_packet.effective == 1&&car[i]->cbc_ishead==0)  //如果自己的名片有效，且不是簇头
		{
			car[i]->cbc_isbone = 1;
			car[i]->cbc_ishead = 0;
			car[i]->cbc_incluster = 1;
			car[i]->bone_of_whom = car[i]->only_inform_packet.scource;
			car[i]->cbc_near = car[i]->only_inform_packet.near;
			cbc_inform_packet new_inform_packet = car[i]->only_inform_packet;   //新的名片
			new_inform_packet.near = i;
			if (new_inform_packet.mainstay.size() != 0)   //展臂还没结束
			{
				car[i]->cbc_far = car[i]->only_inform_packet.mainstay.front();
				if (car[i]->is_reachable[car[i]->only_inform_packet.mainstay.front()]==1)  //如果这条边真是存在
				{
					
					car[car[i]->only_inform_packet.mainstay.front()]->bone_of_whom = new_inform_packet.scource;  //我是簇头的骨干
					//cout << i << "作为" << car[i]->bone_of_whom << "的骨干节点,他的远端为,继续转发节点名片" << car[i]->cbc_far << endl;
					new_inform_packet.mainstay.pop_front();
					car[car[i]->only_inform_packet.mainstay.front()]->virtual_only_inform_packet = new_inform_packet;
				}
			}
			else
			{
				car[i]->cbc_far = -1;  //臂展结束，没有远端节点
			}
		}
	}
}

void receive_exchange_inform_packet(node** car, int N, int cluster_radius, int time, int refresh_interval)   //收到其他节点的名片，用以更新自己的路由表 时间戳相隔太大就放弃
{
	for (int i = 0; i < N; i++)  //遍历全部节点
	{
		if (car[i]->cbc_ishead == 1)    //如果是簇头，不是的话不理这闲篇子
		{
			for (int n = 0; n < N; n++)   //遍历路由表，查看时间戳是否有效
			{
				if (time - car[i]->cbc_routing_table[n].time_stamp > refresh_interval)     //时间戳过期了
				{
					car[i]->cbc_routing_table[n].cluster_head = -1;
					car[i]->cbc_routing_table[n].gateway = -1;
					car[i]->cbc_routing_table[n].length = M_A_X;  //距离设为最大
					car[i]->cbc_routing_table[n].time_stamp = time;
					car[i]->cbc_routing_table[n].in_this_cluster = false;
					car[i]->cbc_routing_table[n].gateway_mainstay_number = -1;
				}
			}
		}
		for (list<cbc_inform_packet>::iterator j = car[i]->exchange_packet_information_queue.begin(); j != car[i]->exchange_packet_information_queue.end();)  //这里i++ 写在下面,需要处理一些删除操作
		{
			if (car[i]->cbc_ishead == 1) //i节点他是簇头
			{
				for (int m = 0; m < N; m++)   //遍历路由表
				{
					if (car[i]->cbc_routing_table[m].in_this_cluster == 1)    //m节点通过本簇可以直接送达,不管的其他数据包说什么，不修改路由表
					{
					}
					else
					{
						if (j->cbc_routing_table_packeted[m].cluster_head != -1 && (time - j->cbc_routing_table_packeted[m].time_stamp < refresh_interval))     //发来的路由表中有关于m消息,并且时间戳是有效的
						{
							if (car[i]->cbc_routing_table[m].cluster_head == -1)  //我本身没有这条记录，发来的有这条记录
							{
								if (car[i]->cbc_routing_table[j->cbc_routing_table_packeted[m].gateway].gateway_mainstay_number != -1)   //查到网关的路径是有效的
								{
									car[i]->cbc_routing_table[m].cluster_head = j->cbc_routing_table_packeted[m].cluster_head;
									car[i]->cbc_routing_table[m].gateway = j->cbc_routing_table_packeted[m].gateway;
									car[i]->cbc_routing_table[m].length = j->cbc_routing_table_packeted[m].length;
									car[i]->cbc_routing_table[m].in_this_cluster = 0;
									car[i]->cbc_routing_table[m].time_stamp = j->cbc_routing_table_packeted[m].time_stamp;
									car[i]->cbc_routing_table[m].gateway_mainstay_number = car[i]->cbc_routing_table[j->cbc_routing_table_packeted[m].gateway].gateway_mainstay_number;
								}
							}
							else  //我本身有这条记录
							{
								if (j->cbc_routing_table_packeted[m].length < car[i]->cbc_routing_table[m].length)  //但是新送来的路由表中 长度比我自身记录的要短
								{
									if (car[i]->cbc_routing_table[j->cbc_routing_table_packeted[m].gateway].gateway_mainstay_number != -1)   //查到网关的路径是有效的
									{
										car[i]->cbc_routing_table[m].cluster_head = j->cbc_routing_table_packeted[m].cluster_head;
										car[i]->cbc_routing_table[m].gateway = j->cbc_routing_table_packeted[m].gateway;
										car[i]->cbc_routing_table[m].length = j->cbc_routing_table_packeted[m].length;
										car[i]->cbc_routing_table[m].in_this_cluster = 0;
										car[i]->cbc_routing_table[m].time_stamp = j->cbc_routing_table_packeted[m].time_stamp;
										car[i]->cbc_routing_table[m].gateway_mainstay_number = car[i]->cbc_routing_table[j->cbc_routing_table_packeted[m].gateway].gateway_mainstay_number;
									}
								}
								if (j->cbc_routing_table_packeted[m].length == car[i]->cbc_routing_table[m].length&&car[i]->cbc_routing_table[m].time_stamp < j->cbc_routing_table_packeted[m].time_stamp)  //长度加一跟自己的一样长 但是新发来的包里面信息 时间戳比自己的新
								{
									if (car[i]->cbc_routing_table[j->cbc_routing_table_packeted[m].gateway].gateway_mainstay_number != -1)   //查询到 到网关的路径是有效的
									{
										car[i]->cbc_routing_table[m].cluster_head = j->cbc_routing_table_packeted[m].cluster_head;
										car[i]->cbc_routing_table[m].gateway = j->cbc_routing_table_packeted[m].gateway;
										car[i]->cbc_routing_table[m].length = j->cbc_routing_table_packeted[m].length;
										car[i]->cbc_routing_table[m].in_this_cluster = 0;
										car[i]->cbc_routing_table[m].time_stamp = j->cbc_routing_table_packeted[m].time_stamp;
										car[i]->cbc_routing_table[m].gateway_mainstay_number = car[i]->cbc_routing_table[j->cbc_routing_table_packeted[m].gateway].gateway_mainstay_number;
									}
								}
							}
						}//j中有关于另一个簇的路由的信息
					}
				}//遍历路由表，查看时间戳是否有效
			}
			else   //i节点不是簇头，就不操这个心了,交给簇头节点处理
			{
				if (car[i]->cbc_isbone == 1)
				{
					cbc_inform_packet new_inform_packet = *j;
					if (car[i]->is_reachable[car[i]->cbc_near] == 1)   //如果链路真实存在
					{
						car[car[i]->cbc_near]->virtual_exchange_packet_information_queue.push_back(new_inform_packet);  //把这个通知信息往簇头发
					}
				}
			}
			car[i]->exchange_packet_information_queue.erase(j++);
		}
	}
}

void receive_maintain_packet(node** car, int N, int cluster_radius, int time, int tail_validity)   //收到维护报文
{
	for (int i = 0; i < N; i++)  //遍历全部节点
	{
		for (list<cbc_maintain_packet>::iterator j = car[i]->cbc_maintain_packet_queue.begin(); j != car[i]->cbc_maintain_packet_queue.end();)  //这里i++ 写在下面,需要处理一些删除操作
		{
			if (car[i]->cbc_ishead == 1) //i节点他是簇头
			{
				if (j->scource == car[i]->mainstay_1.back())  //一号臂送来的
				{

					car[i]->cbc_tail_1_validity = tail_validity;
					sort(j->path_control_neighbor.begin(), j->path_control_neighbor.end());
					j->path_control_neighbor.erase(unique(j->path_control_neighbor.begin(), j->path_control_neighbor.end()), j->path_control_neighbor.end());  //先排序，再去重
				//	cout << "簇头" << i << "收到了来自一号臂的维护包" << endl;
				//	cout << "一号臂控制的节点为";
					for (int k = 0; k < j->path_control_neighbor.size(); k++)
					{
						cout << j->path_control_neighbor[k] << " ";
					}
					cout << endl;
					car[i]->mainstay_1_control = j->path_control_neighbor;
				}
				if (j->scource == car[i]->mainstay_2.back())  //二号臂送来的
				{
					car[i]->cbc_tail_2_validity = tail_validity;
					sort(j->path_control_neighbor.begin(), j->path_control_neighbor.end());
					j->path_control_neighbor.erase(unique(j->path_control_neighbor.begin(), j->path_control_neighbor.end()), j->path_control_neighbor.end());  //先排序，再去重	
				//	cout << "簇头" << i << "收到了来自二号臂的维护包" << endl;
				//	cout << "二号臂控制的节点为";
					for (int k = 0; k < j->path_control_neighbor.size(); k++)
					{
						cout << j->path_control_neighbor[k] << " ";
					}
					cout << endl;
					car[i]->mainstay_2_control = j->path_control_neighbor;
				}
				vector<int> temp_whole_path_control;
				temp_whole_path_control.insert(temp_whole_path_control.end(), car[i]->reachable.begin(), car[i]->reachable.end());
				temp_whole_path_control.insert(temp_whole_path_control.end(), car[i]->mainstay_1_control.begin(), car[i]->mainstay_1_control.end());   //两条展臂跟自己的邻居全部加入
				temp_whole_path_control.insert(temp_whole_path_control.end(), car[i]->mainstay_2_control.begin(), car[i]->mainstay_2_control.end());
				sort(temp_whole_path_control.begin(), temp_whole_path_control.end());  //排序
				temp_whole_path_control.erase(unique(temp_whole_path_control.begin(), temp_whole_path_control.end()), temp_whole_path_control.end());  //去重
				car[i]->cluster_control = temp_whole_path_control;  //更新自己的路径信息
			}
			if (car[i]->cbc_isbone == 1) //i节点他不是簇头 是别人的骨干
			{
			//	cout << "骨干" << i << "收到了维护包,由" <<j->scource<<"发向簇头"<<j->destination<< endl;
				cbc_maintain_packet new_cbc_maintain_packet=*j;  //转发
				new_cbc_maintain_packet.current = car[i]->cbc_near;
				new_cbc_maintain_packet.path_control_neighbor.insert(new_cbc_maintain_packet.path_control_neighbor.end(), car[i]->reachable.begin(), car[i]->reachable.end());  //把I控制邻居加入路径控制的里面
				car[new_cbc_maintain_packet.current]->virtual_cbc_maintain_packet_queue.push_back(new_cbc_maintain_packet);
			}
			car[i]->cbc_maintain_packet_queue.erase(j++);
		}
	}
}

void gate_way_exchange_inform_packet(node** car, int N, int cluster_radius, int time, long int &control_packet)   //网关节点交换路由信息
{
	for (int i = 0; i < N; i++)  //遍历全部节点
	{
		for (int n = 0; n < car[i]->reachable.size(); n++)   //遍历全部邻居队
		{
			for (int m = 0; m < n; m++)   //遍历全部邻居对
			{
				if ((car[car[i]->reachable[n]]->cbc_isbone == 1 || car[car[i]->reachable[n]]->cbc_ishead == 1) && (car[car[i]->reachable[m]]->cbc_isbone == 1 || car[car[i]->reachable[m]]->cbc_ishead == 1) && (car[car[i]->reachable[m]]->bone_of_whom != car[car[i]->reachable[n]]->bone_of_whom) && car[car[i]->reachable[m]]->only_inform_packet.effective == 1 && car[car[i]->reachable[n]]->only_inform_packet.effective==1) //两个邻居全是簇头/骨干节点 并且俩邻居不在一个簇中,并且名片必须有效
				{
					cbc_inform_packet new_exchange_packet_1 = car[car[i]->reachable[n]]->only_inform_packet;  //把邻居骨干的信息打成包
				//	cout << i << "作为网关，邻居是什么牛鬼蛇神，蛇神一号" << car[i]->reachable[n] << "他隶属簇头" << car[car[i]->reachable[n]]->bone_of_whom << endl;
				//	cout << i << "作为网关，邻居蛇神二号" << car[i]->reachable[m] << "他隶属簇头" << car[car[i]->reachable[m]]->bone_of_whom << endl;
					for (int o = 0; o < N; o++) //记录打开看
					{
						if (new_exchange_packet_1.cbc_routing_table_packeted[o].cluster_head!=-1)   //交换的这条记录必须有价值才行
						{
							new_exchange_packet_1.cbc_routing_table_packeted[o].gateway = i;  //每一条记录的网关都改成他自己		
							new_exchange_packet_1.cbc_routing_table_packeted[o].cluster_head = car[car[i]->reachable[n]]->bone_of_whom;  //网关跟cluster_head配对使用，只存下一跳，不存全部路径
							new_exchange_packet_1.cbc_routing_table_packeted[o].length = new_exchange_packet_1.cbc_routing_table_packeted[o].length + 1;//经由网关转发过一次，每条记录的长度都加一
						}
					}
					car[car[i]->reachable[m]]->virtual_exchange_packet_information_queue.push_back(new_exchange_packet_1);
					control_packet++;
					cbc_inform_packet new_exchange_packet_2 = car[car[i]->reachable[m]]->only_inform_packet;  //把邻居骨干的信息打成包
					for (int o = 0; o < N; o++) //记录打开看
					{
						if (new_exchange_packet_2.cbc_routing_table_packeted[o].cluster_head != -1)   //交换的这条记录必须有价值才行
						{
							new_exchange_packet_2.cbc_routing_table_packeted[o].gateway = i;  //每一条记录的网关都改成他自己
							new_exchange_packet_2.cbc_routing_table_packeted[o].cluster_head = car[car[i]->reachable[m]]->bone_of_whom;  //网关跟cluster_head配对使用，只存下一跳，不存全部路径
							new_exchange_packet_2.cbc_routing_table_packeted[o].length = new_exchange_packet_2.cbc_routing_table_packeted[o].length + 1;//经由网关转发过一次，每条记录的长度都加一
						}
					}
					car[car[i]->reachable[n]]->virtual_exchange_packet_information_queue.push_back(new_exchange_packet_2);   //交换了信息
					control_packet++;
				}
			}
		}
	}
}

void arm_end_hello(node** car, int N, int cluster_radius, int hello_interval, long int &control_packet)   //臂展末端的节点发的簇维护报，顺带了簇能控制的节点信息hello_interval是这个信息的频繁程度
{
	for (int i = 0; i < N; i++)  //遍历全部节点
	{
		if (car[i]->cbc_isbone == 1 && car[i]->cbc_far == -1) //又是节点远端又是别人的骨干
		{
			if (car[i]->hello_interval == 0)
			{
				//cout << i << "作为" << car[i]->bone_of_whom << "的骨干，维护包发向" << car[i]->cbc_near << endl;
				cbc_maintain_packet new_cbc_maintain_packet;
				new_cbc_maintain_packet.scource = i;
				new_cbc_maintain_packet.current = car[i]->cbc_near;
				new_cbc_maintain_packet.destination = car[i]->bone_of_whom;
				new_cbc_maintain_packet.path_control_neighbor = car[i]->reachable;
				control_packet = control_packet + car[i]->reachable.size();
				if (car[i]->is_reachable[new_cbc_maintain_packet.current] == 1) //这条链路存在
				{
					car[new_cbc_maintain_packet.current]->virtual_cbc_maintain_packet_queue.push_back(new_cbc_maintain_packet);
					control_packet++;
				}
				car[i]->hello_interval = hello_interval;
			}
			car[i]->hello_interval--;
		}
	}
}

void cluster_validity_test(node** car, int N, int cluster_radius, int time, int cbc_business_card_validity)   //簇的保留与否测试
{
	for (int i = 0; i < N; i++)  //遍历全部节点
	{
		if (car[i]->cbc_ishead == 1 && (car[i]->cbc_tail_1_validity <= 0 || car[i]->cbc_tail_2_validity <= 0 || car[i]->cbc_lonely_validity<=0))  //如果收不到邻居的维护信息,或者是个孤独节点，孤独节点生存时间需要限制一下
		{
			car[i]->only_inform_packet.effective = 0;  //名片失效
			car[i]->cbc_ishead = 0;
			car[i]->cbc_isbone = 0;   //是否为骨干网  cbc_isbone=1 是骨干网   cbc_isbone=0  不是骨干网
			car[i]->bone_of_whom = -1;         //我作为骨干网的簇首编号
			car[i]->cbc_near = -1; //作为骨干网近端邻居是谁 
			car[i]->cbc_far = -1;  //作为骨干网远端邻居是谁
			car[i]->stop_announce=0; //暂停期结束，我要开始簇头信息了
			car[i]->wait_to_create_head = M_A_X;
			car[i]->saved_path.clear(); //返回包的路径清空
			car[i]->node_control.clear(); //存着哪些节点被那个节点支配   node_control[6]={2,9,11} 表示6号节点支配着2，9，11三个节点
			car[i]->mainstay_1.clear(); //簇的骨干网
			car[i]->mainstay_2.clear(); //骨干网
			car[i]->cluster_control.clear(); //本簇控制多少个节点,包括了簇头跟骨干网
			car[i]->mainstay_1_control.clear();  //一号展臂能控制的节点
			car[i]->mainstay_2_control.clear(); //二号展臂控制的节点
			car[i]->cbc_routing_table.clear();  //不是簇头了把路由表清空
			car[i]->cbc_maintain_packet_queue.clear();
			car[i]->virtual_cbc_maintain_packet_queue.clear();
			car[i]->my_max_announce_received.max_announce_node_degree = -1;
			car[i]->my_max_announce_received.max_announce_node_ID = -1;
			car[i]->my_max_announce_received.wait_time_window = -1;
			car[i]->my_max_announce_received.is_hearing = 1;
		}
		else
		{
			if (car[i]->mainstay_1.size()!=0)  //如果一号臂展存在
			{
				car[i]->cbc_tail_1_validity--;
			}
			if (car[i]->mainstay_2.size()!= 0)  //如果二号展臂存在
			{
				car[i]->cbc_tail_2_validity--;
			}
			if (car[i]->mainstay_1.size() == 0 && car[i]->mainstay_2.size() == 0) //两条展臂都不存在，限制孤独簇的存活时间
			{
				car[i]->cbc_lonely_validity--;
			}
		}
		if (car[i]->cbc_isbone == 1 && (time - car[i]->only_inform_packet.time_stamp)>=cbc_business_card_validity)    //如果是骨干，但是常年收不到簇的名片更新
		{
			car[i]->only_inform_packet.effective = 0;  //名片失效
			car[i]->cbc_ishead = 0;
			car[i]->cbc_isbone = 0;   //是否为骨干网  cbc_isbone=1 是骨干网   cbc_isbone=0  不是骨干网
			car[i]->bone_of_whom = -1;         //我作为骨干网的簇首编号
			car[i]->cbc_near = -1; //作为骨干网近端邻居是谁 
			car[i]->cbc_far = -1;  //作为骨干网远端邻居是谁
			car[i]->saved_path.clear(); //返回包的路径清空
			car[i]->node_control.clear(); //存着哪些节点被那个节点支配   node_control[6]={2,9,11} 表示6号节点支配着2，9，11三个节点
			car[i]->mainstay_1.clear(); //簇的骨干网
			car[i]->mainstay_2.clear(); //骨干网
			car[i]->cluster_control.clear(); //本簇控制多少个节点,包括了簇头跟骨干网
			car[i]->mainstay_1_control.clear();  //一号展臂能控制的节点
			car[i]->mainstay_2_control.clear(); //二号展臂控制的节点
			car[i]->cbc_routing_table.clear();  //路由表
			car[i]->cbc_maintain_packet_queue.clear();
			car[i]->virtual_cbc_maintain_packet_queue.clear();
			car[i]->my_max_announce_received.max_announce_node_degree = -1;
			car[i]->my_max_announce_received.max_announce_node_ID = -1;
			car[i]->my_max_announce_received.wait_time_window = -1;
			car[i]->my_max_announce_received.is_hearing = 1;
		}
	}
}

void cbc_packet_delivery(node** car, int N, int packet_exiry, long int &cbc_packet_drop, long int &cbc_packet_arrive, long int &cbc_packet_life_time, long int &cbc_control_packet)
{
	for (int i = 0; i < N; i++)   //数据包
	{
		for (list<cbc_packet>::iterator j = car[i]->cbc_queue.begin(); j != car[i]->cbc_queue.end();)   //需要做一些删除操作 ++写在后面
		{
			if (j->T > packet_exiry)
			{
				//cout << "数据包过期失效" << endl;
				cbc_packet_drop++;//计数器		
			}
			else
			{
				if (j->current == j->destination)  //已经送达
				{
					//cout <<"在" << j->current << "位置，数据包成功送达" << "数据包ID为" << j->ID << endl;
					cbc_packet_life_time = cbc_packet_life_time + j->T;
					cbc_packet_arrive++;//计数器
				}
				else
				{
					//cout << "数据包ID为"<<j->ID<<"当前位置为" << j->current << "目的位置" << j->destination << "是否在本簇" << j->already_in_this_cluster << endl;
					//cout << "向网关" << j->go_for_gateway << "当前位置是不是骨干" << car[j->current]->cbc_isbone << "骨干近端节点为" << car[j->current]->cbc_near << endl;
					if (j->go_for_gateway == 1 && j->next_gateway == i)   //朝着网关节点而去，并且当前节点就是网关节点
					{
						int gate_way_useful_flag = 0;
						cbc_packet new_cbc_packet_1 = *j;
						for (int n = 0; n < car[i]->reachable.size(); n++)
						{
							if (car[car[i]->reachable[n]]->bone_of_whom == new_cbc_packet_1.next_head)  //这个骨干恰好是这个头的
							{
								new_cbc_packet_1.current = car[i]->reachable[n];
								new_cbc_packet_1.next_gateway = -1;
								new_cbc_packet_1.next_head = -1;
								new_cbc_packet_1.go_for_gateway = 0;
								new_cbc_packet_1.already_in_this_cluster = 0;
								new_cbc_packet_1.T = new_cbc_packet_1.T + 1;
								if (car[i]->is_reachable[new_cbc_packet_1.current])  //如果这条链路真实存在
								{
									car[new_cbc_packet_1.current]->virtual_cbc_queue.push_back(new_cbc_packet_1);    //起到了作为网关的作用，发给了下一个簇的骨干节点
								}
								gate_way_useful_flag = 1;
								break;
							}
						}
						if (gate_way_useful_flag == 0)
						{
							cbc_packet_drop++;   //它本身是网关，但是邻居里面找不到目的簇，直接丢去数据包
						}
					}
					if (j->go_for_gateway == 1 && j->next_gateway != i) //朝着网关节点而去，但是并且当前节点不是网关节点
					{
						cbc_packet new_cbc_packet_2 = *j;
						new_cbc_packet_2.T = new_cbc_packet_2.T + 1;
						if (car[i]->is_reachable[new_cbc_packet_2.next_gateway] == 1)  //邻居里面有目的网关
						{
							new_cbc_packet_2.current = new_cbc_packet_2.next_gateway;
							car[new_cbc_packet_2.current]->virtual_cbc_queue.push_back(new_cbc_packet_2);
						}
						else   //邻居里面没有目的网关，看看能不能往远处发
						{
							if (car[i]->cbc_far != -1)
							{
								new_cbc_packet_2.current = car[i]->cbc_far;
								if (car[i]->is_reachable[new_cbc_packet_2.current] == 1)
								{
									car[new_cbc_packet_2.current]->virtual_cbc_queue.push_back(new_cbc_packet_2);
								}
								else
								{
									cbc_packet_drop++;
								}
							}
							else
							{
								cbc_packet_drop++;  //走到展臂的末端也是找不到目的地，数据包丢弃
							}
						}
					}
					if (j->already_in_this_cluster == 1)   //表示目的节点就在本簇中，向目的节点转发的
					{
						cbc_packet new_cbc_packet_3 = *j;
						new_cbc_packet_3.T = new_cbc_packet_3.T + 1;
						if (car[i]->is_reachable[new_cbc_packet_3.destination] == 1)  //邻居里面有目的地
						{
							new_cbc_packet_3.current = new_cbc_packet_3.destination;
							car[new_cbc_packet_3.current]->virtual_cbc_queue.push_back(new_cbc_packet_3);
						}
						else   //邻居里面没有目的地，看看能不能往远处发
						{
							if (car[i]->cbc_far != -1)
							{
								new_cbc_packet_3.T = new_cbc_packet_3.T + 1;
								new_cbc_packet_3.current = car[i]->cbc_far;
								if (car[i]->is_reachable[new_cbc_packet_3.current] == 1)
								{
									car[new_cbc_packet_3.current]->virtual_cbc_queue.push_back(new_cbc_packet_3);
								}
								else
								{
									cbc_packet_drop++;
								}
							}
							else
							{
								cbc_packet_drop++;  //走到展臂的末端也是找不到目的地，数据包丢弃
							}
						}
					}
					if (j->already_in_this_cluster == 0 && j->go_for_gateway == 0)  //漫无目的
					{
						if (car[i]->cbc_ishead == 1)  //如果是簇头节点，那好说了，给你个目的
						{
							int is_delivered = 0;
							cbc_packet new_cbc_packet_4 = *j;
							new_cbc_packet_4.T = new_cbc_packet_4.T + 1;
							
							if (car[i]->cbc_routing_table[new_cbc_packet_4.destination].in_this_cluster == true)  //目的地址就在本簇之中
							{
								//cout << "数据包ID为"<<new_cbc_packet_4.ID<<"当前位置为" << i << "是簇头，目的地" << new_cbc_packet_4.destination << "目的地就在本簇中" << "可以通过" << car[i]->cbc_routing_table[new_cbc_packet_4.destination].gateway_mainstay_number <<"联系到"<< endl;
								new_cbc_packet_4.already_in_this_cluster = 1;
								new_cbc_packet_4.go_for_gateway = 0;
								switch (car[i]->cbc_routing_table[new_cbc_packet_4.destination].gateway_mainstay_number)  //骨干如何联系到目的地址
								{
								case 0:   //如果簇首能直接联系到目的地址
									new_cbc_packet_4.current = new_cbc_packet_4.destination;
									if (car[i]->is_reachable[new_cbc_packet_4.current] == 1)
									{
										car[new_cbc_packet_4.current]->virtual_cbc_queue.push_back(new_cbc_packet_4);
									}
									else
									{
										cbc_packet_drop++;
									}
									break;
								case 1:
									new_cbc_packet_4.current = car[i]->mainstay_1.front();
									if (car[i]->is_reachable[new_cbc_packet_4.current] == 1)
									{
										car[new_cbc_packet_4.current]->virtual_cbc_queue.push_back(new_cbc_packet_4);
									}
									else
									{
										cbc_packet_drop++;
									}
									break;
								case 2:
									new_cbc_packet_4.current = car[i]->mainstay_2.front();
									if (car[i]->is_reachable[new_cbc_packet_4.current] == 1)
									{
										car[new_cbc_packet_4.current]->virtual_cbc_queue.push_back(new_cbc_packet_4);
									}
									else
									{
										cbc_packet_drop++;
									}
									break;
								default:
									break;
								}
								//cout << "处理完毕的数据包当前位置为" << new_cbc_packet_4.current << endl;
							}
							else   //目的地不在本簇中
							{
								if (car[i]->cbc_routing_table[new_cbc_packet_4.destination].cluster_head == -1)  //找不到输入哪个簇头
								{		
									car[new_cbc_packet_4.current]->virtual_cbc_queue.push_back(new_cbc_packet_4);  //暂存在本簇的队列中
								}
								else
								{
									new_cbc_packet_4.T = new_cbc_packet_4.T + 1;
									new_cbc_packet_4.next_head = car[i]->cbc_routing_table[new_cbc_packet_4.destination].cluster_head;
									new_cbc_packet_4.next_gateway = car[i]->cbc_routing_table[new_cbc_packet_4.destination].gateway;
									new_cbc_packet_4.already_in_this_cluster = 0;
									new_cbc_packet_4.go_for_gateway = 1;
									switch (car[i]->cbc_routing_table[new_cbc_packet_4.destination].gateway_mainstay_number)  //骨干联系网关
									{
									case 0:   //如果簇首能直接联系到网关
										new_cbc_packet_4.current = car[i]->cbc_routing_table[new_cbc_packet_4.destination].gateway;
										break;
									case 1:   //一号臂展可以网关
										new_cbc_packet_4.current = car[i]->mainstay_1.front();
										break;
									case 2:  //二号臂展可以够到网关
										new_cbc_packet_4.current = car[i]->mainstay_2.front();
										break;
									default:
										cbc_packet_drop++;
										break;
									}
									if (car[i]->is_reachable[new_cbc_packet_4.current] == 1)
									{
										car[new_cbc_packet_4.current]->virtual_cbc_queue.push_back(new_cbc_packet_4);
									}
									else
									{
										cbc_packet_drop++;
									}
								}
							}
						}
						if (car[i]->cbc_isbone == 1)  //如果在骨干网中，好说，直接交给簇头
						{
							
							cbc_packet new_cbc_packet_5 = *j;
							new_cbc_packet_5.T = new_cbc_packet_5.T + 1;
							if (car[i]->cbc_near != -1)
							{
								new_cbc_packet_5.current = car[i]->cbc_near;  //往簇头发								
								if (car[i]->is_reachable[new_cbc_packet_5.current] == 1)
								{
									car[new_cbc_packet_5.current]->virtual_cbc_queue.push_back(new_cbc_packet_5);
								}
								else
								{
									cbc_packet_drop++;
								}
							}
							else  //找不到簇头了，丢弃之
							{
								cbc_packet_drop++;
							}
						}
						if (car[i]->cbc_isbone == 0 && car[i]->cbc_ishead == 0)   //既不是骨干也不是簇头
						{
							cbc_packet new_cbc_packet_6 = *j;
							new_cbc_packet_6.T = new_cbc_packet_6.T + 1;
							int konw_how_to_do = 0;
							for (int n = 0; n < car[i]->reachable.size(); n++)
							{
								if (car[car[i]->reachable[n]]->cbc_isbone == 1 || car[car[i]->reachable[n]]->cbc_ishead == 1)  //只要查询到一个邻居是簇头或者骨干节点
								{
									new_cbc_packet_6.current = car[i]->reachable[n];
									car[new_cbc_packet_6.current]->virtual_cbc_queue.push_back(new_cbc_packet_6);
									konw_how_to_do = 1;
									break;
								}
							}
							if (konw_how_to_do == 0)  //周围连个簇都没有
							{
								car[new_cbc_packet_6.current]->virtual_cbc_queue.push_back(new_cbc_packet_6);  //从哪来的回哪里去
							}
						}

					}
				}
			}
			car[i]->cbc_queue.erase(j++);
		}
	}
}

void create_cbc_packet(node** car, double packet_spawn_rate, int N,int time)
{
	for (int start_packet = 0; start_packet<(int)(packet_spawn_rate*N); start_packet++)
	{
		cbc_packet new_packet = create_single_cbc_packet(N);
		//cout << new_packet.current << " " << new_packet.destination << endl;
		new_packet.ID = time;
		car[new_packet.current]->cbc_queue.push_back(new_packet);  //生成数据包
	}
}

void create_lca_head(node** car,int N,int time,long int &lca_control_packet)
{
	for (int i = 0; i < N; i++)
	{
		int lowest_flag = 1;
		for (int j = 0; j < car[i]->reachable.size(); j++)
		{
			lca_control_packet++;
			if (car[i]->reachable[j]<i&&car[car[i]->reachable[j]]->in_lca_cluster == 0)   //如果有一个邻居编号比自己小,并且这个邻居没在簇中
			{
				lowest_flag = 0;   //那他不是最小的
			}
		}
		if (car[i]->in_lca_cluster==1)
		{
			lowest_flag = 0;   //自己在簇里面就不掺和了
		}
		if (lowest_flag==1)
		{
			if (car[i]->lca_ishead==0)  //原本不是簇头
			{
				car[i]->lca_ishead = 1;  //现在是簇头
				for (int n = 0; n < N; n++)    //开始创建路由表
				{
					lca_routing_table_element new_lca_routing_table_element;
					new_lca_routing_table_element.time_stamp = time;			
					if (car[i]->is_reachable[n]==1||i==n)
					{
						new_lca_routing_table_element.cluster_head = i;
						new_lca_routing_table_element.gateway =-1;
						new_lca_routing_table_element.length = 0;
					}
					else
					{
						new_lca_routing_table_element.cluster_head = -1;
						new_lca_routing_table_element.gateway = -1;
						new_lca_routing_table_element.length = M_A_X;
					}
					car[i]->lca_routing_table.push_back(new_lca_routing_table_element);
				}
			}
			if (car[i]->lca_ishead == 1)  //原本就是簇头
			{
				for (int n = 0; n < N; n++)    //本簇的路由信息更新一下
				{
					if (car[i]->is_reachable[n] == 1 || i == n)
					{
						car[i]->lca_routing_table[n].cluster_head = i;
						car[i]->lca_routing_table[n].gateway = -1;
						car[i]->lca_routing_table[n].length = 0;
						car[i]->lca_routing_table[n].time_stamp = time;
					}
				}
			}
		}
		if (lowest_flag==0)  //有比他ID小的邻居了
		{
			car[i]->lca_ishead = 0;
			car[i]->lca_routing_table.clear();  //路由表清空
		}
	}
}

void lca_check_in_cluseter(node** car, int N)
{
	for (int i = 0; i < N; i++)
	{
		int in_cluster_flag = 0;
		for (int j = 0; j < car[i]->reachable.size(); j++)
		{
			if (car[car[i]->reachable[j]]->lca_ishead==1)  //如果有个邻居为簇头
			{
				in_cluster_flag = 1;
				break;
			}
		}
		if (in_cluster_flag==1)
		{
			car[i]->in_lca_cluster = 1;
		}
		else
		{
			car[i]->in_lca_cluster = 0;
		}
	}
}



void lca_gateway_exchange_routing_table(node** car, int N, int time,long int &lca_control_packet)
{
	for (int i = 0; i < N; i++)
	{
		for (int n = 0; n < car[i]->reachable.size(); n++)
		{
			for (int m = 0; m < n; m++)
			{
				if (car[car[i]->reachable[n]]->lca_ishead == 1 && car[car[i]->reachable[m]]->lca_ishead==1)  //俩邻居都是簇头节点
				{
					//cout << "节点" << i << "作为网关交换" << car[i]->reachable[n] << "与" << car[i]->reachable[m] << "的信息" << endl;
					lca_routing_exchange_packet new_lca_routing_exchange_packet_1;
					new_lca_routing_exchange_packet_1.packeted_lca_routing_table = car[car[i]->reachable[n]]->lca_routing_table;
					for (int k = 0; k < N; k++)   //路由表每一项都拆开 改写
					{
						if (new_lca_routing_exchange_packet_1.packeted_lca_routing_table[k].cluster_head!=-1)   //这个路由信息必须有价值
						{
							new_lca_routing_exchange_packet_1.packeted_lca_routing_table[k].cluster_head = car[i]->reachable[n];
							new_lca_routing_exchange_packet_1.packeted_lca_routing_table[k].gateway = i;
							new_lca_routing_exchange_packet_1.packeted_lca_routing_table[k].length = new_lca_routing_exchange_packet_1.packeted_lca_routing_table[k].length + 1;
						}
					}
					car[car[i]->reachable[m]]->virtual_lca_routing_exchange_packet_queue.push_back(new_lca_routing_exchange_packet_1);
					lca_routing_exchange_packet new_lca_routing_exchange_packet_2;
					new_lca_routing_exchange_packet_2.packeted_lca_routing_table = car[car[i]->reachable[m]]->lca_routing_table;
					for (int k = 0; k < N; k++)   //路由表每一项都拆开 改写
					{
						if (new_lca_routing_exchange_packet_2.packeted_lca_routing_table[k].cluster_head != -1)   //这个路由信息必须有价值
						{
					    	new_lca_routing_exchange_packet_2.packeted_lca_routing_table[k].cluster_head = car[i]->reachable[m];
							new_lca_routing_exchange_packet_2.packeted_lca_routing_table[k].gateway = i;
							new_lca_routing_exchange_packet_2.packeted_lca_routing_table[k].length = new_lca_routing_exchange_packet_2.packeted_lca_routing_table[k].length + 1;
						}
					}
					car[car[i]->reachable[n]]->virtual_lca_routing_exchange_packet_queue.push_back(new_lca_routing_exchange_packet_2);
					lca_control_packet = lca_control_packet + 2;  //两个控制包
				}
			}
		}
	}
}

void arrange_lca_routing_table(node** car, int N, int time, int lca_refresh_interval)
{
	for (int i = 0; i < N; i++)
	{
		if (car[i]->lca_ishead == 1)  //是簇头，检查自己的路由表是否过期
		{
			for (int n = 0; n < N; n++)
			{
				if (time - car[i]->lca_routing_table[n].time_stamp > lca_refresh_interval)   //如果路由表过期
				{
					car[i]->lca_routing_table[n].cluster_head = -1;
					car[i]->lca_routing_table[n].gateway = -1;
					car[i]->lca_routing_table[n].length = -1;
					car[i]->lca_routing_table[n].time_stamp = time;
				}
			}
		}
		for (list<lca_routing_exchange_packet>::iterator j = car[i]->lca_routing_exchange_packet_queue.begin(); j != car[i]->lca_routing_exchange_packet_queue.end();)  //这里i++ 写在下面,需要处理一些删除操作
		{
			if (car[i]->lca_ishead == 1) //i节点他是簇头
			{
				for (int m = 0; m < N; m++)   //遍历路由表
				{
					if (car[i]->is_reachable[m] == 1||i==m)    //m节点可以通过簇头直接送达  不修改路由表
					{
					}
					else
					{
						if (j->packeted_lca_routing_table[m].cluster_head != -1 && (time - j->packeted_lca_routing_table[m].time_stamp < lca_refresh_interval))     //发来的路由表中有关于m消息,并且时间戳是有效的
						{
							//cout << "节点" << i << "收到关于" << m << "路由信息,经由簇头" << j->packeted_lca_routing_table[m].cluster_head << "通过网关" << j->packeted_lca_routing_table[m].gateway << "可以送达" << endl;
							if (car[i]->lca_routing_table[m].cluster_head == -1)  //我本身没有这条记录，发来的有这条记录
							{
								car[i]->lca_routing_table[m].cluster_head = j->packeted_lca_routing_table[m].cluster_head;
								car[i]->lca_routing_table[m].gateway = j->packeted_lca_routing_table[m].gateway;
								car[i]->lca_routing_table[m].length = j->packeted_lca_routing_table[m].length;
								car[i]->lca_routing_table[m].time_stamp = j->packeted_lca_routing_table[m].time_stamp;
							}
							else  //我本身有这条记录
							{
								if (j->packeted_lca_routing_table[m].length < car[i]->lca_routing_table[m].length)  //但是新送来的路由表中 长度比我自身记录的要短
								{
									car[i]->lca_routing_table[m].cluster_head = j->packeted_lca_routing_table[m].cluster_head;
									car[i]->lca_routing_table[m].gateway = j->packeted_lca_routing_table[m].gateway;
									car[i]->lca_routing_table[m].length = j->packeted_lca_routing_table[m].length;
									car[i]->lca_routing_table[m].time_stamp = j->packeted_lca_routing_table[m].time_stamp;
								}
								if (j->packeted_lca_routing_table[m].length == car[i]->lca_routing_table[m].length&&car[i]->lca_routing_table[m].time_stamp < j->packeted_lca_routing_table[m].time_stamp)  //长度加一跟自己的一样长 但是新发来的包里面信息 时间戳比自己的新
								{
									car[i]->lca_routing_table[m].cluster_head = j->packeted_lca_routing_table[m].cluster_head;
									car[i]->lca_routing_table[m].gateway = j->packeted_lca_routing_table[m].gateway;
									car[i]->lca_routing_table[m].length = j->packeted_lca_routing_table[m].length;
									car[i]->lca_routing_table[m].time_stamp = j->packeted_lca_routing_table[m].time_stamp;
								}
							}
						}//j中有关于另一个簇的路由的信息
					}
				}//遍历路由表
			}
			car[i]->lca_routing_exchange_packet_queue.erase(j++);
		}
	}
}

lca_packet create_single_lca_packet(int N,int time)
{
	lca_packet realpacket;
	realpacket.T = 0;
	realpacket.next_head = -1;
	realpacket.next_gateway = -1;
	realpacket.ID = time;
	while (1)
	{
		realpacket.current = rand() % N;
		realpacket.destination = rand() % N;
		if (realpacket.destination != realpacket.current)
		{
			break;
		}
		else
		{
			continue;
		}
	}
	realpacket.scource = realpacket.current;  //数据包源地址
	return realpacket;
}

void create_lca_packet(node** car, double packet_spawn_rate, int N,int time)
{
	for (int start_packet = 0; start_packet<(int)(packet_spawn_rate*N); start_packet++)
	{
		lca_packet new_packet = create_single_lca_packet(N,time);
		car[new_packet.current]->lca_packet_queue.push_back(new_packet);  //生成数据包
	}
}

void lca_packet_delivery(node** car, int N, int packet_expiry, long int &lca_packet_drop, long int &lca_packet_arrive, long int &lca_packet_life_time_sum, long int &lca_control_packet)
{
	for (int i = 0; i < N; i++)
	{
		for (list<lca_packet>::iterator j = car[i]->lca_packet_queue.begin(); j != car[i]->lca_packet_queue.end();)   //需要做一些删除操作 ++写在后面
		{
			//cout << "在节点" << i << "有数据包,ID为" << j->ID << "目的" << j->destination << "下个簇头" << j->next_head << "下个网关" << j->next_gateway << endl;
			if (j->T > packet_expiry)
			{
				//cout << "数据包过期失效" << endl;
				lca_packet_drop++;//计数器		
			}
			else
			{
				if (j->current == j->destination)  //已经送达
				{
					//cout <<"在" << j->current << "位置，数据包成功送达" << "数据包ID为" << j->ID << endl;
					lca_packet_life_time_sum = lca_packet_life_time_sum + j->T;
					lca_packet_arrive++;//计数器
				}
				else
				{
					if (j->next_head == i)   //如果已经送到簇头
					{
						if (car[i]->lca_ishead == 1)  //如果确实是簇头
						{
							if (car[i]->lca_routing_table[j->destination].cluster_head == i)   //如果本簇就可以送达
							{
								lca_packet new_lca_packet = *j;
								new_lca_packet.current = new_lca_packet.destination;
								new_lca_packet.T = new_lca_packet.T + 1;
								if (car[i]->is_reachable[new_lca_packet.current] == 1)  //如果链路真实存在
								{
									car[new_lca_packet.current]->virtual_lca_packet_queue.push_back(new_lca_packet);
								}
								else
								{
									lca_packet_drop++;
								}
							}
							else   //本簇不能送达
							{
								if (car[i]->lca_routing_table[j->destination].cluster_head == -1)   //如果本簇内没有到目的地的信息
								{
									lca_packet new_lca_packet = *j;
									new_lca_packet.T = new_lca_packet.T + 1;
									car[new_lca_packet.current]->virtual_lca_packet_queue.push_back(new_lca_packet);     //从哪来的回哪去
								}
								if (car[i]->lca_routing_table[j->destination].cluster_head != -1)  //如果通过其他簇的转发就可以送达
								{
									lca_packet new_lca_packet = *j;
									new_lca_packet.T = new_lca_packet.T + 1;
									new_lca_packet.current = car[i]->lca_routing_table[new_lca_packet.destination].gateway;
									new_lca_packet.next_head = car[i]->lca_routing_table[new_lca_packet.destination].cluster_head;
									new_lca_packet.next_gateway = car[i]->lca_routing_table[new_lca_packet.destination].gateway;
									if (car[i]->is_reachable[new_lca_packet.current] == 1)  //如果链路真实存在
									{
										car[new_lca_packet.current]->virtual_lca_packet_queue.push_back(new_lca_packet);
									}
									else
									{
										lca_packet_drop++;
									}
								}
							}
						}
						else  //否则随便交给自己的一个簇头邻居
						{
							lca_packet_drop++;
						}
					}
					if (j->next_gateway==i)  //如果当前位置是网关
					{
						lca_packet new_lca_packet = *j;
						new_lca_packet.T = new_lca_packet.T + 1;
						new_lca_packet.current = new_lca_packet.next_head;
						new_lca_packet.next_gateway = -1;
						if (car[i]->is_reachable[new_lca_packet.current]==1)
						{
							car[new_lca_packet.current]->virtual_lca_packet_queue.push_back(new_lca_packet);
						}
						else
						{
							lca_packet_drop++;
						}
					}
					if (j->next_gateway!=i&&j->next_head!=i)
					{
						int have_head_flag = 0;
						if (car[i]->lca_ishead==1)  //i就是簇头
						{
							lca_packet new_lca_packet = *j;
							new_lca_packet.T = new_lca_packet.T + 1;
							new_lca_packet.next_head = i;
							car[new_lca_packet.current]->virtual_lca_packet_queue.push_back(new_lca_packet);
							have_head_flag = 1;
						}
						else
						{
							for (int l = 0; l < car[i]->reachable.size(); l++)    //遍历邻居
							{
								if (car[car[i]->reachable[l]]->lca_ishead == 1)  //有个是簇头
								{
									lca_packet new_lca_packet = *j;
									new_lca_packet.T = new_lca_packet.T + 1;
									new_lca_packet.current = car[i]->reachable[l];   //交给这簇头邻居
									new_lca_packet.next_head = car[i]->reachable[l];
									car[new_lca_packet.current]->virtual_lca_packet_queue.push_back(new_lca_packet);
									have_head_flag = 1;
									break;
								}
							}
						}
						if (have_head_flag==0)   //一个是簇头的邻居都没有
						{
							lca_packet new_lca_packet = *j;
							new_lca_packet.T = new_lca_packet.T + 1;
							car[new_lca_packet.current]->virtual_lca_packet_queue.push_back(new_lca_packet);
						}
					}
				}
			}
			car[i]->lca_packet_queue.erase(j++);
		}
	}
}

void lca_cover_queue(node** car, int N)
{
	for (int process_node = 0; process_node < N; process_node++)
	{
		car[process_node]->lca_routing_exchange_packet_queue.clear();
		car[process_node]->lca_routing_exchange_packet_queue = car[process_node]->virtual_lca_routing_exchange_packet_queue;
		car[process_node]->virtual_lca_routing_exchange_packet_queue.clear();

		car[process_node]->lca_packet_queue.clear();
		car[process_node]->lca_packet_queue = car[process_node]->virtual_lca_packet_queue;
		car[process_node]->virtual_lca_packet_queue.clear();
	}
}

void main()
{
	///////////////////////////////////////////////////////////////////全局变量
	srand((unsigned)time(NULL));
	const int N = 100;//节点个数
	while (true)
	{
		int time = 0;//时间 每个时间为物理意义的0.005秒
		double packet_spawn_rate = 1; //数据包生成率
		int x_div = 10; //区域横着被分成几份
		int y_div = 10; //区域竖着被分成几份 每个路口间隔1000m 默认车速10m/s 
		int block_width = L / x_div;
		double go_straight_possibility = 1 - turn_left_possibility - turn_right_poosibility; //下个路口直行的概率
		int v = 0;  //车速 （物理意义10m/s）
		double r = 2500; //车辆的通信距离  （物理意义 米）
		int simulation_time = 5001;//模拟时间
		int reserve_v[10] = { 1, 2, 5, 10, 15, 20, 25, 30, 40, 50 };  //实验所需的速度表 m/s
		int v_id = 0;
		int packet_expiry = 300;   //300步还不能送达，失去失效性，删除
		////////////////////////////////////////////////////////////////////以下是aodvjr协议需要的变量
		int aodv_RREQ_expiry = 8; //aodvjr请求包的最大生存时间
		int aodvjr_route_expiry = 300;//aodvjr链路生存时间，若超过此时间未收到有效的数据包则链路自动断开
		long int aodvjr_packet_arrive = 0; //送达数据包总量
		long int aodvjr_packet_drop = 0;  //丢弃数据包总量
		long int aodvjr_packet_life_time_sum = 0; //端到端总延迟
		long long int aodvjr_control_packet = 0;  //控制开销
		//////////////////////////////////////////////////////////////////////////////以下是cbc协议用的一些变量
		int cbc_packet_expiry = 300;   //300步还不能送达，失去失效性，删除
		int cbc_cluster_radius = 2; //cbc骨干网的半径
		int cbc_stop_time_increase = 10;//收到驳回包多久不再重发请求信息
		int cbc_forecast_time = 200;  //链路不断的预测时间
		int cbc_refresh_interval = 50;  //路由表的有效期，超过有效期则认为路由失效
		int cbc_hello_interval = 20;   //向簇头 发送簇维护信息
		int cbc_inform_interval = 10;  //簇头节点向骨干广播更新自己的名片信息
		int cbc_tail_validity = 60;  //多久收不到臂展末端的骨干节点，簇头就失效
		int cbc_business_card_validity = 60;  //骨干网多久收不到簇头的更新的名片，骨干网就失效
		long int cbc_packet_arrive = 0; //送达数据包总量
		long int cbc_packet_drop = 0;  //丢弃数据包总量
		long int cbc_packet_life_time_sum = 0; //端到端总延迟
		long int cbc_control_packet = 0;  //控制开销
		//////////////////////////////////////////////////////////////////////////////以下是lca协议用的一些变量
		int lca_packet_expiry = 300;   //300步还不能送达，失去失效性，删除
		long int lca_packet_arrive = 0; //送达数据包总量
		long int lca_packet_drop = 0;  //丢弃数据包总量
		long int lca_packet_life_time_sum = 0; //端到端总延迟
		long int lca_control_packet = 0;  //控制开销
		int lca_refresh_interval = 50;  //路由表的有效期，超过有效期则认为路由失效
		//////////////////////////////////////////////////////////////////////
		while (v_id < 10)  //改变参数循环进行实验
		{
			v = reserve_v[v_id];
			aodvjr_packet_arrive = 0; //送达数据包总量
			aodvjr_packet_drop = 0;  //丢弃数据包总量
			aodvjr_packet_life_time_sum = 0; //端到端总延迟
			aodvjr_control_packet = 0;  //控制开销
			cbc_packet_arrive = 0; //送达数据包总量
			cbc_packet_drop = 0;  //丢弃数据包总量
			cbc_packet_life_time_sum = 0; //端到端总延迟
			cbc_control_packet = 0;  //控制开销
			lca_packet_arrive = 0; //送达数据包总量
			lca_packet_drop = 0;  //丢弃数据包总量
			lca_packet_life_time_sum = 0; //端到端总延迟
			lca_control_packet = 0;  //控制开销
			time = 0;
			node **car = new node*[N];
			////////////////////////////////////////////////////////////节点生成
			create_node(car, block_width, v, r, N);
			////////////////////////////////////////////////////////////
			while (true)  //计时器循环
			{
				time++;
				cout << time << endl;
				if (time % 100 == 1)
				{
					for (int move_node = 0; move_node < N; move_node++)
					{
						car[move_node]->move(block_width, N);
						//	cout << move_node << "号节点坐标是" << car[move_node]->x << "," << car[move_node]->y << "速度为" << car[move_node]->v << "方向为" << car[move_node]->direction << "下次转弯方向为" << car[move_node]->turn.front() << endl;
					}
					aodvjr_calculate_connection(car, r, N, aodvjr_route_expiry);  //aodvjr协议下重新计算临接关系
					//cbc_calculate_connection(car, r, N);  //cbc协议重新计算临接关系
					//use_RREP_maintain(car, N, aodvjr_route_expiry, aodvjr_control_packet);  //发返回确认包维护链路
					//net_work_draw(car, N,time);
				}
				/*
				calculate_route_life(car, N); //计算链路状态，查看是否有链路需要断开
				//net_work_situation(car, N); //观察一下现在网络状态
				create_packet(car, packet_spawn_rate, N); //生成数据包
				handle_aodvjr_RREP_packet(car, N, aodvjr_route_expiry);   //处理反向数据包
				handle_aodvjr_RREQ_packet(car, N, aodv_RREQ_expiry, aodvjr_route_expiry, aodvjr_control_packet);   //处理请求包
				packet_delivery(car, N, aodvjr_route_expiry, aodvjr_packet_arrive, aodvjr_packet_drop, aodvjr_packet_life_time_sum, aodvjr_control_packet, packet_expiry, aodv_RREQ_expiry);//处理数据包
				cover_queue(car, N);//把虚拟队列覆盖至实际队列，完成本步所有数据的处理
				if (time == simulation_time)
				{
				ofstream htest("aodv_速度vs度量.txt", ios::app);
				htest << v << " " << (double)aodvjr_packet_arrive / double(packet_spawn_rate*N*(simulation_time - 1)) << " " << (double)aodvjr_packet_drop / double(packet_spawn_rate*N*(simulation_time - 1)) << " " << aodvjr_control_packet << " " << ((double)aodvjr_packet_life_time_sum) / ((double)aodvjr_packet_arrive) << endl;   ///文本输出
				htest.close();
				}
				*/
				create_cbc_packet(car, packet_spawn_rate, N, time);  //生成数据包
				cluster_validity_test(car, N, cbc_cluster_radius, time, cbc_business_card_validity);  //簇关系是否还能保持
				check_in_cluster(car, N, cbc_cluster_radius);   //检查是否在簇中
				announce_cbchead(car, N, cbc_cluster_radius, cbc_control_packet); //簇头声明
				handle_cbchead_announce(car, N, cbc_cluster_radius, block_width, cbc_forecast_time, cbc_control_packet);  //转发簇头声明
				handle_deny_packet(car, N, cbc_cluster_radius, cbc_stop_time_increase);  //转发驳回包
				check_agree_time_window(car, N, cbc_cluster_radius);   //检查接受请求包的时间窗口，是否发返回包
				handle_agree_packet(car, N, cbc_cluster_radius); //转发同意包
				create_cluster(car, N, cbc_cluster_radius, cbc_tail_validity);//簇建立
				create_routing_table(car, N, time); //生成路由表
				arrange_routing_table(car, N, time); //整理路由表，主要是把簇内节点的时间戳刷新
				inform_cluster(car, N, cbc_cluster_radius, cbc_inform_interval, time, cbc_control_packet); //簇名片生成
				handle_inform_packet(car, N, cbc_cluster_radius); //转发簇名片
				if (time % 30 == 1)
				{
					gate_way_exchange_inform_packet(car, N, cbc_cluster_radius, time, cbc_control_packet);  //网关交换簇名片
				}
				receive_exchange_inform_packet(car, N, cbc_cluster_radius, time, cbc_refresh_interval);	 //收到簇名片，更新自己的路由表	
				cbc_packet_delivery(car, N, cbc_packet_expiry, cbc_packet_drop, cbc_packet_arrive, cbc_packet_life_time_sum, cbc_control_packet); //数据包传递
				arm_end_hello(car, N, cbc_cluster_radius, cbc_hello_interval, cbc_control_packet);  //末尾包向簇头发维持声明
				receive_maintain_packet(car, N, cbc_cluster_radius, time, cbc_tail_validity);  //收到维持声明
				cbc_cover_queue(car, N);//把虚拟队列覆盖至实际队列，完成本步所有数据的处理
				if (time == simulation_time)
				{
					ofstream jtest("cbc_速度vs度量.txt", ios::app);
					jtest << v << " " << (double)cbc_packet_arrive / double(packet_spawn_rate*N*(simulation_time - 1)) << " " << (double)cbc_packet_drop / double(packet_spawn_rate*N*(simulation_time - 1)) << " " << cbc_control_packet << " " << ((double)cbc_packet_life_time_sum) / ((double)cbc_packet_arrive) << endl;   ///文本输出
					jtest.close();
				}

				//////////////////////////////////////////////////////////////////////////////////////////下面是lca协议
				create_lca_packet(car, packet_spawn_rate, N, time);
				lca_check_in_cluseter(car, N);
				if (time % 30 == 1)
				{
					create_lca_head(car, N, time, lca_control_packet);
				}
				lca_net_work_situation(car, N);
				if (time % 30 == 1)
				{
					lca_gateway_exchange_routing_table(car, N, time, lca_control_packet);
				}
				arrange_lca_routing_table(car, N, time, lca_refresh_interval);
				lca_packet_delivery(car, N, lca_packet_expiry, lca_packet_drop, lca_packet_arrive, lca_packet_life_time_sum, lca_control_packet);
				lca_cover_queue(car, N);
				if (time == simulation_time)
				{
					ofstream ktest("lca_速度vs度量.txt", ios::app);
					ktest << v << " " << (double)lca_packet_arrive / double(packet_spawn_rate*N*(simulation_time - 1)) << " " << (double)lca_packet_drop / double(packet_spawn_rate*N*(simulation_time - 1)) << " " << lca_control_packet << " " << ((double)lca_packet_life_time_sum) / ((double)lca_packet_arrive) << endl;   ///文本输出
					ktest.close();
					break;
				}
				cout << "丢弃数据包为" << lca_packet_drop << endl;
			}//计时器
			for (int i = 1; i < N; i++) //删除全部的节点，
			{
				delete car[i];
			}
			v_id++;  //改变参数
		}
	}
}//主程序
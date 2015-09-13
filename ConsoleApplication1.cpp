// ConsoleApplication1.cpp : �������̨Ӧ�ó������ڵ㡣
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
int L = 12000;//ģ�⻷���Ĵ�С
double turn_left_possibility = 0.3; //�¸�·����ת�ļ���30%
double turn_right_poosibility = 0.3; //�¸�·����ת�ĸ���30%


class aodvjr_RREQ_packet {   //aodvjr �������
public:
	int T;
	int scource;
	int destination;
	int current;
	int ID;  //���к�  һ����������Ҫ��ȷ���� scource destination ID
};

class aodvjr_RREP_packet {   //aodvjr�ķ��ذ�
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
	int next_gateway;  //�����ĸ�����
	int next_head;     //ת�����ĸ���ͷ
	int current; //��ǰλ��
	int already_in_this_cluster;
	int go_for_gateway;  //��ʾ���ڴӴ�ͷ��������
	int ID;  //�������ʱ�䣬������
};

class lca_packet {
public:
	int T;
	int ID; //�����ã��������ʱ��
	int scource;
	int destination;
	int next_gateway;  //�����ĸ�����
	int next_head;     //ת�����ĸ���ͷ
	int current; //��ǰλ��
};

class cbc_head_annouce_packet
{
public:
	int T;
	int scource;
	int current;
	int degree; //���ڵ�Ķ�
	vector<int> cbc_announce_path;  //��¼cbc��ͷ�����ڵ��·��
	vector<int> is_passed_by; //��¼��Щ�ڵ����������ˣ������Ǹ��İ�������  is_passed_by[4]=0;��ʾ4�Žڵ�û�о��� is_passed_by[5]=1 ;��ʾ��������5�Žڵ�
	int ID;  //���к�  һ����������Ҫ��ȷ���� scource ID
};

class cbc_deny_packet  //cbc��ͷ����Ĳ��ذ�
{
public:
	int destination;  //���ذ���Ŀ�ĵ�ַ
	int current;
	vector<int> deny_path;  //���ذ���·��
};

class cbc_agree_packet  //cbc��ͷ�����ͬ���
{
public:
	int scource;
	int destination;  //ͬ�����Ŀ�ĵ�ַ
	int depth;  //ͬ��ڵ��Դ�ڵ����ȣ�depth=1��Ϊ�������ھ� depth=2Ϊ2���ھ�
	int current;
	vector<int> control_neighbor; //��reachable��һ���ģ�Դ�ڵ�֧������Щ�ڵ�
	vector<int> agree_path;  //ͬ�����·��
	vector<int> report_path; //ͬ������ص�ʱ�򣬰�·���������������Իظ���ͷ�ڵ�
};

class cbc_maintain_packet
{
public:
	int scource;
	int destination;  //ͬ�����Ŀ�ĵ�ַ
	int current;
	vector<int> path_control_neighbor; //��reachable��һ���ģ�������·����֧����Щ�ڵ�ȫ�����ߴ�ͷ������ά����ͷ�Լ�������
};

class cbc_routing_table_element  //·�ɱ�ĵ���Ԫ�أ��ұ��˵��ض��ڵ��·����Ϣ
{
public:
	int cluster_head; //ͨ�������ͷ�����ҵ���Ŀ�ĵ�
	bool in_this_cluster;  //�Ƿ��ڱ��ؿ���ֱ���ʹ�
	int gateway;  //ͨ���������ؿ����ҵ������  gateway=-1;��ʾ�ҿ���ֱ����ϵ�ϴ�ͷ
	int gateway_mainstay_number;  //�Ǹ����ı�ţ�0��˵�����ױ���Ϳ��Թ������أ�1˵��1��չ�ۿ��Թ��� 2˵��2��չ�ۿ��Թ���(���������ػ���Ŀ�ĵ�) 
	int time_stamp;  //ʱ��� ������Ϣ��ʱ���
	int length;     //���룬�������ٴ����ز��ܵ�Ŀ�Ĵ�ͷ
};

class lca_routing_table_element  //·�ɱ�ĵ���Ԫ��
{
public:
	int cluster_head; //ͨ�������ͷ�����ҵ���Ŀ�ĵ�
	int gateway;  //ͨ���������ؿ����ҵ������  gateway=-1;��ʾ�ҿ���ֱ����ϵ�ϴ�ͷ
	int length;  //���� length=0��ʾ���ڱ�����
	int time_stamp; //ʱ���
};

class cbc_inform_packet  //cbc�ؽ������ȫ���ͷ�㲥����Ƭ
{
public:
	int	effective;    //��Ƭ����Ч��effective=0��ʾ��Ч��effective=1��ʾ��Ч��������Ժ���Ƭ��Ч
	int near;  //�����Ƭ���ĸ��ڵ㴫������
	int scource;
	vector<cbc_routing_table_element> cbc_routing_table_packeted;  //��ɰ���·�ɱ�
	int time_stamp;  //ʱ�����ʱ���̫���Զ�����
	list<int> mainstay;   //�Ǹ�����������Ϣ
};

class max_announce_received  //�ݴ����յ�������������ID
{ 
public:
	int max_announce_node_ID;    //�ݴ����ѽڵ��id
	int max_announce_node_degree;  //�ݴ�ID�Ķ�
	int wait_time_window;  //ʱ�䴰��
	int is_hearing;   //�Ƿ��ڽ��������
	vector<cbc_agree_packet> all_virtual_cbc_agree_packet;//��Ӧ�ڵ��ȫ������ֻҪ�������Ҿͷ��أ����������ҾͶ���
};

class lca_routing_exchange_packet
{
public:
	vector<lca_routing_table_element> packeted_lca_routing_table;  //lca·�ɱ�
};

class node {
public:
	int ID;   //����ID
	int v;
	int x;
	int y;
	double r;  //ͨ�ž���
	int direction;  //��ǰ����  1 �ϣ����� 2 �ң����� 3 �£��ϣ� 4 ������
	list <int> turn; //�¸�·���Ƿ�ת��  0ֱ�� 1��ת 2 ��ת
	vector<int> reachable;//�ɵ���Ľڵ�
	vector<int> is_reachable; //�ٽӹ�ϵ����һ�ֱ�ʾ  is_reachable[5]=1;��ʾ��5�Žڵ����ھӹ�ϵ is_reachable[6]=0 ��ʾ��6�Žڵ㲻���ھӹ�ϵ
	double distance(node p);//��һ������ָ���ȥ
	void move(int block_width, int N);  //block width ָ������ȣ�����ܱ���������·��Ӧ��ת����  
	int virtual_move(node p,int block_width, int N, int time);  //�����ƶ�������Ԥ��ڵ��δ��λ��
	//////////////////////////////////////////////////////////////////////////////////////////////////////aodvjr������Ľڵ㴢��
	list<packet> queue;//���ݰ��Ķ���
	list<aodvjr_RREP_packet> RREP_queue;  //���ذ��Ķ���
	list<aodvjr_RREQ_packet> RREQ_queue;  //������Ķ���
	//////// �������������,�µİ������Ƚ��������,����ͬһ��ʱ�䲽�����ݰ��ظ������ȵ�ÿ��������ʱ����������и�����ʵ����
	list<packet> virtual_queue;//�������  
	list<aodvjr_RREP_packet> virtual_RREP_queue;  //���ⷵ�ذ��Ķ���
	list<aodvjr_RREQ_packet> virtual_RREQ_queue;  //����������Ķ���
	vector<int>  aodvjr_table;  //aodvjr������·��,aodvjr_table[3]=6 ��ʾ�ӵ�ǰλ�õ�3��Ŀ�ĵ� Ҫ��6�Žڵ�
	vector<int>  aodvjr_back_table;  //aodvjr Э��ķ���·��  aodvjr_back_table[5]=7  ��ʾ�ӵ�ǰλ�õ�5��Դ Ҫ��7�Žڵ�     -1��ʾ����·����δ����
	vector<int>  aodvjr_quest_times;  //��ĳ���ڵ��������������������ID
	vector<vector<int>> aodvjr_saved_ID;  //��ʾÿ���ڵ�洢���������ID����aodvjr_saved_ID[5][7]=4��ʾ�Ҵ洢��ԴΪ5Ŀ�Ķ�Ϊ7�������������IDΪ4
	vector<int>  aodvjr_table_expiry;  //aodvjr��������·ά��ʱ�䣬��ʱ���������·�Զ��Ͽ�aodvjr_table_expiry[3]=158��ʾ�ӵ�ǰλ�õ�3��Ŀ�ĵ� ��·ʣ��ʱ��Ϊ 158
	vector<int>  aodvjr_back_expiry;  //aodvjr�ķ�����·ά��ʱ�䣬��ʱ���������·�Զ��Ͽ�aodvjr_back_expiry[5]=128 ��ʾ�ӵ�ǰλ�õ�5��Դ�ڵ� ��·ʣ��ʱ��Ϊ128
	///////////////////////////////////////////////////////////////////////////////////////////////////////cbcЭ���
	int cbc_incluster;//�Ƿ��ڴ��еı�־  cbc_incluster=0 ���ڴ���,cbc_incluster=1 �ڴ���
	int cbc_ishead;   //�Ƿ�Ϊ��ͷ�ڵ�   cbc_ishead=1 �Ǵ�ͷ   cbc_ishead=0 ���Ǵ�ͷ
	int cbc_isbone;   //�Ƿ�Ϊ�Ǹ���  cbc_isbone=1 �ǹǸ���   cbc_isbone=0  ���ǹǸ���
	int cbc_near; //��Ϊ�Ǹ��������ھ���˭ 
	int cbc_far;  //��Ϊ�Ǹ���Զ���ھ���˭
	list<cbc_packet> cbc_queue;//cbc�����ݰ��Ķ���
	list<cbc_packet> virtual_cbc_queue;//cbc�����ݰ��Ķ���
	list<cbc_head_annouce_packet> head_announce_queue;  //���ذ��Ķ���
	list<cbc_head_annouce_packet> virtual_head_announce_queue;  //���ذ����������
	list<cbc_deny_packet> cbc_deny_packet_queue;      //���ذ��Ķ���
	list<cbc_deny_packet> virtual_cbc_deny_packet_queue;  //���ذ����������
	list<cbc_agree_packet> cbc_agree_packet_queue;      //ͬ����Ķ���
	list<cbc_agree_packet> virtual_cbc_agree_packet_queue;  //ͬ������������
	int stop_announce; //��ͷ��������ͣʱ�䣬����յ�deny�������ͣʱ�����ӣ����Եȴ��ھӳɴذ����ӽ�ȥ
	int wait_to_create_head;  //�ȴ���ý�����ͷ
	vector<vector<int>> saved_path; //���ŷ��ذ���·��   saved_path[2]={6,8,10} ˵�������ɵڶ������ذ���������һ��·����·���а����ڵ㣨������ͷ�����ڵ㣩6����ͷ��.8.10
	vector<vector<int>> node_control; //������Щ�ڵ㱻�Ǹ��ڵ�֧��   node_control[6]={2,9,11} ��ʾ6�Žڵ�֧����2��9��11�����ڵ�
	list<int>	mainstay_1; //�صĹǸ���
	list<int>	mainstay_2; //�Ǹ���
	int inform_interval;// ֪ͨ����������������Ż�㲥֪ͨ��Ϣ
	vector<int> cluster_control; //���ؿ��ƶ��ٸ��ڵ�,�����˴�ͷ���Ǹ���
	vector<int> mainstay_1_control;  //һ��չ���ܿ��ƵĽڵ�
	vector<int> mainstay_2_control;  //����չ�ۿ��ƵĽڵ�
	cbc_inform_packet only_inform_packet;      //֪ͨ���������㲥·����Ϣ�����ÿ���Ǹɽڵ�ֻ��һ������Ϊ���ص���Ƭ
	cbc_inform_packet virtual_only_inform_packet;      //֪ͨ���������㲥·����Ϣ�����ÿ���Ǹɽڵ�ֻ��һ������Ϊ���ص���Ƭ
	list<cbc_inform_packet> exchange_packet_information_queue;      //���ؽڵ����Խ�����·����Ϣ�Ĵ洢����
	list<cbc_inform_packet> virtual_exchange_packet_information_queue;  //���ؽڵ����Խ�����·����Ϣ���������
	vector<int> cbc_information_saved_ID; //���ڵ㴢���ȫ��·����Ϣ��ID   cbc_information_saved_ID[7]=564; �Ҵ����ŵ�7�Ŵ�ͷ�㲥�����µ����µ�IDΪ564
	int hello_interval;
	int cbc_tail_1_validity;  //�Ƿ����յ����ԹǸ���1��ά����Ϣ
	int cbc_tail_2_validity;  //�Ƿ����յ����ԹǸ���1��ά����Ϣ
	int cbc_lonely_validity;  //��������ͷһ��չ�۶�û�У���������һֱ����
	int bone_of_whom;         //����Ϊ�Ǹ����Ĵ��ױ��
	vector<cbc_routing_table_element> cbc_routing_table;  //·�ɱ�
	list<cbc_maintain_packet> cbc_maintain_packet_queue;//�����ֱ�չ�ڵ�ĩβ�����Ĵ�ά��������֪��ͷ���ڵ����
	list<cbc_maintain_packet> virtual_cbc_maintain_packet_queue;//ά�������������
	max_announce_received my_max_announce_received;   //��¼��һ��ʱ�����յ�����ѵ�����������Ķȣ�ֻ��������
	///////////////////////////////////////////////////////////////////////////////////////////////////////////// LCA�㷨����ṹ
	int lca_ishead;     // �Ƿ�Ϊlca�Ĵ�ͷ
	vector<lca_routing_table_element> lca_routing_table;  //lca·�ɱ�
	list<lca_routing_exchange_packet> lca_routing_exchange_packet_queue;  //���յ�·�ɱ���Ϣ���������
	list<lca_routing_exchange_packet> virtual_lca_routing_exchange_packet_queue;  //���յ�·�ɱ���Ϣ���������
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
	reachable.clear();//���ٽӹ�ϵ���
	is_reachable.clear();
	for (int i = 0; i < N; i++)
	{
	 is_reachable.push_back(0); //�ٽӹ�ϵ���
	}
	///////////////////////////////�н�
	switch (direction)
	{
	case 1:       //���� ���½�����0��0 
	{
		y = y + v;
		break;
	}
	case 2:      //����
	{
		x = x + v;
		break;
	}
	case 3:      //����
	{
		y = y - v;
		break;
	}
	case 4:      //����
	{
		x = x - v;
		break;
	}
	}
	////////////////////////////////////////////�߽紦��
	if (x <=0 )  //�������  ģ������
	{
		x = 0;
		direction = 2;
	}
	if (x >= L)  //���ұ���
	{
		x = L;
		direction = 4;
	}
	if (y <= 0)  //���±���
	{
		y = 0;
		direction = 1;
	}
	if (y >= L)  //���ϱ���
	{
		y = L;
		direction = 3;
	}
	if (x%block_width == 0 && y%block_width ==0 && x != 0 && y != 0 && x != L&&y != L)   //x y ���ܱ������������ ˵������·�ڣ��ı䷽��
	{
		cout << "��ID��" << ID << "����·��,·������Ϊ" << x << "," << y << endl;
		switch (turn.front())
		{
		case 0:       //����ֱ�в�������
		{
			break;
		}
		case 1:      //��ת
		{
			direction = direction - 1;
			if (direction == 0)
			{
				direction = 4;   //������ת����
			}
			break;
		}
		case 2:      //��ת
		{
			direction = direction + 1;
			if (direction == 5)
			{
				direction = 1;   //������ת�Ǳ�
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
		turn.pop_front();   //ת�����ˣ���һ��ת��Ԥ������
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
	int virtual_direction = direction; //���Ƶ�ǰ���꣬����
	list <int> virtual_turn=turn;
	while (time_last>1)
	{
		time_last--;
		///////////////////////////////�н�
		switch (virtual_direction)
		{
		case 1:       //���� ���½�����0��0 
		{
			virtual_y = virtual_y + v;
			break;
		}
		case 2:      //����
		{
			virtual_x = virtual_x + v;
			break;
		}
		case 3:      //����
		{
			virtual_y = virtual_y - v;
			break;
		}
		case 4:      //����
		{
			virtual_x = virtual_x - v;
			break;
		}
		}
		////////////////////////////////////////////�߽紦��
		if (virtual_x <= 0)  //�������  ģ������
		{
			virtual_x = 0;
			virtual_direction = 2;
		}
		if (virtual_x >= L)  //���ұ���
		{
			virtual_x = L;
			virtual_direction = 4;
		}
		if (virtual_y <= 0)  //���±���
		{
			virtual_y = 0;
			virtual_direction = 1;
		}
		if (virtual_y >= L)  //���ϱ���
		{
			virtual_y = L;
			virtual_direction = 3;
		}
		if (virtual_x%block_width == 0 && virtual_y%block_width == 0 && virtual_x != 0 && virtual_y != 0 && virtual_x != L&&virtual_y != L)   //x y ���ܱ������������ ˵������·�ڣ��ı䷽��
		{
			//cout << "���⳵ID��" << ID << "��������·��,·������Ϊ" << virtual_x << "," << virtual_y << endl;
			switch (virtual_turn.front())
			{
			case 0:       //����ֱ�в�������
			{
				break;
			}
			case 1:      //��ת
			{
				virtual_direction = virtual_direction - 1;
				if (virtual_direction == 0)
				{
					virtual_direction = 4;   //������ת����
				}
				break;
			}
			case 2:      //��ת
			{
				virtual_direction = virtual_direction + 1;
				if (virtual_direction == 5)
				{
					virtual_direction = 1;   //������ת�Ǳ�
				}
				break;
			}
			}
			virtual_turn.pop_front();   //ת�����ˣ���һ��ת��Ԥ������
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////���ڵ��˶����ˣ���p�ڵ���
		switch (virtual_p_direction)
		{
		case 1:       //���� ���½�����0��0 
		{
			virtual_p_y = virtual_p_y + v;
			break;
		}
		case 2:      //����
		{
			virtual_p_x = virtual_p_x + v;
			break;
		}
		case 3:      //����
		{
			virtual_p_y = virtual_p_y - v;
			break;
		}
		case 4:      //����
		{
			virtual_p_x = virtual_x - v;
			break;
		}
		}
		////////////////////////////////////////////�߽紦��
		if (virtual_p_x <= 0)  //�������  ģ������
		{
			virtual_p_x = 0;
			virtual_p_direction = 2;
		}
		if (virtual_x >= L)  //���ұ���
		{
			virtual_p_x = L;
			virtual_p_direction = 4;
		}
		if (virtual_p_y <= 0)  //���±���
		{
			virtual_p_y = 0;
			virtual_p_direction = 1;
		}
		if (virtual_p_y >= L)  //���ϱ���
		{
			virtual_p_y = L;
			virtual_p_direction = 3;
		}
		if (virtual_p_x%block_width == 0 && virtual_p_y%block_width == 0 && virtual_p_x != 0 && virtual_p_y != 0 && virtual_p_x != L&&virtual_y != L)   //x y ���ܱ������������ ˵������·�ڣ��ı䷽��
		{
			//cout << "����p��ID��" << ID << "��������·��,·������Ϊ" << virtual_p_x << "," << virtual_p_y << endl;
			switch (virtual_p_turn.front())
			{
			case 0:       //����ֱ�в�������
			{
				break;
			}
			case 1:      //��ת
			{
				virtual_p_direction = virtual_p_direction - 1;
				if (virtual_p_direction == 0)
				{
					virtual_p_direction = 4;   //������ת����
				}
				break;
			}
			case 2:      //��ת
			{
				virtual_p_direction = virtual_p_direction + 1;
				if (virtual_p_direction == 5)
				{
					virtual_p_direction = 1;   //������ת�Ǳ�
				}
				break;
			}
			}
			virtual_p_turn.pop_front();   //ת�����ˣ���һ��ת��Ԥ������
		}
		if (sqrt((virtual_x - virtual_p_x)*(virtual_x - virtual_p_x) + (virtual_y - virtual_p_y)*(virtual_y - virtual_p_y))>r) // ����Ƚϳ����Ѿ���·����
		{
			is_keep_connected = 0;  //��·����
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
		realpacket.scource = realpacket.current;  //���ݰ�Դ��ַ
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
	realpacket.scource = realpacket.current;  //���ݰ�Դ��ַ
	return realpacket;
}

void create_node(node** car,int block_width,int v,double r,int N)
{
	int pre_turn_time = 80;  //Ԥ��ת����ٴ�
	double rand_num = 0;  //0-1 ֮������С��
	for (int start_node = 0; start_node < N; start_node++)
	{
		car[start_node] = new node();//node��ʵ����ʼ��
		car[start_node]->ID = start_node;
		car[start_node]->v = v;
		car[start_node]->r = r;
		////////////////////////////////////////////////////������aodvЭ������ĳ�ʼ��
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
			all_zero_line.push_back(0);   //������ά�����ʼ��Ϊ0
		}
		for (int i = 0; i < N; i++)
		{
			car[start_node]->aodvjr_back_table.push_back(-1);  //����·�ɶ���ʼ��Ϊ-1
			car[start_node]->aodvjr_table.push_back(-1);
			car[start_node]->aodvjr_quest_times.push_back(0);
			car[start_node]->aodvjr_saved_ID.push_back(all_zero_line); //�����������ռ��
			car[start_node]->node_control.push_back(nothing);     //����յ���ռλ��
			car[start_node]->aodvjr_table_expiry.push_back(0);  //��·ʣ��ʱ��Ķ���Ϊ0
			car[start_node]->aodvjr_back_expiry.push_back(0);  //��Ϊ0
			car[start_node]->is_reachable.push_back(0); //ȫ�������ھ�
		}
		////////////////////////////////////////////////////////����������cbcЭ��Ҫ����һЩ��ʼ��
		car[start_node]->cbc_incluster=0;//�Ƿ��ڴ��еı�־  cbc_incluster=0 ���ڴ���,cbc_incluster=1 �ڴ���
		car[start_node]->cbc_ishead=0;   //�Ƿ�Ϊ��ͷ�ڵ�   cbc_ishead=1 �Ǵ�ͷ   cbc_ishead=0 ���Ǵ�ͷ
		car[start_node]->cbc_isbone=0;   //�Ƿ�Ϊ�Ǹ���  cbc_isbone=1 �ǹǸ���   cbc_isbone=0  ���ǹǸ���
		car[start_node]->bone_of_whom = -1;  //��˭�Ĵ�ͷҲ����
		car[start_node]->cbc_near = -1; //��Ϊ�Ǹ��������ھ���˭ 
		car[start_node]->cbc_far = -1;  //��Ϊ�Ǹ���Զ���ھ���˭
		car[start_node]->my_max_announce_received.max_announce_node_degree = -1;
		car[start_node]->my_max_announce_received.max_announce_node_ID = -1;
		car[start_node]->my_max_announce_received.wait_time_window = -1;
		car[start_node]->my_max_announce_received.is_hearing = 1;
		car[start_node]->only_inform_packet.effective = 0;
		for (int i = 0; i < N; i++)
		{
			car[start_node]->cbc_information_saved_ID.push_back(-1); //�Ҵ����ŵ����д�ͷ�㲥�����µ����µ�IDΪ-1
		}
		car[start_node]->stop_announce=0; 
		car[start_node]->wait_to_create_head = M_A_X;
		car[start_node]->inform_interval=0;// ֪ͨ����������������Ż�㲥֪ͨ��Ϣ
		/////////////////////////////////////////////////////////////////
		rand_num = (double)rand() / RAND_MAX;
		if (rand_num < 0.25)
		{
			car[start_node]->x = ((rand() % (L / block_width - 1)) + 1)*block_width;      //��1000��9000֮���һ�������
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
		for (int i = 0; i < pre_turn_time - 1; i++)        //��Ҫ����ĳ���Ԥ�⼸��
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
		//cout << start_node << "�Žڵ��ʼ�����������" << car[start_node]->x << "," << car[start_node]->y << "�ٶ�Ϊ" << car[start_node]->v << "����Ϊ" << car[start_node]->direction << "�´�ת�䷽��Ϊ" << car[start_node]->turn.front() << endl;
		//////////////////////////////////////////////////////////////������lcaЭ���һЩ��ʼ��
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
	//////////////////////////////////////////////////////////////////�����ٽӹ�ϵ
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
					//cout << process_node << "�Žڵ�ɵ����ھ�Ϊ" << toreach << endl;
				}
				else{}
			}
			else {}
		}
	}
}

void cbc_calculate_connection(node** car, double r, int N)
{
	//////////////////////////////////////////////////////////////////�����ٽӹ�ϵ
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
	cout << "���������" << RREQ_sum << "���ذ�����" << RREP_sum << "���ݰ�����" << queue_sum << endl;
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
			//cout << i << "�Ǵ�ͷ" << endl;
		}
		queue_sum = queue_sum + car[i]->cbc_queue.size();
	}
	cout << "��ͷ����" << head_sum << "���ݰ�����" << queue_sum << endl;
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
			//cout << i << "�Ǵ�ͷ" << endl;
		}
		queue_sum = queue_sum + car[i]->lca_packet_queue.size();
	}
	cout << "��ͷ����" << head_sum << "���ݰ�����" << queue_sum << endl;
	//system("pause");
}

void create_packet(node** car,double packet_spawn_rate,int N)
{
	for (int start_packet = 0; start_packet<(int)(packet_spawn_rate*N); start_packet++)
	{
		packet new_packet = create_single_packet(N);
		//cout << new_packet.current << " " << new_packet.destination << endl;
		car[new_packet.current]->queue.push_back(new_packet);  //�������ݰ�
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
				car[i]->aodvjr_table_expiry[j]--;    //�־�����һ������·����ʱ
				if (car[i]->aodvjr_table_expiry[j] <= 0)  //��ǰ�ڵ㵽Ŀ�Ľڵ��Ѿ��ܾ�û�����ݰ�������
				{
					if (car[i]->aodvjr_table[j] != -1)
					{
						//cout << "��" << i << "��" << j << "����" << car[i]->aodvjr_table[j] << "������·��ʧЧ" << endl;
						car[i]->aodvjr_table[j] = -1;  //��·ʧЧ
					}
				}
				car[i]->aodvjr_back_expiry[j]--;    //�־�����һ����������·����ʱ
				if (car[i]->aodvjr_back_expiry[j] <= 0)  //��ǰ�ڵ㵽Դ�ڵ��Ѿ��ܾ�û�����ݰ�������
				{
					if (car[i]->aodvjr_back_table[j] != -1)
					{
						//cout << "��" << i << "��" << j << "����" << car[i]->aodvjr_back_table[j] << "�ķ���·��ʧЧ" << endl;
						car[i]->aodvjr_back_table[j] = -1;  //��·ʧЧ				
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
			if (i != j)    //ȫ���ڵ�ԣ���i��j���ͷ��ذ���ά����·
			{
				aodvjr_RREP_packet   new_RREP_packet;
				new_RREP_packet.current = car[i]->aodvjr_back_table[j];
				new_RREP_packet.scource = i;
				new_RREP_packet.destination = j;
				new_RREP_packet.T = 0;
				if (new_RREP_packet.current!=-1)  //�����ڷ���·��Ҳ�Ͳ���ά��
				{
					if (car[i]->is_reachable[new_RREP_packet.current] == 1)  //�������������·ȷʵ����
					{
						control_packet++;
						//car[new_RREP_packet.current]->RREP_queue.push_back(new_RREP_packet);
						car[new_RREP_packet.current]->virtual_RREP_queue.push_back(new_RREP_packet);
						if (car[new_RREP_packet.current]->aodvjr_table[i] = i)
						{
							//cout << "����·��(ֻһ��)��" << new_RREP_packet.current << "��Ŀ�ĵ�" << i << "������·��Ϊ" << car[new_RREP_packet.current]->aodvjr_table[i] << "ƥ��" << i << "���´��ʱ��" << endl;
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
	for (int traversal_packet = 0; traversal_packet<N; traversal_packet++)   //�������ݰ�
	{
		for (list<aodvjr_RREP_packet>::iterator i = car[traversal_packet]->RREP_queue.begin(); i != car[traversal_packet]->RREP_queue.end();)  //����i++ д������,��Ҫ����һЩɾ������
		{
			//cout << traversal_packet << "�Žڵ�����AODV���ذ���Դ" << i->scource << "Ŀ��" << i->destination << "��ǰ" << i->current << endl;
			if (i->destination == i->current)                  //���Ŀ���Ѿ��ǵ�ǰ
			{
				//cout << traversal_packet << "���ذ��Ѿ��ʹ�Դ�ڵ㣬·�ɽ���" << endl;
				car[traversal_packet]->RREP_queue.erase(i++); //���ذ��Ѿ�����λ�ã�ɾ��
			}
			else
			{     //���ذ�����ǰ��
				int temp = car[i->current]->aodvjr_back_table[i->destination];
				if (temp != -1)   //�����ڷ���·��
				{
					aodvjr_RREP_packet new_RREP_packet = *i;
					new_RREP_packet.current = temp;
					if (car[traversal_packet]->is_reachable[temp]==1)  //ȷʵ������ôһ����·�����Է������ݰ�
					{
						//car[temp]->RREP_queue.push_back(new_RREP_packet);
						car[temp]->virtual_RREP_queue.push_back(new_RREP_packet);
						if (car[temp]->aodvjr_table[i->scource] == traversal_packet)  //֮ǰ������������·�ɣ����ڷ������ݰ��������ˣ�����һ�´��ʱ��
						{
							car[temp]->aodvjr_table_expiry[i->scource] = aodvjr_route_expiry;
							//cout << "����·�ɣ�" << temp << "��Ŀ�ĵ�" << i->scource << "������·��Ϊ" << car[temp]->aodvjr_table[i->scource] << "ƥ��" << traversal_packet <<"���´��ʱ��"<< endl;
						}
						if (car[temp]->aodvjr_table[i->scource] == -1)
						{
							car[temp]->aodvjr_table[i->scource] = traversal_packet;
							//cout << "����·�ɽ����ӵ�ǰλ�ã�" << temp << "��Ŀ�ĵ�" << i->scource << "������·��Ϊ" << car[temp]->aodvjr_table[i->scource] << endl;
							car[temp]->aodvjr_table_expiry[i->scource] = aodvjr_route_expiry;  //�½�����·�ɣ�����ʱ�丳ֵ
						}
					}
				}
				car[traversal_packet]->RREP_queue.erase(i++);  //�����ܲ����ͣ�ɾ
			}
		}
	}
}

void handle_aodvjr_RREQ_packet(node** car, int N, int aodv_RREQ_expiry,long int aodvjr_route_expiry,long long int &control_packet)
{
	for (int traversal_packet = 0; traversal_packet<N; traversal_packet++)    //����
	{
		for (list<aodvjr_RREQ_packet>::iterator i = car[traversal_packet]->RREQ_queue.begin(); i != car[traversal_packet]->RREQ_queue.end();)  //����i++ д������,��Ҫ����һЩɾ������
		{
			//cout << traversal_packet << "�Žڵ�����AODV�������Դ" << i->scource << "Ŀ��" << i->destination << "��ǰ" << i->current << "  IDΪ" << i->ID << "ʱ��Ϊ" << i->T << endl;
			if (i->T>aodv_RREQ_expiry)  //�������  
			{
				//cout << traversal_packet << "�Žڵ�����AODV�����ʧЧ������ɾ��" << endl;
				car[traversal_packet]->RREQ_queue.erase(i++);
			}
			else
			{
				if (i->destination == i->current)                  //���Ŀ���Ѿ��ǵ�ǰ
				{
					//cout << traversal_packet << "�Žڵ�����AODV���������Ŀ�ģ�ɾ���������ذ�" << endl;
					aodvjr_RREP_packet   new_RREP_packet;
					new_RREP_packet.current = car[i->destination]->aodvjr_back_table[i->scource];
					new_RREP_packet.scource = i->destination;
					new_RREP_packet.destination = i->scource;
					if (new_RREP_packet.current!=-1)  //�����ڷ���·�� ���跢���ذ�
					{
						if (car[i->destination]->is_reachable[new_RREP_packet.current] == 1)  //�������������·ȷʵ����
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
					car[traversal_packet]->RREQ_queue.erase(i++); //���ذ��ѷ���ɾ�������
				}
				else
				{     //�ٴι㲥�����
					for (int j = 0; j < car[i->current]->reachable.size(); j++)
					{
						if (car[i->current]->reachable[j] != i->scource)
						{
							aodvjr_RREQ_packet quest_packet = *i;
							quest_packet.current = car[i->current]->reachable[j];
							quest_packet.T = quest_packet.T + 1;
							control_packet++;
							//cout << "ת����������" << "��Դ�ڵ�Ϊ" << quest_packet.scource << "Ŀ�Ľڵ�Ϊ" << quest_packet.destination << "��ǰλ��Ϊ" << quest_packet.current << "ʱ��Ϊ" << quest_packet.T << "IDΪ" << quest_packet.ID<< endl;
							if (car[quest_packet.current]->aodvjr_back_table[quest_packet.scource] == -1)
							{
								car[quest_packet.current]->aodvjr_back_table[quest_packet.scource] = traversal_packet;  //��ǰ�ڵ㽨����Դ�ķ���·��
								//cout << "����·�ɽ����ӵ�ǰλ�ã�" << quest_packet.current << "��Դ" << quest_packet.scource << "�ķ���·��Ϊ" << car[quest_packet.current]->aodvjr_back_table[quest_packet.scource] << endl;
								car[quest_packet.current]->aodvjr_back_expiry[quest_packet.scource] = aodvjr_route_expiry;
							}
							if (car[car[i->current]->reachable[j]]->aodvjr_saved_ID[quest_packet.scource][quest_packet.destination] < quest_packet.ID)  //���սڵ��ж��£��µ�IDӦ�ñȾɵ�Ҫ�󣬷��򲻽���
							{
								//car[car[i->current]->reachable[j]]->RREQ_queue.push_back(quest_packet);
								car[car[i->current]->reachable[j]]->virtual_RREQ_queue.push_back(quest_packet);
								car[car[i->current]->reachable[j]]->aodvjr_saved_ID[quest_packet.scource][quest_packet.destination] = quest_packet.ID;
							}
						}
					}
					car[traversal_packet]->RREQ_queue.erase(i++);  //���ݰ��Ѿ�ת�� ��ԭ����ɾ��
				}
			}
		}
	}
}

void packet_delivery(node** car, int N, int aodvjr_route_expiry, long int &packet_arrive, long int &packet_drop, long int &packet_life_time,long long int &control_packet, int packet_expiry, int aodv_RREQ_expiry)
{
	for (int traversal_packet = 0; traversal_packet<N; traversal_packet++)   //���ݰ�
	{
		for (list<packet>::iterator i = car[traversal_packet]->queue.begin(); i != car[traversal_packet]->queue.end();)
		{
			if (i->T>packet_expiry)
			{
				//cout << "���ݰ�����ʧЧ" << endl;
				packet_drop++;//������
				car[traversal_packet]->queue.erase(i++);
			}
			else
			{
				//cout << traversal_packet << "�Žڵ����ݰ��Ǵ�" << i->current << "��" << i->destination << endl;
				if (i->current == i->destination)  //�Ѿ��ʹ�
				{
					//cout << "���ݰ��ɹ��ʹ�" << endl;
					packet_life_time = packet_life_time + i->T;
					packet_arrive++;//������
					car[traversal_packet]->queue.erase(i++);
				}
				else
				{
					if (car[i->current]->aodvjr_table[i->destination] == -1)   //��֪����ô��
					{
						car[i->current]->aodvjr_quest_times[i->destination] = car[i->current]->aodvjr_quest_times[i->destination] + 1;  //��ǰ��Ŀ�ĵ������������
						if (car[i->current]->aodvjr_quest_times[i->destination] % (aodv_RREQ_expiry*2)==1)   //�����ذ������������������Ϊ���ڣ�����������
						{
							for (int j = 0; j < car[i->current]->reachable.size(); j++)
							{
								aodvjr_RREQ_packet quest_packet;
								quest_packet.ID = car[i->current]->aodvjr_quest_times[i->destination];
								quest_packet.current = car[i->current]->reachable[j];
								quest_packet.scource = i->current;
								quest_packet.destination = i->destination;
								quest_packet.T = 1;
								//cout << "����������" << "��Դ�ڵ�Ϊ" << quest_packet.scource << "Ŀ�Ľڵ�Ϊ" << quest_packet.destination << "��ǰλ��Ϊ" << quest_packet.current << "IDΪ" << quest_packet.ID<< endl;
								control_packet++;
								if (car[quest_packet.current]->aodvjr_back_table[quest_packet.scource] == -1)
								{
									car[quest_packet.current]->aodvjr_back_table[quest_packet.scource] = traversal_packet;  //��ǰ�ڵ㽨����Դ�ķ���·��
									//cout << "����·�ɽ����ӵ�ǰλ�ã�" << quest_packet.current << "��Դ" << quest_packet.scource << "�ķ���·��Ϊ" << car[quest_packet.current]->aodvjr_back_table[quest_packet.scource] << endl;
									car[quest_packet.current]->aodvjr_back_expiry[quest_packet.scource] = aodvjr_route_expiry;
								}
								//car[car[i->current]->reachable[j]]->RREQ_queue.push_back(quest_packet);
								car[car[i->current]->reachable[j]]->virtual_RREQ_queue.push_back(quest_packet);
								car[car[i->current]->reachable[j]]->aodvjr_saved_ID[quest_packet.scource][quest_packet.destination] = quest_packet.ID;  //�ھ��ڸ��������µı��
							}
						}
						i->T++;
						packet  new_packet = *i;
						car[new_packet.current]->virtual_queue.push_back(new_packet);
						i++;   //���ݰ���Ҫ�������ƽ�������У���������ǰ
					}
					else  //֪�����ݰ���ô��
					{
						packet  new_packet = *i;
						int temp = car[i->current]->aodvjr_table[i->destination];
						new_packet.current = temp;
						new_packet.T = i->T + 1;
						if (car[i->current]->is_reachable[temp] == 1)   //ʵ���������·Ҳ�Ǵ��ڵ�
						{
							//car[temp]->queue.push_back(new_packet);
							car[temp]->virtual_queue.push_back(new_packet);
							if (car[temp]->aodvjr_back_table[i->scource] == i->current)   //�������·�ɱ������¼�����������ݰ�����������ھӣ�����·�ɱ�����Ч
							{
								car[temp]->aodvjr_back_expiry[i->scource] = aodvjr_route_expiry;
								//cout << "Դ" << i->scource << "��" << i->current << "�͵�" << temp << "Ŀ��" << i->destination << "��¼�ķ���·��Ϊ" << car[temp]->aodvjr_back_table[i->scource] << "ƥ�䣬����·�ɸ��´��ʱ��" << endl;
							}
							else
							{
								//cout << "Դ" << i->scource << "��" << i->current << "�͵�" << temp << "Ŀ��" << i->destination << "��¼�ķ���·��Ϊ" << car[temp]->aodvjr_back_table[i->scource] <<"����ƥ��"<< endl;
							}
						}
						else
						{
							//cout << "Դ" << i->scource << "��" << i->current << "�͵�" << temp << "Ŀ��" << i->destination << "������·��ͨû���ʹ�" << endl;
							packet_drop++;
						}
						car[traversal_packet]->queue.erase(i++);
					}
				}
			}
		}
	}
}

void check_in_cluster(node** car, int N, int cluster_radius)   //����Ƿ��ڴ���
{
	for (int i = 0; i < N; i++)
	{
		int in_or_not = 0;
		for (int j = 0; j < car[i]->reachable.size(); j++)
		{
			if (car[car[i]->reachable[j]]->cbc_isbone == 1 || car[car[i]->reachable[j]]->cbc_ishead == 1)
			{
				in_or_not = 1;  //������κ�һ���ھ��Ǵ�ͷ���߹Ǹ�
			}
		}
		car[i]->cbc_incluster = in_or_not;
	}
}

void announce_cbchead(node** car, int N, int cluster_radius, long int &control_packet)
{
	for (int i = 0; i < N; i++)
	{
		if (car[i]->cbc_incluster==0)  //���ڴ���
		{
			if (car[i]->stop_announce == 0)   //��ͣ�������ɹ���������
			{
				for (int o = 0; o < N; o++)   //���Լ�֪���Ľڵ�Ŀ��ƹ�ϵ���
				{
					car[i]->node_control[o].clear();
				}
				car[i]->saved_path.clear();  //�ѷ��ذ����ص�·�����
				car[i]->cluster_control.clear(); //�Ѵ�ͷ���Ƶ�ȫ���ڵ������
				for (int j = 0; j < car[i]->reachable.size(); j++)  //���ھӹ㲥��ͷ����
				{
					if (car[car[i]->reachable[j]]->cbc_ishead == 0 && car[car[i]->reachable[j]]->cbc_isbone == 0)  //�ھӼȲ��Ǵ�ͷҲ���ǹǸ�
					{
						cbc_head_annouce_packet new_cbc_head_announce_packet;
						for (int k = 0; k < N; k++)
						{
							new_cbc_head_announce_packet.is_passed_by.push_back(0);
						}
						new_cbc_head_announce_packet.current = car[i]->reachable[j];
						new_cbc_head_announce_packet.scource = i;
						new_cbc_head_announce_packet.T = 1;
						new_cbc_head_announce_packet.degree = car[i]->reachable.size();  //�ѽڵ�Ķȼ��������
						new_cbc_head_announce_packet.cbc_announce_path.push_back(i);  //��Դ�ڵ�λ��λ�ü���·��֮��
						new_cbc_head_announce_packet.is_passed_by[i] = 1;  //Դ�ڵ�һ���Ǿ����˵�
						control_packet++;
						car[new_cbc_head_announce_packet.current]->virtual_head_announce_queue.push_back(new_cbc_head_announce_packet);  //ѹ���������
					}
				}
				car[i]->stop_announce = car[i]->stop_announce + cluster_radius * 3 + 2;  //�ɹ����������������ͣ���������ݰ��鷺
				car[i]->wait_to_create_head = cluster_radius * 3 + 1;   //�ȴ�һ��ʱ�����ɴ�ͷ
			}
			else
			{
				car[i]->stop_announce--;  //���ɴ�����ͣ�ڣ�ɶҲ����
				car[i]->wait_to_create_head--;
			}	
		}
	}
}

void handle_cbchead_announce(node** car, int N, int cluster_radius, int block_width, int forecast_time,long int &control_packet)  //�����ͷ����
{
	for (int i = 0; i < N; i++)
	{
		for (list<cbc_head_annouce_packet>::iterator j = car[i]->head_announce_queue.begin(); j != car[i]->head_announce_queue.end();)  //����i++ д������,��Ҫ����һЩɾ������
		{
			if (car[i]->cbc_ishead == 0 || car[i]->cbc_isbone == 0)  //���ڴ���
			{
				if (car[i]->virtual_move(*car[j->cbc_announce_path.back()], block_width, N, forecast_time) == 1)  //���Ǵ�ǰ�ᣬ��·����
				{
					if (car[i]->reachable.size() > j->degree || (car[i]->reachable.size() == j->degree&&i<j->scource))  //����ڵ�Ķȴ����յ����ݰ��Ķ�,�ֻ��߶���ȵ���ID�Ƚ�С
					{
						if (car[i]->cbc_incluster==0)   //���Ҹ�����ڵ�һ��,���ڴ��У��ŷ����ذ�
						{
							cbc_deny_packet new_cbc_deny_packet;
							new_cbc_deny_packet.destination = j->scource; //���ذ���Ŀ��Ϊ�������Դ
							new_cbc_deny_packet.deny_path = j->cbc_announce_path;
							int temp = new_cbc_deny_packet.deny_path.back();  //���ذ���Ҫȥ����һ��
							new_cbc_deny_packet.current = temp;
							new_cbc_deny_packet.deny_path.pop_back();
							if (car[i]->is_reachable[temp] == 1)  //���������·ȷʵ����
							{
								car[temp]->virtual_cbc_deny_packet_queue.push_back(new_cbc_deny_packet);  //�����ذ�ѹ�뷵�ض���
								control_packet++;
							}
						}
					}
					else //����ڵ�Ķ�С�ڵ����յ������ݰ��Ķ�
					{
						if (car[i]->my_max_announce_received.is_hearing==1)   //����һ��ڽ���
						{
							if (car[i]->my_max_announce_received.max_announce_node_ID == -1)    //һ�����ݶ�û
							{
								car[i]->my_max_announce_received.wait_time_window = cluster_radius;    //�����յ���һ���������ʼ,��һ�����ȷ�����׵�˭�Ĵ�ͷ
								car[i]->my_max_announce_received.max_announce_node_ID = j->scource;
								car[i]->my_max_announce_received.max_announce_node_degree = j->degree;
								car[i]->my_max_announce_received.all_virtual_cbc_agree_packet.clear();
								cbc_agree_packet new_cbc_agree_packet;
								new_cbc_agree_packet.destination = j->scource; //ͬ�����Ŀ��Ϊ�������Դ
								new_cbc_agree_packet.scource = i;   //��ǰ�ڵ�Ϊͬ�����Դ
								new_cbc_agree_packet.agree_path = j->cbc_announce_path; //ͬ�����·��Ϊ�������·��
								new_cbc_agree_packet.report_path = new_cbc_agree_packet.agree_path;  //�㱨·��Ϊ����·��
								new_cbc_agree_packet.report_path.push_back(i);//�㱨·����ԴҲ���ȥ 
								new_cbc_agree_packet.depth = j->T;    //���ݰ�������ʱ��Ϊͬ��������
								new_cbc_agree_packet.control_neighbor = car[i]->reachable;  //�ѿ�����Щ�ھ�д������
								new_cbc_agree_packet.current = new_cbc_agree_packet.agree_path.back();
								new_cbc_agree_packet.agree_path.pop_back();
								car[i]->my_max_announce_received.all_virtual_cbc_agree_packet.push_back(new_cbc_agree_packet);   //�������
							}
							else   //�һ��������ݵģ�Ҳ����֮ǰ�յ���������
							{
								if (j->scource==car[i]->my_max_announce_received.max_announce_node_ID)    //�������֮ǰ��¼�е���ͬһ���ڵ�
								{
									cbc_agree_packet new_cbc_agree_packet;
									new_cbc_agree_packet.destination = j->scource; //ͬ�����Ŀ��Ϊ�������Դ
									new_cbc_agree_packet.scource = i;   //��ǰ�ڵ�Ϊͬ�����Դ
									new_cbc_agree_packet.agree_path = j->cbc_announce_path; //ͬ�����·��Ϊ�������·��
									new_cbc_agree_packet.report_path = new_cbc_agree_packet.agree_path;  //�㱨·��Ϊ����·��
									new_cbc_agree_packet.report_path.push_back(i);//�㱨·����ԴҲ���ȥ 
									new_cbc_agree_packet.depth = j->T;    //���ݰ�������ʱ��Ϊͬ��������
									new_cbc_agree_packet.control_neighbor = car[i]->reachable;  //�ѿ�����Щ�ھ�д������
									new_cbc_agree_packet.current = new_cbc_agree_packet.agree_path.back();
									new_cbc_agree_packet.agree_path.pop_back();
									car[i]->my_max_announce_received.all_virtual_cbc_agree_packet.push_back(new_cbc_agree_packet);   //�������
								}
								else
								{
									if (j->degree > car[i]->my_max_announce_received.max_announce_node_degree || (j->degree == car[i]->my_max_announce_received.max_announce_node_degree&&j->scource < car[i]->my_max_announce_received.max_announce_node_ID))   //Ҫô�ȴ�Ҫô����ͬ���С,���ָ��õ��ˣ�������
									{
										car[i]->my_max_announce_received.max_announce_node_ID = j->scource;
										car[i]->my_max_announce_received.max_announce_node_degree = j->degree;
										car[i]->my_max_announce_received.all_virtual_cbc_agree_packet.clear();
										cbc_agree_packet new_cbc_agree_packet;
										new_cbc_agree_packet.destination = j->scource; //ͬ�����Ŀ��Ϊ�������Դ
										new_cbc_agree_packet.scource = i;   //��ǰ�ڵ�Ϊͬ�����Դ
										new_cbc_agree_packet.agree_path = j->cbc_announce_path; //ͬ�����·��Ϊ�������·��
										new_cbc_agree_packet.report_path = new_cbc_agree_packet.agree_path;  //�㱨·��Ϊ����·��
										new_cbc_agree_packet.report_path.push_back(i);//�㱨·����ԴҲ���ȥ 
										new_cbc_agree_packet.depth = j->T;    //���ݰ�������ʱ��Ϊͬ��������
										new_cbc_agree_packet.control_neighbor = car[i]->reachable;  //�ѿ�����Щ�ھ�д������
										new_cbc_agree_packet.current = new_cbc_agree_packet.agree_path.back();
										new_cbc_agree_packet.agree_path.pop_back();
									}
								}
							}
						}
					}
					if (j->T < cluster_radius)  //û�е�������������,���԰������ת��һ��
					{			
						for (int k = 0; k < car[i]->reachable.size(); k++)  //���ھӹ㲥��ͷ����
						{
						    cbc_head_annouce_packet new_head_annouce_packet = *j;  //�µ�����ת�����ھ�
							new_head_annouce_packet.current = car[i]->reachable[k];
							new_head_annouce_packet.T = new_head_annouce_packet.T+1;
							new_head_annouce_packet.cbc_announce_path.push_back(i);  //��ת����λ�ü���·��֮��
							new_head_annouce_packet.is_passed_by[i] = 1;  //ת���ڵ�Ҳ��������
							if (new_head_annouce_packet.is_passed_by[car[i]->reachable[k]] == 0 && car[car[i]->reachable[k]]->cbc_isbone == 0 && car[car[i]->reachable[k]]->cbc_ishead == 0)   //ֻ�������û�о����ھ�,�����ھӲ��Ǵ�ͷҲ���ǹǸ�
							{
								car[new_head_annouce_packet.current]->virtual_head_announce_queue.push_back(new_head_annouce_packet);  //ѹ���������
							}
						}
					}
				}
			}
			car[i]->head_announce_queue.erase(j++); //ת������ȶ������ˣ�����ͷ����ɾ��
		}
	}
}

void handle_deny_packet(node** car, int N, int cluster_radius, int stop_time_increase)
{
	for (int i = 0; i < N; i++)
	{
		for (list<cbc_deny_packet>::iterator j = car[i]->cbc_deny_packet_queue.begin(); j != car[i]->cbc_deny_packet_queue.end();)  //����i++ д������,��Ҫ����һЩɾ������
		{
			if (j->destination == j->current)   //�Ѿ��͵�Ŀ�ĵ�
			{
				car[i]->wait_to_create_head = stop_time_increase;  //�յ��˲��ذ�����ͣʱ�����ӣ���ͣ������
			}
			else  //û�͵�
			{
				cbc_deny_packet new_deny_packet = *j;
				new_deny_packet.current = new_deny_packet.deny_path.back(); //���ذ���Ҫȥ����һ��
				new_deny_packet.deny_path.pop_back();
				if (car[i]->is_reachable[new_deny_packet.current] == 1)  //���������·ȷʵ����
				{
					car[new_deny_packet.current]->virtual_cbc_deny_packet_queue.push_back(new_deny_packet);  //�����ذ�ѹ�뷵�ض���
				}
			}
			car[i]->cbc_deny_packet_queue.erase(j++); //���������ɾ�����ذ�
		}
	}
}

void check_agree_time_window(node** car, int N, int cluster_radius)
{
	for (int i = 0; i < N; i++)
	{
		if (car[i]->my_max_announce_received.wait_time_window<=0&&car[i]->my_max_announce_received.max_announce_node_ID!=-1)  //���ʱ�䴰��Ϊ0 ���������¼��һ���˷����Ķ���
		{
			for (int j = 0; j < car[i]->my_max_announce_received.all_virtual_cbc_agree_packet.size(); j++)
			{
				int temp = car[i]->my_max_announce_received.all_virtual_cbc_agree_packet[j].current;
				car[temp]->virtual_cbc_agree_packet_queue.push_back(car[i]->my_max_announce_received.all_virtual_cbc_agree_packet[j]);  //����ʱ����ȫ���������ط�
			}
			car[i]->my_max_announce_received.max_announce_node_ID = -1;
			car[i]->my_max_announce_received.max_announce_node_degree = -1;
			car[i]->my_max_announce_received.is_hearing = 0;   //���ٽ�����
			car[i]->my_max_announce_received.all_virtual_cbc_agree_packet.clear();
			car[i]->my_max_announce_received.wait_time_window = 0; //ͬ��������ȴ�һ��ʱ�䣨�ȴ���ͷ������ �����Ƿ�Ҫ��������
		}
		if (car[i]->my_max_announce_received.wait_time_window <= 0 && car[i]->my_max_announce_received.max_announce_node_ID == -1)  //ͬ����������һ��ʱ���ˣ��Լ�û��¼�����Լ�������
		{
			car[i]->my_max_announce_received.is_hearing = 1;
		}
		if (car[i]->my_max_announce_received.wait_time_window>0)
		{
			car[i]->my_max_announce_received.wait_time_window--;   //ʱ�䴰���Լ�
		}
	}
}

void handle_agree_packet(node** car, int N, int cluster_radius)
{
	for (int i = 0; i < N; i++)
	{
		for (list<cbc_agree_packet>::iterator j = car[i]->cbc_agree_packet_queue.begin(); j != car[i]->cbc_agree_packet_queue.end();)  //����i++ д������,��Ҫ����һЩɾ������
		{
			if (j->destination == j->current)   //�յ���ͬ�����·���洢������׼�������أ�
			{
				car[i]->saved_path.push_back(j->report_path); //�ѻ㱨·���浽�ڵ������֮��ȥ
				if (car[i]->node_control[j->scource].empty()==true)  //���i��Ŀ��ڵ���Ϣ---֧�伯����û����
				{
					
					car[i]->node_control[j->scource] = j->control_neighbor;  //�ѿ��ƵĶ����ھӵ���Ϣ�������ͷ����i��ͷ��֪��j->scource��������Щ�ھ�
				}
			}
			else  //û�͵�
			{
				cbc_agree_packet new_agree_packet = *j;
				new_agree_packet.current = new_agree_packet.agree_path.back();  //���ذ���Ҫȥ����һ��
				new_agree_packet.agree_path.pop_back();
				if (car[i]->is_reachable[new_agree_packet.current] == 1)  //���������·ȷʵ����
				{
					car[new_agree_packet.current]->virtual_cbc_agree_packet_queue.push_back(new_agree_packet);  //�����ذ�ѹ�뷵�ض���
				}
			}
			car[i]->cbc_agree_packet_queue.erase(j++); //���������ɾ�����ذ�
		}
	}
}

void create_cluster(node** car, int N, int cluster_radius,int tail_validity)   //���ɴ�
{
	for (int i = 0; i < N; i++)  //����ȫ���ڵ�
	{
		if (car[i]->cbc_ishead == 0 && car[i]->cbc_incluster == 0)   //�Ȳ��Ǵ�ͷҲû���ڴ���
		{
			if (car[i]->wait_to_create_head ==0)  //��ʾ������������һ��ʱ����
			{
				car[i]->cbc_ishead = 1; //��ͷ�����ˣ��Ҿ��Ǵ�ͷ
				car[i]->cbc_isbone = 0; 
				car[i]->cbc_incluster = 1;//���ڴ�������
				car[i]->inform_interval = 0;
				car[i]->mainstay_1.clear();
				car[i]->mainstay_2.clear();//�Ǹ�������
				car[i]->cbc_tail_1_validity = tail_validity;  //����չ�ۻ����д��ʱ��
				car[i]->cbc_tail_2_validity = tail_validity;
				car[i]->cbc_lonely_validity = tail_validity;
				car[i]->bone_of_whom = i;  //����Ϊ��ͷ���Լ����Լ��ĹǸ���
				if (car[i]->saved_path.size()>=2)    //�����յ�����ͬ���
				{
					int max_control = 0;
					int record_1 = 0;  //����·���ı��
					int record_2 = 0;
					vector<int> path_control_max;
					vector<int> path_control_mainstay_1_max;
					vector<int> path_control_mainstay_2_max;
					for (int n = 0; n < car[i]->saved_path.size(); n++)    //����ȫ����·����
					{
						for (int l = 0; l < n; l++)    //����ȫ����·����
						{
							vector<int>   path_control_node;   //һ����ʱ���飬��·����ȫ�����ܿ��Ƶĵ�����������
							vector<int> path_control_mainstay_1;
							vector<int> path_control_mainstay_2;
							for (int m = 0; m < car[i]->saved_path[n].size(); m++)
							{
								path_control_node.insert(path_control_node.end(), car[i]->node_control[car[i]->saved_path[n][m]].begin(), car[i]->node_control[car[i]->saved_path[n][m]].end());   //��·�����ܿ��Ƶĵ�ȫ��ѹ��ȥ
								path_control_mainstay_1.insert(path_control_mainstay_1.end(), car[i]->node_control[car[i]->saved_path[n][m]].begin(), car[i]->node_control[car[i]->saved_path[n][m]].end());  //һ��չ�۵ĸ�ֵ��1
							}
							for (int o = 0; o < car[i]->saved_path[l].size(); o++)
							{
								path_control_node.insert(path_control_node.end(), car[i]->node_control[car[i]->saved_path[l][o]].begin(), car[i]->node_control[car[i]->saved_path[l][o]].end());   //��·�����ܿ��Ƶĵ�ȫ��ѹ��ȥ								
								path_control_mainstay_2.insert(path_control_mainstay_2.end(), car[i]->node_control[car[i]->saved_path[l][o]].begin(), car[i]->node_control[car[i]->saved_path[l][o]].end());  //����չ�۵ĸ�ֵ��2
							}
							path_control_node.insert(path_control_node.end(), car[i]->reachable.begin(), car[i]->reachable.end());   //��ͷ�Լ����ܿ��Ƶ��ھ�Ҳ�������
							sort(path_control_node.begin(), path_control_node.end());
							path_control_node.erase(unique(path_control_node.begin(), path_control_node.end()), path_control_node.end());  //��������ȥ��
							sort(path_control_mainstay_1.begin(), path_control_mainstay_1.end());
							path_control_mainstay_1.erase(unique(path_control_mainstay_1.begin(), path_control_mainstay_1.end()), path_control_mainstay_1.end());  //��������ȥ��	
							sort(path_control_mainstay_2.begin(), path_control_mainstay_2.end());
							path_control_mainstay_2.erase(unique(path_control_mainstay_2.begin(), path_control_mainstay_2.end()), path_control_mainstay_2.end());  //��������ȥ��	
							if (path_control_node.size()>max_control) //·�����ƵĽڵ����ȼ�¼��Ҫ�󣬱Ƚϳ�һ������������ǿ��·��
							{
								max_control = path_control_node.size();
								record_1 = n;   //��¼�����·���Ķ�Ӧ���
								record_2 = l;
								path_control_max.clear();
								path_control_max = path_control_node;
								path_control_mainstay_1_max.clear();
								path_control_mainstay_2_max.clear();
								path_control_mainstay_1_max = path_control_mainstay_1;
								path_control_mainstay_2_max = path_control_mainstay_2;
							}
							/*
							cout << "·��Ϊ��";
							for (int p = 0; p < car[i]->saved_path[n].size(); p++)
							{
								cout << car[i]->saved_path[n][p] << " ";
							}
							for (int p = 0; p < car[i]->saved_path[l].size(); p++)
							{
								cout << car[i]->saved_path[l][p] << " ";
							}
							cout << endl;
							cout << "·�����ƵĽڵ�Ϊ��";
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
					//cout <<i<< "Ϊ��ͷ���ؽ�������һ��չ��";
					for (int u = 0; u < car[i]->saved_path[record_1].size(); u++)
					{	
						if (car[i]->saved_path[record_1][u]!=i)
						{		
							car[i]->mainstay_1.push_back(car[i]->saved_path[record_1][u]);
						//	cout << car[i]->saved_path[record_1][u] <<" ";
						}	
					}
				//	cout << "�ڶ���չ��";
					for (int u = 0; u < car[i]->saved_path[record_2].size(); u++)
					{
						if (car[i]->saved_path[record_2][u] != i)
						{
							car[i]->mainstay_2.push_back(car[i]->saved_path[record_2][u]);
					//		cout << car[i]->saved_path[record_2][u] << " ";
						}
					}
				//	cout << endl;
				//	cout << "��һ��չ�ۣ����ƵĽڵ�";
					for (int u = 0; u < car[i]->mainstay_1_control.size(); u++)
					{
				//		cout << car[i]->mainstay_1_control[u] << " ";
					}
				//	cout << "�ڶ���չ�ۣ����ƵĽڵ�";
					for (int u = 0; u < car[i]->mainstay_2_control.size(); u++)
					{
				//		cout << car[i]->mainstay_2_control[u] << " ";
					}
				//	cout << endl;
				//	cout << "�����أ����ƵĽڵ�";
					for (int u = 0; u < car[i]->cluster_control.size(); u++)
					{
				//		cout << car[i]->cluster_control[u] << " ";
					}
				//	cout << endl;
				}

				if (car[i]->saved_path.size() ==1)    //ֻ�յ�һ��ͬ�����Ҳ��ֻ��һ��·��
				{
					vector<int>  path_control_node;   //һ����ʱ���飬��·����ȫ�����ܿ��Ƶĵ�����������
					vector<int> path_control_mainstay_1;
					for (int k = 0; k < car[i]->saved_path[0].size(); k++)  //Ψһ��һ�����ذ�
					{
						path_control_node.insert(path_control_node.end(), car[i]->node_control[car[i]->saved_path[0][k]].begin(), car[i]->node_control[car[i]->saved_path[0][k]].end());   //��·�����ܿ��Ƶĵ�ȫ��ѹ��ȥ
						car[i]->mainstay_1.push_back(car[i]->saved_path[0][k]);
					}
					sort(path_control_node.begin(), path_control_node.end());
					path_control_node.erase(unique(path_control_node.begin(), path_control_node.end()), path_control_node.end());  //��������ȥ��
					path_control_mainstay_1 = path_control_node;
					path_control_node.insert(path_control_node.end(), car[i]->reachable.begin(), car[i]->reachable.end());   //��ͷ�Լ����ܿ��Ƶ��ھ�Ҳ�������
					car[i]->cluster_control = path_control_node;
					car[i]->mainstay_1_control = path_control_mainstay_1;
				//	cout << "�ؽ�����ֻ��һ��չ��";
					for (list<int>::iterator q = car[i]->mainstay_1.begin(); q != car[i]->mainstay_1.end(); q++)  //Ψһ��һ�����ذ�
					{
				//		cout << *q ;
					}
				//	cout << endl;
				}
				if (car[i]->saved_path.size() == 0)   //û�յ����ݰ� �����ɴ�
				{
					car[i]->cluster_control = car[i]->reachable;   //�Լ������Լ����ھ�
					car[i]->cbc_lonely_validity = tail_validity;
				}
			}
		}
	}
}

void create_routing_table(node** car, int N,int time)
{
	for (int i = 0; i < N; i++)  //����ȫ���ڵ�
	{
		if (car[i]->cbc_ishead == 1 && car[i]->cbc_routing_table.size() == 0)  //����Ǵ�ͷ�ڵ�,����·�ɱ��ǿյģ�����һ��·�ɱ�
		{
			//cout << i << "��ʼ����·�ɱ�" << endl;
			for (int j = 0; j < N; j++)  //����ȫ���ڵ㣬�����Ŀ�ĵ�
			{
				if (i != j)
				{
					cbc_routing_table_element new_routing_table_element; //�µ�·�ɱ�Ԫ��
					int is_delivered = 0;
					if (car[i]->is_reachable[j] == 1)  //����ͷ����ֱ�ӹ���Ŀ�ĵ�
					{
						//cout << i << "��" << j << "����ֱ���ʹ�" << endl;
						new_routing_table_element.cluster_head = i; //��ͷ���Ǳ���
						new_routing_table_element.in_this_cluster = true;
						new_routing_table_element.gateway_mainstay_number = 0;
						new_routing_table_element.gateway = -1;
						new_routing_table_element.length = 0;
						new_routing_table_element.time_stamp = time;
						is_delivered = 1;
					}
					if (is_delivered == 0)  //��û�ͳ�ȥ,��һ�Ե�һ��չ��
					{
						for (int f = 0; f < car[i]->mainstay_1_control.size(); f++)
						{
							if (car[i]->mainstay_1_control[f] == j)  //��һ��չ�������ҵ���Ŀ��
							{
								new_routing_table_element.cluster_head = i; //��ͷ���Ǳ���
								new_routing_table_element.in_this_cluster = true;  //�ڱ�����
								new_routing_table_element.gateway_mainstay_number = 1;  //��һ��չ�ۿ����ʹ�
								new_routing_table_element.gateway = -1;
								new_routing_table_element.length = 0;
								new_routing_table_element.time_stamp = time;
								is_delivered = 1;
								//cout << i << "��" << j << "����ͨ����һ��չ���ʹ�" << endl;
								break;
							}
						}
					}
					if (is_delivered == 0)  //��û�ͳ�ȥ����һ�Եڶ���չ��
					{
						for (int g = 0; g < car[i]->mainstay_2_control.size(); g++)
						{
							if (car[i]->mainstay_2_control[g] == j)
							{
								new_routing_table_element.cluster_head = i; //��ͷ���Ǳ���
								new_routing_table_element.in_this_cluster = true;  //�ڱ�����
								new_routing_table_element.gateway_mainstay_number = 2;//�ڶ���չ�ۿ����ʹ�
								new_routing_table_element.gateway = -1;
								new_routing_table_element.length = 0;
								new_routing_table_element.time_stamp = time;
								is_delivered = 1;
								//cout << i << "��" << j << "����ͨ���ڶ���չ���ʹ�" << endl;
								break;
							}
						}
					}
					if (is_delivered == 0)   //����չ�۶�û����Ϣ
					{
						new_routing_table_element.cluster_head = -1;  //��֪��Ŀ�ĵ����ĸ��������
						new_routing_table_element.in_this_cluster = false;  //���ڱ�����
						new_routing_table_element.gateway_mainstay_number = -1;
						new_routing_table_element.gateway = -1;
						new_routing_table_element.length = M_A_X;      //�������
						new_routing_table_element.time_stamp = time;
						//cout << i << "��" << j << "�ڱ�����û�п��õ�·����Ϣ" << endl;
					}
					car[i]->cbc_routing_table.push_back(new_routing_table_element);  //����ʲô������ӻ���Ҫ������
				}
				else   //�Լ����Լ���Ҳ���ɸ�·�ɱ�
				{
					cbc_routing_table_element empty_routing_table_element; //�µ�·�ɱ�Ԫ��
					empty_routing_table_element.cluster_head = j;
					empty_routing_table_element.in_this_cluster = 1;
					empty_routing_table_element.gateway_mainstay_number = 0;
					empty_routing_table_element.length = 0;
					empty_routing_table_element.time_stamp = time;
					car[i]->cbc_routing_table.push_back(empty_routing_table_element);  //����ʲô������ӻ���Ҫ������
				}
			}
		}
	}
}

void arrange_routing_table(node** car, int N, int time)
{
	for (int i = 0; i < N; i++)  //����ȫ���ڵ�
	{
		if (car[i]->cbc_ishead == 1 && car[i]->cbc_routing_table.size() != 0)  //����Ǵ�ͷ�ڵ�,����·�ɱ��ǿյģ�����·�ɱ�
		{
			//cout << i << "��ʼ����·�ɱ�" << endl;
			for (int j = 0; j < N; j++)  //����ȫ���ڵ㣬�����Ŀ�ĵ�
			{
				if (i != j)
				{
					int is_delivered = 0;
					if (car[i]->is_reachable[j] == 1)  //����ͷ����ֱ�ӹ���Ŀ�ĵ�
					{
					//	cout << i << "��" << j << "����ֱ���ʹ�" << endl;
						car[i]->cbc_routing_table[j].cluster_head = i; //��ͷ���Ǳ���
						car[i]->cbc_routing_table[j].in_this_cluster = true;
						car[i]->cbc_routing_table[j].gateway_mainstay_number = 0;
						car[i]->cbc_routing_table[j].gateway = -1;
						car[i]->cbc_routing_table[j].length = 0;
						car[i]->cbc_routing_table[j].time_stamp = time;
						is_delivered = 1;
					}
					if (is_delivered == 0)  //��û�ͳ�ȥ,��һ�Ե�һ��չ��
					{
						for (int f = 0; f < car[i]->mainstay_1_control.size(); f++)
						{
							if (car[i]->mainstay_1_control[f] == j)  //��һ��չ�������ҵ���Ŀ��
							{
								car[i]->cbc_routing_table[j].cluster_head = i; //��ͷ���Ǳ���
								car[i]->cbc_routing_table[j].in_this_cluster = true;  //�ڱ�����
								car[i]->cbc_routing_table[j].gateway_mainstay_number = 1;  //��һ��չ�ۿ����ʹ�
								car[i]->cbc_routing_table[j].gateway = -1;
								car[i]->cbc_routing_table[j].length = 0;
								car[i]->cbc_routing_table[j].time_stamp = time;
								is_delivered = 1;
						//		cout << i << "��" << j << "����ͨ����һ��չ���ʹ�" << endl;
								break;
							}
						}
					}
					if (is_delivered == 0)  //��û�ͳ�ȥ����һ�Եڶ���չ��
					{
						for (int g = 0; g < car[i]->mainstay_2_control.size(); g++)
						{
							if (car[i]->mainstay_2_control[g] == j)
							{
								car[i]->cbc_routing_table[j].cluster_head = i; //��ͷ���Ǳ���
								car[i]->cbc_routing_table[j].in_this_cluster = true;  //�ڱ�����
								car[i]->cbc_routing_table[j].gateway_mainstay_number = 2;//�ڶ���չ�ۿ����ʹ�
								car[i]->cbc_routing_table[j].gateway = -1;
								car[i]->cbc_routing_table[j].length = 0;
								car[i]->cbc_routing_table[j].time_stamp = time;
								is_delivered = 1;
						//		cout << i << "��" << j << "����ͨ���ڶ���չ���ʹ�" << endl;
								break;
							}
						}
					}
					if (is_delivered == 0)   //����չ�۶�û����Ϣ
					{
						if (car[i]->cbc_routing_table[j].cluster_head!=-1)
						{
					//		cout << i << "��" << j << "����ͨ��" << car[i]->cbc_routing_table[j].cluster_head << "��ͷ,ͨ��" << car[i]->cbc_routing_table[j].gateway_mainstay_number << "��չ�۵�" << car[i]->cbc_routing_table[j].gateway << "����,����Ϊ" << car[i]->cbc_routing_table[j].length << "ʱ���Ϊ" << car[i]->cbc_routing_table[j].time_stamp << endl;
						}
						else
						{
					//		cout << i << "��" << j << "û�п��õ�·��" << endl;
						}
						
					}
				}
				else   //�Լ����Լ���Ҳ���ɸ�·�ɱ�
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

void inform_cluster(node** car, int N, int cluster_radius, int inform_interval, int time, long int &control_packet)   //֪ͨ�Ǹ���,�����Լ�����Ϣ��ʱ���
{
	for (int i = 0; i < N; i++)  //����ȫ���ڵ�
	{
		if (car[i]->cbc_ishead==1)  //����Ǵ�ͷ
		{
			if (car[i]->inform_interval == 0)   //��������
			{
				cbc_inform_packet new_inform_packet;   //�µ�֪ͨ���ݰ�
				new_inform_packet.scource = i;
				new_inform_packet.cbc_routing_table_packeted = car[i]->cbc_routing_table;   //��·�ɱ����Ž���Ƭ
				new_inform_packet.effective = 1; //֪ͨ����Ч��
				new_inform_packet.near = i;  //�������Լ�
				new_inform_packet.time_stamp = time;  //���ݰ�Ҳ����ʱ���
				car[i]->only_inform_packet = new_inform_packet;//�Լ��������ƬҲ����
				if (car[i]->mainstay_1.empty() == false) //��һ��չ�۲��ǿյ�
				{
					new_inform_packet.mainstay = car[i]->mainstay_1; //��һ��չ�۵����˸��ƹ���
					new_inform_packet.mainstay.pop_front();  //ɾ�����е�һλ��Ԫ�أ�չ��ֻʣ�º������������
					if (car[i]->is_reachable[car[i]->mainstay_1.front()]) //��������ʵ����
					{
						//cout << i << "��Ϊ��ͷ�����һ��չ�۵�" << car[i]->mainstay_1.front() << "�Ǹ�ת��֪ͨ��" << endl;
						car[car[i]->mainstay_1.front()]->virtual_only_inform_packet = new_inform_packet;  //�Ѵ���Ϣ��ֵ���������Ƭ
						control_packet++;
					}
				}
				if (car[i]->mainstay_2.empty() == false) //�ڶ���չ�۲��ǿյ�
				{
					
					new_inform_packet.mainstay.clear();
					new_inform_packet.mainstay = car[i]->mainstay_2;  //�ڶ���չ�۵�������Ϣ���ƹ���
					new_inform_packet.mainstay.pop_front();
					if (car[i]->is_reachable[car[i]->mainstay_2.front()])  //��������ʵ����
					{
						//cout << i << "��Ϊ��ͷ����ڶ���չ�۵�" << car[i]->mainstay_2.front() << "�Ǹ�ת��֪ͨ��" << endl;
						car[car[i]->mainstay_2.front()]->virtual_only_inform_packet = new_inform_packet;
						control_packet++;
					}
				}
				car[i]->inform_interval = inform_interval;  //��������
			}
			else
			{
				car[i]->inform_interval--;
			}
		}
	}
}

void handle_inform_packet(node** car, int N, int cluster_radius)   //֪ͨ��Ϣ���Ƹ��Ǹ���
{
	for (int i = 0; i < N; i++)  //����ȫ���ڵ�
	{
		if (car[i]->only_inform_packet.effective == 1&&car[i]->cbc_ishead==0)  //����Լ�����Ƭ��Ч���Ҳ��Ǵ�ͷ
		{
			car[i]->cbc_isbone = 1;
			car[i]->cbc_ishead = 0;
			car[i]->cbc_incluster = 1;
			car[i]->bone_of_whom = car[i]->only_inform_packet.scource;
			car[i]->cbc_near = car[i]->only_inform_packet.near;
			cbc_inform_packet new_inform_packet = car[i]->only_inform_packet;   //�µ���Ƭ
			new_inform_packet.near = i;
			if (new_inform_packet.mainstay.size() != 0)   //չ�ۻ�û����
			{
				car[i]->cbc_far = car[i]->only_inform_packet.mainstay.front();
				if (car[i]->is_reachable[car[i]->only_inform_packet.mainstay.front()]==1)  //������������Ǵ���
				{
					
					car[car[i]->only_inform_packet.mainstay.front()]->bone_of_whom = new_inform_packet.scource;  //���Ǵ�ͷ�ĹǸ�
					//cout << i << "��Ϊ" << car[i]->bone_of_whom << "�ĹǸɽڵ�,����Զ��Ϊ,����ת���ڵ���Ƭ" << car[i]->cbc_far << endl;
					new_inform_packet.mainstay.pop_front();
					car[car[i]->only_inform_packet.mainstay.front()]->virtual_only_inform_packet = new_inform_packet;
				}
			}
			else
			{
				car[i]->cbc_far = -1;  //��չ������û��Զ�˽ڵ�
			}
		}
	}
}

void receive_exchange_inform_packet(node** car, int N, int cluster_radius, int time, int refresh_interval)   //�յ������ڵ����Ƭ�����Ը����Լ���·�ɱ� ʱ������̫��ͷ���
{
	for (int i = 0; i < N; i++)  //����ȫ���ڵ�
	{
		if (car[i]->cbc_ishead == 1)    //����Ǵ�ͷ�����ǵĻ���������ƪ��
		{
			for (int n = 0; n < N; n++)   //����·�ɱ��鿴ʱ����Ƿ���Ч
			{
				if (time - car[i]->cbc_routing_table[n].time_stamp > refresh_interval)     //ʱ���������
				{
					car[i]->cbc_routing_table[n].cluster_head = -1;
					car[i]->cbc_routing_table[n].gateway = -1;
					car[i]->cbc_routing_table[n].length = M_A_X;  //������Ϊ���
					car[i]->cbc_routing_table[n].time_stamp = time;
					car[i]->cbc_routing_table[n].in_this_cluster = false;
					car[i]->cbc_routing_table[n].gateway_mainstay_number = -1;
				}
			}
		}
		for (list<cbc_inform_packet>::iterator j = car[i]->exchange_packet_information_queue.begin(); j != car[i]->exchange_packet_information_queue.end();)  //����i++ д������,��Ҫ����һЩɾ������
		{
			if (car[i]->cbc_ishead == 1) //i�ڵ����Ǵ�ͷ
			{
				for (int m = 0; m < N; m++)   //����·�ɱ�
				{
					if (car[i]->cbc_routing_table[m].in_this_cluster == 1)    //m�ڵ�ͨ�����ؿ���ֱ���ʹ�,���ܵ��������ݰ�˵ʲô�����޸�·�ɱ�
					{
					}
					else
					{
						if (j->cbc_routing_table_packeted[m].cluster_head != -1 && (time - j->cbc_routing_table_packeted[m].time_stamp < refresh_interval))     //������·�ɱ����й���m��Ϣ,����ʱ�������Ч��
						{
							if (car[i]->cbc_routing_table[m].cluster_head == -1)  //�ұ���û��������¼����������������¼
							{
								if (car[i]->cbc_routing_table[j->cbc_routing_table_packeted[m].gateway].gateway_mainstay_number != -1)   //�鵽���ص�·������Ч��
								{
									car[i]->cbc_routing_table[m].cluster_head = j->cbc_routing_table_packeted[m].cluster_head;
									car[i]->cbc_routing_table[m].gateway = j->cbc_routing_table_packeted[m].gateway;
									car[i]->cbc_routing_table[m].length = j->cbc_routing_table_packeted[m].length;
									car[i]->cbc_routing_table[m].in_this_cluster = 0;
									car[i]->cbc_routing_table[m].time_stamp = j->cbc_routing_table_packeted[m].time_stamp;
									car[i]->cbc_routing_table[m].gateway_mainstay_number = car[i]->cbc_routing_table[j->cbc_routing_table_packeted[m].gateway].gateway_mainstay_number;
								}
							}
							else  //�ұ�����������¼
							{
								if (j->cbc_routing_table_packeted[m].length < car[i]->cbc_routing_table[m].length)  //������������·�ɱ��� ���ȱ��������¼��Ҫ��
								{
									if (car[i]->cbc_routing_table[j->cbc_routing_table_packeted[m].gateway].gateway_mainstay_number != -1)   //�鵽���ص�·������Ч��
									{
										car[i]->cbc_routing_table[m].cluster_head = j->cbc_routing_table_packeted[m].cluster_head;
										car[i]->cbc_routing_table[m].gateway = j->cbc_routing_table_packeted[m].gateway;
										car[i]->cbc_routing_table[m].length = j->cbc_routing_table_packeted[m].length;
										car[i]->cbc_routing_table[m].in_this_cluster = 0;
										car[i]->cbc_routing_table[m].time_stamp = j->cbc_routing_table_packeted[m].time_stamp;
										car[i]->cbc_routing_table[m].gateway_mainstay_number = car[i]->cbc_routing_table[j->cbc_routing_table_packeted[m].gateway].gateway_mainstay_number;
									}
								}
								if (j->cbc_routing_table_packeted[m].length == car[i]->cbc_routing_table[m].length&&car[i]->cbc_routing_table[m].time_stamp < j->cbc_routing_table_packeted[m].time_stamp)  //���ȼ�һ���Լ���һ���� �����·����İ�������Ϣ ʱ������Լ�����
								{
									if (car[i]->cbc_routing_table[j->cbc_routing_table_packeted[m].gateway].gateway_mainstay_number != -1)   //��ѯ�� �����ص�·������Ч��
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
						}//j���й�����һ���ص�·�ɵ���Ϣ
					}
				}//����·�ɱ��鿴ʱ����Ƿ���Ч
			}
			else   //i�ڵ㲻�Ǵ�ͷ���Ͳ����������,������ͷ�ڵ㴦��
			{
				if (car[i]->cbc_isbone == 1)
				{
					cbc_inform_packet new_inform_packet = *j;
					if (car[i]->is_reachable[car[i]->cbc_near] == 1)   //�����·��ʵ����
					{
						car[car[i]->cbc_near]->virtual_exchange_packet_information_queue.push_back(new_inform_packet);  //�����֪ͨ��Ϣ����ͷ��
					}
				}
			}
			car[i]->exchange_packet_information_queue.erase(j++);
		}
	}
}

void receive_maintain_packet(node** car, int N, int cluster_radius, int time, int tail_validity)   //�յ�ά������
{
	for (int i = 0; i < N; i++)  //����ȫ���ڵ�
	{
		for (list<cbc_maintain_packet>::iterator j = car[i]->cbc_maintain_packet_queue.begin(); j != car[i]->cbc_maintain_packet_queue.end();)  //����i++ д������,��Ҫ����һЩɾ������
		{
			if (car[i]->cbc_ishead == 1) //i�ڵ����Ǵ�ͷ
			{
				if (j->scource == car[i]->mainstay_1.back())  //һ�ű�������
				{

					car[i]->cbc_tail_1_validity = tail_validity;
					sort(j->path_control_neighbor.begin(), j->path_control_neighbor.end());
					j->path_control_neighbor.erase(unique(j->path_control_neighbor.begin(), j->path_control_neighbor.end()), j->path_control_neighbor.end());  //��������ȥ��
				//	cout << "��ͷ" << i << "�յ�������һ�ű۵�ά����" << endl;
				//	cout << "һ�űۿ��ƵĽڵ�Ϊ";
					for (int k = 0; k < j->path_control_neighbor.size(); k++)
					{
						cout << j->path_control_neighbor[k] << " ";
					}
					cout << endl;
					car[i]->mainstay_1_control = j->path_control_neighbor;
				}
				if (j->scource == car[i]->mainstay_2.back())  //���ű�������
				{
					car[i]->cbc_tail_2_validity = tail_validity;
					sort(j->path_control_neighbor.begin(), j->path_control_neighbor.end());
					j->path_control_neighbor.erase(unique(j->path_control_neighbor.begin(), j->path_control_neighbor.end()), j->path_control_neighbor.end());  //��������ȥ��	
				//	cout << "��ͷ" << i << "�յ������Զ��ű۵�ά����" << endl;
				//	cout << "���űۿ��ƵĽڵ�Ϊ";
					for (int k = 0; k < j->path_control_neighbor.size(); k++)
					{
						cout << j->path_control_neighbor[k] << " ";
					}
					cout << endl;
					car[i]->mainstay_2_control = j->path_control_neighbor;
				}
				vector<int> temp_whole_path_control;
				temp_whole_path_control.insert(temp_whole_path_control.end(), car[i]->reachable.begin(), car[i]->reachable.end());
				temp_whole_path_control.insert(temp_whole_path_control.end(), car[i]->mainstay_1_control.begin(), car[i]->mainstay_1_control.end());   //����չ�۸��Լ����ھ�ȫ������
				temp_whole_path_control.insert(temp_whole_path_control.end(), car[i]->mainstay_2_control.begin(), car[i]->mainstay_2_control.end());
				sort(temp_whole_path_control.begin(), temp_whole_path_control.end());  //����
				temp_whole_path_control.erase(unique(temp_whole_path_control.begin(), temp_whole_path_control.end()), temp_whole_path_control.end());  //ȥ��
				car[i]->cluster_control = temp_whole_path_control;  //�����Լ���·����Ϣ
			}
			if (car[i]->cbc_isbone == 1) //i�ڵ������Ǵ�ͷ �Ǳ��˵ĹǸ�
			{
			//	cout << "�Ǹ�" << i << "�յ���ά����,��" <<j->scource<<"�����ͷ"<<j->destination<< endl;
				cbc_maintain_packet new_cbc_maintain_packet=*j;  //ת��
				new_cbc_maintain_packet.current = car[i]->cbc_near;
				new_cbc_maintain_packet.path_control_neighbor.insert(new_cbc_maintain_packet.path_control_neighbor.end(), car[i]->reachable.begin(), car[i]->reachable.end());  //��I�����ھӼ���·�����Ƶ�����
				car[new_cbc_maintain_packet.current]->virtual_cbc_maintain_packet_queue.push_back(new_cbc_maintain_packet);
			}
			car[i]->cbc_maintain_packet_queue.erase(j++);
		}
	}
}

void gate_way_exchange_inform_packet(node** car, int N, int cluster_radius, int time, long int &control_packet)   //���ؽڵ㽻��·����Ϣ
{
	for (int i = 0; i < N; i++)  //����ȫ���ڵ�
	{
		for (int n = 0; n < car[i]->reachable.size(); n++)   //����ȫ���ھӶ�
		{
			for (int m = 0; m < n; m++)   //����ȫ���ھӶ�
			{
				if ((car[car[i]->reachable[n]]->cbc_isbone == 1 || car[car[i]->reachable[n]]->cbc_ishead == 1) && (car[car[i]->reachable[m]]->cbc_isbone == 1 || car[car[i]->reachable[m]]->cbc_ishead == 1) && (car[car[i]->reachable[m]]->bone_of_whom != car[car[i]->reachable[n]]->bone_of_whom) && car[car[i]->reachable[m]]->only_inform_packet.effective == 1 && car[car[i]->reachable[n]]->only_inform_packet.effective==1) //�����ھ�ȫ�Ǵ�ͷ/�Ǹɽڵ� �������ھӲ���һ������,������Ƭ������Ч
				{
					cbc_inform_packet new_exchange_packet_1 = car[car[i]->reachable[n]]->only_inform_packet;  //���ھӹǸɵ���Ϣ��ɰ�
				//	cout << i << "��Ϊ���أ��ھ���ʲôţ����������һ��" << car[i]->reachable[n] << "��������ͷ" << car[car[i]->reachable[n]]->bone_of_whom << endl;
				//	cout << i << "��Ϊ���أ��ھ��������" << car[i]->reachable[m] << "��������ͷ" << car[car[i]->reachable[m]]->bone_of_whom << endl;
					for (int o = 0; o < N; o++) //��¼�򿪿�
					{
						if (new_exchange_packet_1.cbc_routing_table_packeted[o].cluster_head!=-1)   //������������¼�����м�ֵ����
						{
							new_exchange_packet_1.cbc_routing_table_packeted[o].gateway = i;  //ÿһ����¼�����ض��ĳ����Լ�		
							new_exchange_packet_1.cbc_routing_table_packeted[o].cluster_head = car[car[i]->reachable[n]]->bone_of_whom;  //���ظ�cluster_head���ʹ�ã�ֻ����һ��������ȫ��·��
							new_exchange_packet_1.cbc_routing_table_packeted[o].length = new_exchange_packet_1.cbc_routing_table_packeted[o].length + 1;//��������ת����һ�Σ�ÿ����¼�ĳ��ȶ���һ
						}
					}
					car[car[i]->reachable[m]]->virtual_exchange_packet_information_queue.push_back(new_exchange_packet_1);
					control_packet++;
					cbc_inform_packet new_exchange_packet_2 = car[car[i]->reachable[m]]->only_inform_packet;  //���ھӹǸɵ���Ϣ��ɰ�
					for (int o = 0; o < N; o++) //��¼�򿪿�
					{
						if (new_exchange_packet_2.cbc_routing_table_packeted[o].cluster_head != -1)   //������������¼�����м�ֵ����
						{
							new_exchange_packet_2.cbc_routing_table_packeted[o].gateway = i;  //ÿһ����¼�����ض��ĳ����Լ�
							new_exchange_packet_2.cbc_routing_table_packeted[o].cluster_head = car[car[i]->reachable[m]]->bone_of_whom;  //���ظ�cluster_head���ʹ�ã�ֻ����һ��������ȫ��·��
							new_exchange_packet_2.cbc_routing_table_packeted[o].length = new_exchange_packet_2.cbc_routing_table_packeted[o].length + 1;//��������ת����һ�Σ�ÿ����¼�ĳ��ȶ���һ
						}
					}
					car[car[i]->reachable[n]]->virtual_exchange_packet_information_queue.push_back(new_exchange_packet_2);   //��������Ϣ
					control_packet++;
				}
			}
		}
	}
}

void arm_end_hello(node** car, int N, int cluster_radius, int hello_interval, long int &control_packet)   //��չĩ�˵Ľڵ㷢�Ĵ�ά������˳���˴��ܿ��ƵĽڵ���Ϣhello_interval�������Ϣ��Ƶ���̶�
{
	for (int i = 0; i < N; i++)  //����ȫ���ڵ�
	{
		if (car[i]->cbc_isbone == 1 && car[i]->cbc_far == -1) //���ǽڵ�Զ�����Ǳ��˵ĹǸ�
		{
			if (car[i]->hello_interval == 0)
			{
				//cout << i << "��Ϊ" << car[i]->bone_of_whom << "�ĹǸɣ�ά��������" << car[i]->cbc_near << endl;
				cbc_maintain_packet new_cbc_maintain_packet;
				new_cbc_maintain_packet.scource = i;
				new_cbc_maintain_packet.current = car[i]->cbc_near;
				new_cbc_maintain_packet.destination = car[i]->bone_of_whom;
				new_cbc_maintain_packet.path_control_neighbor = car[i]->reachable;
				control_packet = control_packet + car[i]->reachable.size();
				if (car[i]->is_reachable[new_cbc_maintain_packet.current] == 1) //������·����
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

void cluster_validity_test(node** car, int N, int cluster_radius, int time, int cbc_business_card_validity)   //�صı���������
{
	for (int i = 0; i < N; i++)  //����ȫ���ڵ�
	{
		if (car[i]->cbc_ishead == 1 && (car[i]->cbc_tail_1_validity <= 0 || car[i]->cbc_tail_2_validity <= 0 || car[i]->cbc_lonely_validity<=0))  //����ղ����ھӵ�ά����Ϣ,�����Ǹ��¶��ڵ㣬�¶��ڵ�����ʱ����Ҫ����һ��
		{
			car[i]->only_inform_packet.effective = 0;  //��ƬʧЧ
			car[i]->cbc_ishead = 0;
			car[i]->cbc_isbone = 0;   //�Ƿ�Ϊ�Ǹ���  cbc_isbone=1 �ǹǸ���   cbc_isbone=0  ���ǹǸ���
			car[i]->bone_of_whom = -1;         //����Ϊ�Ǹ����Ĵ��ױ��
			car[i]->cbc_near = -1; //��Ϊ�Ǹ��������ھ���˭ 
			car[i]->cbc_far = -1;  //��Ϊ�Ǹ���Զ���ھ���˭
			car[i]->stop_announce=0; //��ͣ�ڽ�������Ҫ��ʼ��ͷ��Ϣ��
			car[i]->wait_to_create_head = M_A_X;
			car[i]->saved_path.clear(); //���ذ���·�����
			car[i]->node_control.clear(); //������Щ�ڵ㱻�Ǹ��ڵ�֧��   node_control[6]={2,9,11} ��ʾ6�Žڵ�֧����2��9��11�����ڵ�
			car[i]->mainstay_1.clear(); //�صĹǸ���
			car[i]->mainstay_2.clear(); //�Ǹ���
			car[i]->cluster_control.clear(); //���ؿ��ƶ��ٸ��ڵ�,�����˴�ͷ���Ǹ���
			car[i]->mainstay_1_control.clear();  //һ��չ���ܿ��ƵĽڵ�
			car[i]->mainstay_2_control.clear(); //����չ�ۿ��ƵĽڵ�
			car[i]->cbc_routing_table.clear();  //���Ǵ�ͷ�˰�·�ɱ����
			car[i]->cbc_maintain_packet_queue.clear();
			car[i]->virtual_cbc_maintain_packet_queue.clear();
			car[i]->my_max_announce_received.max_announce_node_degree = -1;
			car[i]->my_max_announce_received.max_announce_node_ID = -1;
			car[i]->my_max_announce_received.wait_time_window = -1;
			car[i]->my_max_announce_received.is_hearing = 1;
		}
		else
		{
			if (car[i]->mainstay_1.size()!=0)  //���һ�ű�չ����
			{
				car[i]->cbc_tail_1_validity--;
			}
			if (car[i]->mainstay_2.size()!= 0)  //�������չ�۴���
			{
				car[i]->cbc_tail_2_validity--;
			}
			if (car[i]->mainstay_1.size() == 0 && car[i]->mainstay_2.size() == 0) //����չ�۶������ڣ����ƹ¶��صĴ��ʱ��
			{
				car[i]->cbc_lonely_validity--;
			}
		}
		if (car[i]->cbc_isbone == 1 && (time - car[i]->only_inform_packet.time_stamp)>=cbc_business_card_validity)    //����ǹǸɣ����ǳ����ղ����ص���Ƭ����
		{
			car[i]->only_inform_packet.effective = 0;  //��ƬʧЧ
			car[i]->cbc_ishead = 0;
			car[i]->cbc_isbone = 0;   //�Ƿ�Ϊ�Ǹ���  cbc_isbone=1 �ǹǸ���   cbc_isbone=0  ���ǹǸ���
			car[i]->bone_of_whom = -1;         //����Ϊ�Ǹ����Ĵ��ױ��
			car[i]->cbc_near = -1; //��Ϊ�Ǹ��������ھ���˭ 
			car[i]->cbc_far = -1;  //��Ϊ�Ǹ���Զ���ھ���˭
			car[i]->saved_path.clear(); //���ذ���·�����
			car[i]->node_control.clear(); //������Щ�ڵ㱻�Ǹ��ڵ�֧��   node_control[6]={2,9,11} ��ʾ6�Žڵ�֧����2��9��11�����ڵ�
			car[i]->mainstay_1.clear(); //�صĹǸ���
			car[i]->mainstay_2.clear(); //�Ǹ���
			car[i]->cluster_control.clear(); //���ؿ��ƶ��ٸ��ڵ�,�����˴�ͷ���Ǹ���
			car[i]->mainstay_1_control.clear();  //һ��չ���ܿ��ƵĽڵ�
			car[i]->mainstay_2_control.clear(); //����չ�ۿ��ƵĽڵ�
			car[i]->cbc_routing_table.clear();  //·�ɱ�
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
	for (int i = 0; i < N; i++)   //���ݰ�
	{
		for (list<cbc_packet>::iterator j = car[i]->cbc_queue.begin(); j != car[i]->cbc_queue.end();)   //��Ҫ��һЩɾ������ ++д�ں���
		{
			if (j->T > packet_exiry)
			{
				//cout << "���ݰ�����ʧЧ" << endl;
				cbc_packet_drop++;//������		
			}
			else
			{
				if (j->current == j->destination)  //�Ѿ��ʹ�
				{
					//cout <<"��" << j->current << "λ�ã����ݰ��ɹ��ʹ�" << "���ݰ�IDΪ" << j->ID << endl;
					cbc_packet_life_time = cbc_packet_life_time + j->T;
					cbc_packet_arrive++;//������
				}
				else
				{
					//cout << "���ݰ�IDΪ"<<j->ID<<"��ǰλ��Ϊ" << j->current << "Ŀ��λ��" << j->destination << "�Ƿ��ڱ���" << j->already_in_this_cluster << endl;
					//cout << "������" << j->go_for_gateway << "��ǰλ���ǲ��ǹǸ�" << car[j->current]->cbc_isbone << "�Ǹɽ��˽ڵ�Ϊ" << car[j->current]->cbc_near << endl;
					if (j->go_for_gateway == 1 && j->next_gateway == i)   //�������ؽڵ��ȥ�����ҵ�ǰ�ڵ�������ؽڵ�
					{
						int gate_way_useful_flag = 0;
						cbc_packet new_cbc_packet_1 = *j;
						for (int n = 0; n < car[i]->reachable.size(); n++)
						{
							if (car[car[i]->reachable[n]]->bone_of_whom == new_cbc_packet_1.next_head)  //����Ǹ�ǡ�������ͷ��
							{
								new_cbc_packet_1.current = car[i]->reachable[n];
								new_cbc_packet_1.next_gateway = -1;
								new_cbc_packet_1.next_head = -1;
								new_cbc_packet_1.go_for_gateway = 0;
								new_cbc_packet_1.already_in_this_cluster = 0;
								new_cbc_packet_1.T = new_cbc_packet_1.T + 1;
								if (car[i]->is_reachable[new_cbc_packet_1.current])  //���������·��ʵ����
								{
									car[new_cbc_packet_1.current]->virtual_cbc_queue.push_back(new_cbc_packet_1);    //������Ϊ���ص����ã���������һ���صĹǸɽڵ�
								}
								gate_way_useful_flag = 1;
								break;
							}
						}
						if (gate_way_useful_flag == 0)
						{
							cbc_packet_drop++;   //�����������أ������ھ������Ҳ���Ŀ�Ĵأ�ֱ�Ӷ�ȥ���ݰ�
						}
					}
					if (j->go_for_gateway == 1 && j->next_gateway != i) //�������ؽڵ��ȥ�����ǲ��ҵ�ǰ�ڵ㲻�����ؽڵ�
					{
						cbc_packet new_cbc_packet_2 = *j;
						new_cbc_packet_2.T = new_cbc_packet_2.T + 1;
						if (car[i]->is_reachable[new_cbc_packet_2.next_gateway] == 1)  //�ھ�������Ŀ������
						{
							new_cbc_packet_2.current = new_cbc_packet_2.next_gateway;
							car[new_cbc_packet_2.current]->virtual_cbc_queue.push_back(new_cbc_packet_2);
						}
						else   //�ھ�����û��Ŀ�����أ������ܲ�����Զ����
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
								cbc_packet_drop++;  //�ߵ�չ�۵�ĩ��Ҳ���Ҳ���Ŀ�ĵأ����ݰ�����
							}
						}
					}
					if (j->already_in_this_cluster == 1)   //��ʾĿ�Ľڵ���ڱ����У���Ŀ�Ľڵ�ת����
					{
						cbc_packet new_cbc_packet_3 = *j;
						new_cbc_packet_3.T = new_cbc_packet_3.T + 1;
						if (car[i]->is_reachable[new_cbc_packet_3.destination] == 1)  //�ھ�������Ŀ�ĵ�
						{
							new_cbc_packet_3.current = new_cbc_packet_3.destination;
							car[new_cbc_packet_3.current]->virtual_cbc_queue.push_back(new_cbc_packet_3);
						}
						else   //�ھ�����û��Ŀ�ĵأ������ܲ�����Զ����
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
								cbc_packet_drop++;  //�ߵ�չ�۵�ĩ��Ҳ���Ҳ���Ŀ�ĵأ����ݰ�����
							}
						}
					}
					if (j->already_in_this_cluster == 0 && j->go_for_gateway == 0)  //����Ŀ��
					{
						if (car[i]->cbc_ishead == 1)  //����Ǵ�ͷ�ڵ㣬�Ǻ�˵�ˣ������Ŀ��
						{
							int is_delivered = 0;
							cbc_packet new_cbc_packet_4 = *j;
							new_cbc_packet_4.T = new_cbc_packet_4.T + 1;
							
							if (car[i]->cbc_routing_table[new_cbc_packet_4.destination].in_this_cluster == true)  //Ŀ�ĵ�ַ���ڱ���֮��
							{
								//cout << "���ݰ�IDΪ"<<new_cbc_packet_4.ID<<"��ǰλ��Ϊ" << i << "�Ǵ�ͷ��Ŀ�ĵ�" << new_cbc_packet_4.destination << "Ŀ�ĵؾ��ڱ�����" << "����ͨ��" << car[i]->cbc_routing_table[new_cbc_packet_4.destination].gateway_mainstay_number <<"��ϵ��"<< endl;
								new_cbc_packet_4.already_in_this_cluster = 1;
								new_cbc_packet_4.go_for_gateway = 0;
								switch (car[i]->cbc_routing_table[new_cbc_packet_4.destination].gateway_mainstay_number)  //�Ǹ������ϵ��Ŀ�ĵ�ַ
								{
								case 0:   //���������ֱ����ϵ��Ŀ�ĵ�ַ
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
								//cout << "������ϵ����ݰ���ǰλ��Ϊ" << new_cbc_packet_4.current << endl;
							}
							else   //Ŀ�ĵز��ڱ�����
							{
								if (car[i]->cbc_routing_table[new_cbc_packet_4.destination].cluster_head == -1)  //�Ҳ��������ĸ���ͷ
								{		
									car[new_cbc_packet_4.current]->virtual_cbc_queue.push_back(new_cbc_packet_4);  //�ݴ��ڱ��صĶ�����
								}
								else
								{
									new_cbc_packet_4.T = new_cbc_packet_4.T + 1;
									new_cbc_packet_4.next_head = car[i]->cbc_routing_table[new_cbc_packet_4.destination].cluster_head;
									new_cbc_packet_4.next_gateway = car[i]->cbc_routing_table[new_cbc_packet_4.destination].gateway;
									new_cbc_packet_4.already_in_this_cluster = 0;
									new_cbc_packet_4.go_for_gateway = 1;
									switch (car[i]->cbc_routing_table[new_cbc_packet_4.destination].gateway_mainstay_number)  //�Ǹ���ϵ����
									{
									case 0:   //���������ֱ����ϵ������
										new_cbc_packet_4.current = car[i]->cbc_routing_table[new_cbc_packet_4.destination].gateway;
										break;
									case 1:   //һ�ű�չ��������
										new_cbc_packet_4.current = car[i]->mainstay_1.front();
										break;
									case 2:  //���ű�չ���Թ�������
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
						if (car[i]->cbc_isbone == 1)  //����ڹǸ����У���˵��ֱ�ӽ�����ͷ
						{
							
							cbc_packet new_cbc_packet_5 = *j;
							new_cbc_packet_5.T = new_cbc_packet_5.T + 1;
							if (car[i]->cbc_near != -1)
							{
								new_cbc_packet_5.current = car[i]->cbc_near;  //����ͷ��								
								if (car[i]->is_reachable[new_cbc_packet_5.current] == 1)
								{
									car[new_cbc_packet_5.current]->virtual_cbc_queue.push_back(new_cbc_packet_5);
								}
								else
								{
									cbc_packet_drop++;
								}
							}
							else  //�Ҳ�����ͷ�ˣ�����֮
							{
								cbc_packet_drop++;
							}
						}
						if (car[i]->cbc_isbone == 0 && car[i]->cbc_ishead == 0)   //�Ȳ��ǹǸ�Ҳ���Ǵ�ͷ
						{
							cbc_packet new_cbc_packet_6 = *j;
							new_cbc_packet_6.T = new_cbc_packet_6.T + 1;
							int konw_how_to_do = 0;
							for (int n = 0; n < car[i]->reachable.size(); n++)
							{
								if (car[car[i]->reachable[n]]->cbc_isbone == 1 || car[car[i]->reachable[n]]->cbc_ishead == 1)  //ֻҪ��ѯ��һ���ھ��Ǵ�ͷ���߹Ǹɽڵ�
								{
									new_cbc_packet_6.current = car[i]->reachable[n];
									car[new_cbc_packet_6.current]->virtual_cbc_queue.push_back(new_cbc_packet_6);
									konw_how_to_do = 1;
									break;
								}
							}
							if (konw_how_to_do == 0)  //��Χ�����ض�û��
							{
								car[new_cbc_packet_6.current]->virtual_cbc_queue.push_back(new_cbc_packet_6);  //�������Ļ�����ȥ
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
		car[new_packet.current]->cbc_queue.push_back(new_packet);  //�������ݰ�
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
			if (car[i]->reachable[j]<i&&car[car[i]->reachable[j]]->in_lca_cluster == 0)   //�����һ���ھӱ�ű��Լ�С,��������ھ�û�ڴ���
			{
				lowest_flag = 0;   //����������С��
			}
		}
		if (car[i]->in_lca_cluster==1)
		{
			lowest_flag = 0;   //�Լ��ڴ�����Ͳ�������
		}
		if (lowest_flag==1)
		{
			if (car[i]->lca_ishead==0)  //ԭ�����Ǵ�ͷ
			{
				car[i]->lca_ishead = 1;  //�����Ǵ�ͷ
				for (int n = 0; n < N; n++)    //��ʼ����·�ɱ�
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
			if (car[i]->lca_ishead == 1)  //ԭ�����Ǵ�ͷ
			{
				for (int n = 0; n < N; n++)    //���ص�·����Ϣ����һ��
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
		if (lowest_flag==0)  //�б���IDС���ھ���
		{
			car[i]->lca_ishead = 0;
			car[i]->lca_routing_table.clear();  //·�ɱ����
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
			if (car[car[i]->reachable[j]]->lca_ishead==1)  //����и��ھ�Ϊ��ͷ
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
				if (car[car[i]->reachable[n]]->lca_ishead == 1 && car[car[i]->reachable[m]]->lca_ishead==1)  //���ھӶ��Ǵ�ͷ�ڵ�
				{
					//cout << "�ڵ�" << i << "��Ϊ���ؽ���" << car[i]->reachable[n] << "��" << car[i]->reachable[m] << "����Ϣ" << endl;
					lca_routing_exchange_packet new_lca_routing_exchange_packet_1;
					new_lca_routing_exchange_packet_1.packeted_lca_routing_table = car[car[i]->reachable[n]]->lca_routing_table;
					for (int k = 0; k < N; k++)   //·�ɱ�ÿһ��� ��д
					{
						if (new_lca_routing_exchange_packet_1.packeted_lca_routing_table[k].cluster_head!=-1)   //���·����Ϣ�����м�ֵ
						{
							new_lca_routing_exchange_packet_1.packeted_lca_routing_table[k].cluster_head = car[i]->reachable[n];
							new_lca_routing_exchange_packet_1.packeted_lca_routing_table[k].gateway = i;
							new_lca_routing_exchange_packet_1.packeted_lca_routing_table[k].length = new_lca_routing_exchange_packet_1.packeted_lca_routing_table[k].length + 1;
						}
					}
					car[car[i]->reachable[m]]->virtual_lca_routing_exchange_packet_queue.push_back(new_lca_routing_exchange_packet_1);
					lca_routing_exchange_packet new_lca_routing_exchange_packet_2;
					new_lca_routing_exchange_packet_2.packeted_lca_routing_table = car[car[i]->reachable[m]]->lca_routing_table;
					for (int k = 0; k < N; k++)   //·�ɱ�ÿһ��� ��д
					{
						if (new_lca_routing_exchange_packet_2.packeted_lca_routing_table[k].cluster_head != -1)   //���·����Ϣ�����м�ֵ
						{
					    	new_lca_routing_exchange_packet_2.packeted_lca_routing_table[k].cluster_head = car[i]->reachable[m];
							new_lca_routing_exchange_packet_2.packeted_lca_routing_table[k].gateway = i;
							new_lca_routing_exchange_packet_2.packeted_lca_routing_table[k].length = new_lca_routing_exchange_packet_2.packeted_lca_routing_table[k].length + 1;
						}
					}
					car[car[i]->reachable[n]]->virtual_lca_routing_exchange_packet_queue.push_back(new_lca_routing_exchange_packet_2);
					lca_control_packet = lca_control_packet + 2;  //�������ư�
				}
			}
		}
	}
}

void arrange_lca_routing_table(node** car, int N, int time, int lca_refresh_interval)
{
	for (int i = 0; i < N; i++)
	{
		if (car[i]->lca_ishead == 1)  //�Ǵ�ͷ������Լ���·�ɱ��Ƿ����
		{
			for (int n = 0; n < N; n++)
			{
				if (time - car[i]->lca_routing_table[n].time_stamp > lca_refresh_interval)   //���·�ɱ����
				{
					car[i]->lca_routing_table[n].cluster_head = -1;
					car[i]->lca_routing_table[n].gateway = -1;
					car[i]->lca_routing_table[n].length = -1;
					car[i]->lca_routing_table[n].time_stamp = time;
				}
			}
		}
		for (list<lca_routing_exchange_packet>::iterator j = car[i]->lca_routing_exchange_packet_queue.begin(); j != car[i]->lca_routing_exchange_packet_queue.end();)  //����i++ д������,��Ҫ����һЩɾ������
		{
			if (car[i]->lca_ishead == 1) //i�ڵ����Ǵ�ͷ
			{
				for (int m = 0; m < N; m++)   //����·�ɱ�
				{
					if (car[i]->is_reachable[m] == 1||i==m)    //m�ڵ����ͨ����ͷֱ���ʹ�  ���޸�·�ɱ�
					{
					}
					else
					{
						if (j->packeted_lca_routing_table[m].cluster_head != -1 && (time - j->packeted_lca_routing_table[m].time_stamp < lca_refresh_interval))     //������·�ɱ����й���m��Ϣ,����ʱ�������Ч��
						{
							//cout << "�ڵ�" << i << "�յ�����" << m << "·����Ϣ,���ɴ�ͷ" << j->packeted_lca_routing_table[m].cluster_head << "ͨ������" << j->packeted_lca_routing_table[m].gateway << "�����ʹ�" << endl;
							if (car[i]->lca_routing_table[m].cluster_head == -1)  //�ұ���û��������¼����������������¼
							{
								car[i]->lca_routing_table[m].cluster_head = j->packeted_lca_routing_table[m].cluster_head;
								car[i]->lca_routing_table[m].gateway = j->packeted_lca_routing_table[m].gateway;
								car[i]->lca_routing_table[m].length = j->packeted_lca_routing_table[m].length;
								car[i]->lca_routing_table[m].time_stamp = j->packeted_lca_routing_table[m].time_stamp;
							}
							else  //�ұ�����������¼
							{
								if (j->packeted_lca_routing_table[m].length < car[i]->lca_routing_table[m].length)  //������������·�ɱ��� ���ȱ��������¼��Ҫ��
								{
									car[i]->lca_routing_table[m].cluster_head = j->packeted_lca_routing_table[m].cluster_head;
									car[i]->lca_routing_table[m].gateway = j->packeted_lca_routing_table[m].gateway;
									car[i]->lca_routing_table[m].length = j->packeted_lca_routing_table[m].length;
									car[i]->lca_routing_table[m].time_stamp = j->packeted_lca_routing_table[m].time_stamp;
								}
								if (j->packeted_lca_routing_table[m].length == car[i]->lca_routing_table[m].length&&car[i]->lca_routing_table[m].time_stamp < j->packeted_lca_routing_table[m].time_stamp)  //���ȼ�һ���Լ���һ���� �����·����İ�������Ϣ ʱ������Լ�����
								{
									car[i]->lca_routing_table[m].cluster_head = j->packeted_lca_routing_table[m].cluster_head;
									car[i]->lca_routing_table[m].gateway = j->packeted_lca_routing_table[m].gateway;
									car[i]->lca_routing_table[m].length = j->packeted_lca_routing_table[m].length;
									car[i]->lca_routing_table[m].time_stamp = j->packeted_lca_routing_table[m].time_stamp;
								}
							}
						}//j���й�����һ���ص�·�ɵ���Ϣ
					}
				}//����·�ɱ�
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
	realpacket.scource = realpacket.current;  //���ݰ�Դ��ַ
	return realpacket;
}

void create_lca_packet(node** car, double packet_spawn_rate, int N,int time)
{
	for (int start_packet = 0; start_packet<(int)(packet_spawn_rate*N); start_packet++)
	{
		lca_packet new_packet = create_single_lca_packet(N,time);
		car[new_packet.current]->lca_packet_queue.push_back(new_packet);  //�������ݰ�
	}
}

void lca_packet_delivery(node** car, int N, int packet_expiry, long int &lca_packet_drop, long int &lca_packet_arrive, long int &lca_packet_life_time_sum, long int &lca_control_packet)
{
	for (int i = 0; i < N; i++)
	{
		for (list<lca_packet>::iterator j = car[i]->lca_packet_queue.begin(); j != car[i]->lca_packet_queue.end();)   //��Ҫ��һЩɾ������ ++д�ں���
		{
			//cout << "�ڽڵ�" << i << "�����ݰ�,IDΪ" << j->ID << "Ŀ��" << j->destination << "�¸���ͷ" << j->next_head << "�¸�����" << j->next_gateway << endl;
			if (j->T > packet_expiry)
			{
				//cout << "���ݰ�����ʧЧ" << endl;
				lca_packet_drop++;//������		
			}
			else
			{
				if (j->current == j->destination)  //�Ѿ��ʹ�
				{
					//cout <<"��" << j->current << "λ�ã����ݰ��ɹ��ʹ�" << "���ݰ�IDΪ" << j->ID << endl;
					lca_packet_life_time_sum = lca_packet_life_time_sum + j->T;
					lca_packet_arrive++;//������
				}
				else
				{
					if (j->next_head == i)   //����Ѿ��͵���ͷ
					{
						if (car[i]->lca_ishead == 1)  //���ȷʵ�Ǵ�ͷ
						{
							if (car[i]->lca_routing_table[j->destination].cluster_head == i)   //������ؾͿ����ʹ�
							{
								lca_packet new_lca_packet = *j;
								new_lca_packet.current = new_lca_packet.destination;
								new_lca_packet.T = new_lca_packet.T + 1;
								if (car[i]->is_reachable[new_lca_packet.current] == 1)  //�����·��ʵ����
								{
									car[new_lca_packet.current]->virtual_lca_packet_queue.push_back(new_lca_packet);
								}
								else
								{
									lca_packet_drop++;
								}
							}
							else   //���ز����ʹ�
							{
								if (car[i]->lca_routing_table[j->destination].cluster_head == -1)   //���������û�е�Ŀ�ĵص���Ϣ
								{
									lca_packet new_lca_packet = *j;
									new_lca_packet.T = new_lca_packet.T + 1;
									car[new_lca_packet.current]->virtual_lca_packet_queue.push_back(new_lca_packet);     //�������Ļ���ȥ
								}
								if (car[i]->lca_routing_table[j->destination].cluster_head != -1)  //���ͨ�������ص�ת���Ϳ����ʹ�
								{
									lca_packet new_lca_packet = *j;
									new_lca_packet.T = new_lca_packet.T + 1;
									new_lca_packet.current = car[i]->lca_routing_table[new_lca_packet.destination].gateway;
									new_lca_packet.next_head = car[i]->lca_routing_table[new_lca_packet.destination].cluster_head;
									new_lca_packet.next_gateway = car[i]->lca_routing_table[new_lca_packet.destination].gateway;
									if (car[i]->is_reachable[new_lca_packet.current] == 1)  //�����·��ʵ����
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
						else  //������㽻���Լ���һ����ͷ�ھ�
						{
							lca_packet_drop++;
						}
					}
					if (j->next_gateway==i)  //�����ǰλ��������
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
						if (car[i]->lca_ishead==1)  //i���Ǵ�ͷ
						{
							lca_packet new_lca_packet = *j;
							new_lca_packet.T = new_lca_packet.T + 1;
							new_lca_packet.next_head = i;
							car[new_lca_packet.current]->virtual_lca_packet_queue.push_back(new_lca_packet);
							have_head_flag = 1;
						}
						else
						{
							for (int l = 0; l < car[i]->reachable.size(); l++)    //�����ھ�
							{
								if (car[car[i]->reachable[l]]->lca_ishead == 1)  //�и��Ǵ�ͷ
								{
									lca_packet new_lca_packet = *j;
									new_lca_packet.T = new_lca_packet.T + 1;
									new_lca_packet.current = car[i]->reachable[l];   //�������ͷ�ھ�
									new_lca_packet.next_head = car[i]->reachable[l];
									car[new_lca_packet.current]->virtual_lca_packet_queue.push_back(new_lca_packet);
									have_head_flag = 1;
									break;
								}
							}
						}
						if (have_head_flag==0)   //һ���Ǵ�ͷ���ھӶ�û��
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
	///////////////////////////////////////////////////////////////////ȫ�ֱ���
	srand((unsigned)time(NULL));
	const int N = 100;//�ڵ����
	while (true)
	{
		int time = 0;//ʱ�� ÿ��ʱ��Ϊ���������0.005��
		double packet_spawn_rate = 1; //���ݰ�������
		int x_div = 10; //������ű��ֳɼ���
		int y_div = 10; //�������ű��ֳɼ��� ÿ��·�ڼ��1000m Ĭ�ϳ���10m/s 
		int block_width = L / x_div;
		double go_straight_possibility = 1 - turn_left_possibility - turn_right_poosibility; //�¸�·��ֱ�еĸ���
		int v = 0;  //���� ����������10m/s��
		double r = 2500; //������ͨ�ž���  ���������� �ף�
		int simulation_time = 5001;//ģ��ʱ��
		int reserve_v[10] = { 1, 2, 5, 10, 15, 20, 25, 30, 40, 50 };  //ʵ��������ٶȱ� m/s
		int v_id = 0;
		int packet_expiry = 300;   //300���������ʹʧȥʧЧ�ԣ�ɾ��
		////////////////////////////////////////////////////////////////////������aodvjrЭ����Ҫ�ı���
		int aodv_RREQ_expiry = 8; //aodvjr��������������ʱ��
		int aodvjr_route_expiry = 300;//aodvjr��·����ʱ�䣬��������ʱ��δ�յ���Ч�����ݰ�����·�Զ��Ͽ�
		long int aodvjr_packet_arrive = 0; //�ʹ����ݰ�����
		long int aodvjr_packet_drop = 0;  //�������ݰ�����
		long int aodvjr_packet_life_time_sum = 0; //�˵������ӳ�
		long long int aodvjr_control_packet = 0;  //���ƿ���
		//////////////////////////////////////////////////////////////////////////////������cbcЭ���õ�һЩ����
		int cbc_packet_expiry = 300;   //300���������ʹʧȥʧЧ�ԣ�ɾ��
		int cbc_cluster_radius = 2; //cbc�Ǹ����İ뾶
		int cbc_stop_time_increase = 10;//�յ����ذ���ò����ط�������Ϣ
		int cbc_forecast_time = 200;  //��·���ϵ�Ԥ��ʱ��
		int cbc_refresh_interval = 50;  //·�ɱ����Ч�ڣ�������Ч������Ϊ·��ʧЧ
		int cbc_hello_interval = 20;   //���ͷ ���ʹ�ά����Ϣ
		int cbc_inform_interval = 10;  //��ͷ�ڵ���Ǹɹ㲥�����Լ�����Ƭ��Ϣ
		int cbc_tail_validity = 60;  //����ղ�����չĩ�˵ĹǸɽڵ㣬��ͷ��ʧЧ
		int cbc_business_card_validity = 60;  //�Ǹ�������ղ�����ͷ�ĸ��µ���Ƭ���Ǹ�����ʧЧ
		long int cbc_packet_arrive = 0; //�ʹ����ݰ�����
		long int cbc_packet_drop = 0;  //�������ݰ�����
		long int cbc_packet_life_time_sum = 0; //�˵������ӳ�
		long int cbc_control_packet = 0;  //���ƿ���
		//////////////////////////////////////////////////////////////////////////////������lcaЭ���õ�һЩ����
		int lca_packet_expiry = 300;   //300���������ʹʧȥʧЧ�ԣ�ɾ��
		long int lca_packet_arrive = 0; //�ʹ����ݰ�����
		long int lca_packet_drop = 0;  //�������ݰ�����
		long int lca_packet_life_time_sum = 0; //�˵������ӳ�
		long int lca_control_packet = 0;  //���ƿ���
		int lca_refresh_interval = 50;  //·�ɱ����Ч�ڣ�������Ч������Ϊ·��ʧЧ
		//////////////////////////////////////////////////////////////////////
		while (v_id < 10)  //�ı����ѭ������ʵ��
		{
			v = reserve_v[v_id];
			aodvjr_packet_arrive = 0; //�ʹ����ݰ�����
			aodvjr_packet_drop = 0;  //�������ݰ�����
			aodvjr_packet_life_time_sum = 0; //�˵������ӳ�
			aodvjr_control_packet = 0;  //���ƿ���
			cbc_packet_arrive = 0; //�ʹ����ݰ�����
			cbc_packet_drop = 0;  //�������ݰ�����
			cbc_packet_life_time_sum = 0; //�˵������ӳ�
			cbc_control_packet = 0;  //���ƿ���
			lca_packet_arrive = 0; //�ʹ����ݰ�����
			lca_packet_drop = 0;  //�������ݰ�����
			lca_packet_life_time_sum = 0; //�˵������ӳ�
			lca_control_packet = 0;  //���ƿ���
			time = 0;
			node **car = new node*[N];
			////////////////////////////////////////////////////////////�ڵ�����
			create_node(car, block_width, v, r, N);
			////////////////////////////////////////////////////////////
			while (true)  //��ʱ��ѭ��
			{
				time++;
				cout << time << endl;
				if (time % 100 == 1)
				{
					for (int move_node = 0; move_node < N; move_node++)
					{
						car[move_node]->move(block_width, N);
						//	cout << move_node << "�Žڵ�������" << car[move_node]->x << "," << car[move_node]->y << "�ٶ�Ϊ" << car[move_node]->v << "����Ϊ" << car[move_node]->direction << "�´�ת�䷽��Ϊ" << car[move_node]->turn.front() << endl;
					}
					aodvjr_calculate_connection(car, r, N, aodvjr_route_expiry);  //aodvjrЭ�������¼����ٽӹ�ϵ
					//cbc_calculate_connection(car, r, N);  //cbcЭ�����¼����ٽӹ�ϵ
					//use_RREP_maintain(car, N, aodvjr_route_expiry, aodvjr_control_packet);  //������ȷ�ϰ�ά����·
					//net_work_draw(car, N,time);
				}
				/*
				calculate_route_life(car, N); //������·״̬���鿴�Ƿ�����·��Ҫ�Ͽ�
				//net_work_situation(car, N); //�۲�һ����������״̬
				create_packet(car, packet_spawn_rate, N); //�������ݰ�
				handle_aodvjr_RREP_packet(car, N, aodvjr_route_expiry);   //���������ݰ�
				handle_aodvjr_RREQ_packet(car, N, aodv_RREQ_expiry, aodvjr_route_expiry, aodvjr_control_packet);   //���������
				packet_delivery(car, N, aodvjr_route_expiry, aodvjr_packet_arrive, aodvjr_packet_drop, aodvjr_packet_life_time_sum, aodvjr_control_packet, packet_expiry, aodv_RREQ_expiry);//�������ݰ�
				cover_queue(car, N);//��������и�����ʵ�ʶ��У���ɱ����������ݵĴ���
				if (time == simulation_time)
				{
				ofstream htest("aodv_�ٶ�vs����.txt", ios::app);
				htest << v << " " << (double)aodvjr_packet_arrive / double(packet_spawn_rate*N*(simulation_time - 1)) << " " << (double)aodvjr_packet_drop / double(packet_spawn_rate*N*(simulation_time - 1)) << " " << aodvjr_control_packet << " " << ((double)aodvjr_packet_life_time_sum) / ((double)aodvjr_packet_arrive) << endl;   ///�ı����
				htest.close();
				}
				*/
				create_cbc_packet(car, packet_spawn_rate, N, time);  //�������ݰ�
				cluster_validity_test(car, N, cbc_cluster_radius, time, cbc_business_card_validity);  //�ع�ϵ�Ƿ��ܱ���
				check_in_cluster(car, N, cbc_cluster_radius);   //����Ƿ��ڴ���
				announce_cbchead(car, N, cbc_cluster_radius, cbc_control_packet); //��ͷ����
				handle_cbchead_announce(car, N, cbc_cluster_radius, block_width, cbc_forecast_time, cbc_control_packet);  //ת����ͷ����
				handle_deny_packet(car, N, cbc_cluster_radius, cbc_stop_time_increase);  //ת�����ذ�
				check_agree_time_window(car, N, cbc_cluster_radius);   //�������������ʱ�䴰�ڣ��Ƿ񷢷��ذ�
				handle_agree_packet(car, N, cbc_cluster_radius); //ת��ͬ���
				create_cluster(car, N, cbc_cluster_radius, cbc_tail_validity);//�ؽ���
				create_routing_table(car, N, time); //����·�ɱ�
				arrange_routing_table(car, N, time); //����·�ɱ���Ҫ�ǰѴ��ڽڵ��ʱ���ˢ��
				inform_cluster(car, N, cbc_cluster_radius, cbc_inform_interval, time, cbc_control_packet); //����Ƭ����
				handle_inform_packet(car, N, cbc_cluster_radius); //ת������Ƭ
				if (time % 30 == 1)
				{
					gate_way_exchange_inform_packet(car, N, cbc_cluster_radius, time, cbc_control_packet);  //���ؽ�������Ƭ
				}
				receive_exchange_inform_packet(car, N, cbc_cluster_radius, time, cbc_refresh_interval);	 //�յ�����Ƭ�������Լ���·�ɱ�	
				cbc_packet_delivery(car, N, cbc_packet_expiry, cbc_packet_drop, cbc_packet_arrive, cbc_packet_life_time_sum, cbc_control_packet); //���ݰ�����
				arm_end_hello(car, N, cbc_cluster_radius, cbc_hello_interval, cbc_control_packet);  //ĩβ�����ͷ��ά������
				receive_maintain_packet(car, N, cbc_cluster_radius, time, cbc_tail_validity);  //�յ�ά������
				cbc_cover_queue(car, N);//��������и�����ʵ�ʶ��У���ɱ����������ݵĴ���
				if (time == simulation_time)
				{
					ofstream jtest("cbc_�ٶ�vs����.txt", ios::app);
					jtest << v << " " << (double)cbc_packet_arrive / double(packet_spawn_rate*N*(simulation_time - 1)) << " " << (double)cbc_packet_drop / double(packet_spawn_rate*N*(simulation_time - 1)) << " " << cbc_control_packet << " " << ((double)cbc_packet_life_time_sum) / ((double)cbc_packet_arrive) << endl;   ///�ı����
					jtest.close();
				}

				//////////////////////////////////////////////////////////////////////////////////////////������lcaЭ��
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
					ofstream ktest("lca_�ٶ�vs����.txt", ios::app);
					ktest << v << " " << (double)lca_packet_arrive / double(packet_spawn_rate*N*(simulation_time - 1)) << " " << (double)lca_packet_drop / double(packet_spawn_rate*N*(simulation_time - 1)) << " " << lca_control_packet << " " << ((double)lca_packet_life_time_sum) / ((double)lca_packet_arrive) << endl;   ///�ı����
					ktest.close();
					break;
				}
				cout << "�������ݰ�Ϊ" << lca_packet_drop << endl;
			}//��ʱ��
			for (int i = 1; i < N; i++) //ɾ��ȫ���Ľڵ㣬
			{
				delete car[i];
			}
			v_id++;  //�ı����
		}
	}
}//������
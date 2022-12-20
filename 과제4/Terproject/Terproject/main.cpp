#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <vector>
#include <random>
#include <concurrent_priority_queue.h>
#include "SESSION.h"
#include "LOCAL_SESSION.h"
#include "DB_OBJ.h"
#include "TIMER_EVENT.h"
#include "LUA_OBJECT.h"
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

using namespace std;

array<int, 11> levelExp = { 0, 100, 200, 300, 400 ,500, 600, 700, 800, 900, 1200 };
array<int, 11> levelMaxHp = { 100, 200, 500, 600 ,700, 900, 1000, 1100, 1200, 1500, 1700 };
array<int, 11> levelAttackDamage = { 50, 70, 100, 120 ,150, 200, 220, 250, 300, 330, 400 };
array<SESSION, MAX_USER + MAX_NPC> clients;
array < array<LOCAL_SESSION, 100>, 100> gameMap;
pair<int, int> AStartObstacle[31];
SOCKET listenSocket;
SOCKET clientSocket;
EXP_OVER acceptOver;
HANDLE g_iocpHandle;

random_device npcRd;
default_random_engine npcDre(npcRd());
uniform_int_distribution<int> npcRandDirUid(0, 3); // inclusive
uniform_int_distribution<int> TestRandDirUid(0, 1999); // inclusive

chrono::system_clock::time_point g_nowTime = chrono::system_clock::now();
concurrency::concurrent_priority_queue<TIMER_EVENT> eventTimerQueue;

//main func
bool isPc(int id);
bool can_see(int from, int to);
bool canAttack(int from, int to);
int get_new_client_id();
void process_packet(int c_id, char* packet);
void disconnect(int c_id);
void worker_thread();
void AttackNpc(int cId, int npcId);

void UpdateNearList(SESSION& updateSession, int c_id);

//NPC func
void InitializeNPC();
void WakeUpNPC(int npcId, int waker);
void MoveRandNPC(int npcId);
bool Agro_NPC(int npcId, int cId);
bool AbleAttack_NPC(int npcId, int cId);

//Timer
void TimerWorkerThread();

constexpr int VIEW_RANGE = 8;
constexpr int AGRO_RANGE = 6;
constexpr int Attack_RANGE = 3;
int main()
{
	cout << "initialize Map" << endl;
	{
		AStartObstacle[0] = (make_pair(5, 1));
		AStartObstacle[1] = (make_pair(6, 1));
		AStartObstacle[2] = (make_pair(7, 1));
		AStartObstacle[3] = (make_pair(2, 3));
		AStartObstacle[4] = (make_pair(5, 3));
		AStartObstacle[5] = (make_pair(14, 3));
		AStartObstacle[6] = (make_pair(17, 4));
		AStartObstacle[7] = (make_pair(2, 6));
		AStartObstacle[8] = (make_pair(10, 6));
		AStartObstacle[9] = (make_pair(6, 8));
		AStartObstacle[10] = (make_pair(7, 8));
		AStartObstacle[11] = (make_pair(8, 8));
		AStartObstacle[12] = (make_pair(9, 8));
		AStartObstacle[13] = (make_pair(10, 8));
		AStartObstacle[14] = (make_pair(13, 8));
		AStartObstacle[15] = (make_pair(6, 9));
		AStartObstacle[16] = (make_pair(7, 9));
		AStartObstacle[17] = (make_pair(8, 9));
		AStartObstacle[18] = (make_pair(9, 9));
		AStartObstacle[19] = (make_pair(10, 9));
		AStartObstacle[20] = (make_pair(17, 10));
		AStartObstacle[21] = (make_pair(2, 11));
		AStartObstacle[22] = (make_pair(15, 13));
		AStartObstacle[23] = (make_pair(13, 14));
		AStartObstacle[24] = (make_pair(2, 15));
		AStartObstacle[25] = (make_pair(6, 15));
		AStartObstacle[26] = (make_pair(16, 15));
		AStartObstacle[27] = (make_pair(16, 16));
		AStartObstacle[28] = (make_pair(7, 17));
		AStartObstacle[29] = (make_pair(8, 17));
		AStartObstacle[30] = (make_pair(16, 17));
	}
	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			gameMap[i][j].SetPos(i, j);
			// initialize Obj on Local Map
		}
	}
	InitializeNPC();


	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		cout << "wsaStartUp Error" << endl;
		WSACleanup();
		return -1;
	}

	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	bind(listenSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(listenSocket, SOMAXCONN);

	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);

	g_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket), g_iocpHandle, 9999, 0);

	clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	acceptOver._comp_type = OP_ACCEPT;
	AcceptEx(listenSocket, clientSocket, acceptOver._send_buf, 0, addr_size + 16, addr_size + 16, 0, &acceptOver._over);

	thread timerThread = thread(TimerWorkerThread);
	vector <thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();

	for (int i = 0; i < num_threads; ++i) {

		worker_threads.emplace_back(worker_thread);
	}

	for (auto& th : worker_threads)
		th.join();
	timerThread.join();

	closesocket(listenSocket);
	WSACleanup();
}

bool isPc(int id)
{
	return id < MAX_USER;
}

bool can_see(int from, int to)
{
	if ((int)abs(clients[from].x - clients[to].x) > VIEW_RANGE)
		return false;
	if ((int)abs(clients[from].y - clients[to].y) > VIEW_RANGE)
		return false;
	return true;
}

bool canAttack(int from, int to)
{
	if ((int)abs(clients[from].x - clients[to].x) > Attack_RANGE)
		return false;
	if ((int)abs(clients[from].y - clients[to].y) > Attack_RANGE)
		return false;
	return true;
}

int get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		lock_guard <mutex> ll{ clients[i]._s_lock };
		if (clients[i]._state == ST_FREE)
			return i;
	}
	return -1;
}

void process_packet(int c_id, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN:
	{
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		strcpy_s(clients[c_id]._name, p->name);

		std::string tempName = clients[c_id]._name;
		if (std::string::npos != tempName.find("Test")) {
			clients[c_id].x = TestRandDirUid(npcDre);
			clients[c_id].y = TestRandDirUid(npcDre);
			clients[c_id].hp = 1000;
			clients[c_id].maxHp = 1000;
			clients[c_id].exp = 0;
			clients[c_id].level = 1;

			clients[c_id].send_login_info_packet();

			clients[c_id].myLocalSectionIndex.first = clients[c_id].x / 20;
			clients[c_id].myLocalSectionIndex.second = clients[c_id].y / 20;
			{
				lock_guard<mutex> ll{ clients[c_id]._s_lock };
				clients[c_id]._state = ST_INGAME;
			}
			UpdateNearList(clients[c_id], c_id);

			for (auto& pl : clients[c_id]._view_list) {
				if (isPc(pl)) clients[pl].send_add_player_packet(c_id, clients);
				else WakeUpNPC(pl, c_id);
				clients[c_id].send_add_player_packet(clients[pl]._id, clients);
			}
		}
		else {
			EXP_OVER* exOver = new EXP_OVER();
			exOver->_comp_type = OP_DB_GET_PLAYER_INFO;
			std::memcpy(exOver->_send_buf, p->name, strlen(p->name));
			std::memcpy(clients[c_id].playerID, p->name, strlen(p->name));
			exOver->_send_buf[strlen(p->name)] = 0;
			PostQueuedCompletionStatus(g_iocpHandle, strlen(p->name), c_id, &exOver->_over);
		}
	}
	break;
	case CS_MOVE:
	{
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		short x = clients[c_id].x;
		short y = clients[c_id].y;
		switch (p->direction) {
		case 1: if (y > 0) y--; break;
		case 2: if (y < W_HEIGHT - 1) y++; break;
		case 3: if (x > 0) x--; break;
		case 4: if (x < W_WIDTH - 1) x++; break;
		}

		//map Object Collision
		if (gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second].CollisionObject(x, y)) {
			x = clients[c_id].x;
			y = clients[c_id].y;
		}

		if (clients[c_id].x == x && clients[c_id].y == y)
			return;

		clients[c_id].x = x;
		clients[c_id].y = y;

		gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second].UpdatePlayers(clients[c_id], gameMap);//현재 로컬 최신화

		//뷰 리스트 업데이트를 위한 new near List 생성

		clients[c_id]._vl.lock();
		unordered_set<int> old_vlist = clients[c_id]._view_list;
		clients[c_id]._vl.unlock();

		UpdateNearList(clients[c_id], c_id);

		clients[c_id].send_move_packet(c_id, clients);
		for (auto& pl : clients[c_id]._view_list) {
			auto& cpl = clients[pl];
			cpl._vl.lock();
			if (cpl._view_list.count(c_id) > 0) { // 상대 클라에 내가 존재 한다면
				cpl._vl.unlock();
				if (isPc(pl))
					cpl.send_move_packet(c_id, clients);
				else WakeUpNPC(pl, c_id);
			}
			else {// 없다면
				cpl._vl.unlock();
				if (isPc(pl))
					cpl.send_add_player_packet(c_id, clients);
				else {
					clients[pl]._vl.lock();
					clients[pl]._view_list.insert(c_id);
					clients[pl]._vl.unlock();
					WakeUpNPC(pl, c_id);
				}
			}
			if (old_vlist.count(pl) == 0) {// 이전 리스트에 상대 클라가 없다면
				/*if (isPc(pl))
					clients[pl].send_add_player_packet(c_id, clients);
				else {
					clients[pl]._vl.lock();
					clients[pl]._view_list.insert(c_id);
					clients[pl]._vl.unlock();
					WakeUpNPC(pl, c_id);
				}*/
				clients[c_id].send_add_player_packet(pl, clients);
			}
		}

		for (auto& pl : old_vlist) // 이전 리스트 중에
			if (0 == clients[c_id]._view_list.count(pl)) { // 현재 리스트에 존재 안한다 => 삭제
				clients[c_id].send_remove_player_packet(pl);
				if (isPc(pl))
					clients[pl].send_remove_player_packet(c_id);
				else {
					clients[pl]._vl.lock();
					if (clients[pl]._view_list.count(c_id) != 0)
						clients[pl]._view_list.erase(c_id);
					clients[pl]._vl.unlock();
				}
			}
	}
	break;
	case CS_ATTACK:
	{
		if (clients[c_id].GetAbleAttack()) {
			SC_ATTACK_PACKET packet;
			packet.size = sizeof(SC_ATTACK_PACKET);
			packet.type = SC_ATTACK;
			clients[c_id].do_send(&packet);

			TIMER_EVENT ev{ c_id, chrono::system_clock::now() + 1s, EV_PLAYER_ATTACK_COOL, 0 };
			eventTimerQueue.push(ev);

			for (auto& npc : clients[c_id]._view_list) {
				if (!isPc(npc) && canAttack(c_id, npc) && clients[npc].myLua->GetArrive())
					AttackNpc(c_id, npc);
			}
			clients[c_id].SetAbleAttack(false);
		}
	}
	break;
	case CS_LOGOUT:
	{
		EXP_OVER* expOver = new EXP_OVER();
		expOver->_comp_type = OP_DB_SAVE_PLAYER;
		PostQueuedCompletionStatus(g_iocpHandle, 1, c_id, &expOver->_over);
	}
	break;
	}
}

void disconnect(int c_id)
{
	clients[c_id]._vl.lock();
	unordered_set <int> vl = clients[c_id]._view_list;
	clients[c_id]._vl.unlock();
	for (auto& p_id : vl) {
		auto& pl = clients[p_id];
		{
			lock_guard<mutex> ll(pl._s_lock);
			if (ST_INGAME != pl._state) continue;
		}
		if (pl._id == c_id) continue;
		pl.send_remove_player_packet(c_id);
	}
	closesocket(clients[c_id]._socket);

	lock_guard<mutex> ll(clients[c_id]._s_lock);
	clients[c_id]._state = ST_FREE;
}

void worker_thread()
{
	DB_OBJ dbObj = DB_OBJ{};
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(g_iocpHandle, &num_bytes, &key, &over, INFINITE);
		EXP_OVER* ex_over = reinterpret_cast<EXP_OVER*>(over);
		if (FALSE == ret) {
			if (ex_over->_comp_type == OP_ACCEPT) cout << "Accept Error";
			else {
				cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				if (ex_over->_comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}
		switch (ex_over->_comp_type) {
		case OP_ACCEPT: {
			int client_id = get_new_client_id();
			if (client_id != -1) {
				{
					lock_guard<mutex> ll(clients[client_id]._s_lock);
					clients[client_id]._state = ST_ALLOC;
				}
				clients[client_id].x = 0;
				clients[client_id].y = 0;
				clients[client_id]._id = client_id;
				clients[client_id]._name[0] = 0;
				clients[client_id]._prev_remain = 0;
				clients[client_id]._view_list.clear();
				clients[client_id]._socket = clientSocket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), g_iocpHandle, client_id, 0);
				clients[client_id].do_recv();
				clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else {
				cout << "Max user exceeded.\n";
			}
			ZeroMemory(&acceptOver._over, sizeof(acceptOver._over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(listenSocket, clientSocket, acceptOver._send_buf, 0, addr_size + 16, addr_size + 16, 0, &acceptOver._over);
			break;
		}
		case OP_RECV: {
			int remain_data = num_bytes + clients[key]._prev_remain;
			char* p = ex_over->_send_buf;
			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			clients[key]._prev_remain = remain_data;
			if (remain_data > 0) {
				std::memcpy(ex_over->_send_buf, p, remain_data);
			}
			clients[key].do_recv();
			break;
		}
		case OP_SEND:
			delete ex_over;
			break;
		case OP_DB_GET_PLAYER_INFO:
		{
			string userStr{ ex_over->_send_buf, strlen(ex_over->_send_buf) };
			wstring userId;
			userId.assign(userStr.begin(), userStr.end());
			wstring playerName;

			dbObj.GetPlayerInfo(userId, playerName, clients[key].x, clients[key].y, clients[key].level, clients[key].exp, clients[key].hp, clients[key].maxHp, clients[key].attackDamage);

			if (playerName.empty()) {
				SC_LOGIN_FAIL_PACKET failPacket;
				failPacket.size = 2;
				failPacket.type = SC_LOGIN_FAIL;
				clients[key].do_send(&failPacket);
			}
			else {
				string playerStr;
				playerStr.assign(playerName.begin(), playerName.end());
				std::memcpy(clients[key]._name, playerStr.c_str(), NAME_SIZE);
				clients[key].send_login_info_packet();

				clients[key].myLocalSectionIndex.first = clients[key].x / 20;
				clients[key].myLocalSectionIndex.second = clients[key].y / 20;
				{
					lock_guard<mutex> ll{ clients[key]._s_lock };
					clients[key]._state = ST_INGAME;
				}
				UpdateNearList(clients[key], key);

				for (auto& pl : clients[key]._view_list) {
					if (isPc(pl)) clients[pl].send_add_player_packet(key, clients);
					else WakeUpNPC(pl, key);
					clients[key].send_add_player_packet(clients[pl]._id, clients);
				}
			}
			delete ex_over;
		}
		break;
		case OP_NPC_MOVE:
		{
			bool keep_alive = false;
			bool isAgro = false;
			if (clients[key].myLua->GetArrive()) {
				for (auto index : clients[key]._view_list) {
					if (clients[index]._state != ST_INGAME) continue;
					if (can_see(static_cast<int>(key), index)) {
						if (clients[key].myLua->type == NPC_TYPE::AGRO) {
							if (!clients[key].myLua->isChase) {
								for (auto& vlIndex : clients[key]._view_list) {
									if (Agro_NPC(key, vlIndex)) {
										if (clients[key].myLua->ActiveChase()) {
											clients[key].myLua->SetChaseId(vlIndex);
											clients[key].myLua->AStarLoad(clients[key].x, clients[key].y, clients[vlIndex].x, clients[vlIndex].y);
											isAgro = true;
											break;
										}
									}
								}
							}
						}
						keep_alive = true;
						break;
					}
				}
				if (isAgro) {
					EXP_OVER* expOver = new EXP_OVER();
					expOver->_comp_type = OP_NPC_CHASE_MOVE;
					PostQueuedCompletionStatus(g_iocpHandle, 1, key, &expOver->_over);
					//cout << "AGRO NPC: " << key << endl;
				}
				else if (true == keep_alive) {
					MoveRandNPC(static_cast<int>(key));
					{
						TIMER_EVENT ev{ key, chrono::system_clock::now() + 1s, EV_RANDOM_MOVE, 0 };
						eventTimerQueue.push(ev);
					}
				}
				else {
					if (clients[key].myLua != nullptr) {
						clients[key].myLua->InActiveNPC();
						if (clients[key].myLua->InActiveChase())
							clients[key].myLua->SetChaseId(-1);
					}
				}
			}
			delete ex_over;
		}
		break;
		case OP_NPC_CHASE_MOVE:
		{
			if (clients[key].myLua->GetArrive() && clients[key].myLua->isChase) {
				//이미 붙어 있다면
				int chaseId = clients[key].myLua->GetChaseId();
				if (chaseId < 0)
				{
					if (clients[key].myLua->InActiveChase()) {
						TIMER_EVENT ev{ key, chrono::system_clock::now() + 2s, EV_RANDOM_MOVE, 0 };
						eventTimerQueue.push(ev);
					}
				}
				else if (clients[key].x == clients[chaseId].x && clients[key].y == clients[chaseId].y) {
					if (AbleAttack_NPC(key, chaseId) && clients[key].myLua->GetArrive() && clients[chaseId]._name[0] != 'T') {
						clients[chaseId].hp = clients[chaseId].hp - clients[key].attackDamage;
						if (clients[chaseId].hp < 0) {
							clients[chaseId].exp /= 2;
							clients[chaseId].maxHp = levelMaxHp[clients[chaseId].level - 1];
							clients[chaseId].hp = clients[chaseId].maxHp;

							clients[chaseId].x = 0;
							clients[chaseId].y = 0;

							gameMap[clients[chaseId].myLocalSectionIndex.first][clients[chaseId].myLocalSectionIndex.second].DeletePlayers(clients[chaseId]);//현재 로컬 최신화
							clients[chaseId].myLocalSectionIndex = std::make_pair(0, 0);
							gameMap[0][0].InsertPlayers(clients[chaseId]);

							clients[chaseId].send_move_packet(chaseId, clients);

							//뷰 리스트 업데이트를 위한 new near List 생성							
							clients[chaseId]._vl.lock();
							unordered_set<int> old_vlist = clients[chaseId]._view_list;
							clients[chaseId]._vl.unlock();


							UpdateNearList(clients[chaseId], chaseId);
							for (auto& pl : clients[chaseId]._view_list) {
								auto& cpl = clients[pl];
								cpl._vl.lock();
								if (cpl._view_list.count(chaseId) > 0) {
									cpl._vl.unlock();
									if (isPc(pl))
										cpl.send_move_packet(chaseId, clients);
								}
								else {
									cpl._vl.unlock();
									if (isPc(pl))
										cpl.send_add_player_packet(chaseId, clients);
									else {
										clients[pl]._vl.lock();
										clients[pl]._view_list.insert(chaseId);
										clients[pl]._vl.unlock();
										WakeUpNPC(pl, chaseId);
									}
								}

								if (old_vlist.count(pl) == 0) {
									/*if (isPc(pl))
										clients[pl].send_add_player_packet(chaseId, clients);
									else {
										clients[pl]._vl.lock();
										clients[pl]._view_list.insert(chaseId);
										clients[pl]._vl.unlock();
										WakeUpNPC(pl, chaseId);
									}*/
									clients[chaseId].send_add_player_packet(pl, clients);
								}
							}

							for (auto& pl : old_vlist)
								if (0 == clients[chaseId]._view_list.count(pl)) {
									clients[chaseId].send_remove_player_packet(pl);
									if (isPc(pl))
										clients[pl].send_remove_player_packet(chaseId);
									else {
										clients[pl]._vl.lock();
										clients[pl]._view_list.erase(chaseId);
										clients[pl]._vl.unlock();
									}
								}

							SC_STAT_CHANGEL_PACKET sendPakcet;
							sendPakcet.size = sizeof(SC_STAT_CHANGEL_PACKET);
							sendPakcet.id = chaseId;
							sendPakcet.hp = clients[chaseId].maxHp;
							sendPakcet.level = clients[chaseId].level;
							sendPakcet.max_hp = clients[chaseId].maxHp;
							sendPakcet.type = SC_STAT_CHANGE;
							sendPakcet.exp = clients[chaseId].exp;
							sendPakcet.max_exp = levelExp[clients[chaseId].level];
							clients[chaseId].do_send(&sendPakcet);

							EXP_OVER* expOver = new EXP_OVER();
							expOver->_comp_type = OP_DB_SAVE_PLAYER;
							PostQueuedCompletionStatus(g_iocpHandle, 1, chaseId, &expOver->_over);
							if (clients[key].myLua->InActiveChase()) {
								TIMER_EVENT ev{ key, chrono::system_clock::now() + 2s, EV_RANDOM_MOVE, 0 };
								eventTimerQueue.push(ev);
							}
						}
						else {
							SC_STAT_CHANGEL_PACKET sendPakcet;
							sendPakcet.size = sizeof(SC_STAT_CHANGEL_PACKET);
							sendPakcet.id = chaseId;
							sendPakcet.hp = clients[chaseId].hp;
							sendPakcet.level = clients[chaseId].level;
							sendPakcet.max_hp = clients[chaseId].maxHp;
							sendPakcet.type = SC_STAT_CHANGE;
							sendPakcet.exp = clients[chaseId].exp;
							sendPakcet.max_exp = levelExp[clients[chaseId].level];
							clients[chaseId].do_send(&sendPakcet);
						}						
					}
					TIMER_EVENT ev{ key, chrono::system_clock::now() + 2s, EV_CHASE_MOVE, 0 };
					eventTimerQueue.push(ev);
				}
				else {
					//길찾기 실행
					//AStarLoad(clients[key].x, clients[key].y, clients[chaseId].x, clients[chaseId].y);
					pair<int, int> res = clients[key].myLua->GetNextNode();
					//clients[key].x = res.first;
					//clients[key].y = res.second;


					//clients[key]._vl.lock();
					//unordered_set<int> old_vlist = clients[key]._view_list;
					//clients[key]._vl.unlock();

					//gameMap[clients[key].myLocalSectionIndex.first][clients[key].myLocalSectionIndex.second].UpdatePlayers(clients[key], gameMap);//현재 로컬 최신화
					//UpdateNearList(clients[key], key);

					////npc not send
					////clients[npcId].send_move_packet(npcId, clients);

					//for (auto& pl : clients[key]._view_list) {
					//	auto& cpl = clients[pl];
					//	cpl._vl.lock();
					//	if (cpl._view_list.count(key) > 0) {
					//		cpl._vl.unlock();
					//		if (isPc(pl))
					//			cpl.send_move_packet(key, clients);
					//	}
					//	else {
					//		cpl._vl.unlock();
					//		if (isPc(pl))
					//			clients[pl].send_add_player_packet(key, clients);
					//	}
					//}

					//for (auto& pl : old_vlist)
					//	if (0 == clients[key]._view_list.count(pl))
					//		if (isPc(pl))
					//			clients[pl].send_remove_player_packet(key);

					//TIMER_EVENT ev{ key, chrono::system_clock::now() + 1s, EV_CHASE_MOVE, 0 };
					//eventTimerQueue.push(ev);
					//결과 값(길 list)이 비었다면
					if (res.first < 0 || res.second < 0) {
						//새로 길 찾기 실행
						if (!Agro_NPC(key, chaseId)) {
							clients[key].myLua->InActiveChase();
							TIMER_EVENT ev{ key, chrono::system_clock::now() + 2s, EV_RANDOM_MOVE, 0 };
							eventTimerQueue.push(ev);
						}
						else {
							clients[key].myLua->AStarLoad(clients[key].x, clients[key].y, clients[chaseId].x, clients[chaseId].y);
							pair<int, int> res = clients[key].myLua->GetNextNode();
							if (res.first > 0 && res.second > 0) {
								clients[key].x = res.first;
								clients[key].y = res.second;
							}

							clients[key]._vl.lock();
							unordered_set<int> old_vlist = clients[key]._view_list;
							clients[key]._vl.unlock();

							gameMap[clients[key].myLocalSectionIndex.first][clients[key].myLocalSectionIndex.second].UpdatePlayers(clients[key], gameMap);//현재 로컬 최신화
							UpdateNearList(clients[key], key);

							//npc not send
							//clients[npcId].send_move_packet(npcId, clients);

							for (auto& pl : clients[key]._view_list) {
								auto& cpl = clients[pl];
								cpl._vl.lock();
								if (cpl._view_list.count(key) > 0) {
									cpl._vl.unlock();
									if (isPc(pl))
										cpl.send_move_packet(key, clients);
								}
								else {
									cpl._vl.unlock();
									if (isPc(pl))
										clients[pl].send_add_player_packet(key, clients);
								}
							}

							for (auto& pl : old_vlist)
								if (0 == clients[key]._view_list.count(pl))
									if (isPc(pl))
										clients[pl].send_remove_player_packet(key);

							TIMER_EVENT ev{ key, chrono::system_clock::now() + 1s, EV_CHASE_MOVE, 0 };
							eventTimerQueue.push(ev);
						}
					}
					else {
						//길이 존재 한다면 고고
						clients[key].x = res.first;
						clients[key].y = res.second;

						clients[key]._vl.lock();
						unordered_set<int> old_vlist = clients[key]._view_list;
						clients[key]._vl.unlock();

						gameMap[clients[key].myLocalSectionIndex.first][clients[key].myLocalSectionIndex.second].UpdatePlayers(clients[key], gameMap);//현재 로컬 최신화
						UpdateNearList(clients[key], key);

						//npc not send
						//clients[npcId].send_move_packet(npcId, clients);

						for (auto& pl : clients[key]._view_list) {
							auto& cpl = clients[pl];
							cpl._vl.lock();
							if (cpl._view_list.count(key) > 0) {
								cpl._vl.unlock();
								if (isPc(pl))
									cpl.send_move_packet(key, clients);
							}
							else {
								cpl._vl.unlock();
								if (isPc(pl))
									clients[pl].send_add_player_packet(key, clients);
							}
						}

						for (auto& pl : old_vlist)
							if (0 == clients[key]._view_list.count(pl))
								if (isPc(pl))
									clients[pl].send_remove_player_packet(key);

						TIMER_EVENT ev{ key, chrono::system_clock::now() + 1s, EV_CHASE_MOVE, 0 };
						eventTimerQueue.push(ev);
					}
				}
			}
			delete ex_over;
		}
		break;
		case OP_DB_SAVE_PLAYER:
		{
			std::string idStr = clients[key].playerID;
			std::wstring idWstr;
			idWstr.assign(idStr.begin(), idStr.end());
			dbObj.SavePlayerInfo(idWstr, clients[key].x, clients[key].y, clients[key].level, clients[key].exp, clients[key].hp, clients[key].maxHp, clients[key].attackDamage);
			delete ex_over;
		}
		break;
		case OP_DB_AUTO_SAVE_PLAYER:
		{
			for (int i = 0; i < MAX_USER; i++) {
				if (clients[i]._name[0] != 'T') {
					std::string idStr = clients[i].playerID;
					std::wstring idWstr;
					idWstr.assign(idStr.begin(), idStr.end());
					dbObj.SavePlayerInfo(idWstr, clients[i].x, clients[i].y, clients[i].level, clients[i].exp, clients[i].hp, clients[i].maxHp, clients[i].attackDamage);
				}
			}
			TIMER_EVENT ev{ key, chrono::system_clock::now() + 300s, EV_AUTO_SAVE, 0 };//5분 마다 오토 세이브
			eventTimerQueue.push(ev);
			delete ex_over;
		}
		break;
		/*case OP_DB_SET_PLAYER_POSITION:
		{
			string userStr{ clients[key]._user_ID };
			wstring userId;
			userId.assign(userStr.begin(), userStr.end());
			DBmutex.lock();
			SetPlayerPosition(userId, clients[key].x, clients[key].y);
			DBmutex.unlock();
		}
		break;*/
		}

	}
}

void AttackNpc(int cId, int npcId)
{
	clients[npcId].hp = clients[npcId].hp - clients[cId].attackDamage;
	if (clients[npcId].hp > 0) {
		SC_STAT_CHANGEL_PACKET sendPakcet;
		sendPakcet.id = npcId;
		sendPakcet.size = sizeof(SC_STAT_CHANGEL_PACKET);
		sendPakcet.hp = clients[npcId].hp;
		sendPakcet.level = clients[npcId].level;
		sendPakcet.max_hp = clients[npcId].maxHp;
		sendPakcet.type = SC_STAT_CHANGE;
		sendPakcet.exp = clients[npcId].exp;
		sendPakcet.max_exp = levelExp[clients[npcId].level];
		clients[cId].do_send(&sendPakcet);
	}
	if (clients[npcId].hp < 0) {
		if (clients[npcId].myLua->DieNpc()) {
			clients[cId].exp += clients[npcId].exp;
			if (clients[cId].level < 11) {
				if (levelExp[clients[cId].level] < clients[cId].exp) {
					clients[cId].exp -= levelExp[clients[cId].level];
					clients[cId].level += 1;
					clients[cId].maxHp = levelMaxHp[clients[cId].level - 1];
					clients[cId].attackDamage = levelAttackDamage[clients[cId].level - 1];
					clients[cId].hp = clients[cId].maxHp;
				}
			}

			SC_STAT_CHANGEL_PACKET sendPakcet;
			sendPakcet.id = npcId;
			sendPakcet.size = sizeof(SC_STAT_CHANGEL_PACKET);
			sendPakcet.hp = 0;
			sendPakcet.level = clients[npcId].level;
			sendPakcet.max_hp = clients[npcId].maxHp;
			sendPakcet.type = SC_STAT_CHANGE;
			sendPakcet.exp = clients[npcId].exp;
			sendPakcet.max_exp = levelExp[clients[npcId].level];
			clients[cId].do_send(&sendPakcet);

			SC_STAT_CHANGEL_PACKET npcsendPakcet;
			npcsendPakcet.size = sizeof(SC_STAT_CHANGEL_PACKET);
			npcsendPakcet.id = cId;
			npcsendPakcet.hp = clients[cId].hp;
			npcsendPakcet.level = clients[cId].level;
			npcsendPakcet.max_hp = clients[cId].maxHp;
			npcsendPakcet.type = SC_STAT_CHANGE;
			npcsendPakcet.exp = clients[cId].exp;
			npcsendPakcet.max_exp = levelExp[clients[cId].level];
			clients[cId].do_send(&npcsendPakcet);

			if (clients[cId]._name[0] != 'T') {
				EXP_OVER* expOver = new EXP_OVER();
				expOver->_comp_type = OP_DB_SAVE_PLAYER;
				PostQueuedCompletionStatus(g_iocpHandle, 1, cId, &expOver->_over);
			}
			SC_REMOVE_OBJECT_PACKET removePakcet;// 죽었으니 제거
			removePakcet.type = SC_REMOVE_OBJECT;
			removePakcet.size = sizeof(SC_REMOVE_OBJECT_PACKET);
			removePakcet.id = npcId;

			for (auto& vlIndex : clients[npcId]._view_list)// npc의 뷰 리스트가 가지고 있는 클라이언트들에게 전송
				clients[vlIndex].do_send(&removePakcet);

			TIMER_EVENT ev{ npcId, chrono::system_clock::now() + 30s, EV_RESPAWN_NPC, 0 };
			eventTimerQueue.push(ev);
		}
	}
}

void UpdateNearList(SESSION& updateSession, int c_id)
{
	if (isPc(c_id)) {
		std::unordered_set<int> findMyLocal;
		{
			lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second].playersLock };
			findMyLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second].players;
		}
		for (auto& id : findMyLocal) { // current my local
			if (clients[id]._state != ST_INGAME) continue;
			if (clients[id]._id == c_id) continue;
			if (!isPc(id)) {
				if (!clients[id].myLua->GetArrive() || clients[id].hp < 1)
					continue;
			}
			if (can_see(c_id, id)) {
				updateSession._vl.lock();
				updateSession._view_list.insert(id);
				updateSession._vl.unlock();
			}
		}
		//근첩한 local 탐색
		if (clients[c_id].x % 20 < 7) { // 좌로 붙은 섹션
			if (clients[c_id].y % 20 < 7) {// 위로 붙은 부분
				if (clients[c_id].myLocalSectionIndex.second > 0 && clients[c_id].myLocalSectionIndex.first > 0) {
					{
						std::unordered_set<int> findNearLocal;
						{
							lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second - 1].playersLock };
							findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second - 1].players;
						}
						for (auto& id : findNearLocal) {
							if (clients[id]._state != ST_INGAME) continue;
							if (clients[id]._id == c_id) continue;
							if (!isPc(id)) {
								if (!clients[id].myLua->GetArrive())
									continue;
							}
							if (can_see(c_id, id)) {
								updateSession._vl.lock();
								updateSession._view_list.insert(id);
								updateSession._vl.unlock();
							}
						}
					}
					{
						std::unordered_set<int> findNearLocal;
						{
							lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].playersLock };
							findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].players;
						}
						for (auto& id : findNearLocal) {
							if (clients[id]._state != ST_INGAME) continue;
							if (clients[id]._id == c_id) continue;
							if (!isPc(id)) {
								if (!clients[id].myLua->GetArrive())
									continue;
							}
							if (can_see(c_id, id)) {
								updateSession._vl.lock();
								updateSession._view_list.insert(id);
								updateSession._vl.unlock();
							}
						}
					}
					{
						std::unordered_set<int> findNearLocal;
						{
							lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].playersLock };
							findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].players;
						}
						for (auto& id : findNearLocal) {
							if (clients[id]._state != ST_INGAME) continue;
							if (clients[id]._id == c_id) continue;
							if (!isPc(id)) {
								if (!clients[id].myLua->GetArrive())
									continue;
							}
							if (can_see(c_id, id)) {
								updateSession._vl.lock();
								updateSession._view_list.insert(id);
								updateSession._vl.unlock();
							}
						}
					}
				}
				else if (clients[c_id].myLocalSectionIndex.first > 0) {
					{
						std::unordered_set<int> findNearLocal;
						{
							lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].playersLock };
							findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].players;
						}
						for (auto& id : findNearLocal) {
							if (clients[id]._state != ST_INGAME) continue;
							if (clients[id]._id == c_id) continue;
							if (!isPc(id)) {
								if (!clients[id].myLua->GetArrive())
									continue;
							}
							if (can_see(c_id, id)) {
								updateSession._vl.lock();
								updateSession._view_list.insert(id);
								updateSession._vl.unlock();
							}
						}
					}
				}
				else if (clients[c_id].myLocalSectionIndex.second > 0) {
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
			}
			else if (clients[c_id].y % 20 > 13) { // 아래로 붙은 부분
				if (clients[c_id].myLocalSectionIndex.second < 19 && clients[c_id].myLocalSectionIndex.first > 0) {
					{
						std::unordered_set<int> findNearLocal;
						{
							lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second + 1].playersLock };
							findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second + 1].players;
						}
						for (auto& id : findNearLocal) {
							if (clients[id]._state != ST_INGAME) continue;
							if (clients[id]._id == c_id) continue;
							if (!isPc(id)) {
								if (!clients[id].myLua->GetArrive())
									continue;
							}
							if (can_see(c_id, id)) {
								updateSession._vl.lock();
								updateSession._view_list.insert(id);
								updateSession._vl.unlock();
							}
						}
					}
					{
						std::unordered_set<int> findNearLocal;
						{
							lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].playersLock };
							findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].players;
						}
						for (auto& id : findNearLocal) {
							if (clients[id]._state != ST_INGAME) continue;
							if (clients[id]._id == c_id) continue;
							if (!isPc(id)) {
								if (!clients[id].myLua->GetArrive())
									continue;
							}
							if (can_see(c_id, id)) {
								updateSession._vl.lock();
								updateSession._view_list.insert(id);
								updateSession._vl.unlock();
							}
						}
					}
					{
						std::unordered_set<int> findNearLocal;
						{
							lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].playersLock };
							findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].players;
						}
						for (auto& id : findNearLocal) {
							if (clients[id]._state != ST_INGAME) continue;
							if (clients[id]._id == c_id) continue;
							if (!isPc(id)) {
								if (!clients[id].myLua->GetArrive())
									continue;
							}
							if (can_see(c_id, id)) {
								updateSession._vl.lock();
								updateSession._view_list.insert(id);
								updateSession._vl.unlock();
							}
						}
					}
				}
				else if (clients[c_id].myLocalSectionIndex.second < 19) {
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
				else if (clients[c_id].myLocalSectionIndex.first > 0) {
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
			}
			if (clients[c_id].myLocalSectionIndex.first > 0) {
				std::unordered_set<int> findNearLocal;
				{
					lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].playersLock };
					findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].players;
				}
				for (auto& id : findNearLocal) {
					if (clients[id]._state != ST_INGAME) continue;
					if (clients[id]._id == c_id) continue;
					if (!isPc(id)) {
						if (!clients[id].myLua->GetArrive())
							continue;
					}
					if (can_see(c_id, id)) {
						updateSession._vl.lock();
						updateSession._view_list.insert(id);
						updateSession._vl.unlock();
					}
				}
			}
		}
		else if (clients[c_id].x % 20 > 13) { // 우로 붙은 섹션
			if (clients[c_id].y % 20 < 7) {
				if (clients[c_id].myLocalSectionIndex.second > 0 && clients[c_id].myLocalSectionIndex.first < 19) {
					{
						std::unordered_set<int> findNearLocal;
						{
							lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second - 1].playersLock };
							findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second - 1].players;
						}
						for (auto& id : findNearLocal) {
							if (clients[id]._state != ST_INGAME) continue;
							if (clients[id]._id == c_id) continue;
							if (!isPc(id)) {
								if (!clients[id].myLua->GetArrive())
									continue;
							}
							if (can_see(c_id, id)) {
								updateSession._vl.lock();
								updateSession._view_list.insert(id);
								updateSession._vl.unlock();
							}
						}
					}
					{
						std::unordered_set<int> findNearLocal;
						{
							lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].playersLock };
							findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].players;
						}
						for (auto& id : findNearLocal) {
							if (clients[id]._state != ST_INGAME) continue;
							if (clients[id]._id == c_id) continue;
							if (!isPc(id)) {
								if (!clients[id].myLua->GetArrive())
									continue;
							}
							if (can_see(c_id, id)) {
								updateSession._vl.lock();
								updateSession._view_list.insert(id);
								updateSession._vl.unlock();
							}
						}
					}
					{
						std::unordered_set<int> findNearLocal;
						{
							lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].playersLock };
							findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].players;
						}
						for (auto& id : findNearLocal) {
							if (clients[id]._state != ST_INGAME) continue;
							if (clients[id]._id == c_id) continue;
							if (!isPc(id)) {
								if (!clients[id].myLua->GetArrive())
									continue;
							}
							if (can_see(c_id, id)) {
								updateSession._vl.lock();
								updateSession._view_list.insert(id);
								updateSession._vl.unlock();
							}
						}

					}
				}
				else if (clients[c_id].myLocalSectionIndex.second > 0) {
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
				else if (clients[c_id].myLocalSectionIndex.first < 19) {
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
			}
			else if (clients[c_id].y % 20 > 13) {
				if (clients[c_id].myLocalSectionIndex.second < 19 && clients[c_id].myLocalSectionIndex.first < 19) {
					{
						std::unordered_set<int> findNearLocal;
						{
							lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second + 1].playersLock };
							findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second + 1].players;
						}
						for (auto& id : findNearLocal) {
							if (clients[id]._state != ST_INGAME) continue;
							if (clients[id]._id == c_id) continue;
							if (!isPc(id)) {
								if (!clients[id].myLua->GetArrive())
									continue;
							}
							if (can_see(c_id, id)) {
								updateSession._vl.lock();
								updateSession._view_list.insert(id);
								updateSession._vl.unlock();
							}
						}
					}
					{
						std::unordered_set<int> findNearLocal;
						{
							lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].playersLock };
							findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].players;
						}
						for (auto& id : findNearLocal) {
							if (clients[id]._state != ST_INGAME) continue;
							if (clients[id]._id == c_id) continue;
							if (!isPc(id)) {
								if (!clients[id].myLua->GetArrive())
									continue;
							}
							if (can_see(c_id, id)) {
								updateSession._vl.lock();
								updateSession._view_list.insert(id);
								updateSession._vl.unlock();
							}
						}
					}
					{
						std::unordered_set<int> findNearLocal;
						{
							lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].playersLock };
							findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].players;
						}
						for (auto& id : findNearLocal) {
							if (clients[id]._state != ST_INGAME) continue;
							if (clients[id]._id == c_id) continue;
							if (!isPc(id)) {
								if (!clients[id].myLua->GetArrive())
									continue;
							}
							if (can_see(c_id, id)) {
								updateSession._vl.lock();
								updateSession._view_list.insert(id);
								updateSession._vl.unlock();
							}
						}
					}
				}
				else if (clients[c_id].myLocalSectionIndex.second < 19) {
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
				else if (clients[c_id].myLocalSectionIndex.first < 19) {
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
			}
			if (clients[c_id].myLocalSectionIndex.first < 19) {
				std::unordered_set<int> findNearLocal;
				{
					lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].playersLock };
					findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].players;
				}
				for (auto& id : findNearLocal) {
					if (clients[id]._state != ST_INGAME) continue;
					if (clients[id]._id == c_id) continue;
					if (!isPc(id)) {
						if (!clients[id].myLua->GetArrive())
							continue;
					}
					if (can_see(c_id, id)) {
						updateSession._vl.lock();
						updateSession._view_list.insert(id);
						updateSession._vl.unlock();
					}
				}
			}
		}
		else { // x는 내부에 잘 있고 y만 체크
			if (clients[c_id].y % 20 < 7) {
				if (clients[c_id].myLocalSectionIndex.second > 0) {
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
			}
			else if (clients[c_id].y % 20 > 13) {
				if (clients[c_id].myLocalSectionIndex.second < 19) {
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
			}
		}
		return;
	}
	//npc view list -> players
	std::unordered_set<int> findMyLocal;
	{
		lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second].playersLock };
		findMyLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second].players;
	}
	for (auto& id : findMyLocal) { // current my local
		if (clients[id]._state != ST_INGAME) continue;
		if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
		if (!isPc(id)) {
			if (!clients[id].myLua->GetArrive())
				continue;
		}
		if (can_see(c_id, id)) {
			updateSession._vl.lock();
			updateSession._view_list.insert(id);
			updateSession._vl.unlock();
		}
	}
	//근첩한 local 탐색
	if (clients[c_id].x % 20 < 7) { // 좌로 붙은 섹션
		if (clients[c_id].y % 20 < 7) {// 위로 붙은 부분
			if (clients[c_id].myLocalSectionIndex.second > 0 && clients[c_id].myLocalSectionIndex.first > 0) {
				{
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second - 1].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second - 1].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
				{
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
				{
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
			}
			else if (clients[c_id].myLocalSectionIndex.first > 0) {
				{
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
			}
			else if (clients[c_id].myLocalSectionIndex.second > 0) {
				std::unordered_set<int> findNearLocal;
				{
					lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].playersLock };
					findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].players;
				}
				for (auto& id : findNearLocal) {
					if (clients[id]._state != ST_INGAME) continue;
					if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
					if (!isPc(id)) {
						if (!clients[id].myLua->GetArrive())
							continue;
					}
					if (can_see(c_id, id)) {
						updateSession._vl.lock();
						updateSession._view_list.insert(id);
						updateSession._vl.unlock();
					}
				}
			}
		}
		else if (clients[c_id].y % 20 > 13) { // 아래로 붙은 부분
			if (clients[c_id].myLocalSectionIndex.second < 19 && clients[c_id].myLocalSectionIndex.first > 0) {
				{
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second + 1].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second + 1].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
				{
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
				{
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
			}
			else if (clients[c_id].myLocalSectionIndex.second < 19) {
				std::unordered_set<int> findNearLocal;
				{
					lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].playersLock };
					findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].players;
				}
				for (auto& id : findNearLocal) {
					if (clients[id]._state != ST_INGAME) continue;
					if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
					if (!isPc(id)) {
						if (!clients[id].myLua->GetArrive())
							continue;
					}
					if (can_see(c_id, id)) {
						updateSession._vl.lock();
						updateSession._view_list.insert(id);
						updateSession._vl.unlock();
					}
				}
			}
			else if (clients[c_id].myLocalSectionIndex.first > 0) {
				std::unordered_set<int> findNearLocal;
				{
					lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].playersLock };
					findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].players;
				}
				for (auto& id : findNearLocal) {
					if (clients[id]._state != ST_INGAME) continue;
					if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
					if (!isPc(id)) {
						if (!clients[id].myLua->GetArrive())
							continue;
					}
					if (can_see(c_id, id)) {
						updateSession._vl.lock();
						updateSession._view_list.insert(id);
						updateSession._vl.unlock();
					}
				}
			}
		}
		if (clients[c_id].myLocalSectionIndex.first > 0) {
			std::unordered_set<int> findNearLocal;
			{
				lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].playersLock };
				findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first - 1][clients[c_id].myLocalSectionIndex.second].players;
			}
			for (auto& id : findNearLocal) {
				if (clients[id]._state != ST_INGAME) continue;
				if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
				if (!isPc(id)) {
					if (!clients[id].myLua->GetArrive())
						continue;
				}
				if (can_see(c_id, id)) {
					updateSession._vl.lock();
					updateSession._view_list.insert(id);
					updateSession._vl.unlock();
				}
			}
		}
	}
	else if (clients[c_id].x % 20 > 13) { // 우로 붙은 섹션
		if (clients[c_id].y % 20 < 7) {
			if (clients[c_id].myLocalSectionIndex.second > 0 && clients[c_id].myLocalSectionIndex.first < 19) {
				{
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second - 1].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second - 1].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
				{
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
				{
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}

				}
			}
			else if (clients[c_id].myLocalSectionIndex.second > 0) {
				std::unordered_set<int> findNearLocal;
				{
					lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].playersLock };
					findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].players;
				}
				for (auto& id : findNearLocal) {
					if (clients[id]._state != ST_INGAME) continue;
					if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
					if (!isPc(id)) {
						if (!clients[id].myLua->GetArrive())
							continue;
					}
					if (can_see(c_id, id)) {
						updateSession._vl.lock();
						updateSession._view_list.insert(id);
						updateSession._vl.unlock();
					}
				}
			}
			else if (clients[c_id].myLocalSectionIndex.first < 19) {
				std::unordered_set<int> findNearLocal;
				{
					lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].playersLock };
					findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].players;
				}
				for (auto& id : findNearLocal) {
					if (clients[id]._state != ST_INGAME) continue;
					if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
					if (!isPc(id)) {
						if (!clients[id].myLua->GetArrive())
							continue;
					}
					if (can_see(c_id, id)) {
						updateSession._vl.lock();
						updateSession._view_list.insert(id);
						updateSession._vl.unlock();
					}
				}
			}
		}
		else if (clients[c_id].y % 20 > 13) {
			if (clients[c_id].myLocalSectionIndex.second < 19 && clients[c_id].myLocalSectionIndex.first < 19) {
				{
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second + 1].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second + 1].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
				{
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
				{
					std::unordered_set<int> findNearLocal;
					{
						lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].playersLock };
						findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].players;
					}
					for (auto& id : findNearLocal) {
						if (clients[id]._state != ST_INGAME) continue;
						if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
						if (!isPc(id)) {
							if (!clients[id].myLua->GetArrive())
								continue;
						}
						if (can_see(c_id, id)) {
							updateSession._vl.lock();
							updateSession._view_list.insert(id);
							updateSession._vl.unlock();
						}
					}
				}
			}
			else if (clients[c_id].myLocalSectionIndex.second < 19) {
				std::unordered_set<int> findNearLocal;
				{
					lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].playersLock };
					findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].players;
				}
				for (auto& id : findNearLocal) {
					if (clients[id]._state != ST_INGAME) continue;
					if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
					if (!isPc(id)) {
						if (!clients[id].myLua->GetArrive())
							continue;
					}
					if (can_see(c_id, id)) {
						updateSession._vl.lock();
						updateSession._view_list.insert(id);
						updateSession._vl.unlock();
					}
				}
			}
			else if (clients[c_id].myLocalSectionIndex.first < 19) {
				std::unordered_set<int> findNearLocal;
				{
					lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].playersLock };
					findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].players;
				}
				for (auto& id : findNearLocal) {
					if (clients[id]._state != ST_INGAME) continue;
					if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
					if (!isPc(id)) {
						if (!clients[id].myLua->GetArrive())
							continue;
					}
					if (can_see(c_id, id)) {
						updateSession._vl.lock();
						updateSession._view_list.insert(id);
						updateSession._vl.unlock();
					}
				}
			}
		}
		if (clients[c_id].myLocalSectionIndex.first < 19) {
			std::unordered_set<int> findNearLocal;
			{
				lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].playersLock };
				findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first + 1][clients[c_id].myLocalSectionIndex.second].players;
			}
			for (auto& id : findNearLocal) {
				if (clients[id]._state != ST_INGAME) continue;
				if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
				if (!isPc(id)) {
					if (!clients[id].myLua->GetArrive())
						continue;
				}
				if (can_see(c_id, id)) {
					updateSession._vl.lock();
					updateSession._view_list.insert(id);
					updateSession._vl.unlock();
				}
			}
		}
	}
	else { // x는 내부에 잘 있고 y만 체크
		if (clients[c_id].y % 20 < 7) {
			if (clients[c_id].myLocalSectionIndex.second > 0) {
				std::unordered_set<int> findNearLocal;
				{
					lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].playersLock };
					findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second - 1].players;
				}
				for (auto& id : findNearLocal) {
					if (clients[id]._state != ST_INGAME) continue;
					if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
					if (!isPc(id)) {
						if (!clients[id].myLua->GetArrive())
							continue;
					}
					if (can_see(c_id, id)) {
						updateSession._vl.lock();
						updateSession._view_list.insert(id);
						updateSession._vl.unlock();
					}
				}
			}
		}
		else if (clients[c_id].y % 20 > 13) {
			if (clients[c_id].myLocalSectionIndex.second < 19) {
				std::unordered_set<int> findNearLocal;
				{
					lock_guard <mutex> ll{ gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].playersLock };
					findNearLocal = gameMap[clients[c_id].myLocalSectionIndex.first][clients[c_id].myLocalSectionIndex.second + 1].players;
				}
				for (auto& id : findNearLocal) {
					if (clients[id]._state != ST_INGAME) continue;
					if (clients[id]._id == c_id || !isPc(clients[id]._id)) continue;
					if (!isPc(id)) {
						if (!clients[id].myLua->GetArrive())
							continue;
					}
					if (can_see(c_id, id)) {
						updateSession._vl.lock();
						updateSession._view_list.insert(id);
						updateSession._vl.unlock();
					}
				}
			}
		}
	}
}

void InitializeNPC()
{
	cout << "NPC intialize begin.\n";
	for (int i = MAX_USER; i < MAX_USER + 3; ++i) {
		clients[i]._id = i;
		clients[i].myLua = new LUA_OBJECT(clients[i]._id, "lua_script/boss.lua");
		clients[i].maxHp = 5000;
		clients[i].hp = 5000;
		clients[i].level = 10;
		clients[i].exp = 250;
		clients[i].attackDamage = 1000;

		clients[i]._state = ST_INGAME;
		std::memcpy(clients[i]._name, "boss", 4);
		clients[i].myLocalSectionIndex = make_pair(clients[i].x / 20, clients[i].y / 20);
		gameMap[clients[i].myLocalSectionIndex.first][clients[i].myLocalSectionIndex.second].InsertPlayers(clients[i]);
		clients[i]._state = ST_INGAME;
	}
	for (int i = MAX_USER + 3; i < MAX_USER + MAX_NPC / 2; ++i) {
		clients[i]._id = i;
		clients[i]._state = ST_INGAME;
		clients[i].myLua = new LUA_OBJECT(clients[i]._id, NPC_TYPE::AGRO);
		clients[i].maxHp = 250;
		clients[i].hp = 250;
		clients[i].level = 4;
		clients[i].exp = 70;
		clients[i].attackDamage = 70;
		string name = "AGRO";
		name.append(std::to_string(i));
		std::memcpy(clients[i]._name, name.c_str(), name.size());
		clients[i].myLocalSectionIndex = make_pair(clients[i].x / 20, clients[i].y / 20);
		gameMap[clients[i].myLocalSectionIndex.first][clients[i].myLocalSectionIndex.second].InsertPlayers(clients[i]);
	}
	for (int i = MAX_USER + MAX_NPC / 2; i < MAX_USER + MAX_NPC; ++i) {
		clients[i]._id = i;
		string name = "PEACE";
		clients[i].maxHp = 600;
		clients[i].hp = 600;
		clients[i].level = 2;
		clients[i].exp = 120;
		clients[i].attackDamage = 120;
		name.append(std::to_string(i));
		std::memcpy(clients[i]._name, name.c_str(), name.size());
		clients[i].myLua = new LUA_OBJECT(clients[i]._id, NPC_TYPE::PEACE);
		//clients[i].myLua = new LUA_OBJECT(clients[i]._id, NPC_TYPE::AGRO);
		clients[i].myLocalSectionIndex = make_pair(clients[i].x / 20, clients[i].y / 20);
		gameMap[clients[i].myLocalSectionIndex.first][clients[i].myLocalSectionIndex.second].InsertPlayers(clients[i]);
		clients[i]._state = ST_INGAME;
	}
	cout << "NPC initialize end.\n";
	TIMER_EVENT ev{ 1, chrono::system_clock::now() + 300s, EV_AUTO_SAVE, 0 };//5분 마다 오토 세이브 시작
	eventTimerQueue.push(ev);
}

void WakeUpNPC(int npcId, int waker)
{
	if (clients[npcId].myLua->ActiveNPC() && clients[npcId].myLua->GetArrive()) {
		//cout << "wakeUp npc: " << npcId << endl;
		TIMER_EVENT ev{ npcId, chrono::system_clock::now() + 1s, EV_RANDOM_MOVE, 0 };
		eventTimerQueue.push(ev);
	}
}

void MoveRandNPC(int npcId)
{
	//cout << "npc move" << endl;
	/*if (clients[npcId].myLua->type == NPC_TYPE::AGRO) {
		for (auto& vlIndex : clients[npcId]._view_list) {
			if (Agro_NPC(npcId, vlIndex)) {
				if (clients[npcId].myLua->ActiveChase()) {
					clients[npcId].myLua->SetChaseId(vlIndex);
					EXP_OVER* expOver = new EXP_OVER();
					expOver->_comp_type = OP_NPC_CHASE_MOVE;
					PostQueuedCompletionStatus(g_iocpHandle, 1, npcId, &expOver->_over);
					cout << "AGRO NPC: " << npcId << endl;
				}
				return false;
			}
		}
	}*/
	if (clients[npcId].myLua->InActiveChase())
		clients[npcId].myLua->SetChaseId(-1);

	int x = clients[npcId].x;
	int y = clients[npcId].y;
	switch (npcRandDirUid(npcDre)) {
	case 0: if (x < (W_WIDTH - 1)) x++; break;
	case 1: if (x > 0) x--; break;
	case 2: if (y < (W_HEIGHT - 1)) y++; break;
	case 3:if (y > 0) y--; break;
	}

	//map Object Collision
	if (gameMap[clients[npcId].myLocalSectionIndex.first][clients[npcId].myLocalSectionIndex.second].CollisionObject(x, y)) {
		x = clients[npcId].x;
		y = clients[npcId].y;
	}
	//if (clients[npcId].x == x && clients[npcId].y == y) return true;// 안움직인거.
	clients[npcId].x = x;
	clients[npcId].y = y;

	clients[npcId]._vl.lock();
	unordered_set<int> old_vlist = clients[npcId]._view_list;
	clients[npcId]._vl.unlock();

	gameMap[clients[npcId].myLocalSectionIndex.first][clients[npcId].myLocalSectionIndex.second].UpdatePlayers(clients[npcId], gameMap);//현재 로컬 최신화
	UpdateNearList(clients[npcId], npcId);

	//npc not send
	//clients[npcId].send_move_packet(npcId, clients);
	//if (clients[npcId]._view_list.size() < 1)return true;
	for (auto& pl : clients[npcId]._view_list) {
		auto& cpl = clients[pl];
		cpl._vl.lock();
		if (cpl._view_list.count(npcId) > 0) {
			cpl._vl.unlock();
			if (isPc(pl))
				cpl.send_move_packet(npcId, clients);
		}
		else {
			cpl._vl.unlock();
			if (isPc(pl))
				clients[pl].send_add_player_packet(npcId, clients);
		}
	}

	for (auto& pl : old_vlist)
		if (0 == clients[npcId]._view_list.count(pl))
			if (isPc(pl))
				clients[pl].send_remove_player_packet(npcId);
	//return true;
}

bool Agro_NPC(int npcId, int cId)
{
	if (abs(clients[npcId].x - clients[cId].x) > AGRO_RANGE)
		return false;
	if (abs(clients[npcId].y - clients[cId].y) > AGRO_RANGE)
		return false;
	return true;
}

bool AbleAttack_NPC(int npcId, int cId)
{
	if (abs(clients[npcId].x - clients[cId].x) > 3) return false;
	if (abs(clients[npcId].y - clients[cId].y) > 3) return false;
	return true;
}

void TimerWorkerThread()
{
	while (true) {
		TIMER_EVENT ev;
		auto current_time = chrono::system_clock::now();
		if (true == eventTimerQueue.try_pop(ev)) {
			if (ev.wakeupTime > current_time) {
				eventTimerQueue.push(ev);		// 최적화 필요
				// timer_queue에 다시 넣지 않고 처리해야 한다.
				this_thread::sleep_for(1ms);  // 실행시간이 아직 안되었으므로 잠시 대기
				continue;
			}
			switch (ev.eventId) {
			case EV_RANDOM_MOVE:
			{
				EXP_OVER* ov = new EXP_OVER;
				ov->_comp_type = OP_NPC_MOVE;
				PostQueuedCompletionStatus(g_iocpHandle, 1, ev.objId, &ov->_over);
			}
			break;
			case EV_CHASE_MOVE:
			{
				EXP_OVER* ov = new EXP_OVER;
				ov->_comp_type = OP_NPC_CHASE_MOVE;
				PostQueuedCompletionStatus(g_iocpHandle, 1, ev.objId, &ov->_over);
			}
			break;
			case EV_PLAYER_ATTACK_COOL:
			{
				SC_ATTACK_COOL_PACKET packet;
				packet.size = sizeof(SC_ATTACK_COOL_PACKET);
				packet.type = SC_ATTACK_COOL;
				clients[ev.objId].do_send(&packet);
				clients[ev.objId].SetAbleAttack(true);
			}
			break;
			case EV_RESPAWN_NPC:
			{
				clients[ev.objId].myLua->RespawnNpc();
			}
			break;
			case EV_AUTO_SAVE:
			{
				EXP_OVER* ov = new EXP_OVER;
				ov->_comp_type = OP_DB_AUTO_SAVE_PLAYER;
				PostQueuedCompletionStatus(g_iocpHandle, 1, ev.objId, &ov->_over);
			}
			break;
			default: break;
			}
			continue;		// 즉시 다음 작업 꺼내기
		}
		this_thread::sleep_for(1ms);   // timer_queue가 비어 있으니 잠시 기다렸다가 다시 시작
	}
}

#include "stdafx.h"
#include "DeprecatedRoom.h"





//DeprecatedRoom::DeprecatedRoom(std::vector<std::shared_ptr<UserSession>>& userRefVec, std::shared_ptr<MonsterMapData>& mapDataRef, std::shared_ptr<NavMapData>& navMapDataRef)
//	: m_updateCnt(0), m_roomState(ROOM_STATE::ROOM_COMMON), m_isContinueHeal(false)
//	, m_stageMapData(mapDataRef), m_bossMapData(navMapDataRef), prevUpdateTime(std::chrono::high_resolution_clock::now())
//{
//	std::lock_guard<std::shared_mutex> userLockGuard(m_userSessionsLock);
//	for (auto& userRef : userRefVec) {
//		if (nullptr != userRef)
//			m_userSessions.insert(userRef);
//	}
//}
//
//DeprecatedRoom::DeprecatedRoom(std::shared_ptr<UserSession>& userRef, std::shared_ptr<MonsterMapData>& mapDataRef, std::shared_ptr<NavMapData>& navMapDataRef)
//	: m_updateCnt(0), m_roomState(ROOM_STATE::ROOM_COMMON)
//	, m_stageMapData(mapDataRef), m_bossMapData(navMapDataRef)
//{
//	spdlog::warn("DeprecatedRoom::DeprecatedRoom() - make DeprecatedRoom for Alone Test");
//
//	std::lock_guard<std::shared_mutex> userLockGuard(m_userSessionsLock);
//	m_userSessions.insert(userRef);
//}
//
//DeprecatedRoom::~DeprecatedRoom()
//{
//}
//
//void DeprecatedRoom::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
//{
//	static constexpr int MAX_ROOM_TICK = 50;
//	static std::atomic_bool isPrint = false;
//	if (ROOM_STATE::ROOM_END == m_roomState) {
//		IocpEventManager::GetInstance().DeleteExpOver(over);
//		return;
//	}
//	using namespace std::chrono;
//	const auto& opCode = over->GetOpCode();
//	switch (opCode)
//	{
//		//DeprecatedRoom - update
//	case IOCP_OP_CODE::OP_ROOM_UPDATE:
//	{
//		if (ROOM_STATE::ROOM_END == m_roomState) return;
//
//		ProcessPrevUpdateEvent();
//		//auto currentTime = std::chrono::high_resolution_clock::now();
//		Update();
//
//		//InsertTimerEvent(TIMER_EVENT_TYPE::EV_SEND_NPC_MOVE, 1ms);
//		//타이머에 우선 넣고 이후 Update()에서 나온 Send이벤트 처리
//		InsertTimerEvent(TIMER_EVENT_TYPE::EV_ROOM_UPDATE, 30ms);
//		//auto updateEndTime = std::chrono::high_resolution_clock::now();
//
//		//auto UpdateTimeDuration = std::chrono::duration_cast<milliseconds>(updateEndTime - currentTime).count();
//		//if (UpdateTimeDuration > 20) {
//		//	spdlog::warn("DeprecatedRoom Update Time: {}ms", UpdateTimeDuration);
//		//}
//
//		//auto diffPrevUpdateTime = std::chrono::duration_cast<milliseconds>(currentTime - prevUpdateTime).count();
//		//if (diffPrevUpdateTime > MAX_ROOM_TICK) {
//		//	if (!isPrint) {
//		//		int roomCnt = RoomManager::GetInstance().globalRoomCnt;
//		//		spdlog::warn("DeprecatedRoom Update Tick: {}ms, DeprecatedRoom Cnt: {}", diffPrevUpdateTime, roomCnt);
//		//		isPrint = true;
//		//	}
//		//	//spdlog::warn("DeprecatedRoom Update Tick: {}ms", diffPrevUpdateTime);
//		//}
//		//prevUpdateTime = updateEndTime;
//		ProcessAfterUpdateSendEvent();
//	}
//	break;
//	case IOCP_OP_CODE::OP_SEND_NPC_MOVE:
//	{
//
//	}
//	break;
//	case IOCP_OP_CODE::OP_GAME_STATE_SEND:
//	{
//		GameStateSend();
//		using namespace std::chrono;
//		InsertTimerEvent(TIMER_EVENT_TYPE::EV_SEND_GAME_STATE, 50ms);
//	}
//	break;
//
//	//DeprecatedRoom - player skill
//	case IOCP_OP_CODE::OP_PLAYER_HEAL_START:
//	{
//		m_isContinueHeal = true;
//		auto healStartPacket = std::make_shared<SERVER_PACKET::NotifyPacket>(static_cast<char>(SERVER_PACKET::TYPE::HEAL_START));
//		BroadCastPacket(healStartPacket);
//		PlayerHeal();
//	}
//	break;
//	case IOCP_OP_CODE::OP_PLAYER_HEAL:
//	{
//		PlayerHeal();
//	}
//	break;
//	case IOCP_OP_CODE::OP_PLAYER_HEAL_END:
//	{
//		auto healEndPacket = std::make_shared<SERVER_PACKET::NotifyPacket>(static_cast<char>(SERVER_PACKET::TYPE::HEAL_END));
//		BroadCastPacket(healEndPacket);
//		m_isContinueHeal = false;
//	}
//	break;
//	case IOCP_OP_CODE::OP_PLAYER_APPLY_SIELD:
//	{
//		PlayerApplyShield();
//	}
//	break;
//	case IOCP_OP_CODE::OP_PLAYER_REMOVE_SIELD:
//	{
//		PlayerRemoveShield();
//	}
//	break;
//	case IOCP_OP_CODE::OP_RAIN_ARROW_ATTACK:
//	{
//		RainArrowAttack();
//	}
//	break;
//	default:
//	{
//		spdlog::warn("DeprecatedRoom::Execute() - Invalid OP_CODE: {}", static_cast<int>(opCode));
//	}
//	break;
//	}
//	IocpEventManager::GetInstance().DeleteExpOver(over);
//}
//
//void DeprecatedRoom::Fail(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
//{
//	IocpEventManager::GetInstance().DeleteExpOver(over);
//}
//
//void DeprecatedRoom::Start()
//{
//	auto userSessions = GetAllUserSessions();
//	for (auto& user : userSessions) {
//		std::shared_ptr< SERVER_PACKET::IntoGamePacket> sendPacket = std::make_shared< SERVER_PACKET::IntoGamePacket>();
//		sendPacket;
//		for (int i = 0; i < 15; ++i) {
//			sendPacket->monsterData[i].id = i;
//			sendPacket->monsterData[i].position = m_smallMonsters[i]->GetPosition();
//			sendPacket->monsterData[i].lookVector = m_smallMonsters[i]->GetLookVector();
//			sendPacket->monsterData[i].maxHp = m_smallMonsters[i]->GetMaxHp();
//		}
//
//		sendPacket->role = user->GetIngameRole();
//		user->DoSend(sendPacket);
//	}
//
//	using namespace std::chrono;
//	InsertTimerEvent(TIMER_EVENT_TYPE::EV_ROOM_UPDATE, 20ms);
//	InsertTimerEvent(TIMER_EVENT_TYPE::EV_SEND_GAME_STATE, 50ms);
//}
//
//std::vector<std::shared_ptr<SmallMonsterObject>>& DeprecatedRoom::GetSmallMonsters()
//{
//	return m_smallMonsters;
//}
//
//std::vector<std::shared_ptr<CharacterObject>> DeprecatedRoom::GetCharacters() const
//{
//	std::vector<std::shared_ptr<CharacterObject>> characters;
//	characters.reserve(4);
//	for (auto& character : m_characters)
//		characters.push_back(character.second);
//	return characters;
//}
//
//std::vector<std::shared_ptr<LiveObject>> DeprecatedRoom::GetLiveObjects() const
//{
//	std::vector<std::shared_ptr<LiveObject>> liveObjects;
//	liveObjects.reserve(15);
//	for (auto& character : m_characters)
//		liveObjects.push_back(std::static_pointer_cast<LiveObject>(character.second->shared_from_this()));
//	if (ROOM_STATE::ROOM_COMMON == m_roomState) {
//		for (auto& monster : m_smallMonsters)
//			liveObjects.push_back(std::static_pointer_cast<LiveObject>(monster->shared_from_this()));
//	}
//	else {
//		liveObjects.push_back(std::static_pointer_cast<LiveObject>(m_bossMonster->shared_from_this()));
//		//boss Insert
//	}
//	return liveObjects;
//}
//
//std::vector<ROLE> DeprecatedRoom::GetConnectedUserRoles()
//{
//	auto userSessions = GetAllUserSessions();
//	std::vector<ROLE> roles;
//	roles.reserve(userSessions.size());
//	for (const auto& userSession : userSessions) {
//		roles.push_back(userSession->GetIngameRole());
//	}
//	return roles;
//}
//
//std::vector<ROLE> DeprecatedRoom::GetLiveRoles()
//{
//	auto connectedUserRoles = GetConnectedUserRoles();
//	std::vector<ROLE> liveRoles;
//	liveRoles.reserve(connectedUserRoles.size());
//	for (const auto& role : connectedUserRoles) {
//		if (!m_characters[role]->IsAlive()) continue;
//		liveRoles.push_back(role);
//	}
//
//	return liveRoles;
//}
//
//void DeprecatedRoom::InsertProjectileObject(std::shared_ptr<ProjectileObject> projectileObject)
//{
//	//prevUpdateEvent는 Update()내부에서 싱글하게 돌아감.
//	m_projectileObjects.push_back(projectileObject);
//}
//
//void DeprecatedRoom::UpdateProjectileObject()
//{
//	for (auto projectileIter = m_projectileObjects.begin(); projectileIter != m_projectileObjects.end();) {
//		(*projectileIter)->Update();
//		const bool isDestroyTime = (*projectileIter)->IsDestroy();
//		if (isDestroyTime) {
//			projectileIter = m_projectileObjects.erase(projectileIter);
//			continue;
//		}
//		++projectileIter;
//	}
//}
//
//std::shared_ptr<MapData> DeprecatedRoom::GetMapData() const
//{
//	if (ROOM_STATE::ROOM_COMMON == m_roomState) {
//		return m_stageMapData;
//	}
//	return m_bossMapData;
//}
//
//std::shared_ptr<NavMapData> DeprecatedRoom::GetBossMapData() const
//{
//	return m_bossMapData;
//}
//
//void DeprecatedRoom::InsertAftrerUpdateSendEvent(std::shared_ptr<RoomSendEvent> roomEvent)
//{
//	m_afterUpdateSendEvent.push(roomEvent);
//	++m_afterUpdateEventCnt;
//}
//
//void DeprecatedRoom::InsertPrevUpdateEvent(std::shared_ptr<PrevUpdateEvent> prevUpdate)
//{
//	m_prevUpdateEvent.push(prevUpdate);
//	++m_prevUpdateEventCnt;
//}
//
//void DeprecatedRoom::ProcessAfterUpdateSendEvent()
//{
//	int prccessRoomEventCnt = m_afterUpdateEventCnt;
//	m_afterUpdateEventCnt -= prccessRoomEventCnt;
//	for (int i = 0; i < prccessRoomEventCnt; ++i) {
//		std::shared_ptr<RoomSendEvent> currentEvent = nullptr;
//		bool isSuccess = m_afterUpdateSendEvent.try_pop(currentEvent);
//		if (isSuccess) {
//			auto sendPacket = currentEvent->GetPacketHeader();
//			BroadCastPacket(sendPacket);
//		}
//		else {
//			m_afterUpdateEventCnt += prccessRoomEventCnt;
//			return;
//		}
//	}
//}
//
//void DeprecatedRoom::ProcessPrevUpdateEvent()
//{
//	int prccessPrevEventCnt = m_prevUpdateEventCnt;
//	m_prevUpdateEventCnt -= prccessPrevEventCnt;
//	for (int i = 0; i < prccessPrevEventCnt; ++i) {
//		std::shared_ptr<PrevUpdateEvent> currentEvent = nullptr;
//		bool isSuccess = m_prevUpdateEvent.try_pop(currentEvent);
//		if (isSuccess) {
//			currentEvent->ProcessEvent();
//		}
//		else {
//			m_prevUpdateEventCnt += prccessPrevEventCnt;
//			return;
//		}
//	}
//}
//
//void DeprecatedRoom::InsertTimerEvent(const TIMER_EVENT_TYPE& eventType, const std::chrono::milliseconds& updateTick)
//{
//	auto timerEvent = std::make_shared<TIMER::RoomEvent>(eventType, updateTick, std::static_pointer_cast<DeprecatedRoom>(shared_from_this()));
//	TIMER::Timer::GetInstance().InsertTimerEvent(std::static_pointer_cast<TIMER::EventBase>(timerEvent));
//}
//
//void DeprecatedRoom::InserTimerEvent(std::shared_ptr<TIMER::EventBase> timerEvent)
//{
//	TIMER::Timer::GetInstance().InsertTimerEvent(timerEvent);
//}
//
//void DeprecatedRoom::SetBossRoad(std::shared_ptr<std::list<XMFLOAT3>> road)
//{
//	m_bossMonster->UpdateRoad(road);
//}
//
//void DeprecatedRoom::SetBossAggro(std::shared_ptr<std::list<XMFLOAT3>> road, std::shared_ptr<CharacterObject> characteRef)
//{
//	m_bossMonster->UpdateAggro(characteRef, road);
//}
//
//void DeprecatedRoom::ForceGameEnd()
//{
//	if (ROOM_STATE::ROOM_END != m_roomState)
//		SetRoomEndState();
//}
//
//void DeprecatedRoom::DisconnectUser(std::shared_ptr<UserSession> disconnectedUserRef)
//{
//	std::lock_guard<std::shared_mutex> sharedLockGuard{ m_userSessionsLock };
//	m_userSessions.erase(disconnectedUserRef);
//	if (m_userSessions.size() == 0) {
//		RoomManager::GetInstance().EraseRoom(std::static_pointer_cast<DeprecatedRoom>(shared_from_this()));
//	}
//}
//
//bool DeprecatedRoom::ReconnectUser(std::shared_ptr<UserSession> reconnectUserRef)
//{
//	if (ROOM_STATE::ROOM_END == m_roomState) return false;
//	std::lock_guard<std::shared_mutex> sharedLockGuard{ m_userSessionsLock };
//	m_userSessions.insert(reconnectUserRef);
//	return true;
//}
//
//void DeprecatedRoom::Update()
//{
//	static constexpr int GAME_STATE_SEND_CNT = 3;
//
//	auto ableRole = GetLiveRoles();
//	if (ableRole.size() == 0) {
//		SetRoomEndState();
//		return;
//	}
//	//Update GameObject
//	int aliveCharacterCnt = 0;
//	for (auto& character : m_characters) {
//		character.second->Update();
//		if (character.second->IsAlive())
//			++aliveCharacterCnt;
//	}
//
//	if (aliveCharacterCnt == 0) {//모든 플레이어가 죽어도 게임 종료
//		SetRoomEndState();
//		return;
//	}
//
//	if (ROOM_STATE::ROOM_COMMON == m_roomState) {
//		for (auto& smallMonster : m_smallMonsters)
//			smallMonster->Update();
//	}
//	else if (ROOM_STATE::ROOM_BOSS == m_roomState) {
//		if (m_bossStartTime < std::chrono::high_resolution_clock::now())
//			m_bossMonster->Update();
//	}
//
//	//투사체 업데이트
//	UpdateProjectileObject();
//
//	if (!m_bossMonster->IsAlive()) {//게임 종료 조건
//		SetRoomEndState();
//		return;
//	}
//}
//
//void DeprecatedRoom::GameStateSend()
//{
//	//현재 보낼 게임상태에 적용된 룸 상태와 현재 룸 상태가 다르다면 보내지 않음.
//
//	//if (m_roomState != m_applyRoomStateForGameState) return;
//	if (ROOM_STATE::ROOM_COMMON == m_roomState) {
//		SendGameStateStage();
//		return;
//	}
//	SendGameStateBoss();
//}
//
//std::vector<std::shared_ptr<UserSession>> DeprecatedRoom::GetAllUserSessions()
//{
//	std::vector<std::shared_ptr<UserSession>> userSessions;
//	userSessions.reserve(4);
//	std::shared_lock<std::shared_mutex> userLockGuard(m_userSessionsLock);
//	for (auto& userSession : m_userSessions) {
//		userSessions.push_back(userSession);
//	}
//	return userSessions;
//}
//
//void DeprecatedRoom::SendGameStateStage()
//{
//	auto gameStatePacket = std::make_shared<SERVER_PACKET::GameState_STAGE>();
//	int characterIdx = 0;
//	for (auto& character : m_characters) {
//		gameStatePacket->userState[characterIdx].role = character.first;
//		gameStatePacket->userState[characterIdx].hp = character.second->GetHp();
//		gameStatePacket->userState[characterIdx].position = character.second->GetPosition();
//		gameStatePacket->userState[characterIdx].resetShield = character.second->GetShield();
//		gameStatePacket->userState[characterIdx].time = character.second->GetLastUpdateTime();
//		++characterIdx;
//	}
//	for (auto& monster : m_smallMonsters) {
//		gameStatePacket->smallMonster[monster->GetIdx()].isAlive = monster->IsAlive();
//		gameStatePacket->smallMonster[monster->GetIdx()].idx = monster->GetIdx();
//		gameStatePacket->smallMonster[monster->GetIdx()].hp = monster->GetHp();
//		gameStatePacket->smallMonster[monster->GetIdx()].position = monster->GetPosition();
//		gameStatePacket->smallMonster[monster->GetIdx()].time = monster->GetLastUpdateTime();
//	}
//	BroadCastPacket(gameStatePacket);
//}
//
//void DeprecatedRoom::SendGameStateBoss()
//{
//	auto gameStatePacket = std::make_shared<SERVER_PACKET::GameState_BOSS>();
//	int characterIdx = 0;
//	for (auto& character : m_characters) {
//		gameStatePacket->userState[characterIdx].role = character.first;
//		gameStatePacket->userState[characterIdx].hp = character.second->GetHp();
//		gameStatePacket->userState[characterIdx].position = character.second->GetPosition();
//		gameStatePacket->userState[characterIdx].resetShield = character.second->GetShield();
//		gameStatePacket->userState[characterIdx].time = character.second->GetLastUpdateTime();
//		++characterIdx;
//	}
//	gameStatePacket->bossState.hp = m_bossMonster->GetHp();
//	gameStatePacket->bossState.position = m_bossMonster->GetPosition();
//	gameStatePacket->bossState.time = m_bossMonster->GetLastUpdateTime();
//	BroadCastPacket(gameStatePacket);
//}
//
//void DeprecatedRoom::BroadCastPacket(std::shared_ptr<PacketHeader> packetData)
//{
//	auto userSessions = GetAllUserSessions();
//	for (auto& userSession : userSessions) {
//		userSession->DoSend(packetData);
//	}
//}
//
//void DeprecatedRoom::MultiCastCastPacket(std::shared_ptr<PacketHeader> packetData, const ROLE& exclusiveRoles)
//{
//	auto userSessions = GetAllUserSessions();
//	for (auto& userSession : userSessions) {
//		if (userSession->GetIngameRole() != exclusiveRoles)
//		{
//			userSession->DoSend(packetData);
//		}
//	}
//}
//
//void DeprecatedRoom::MultiCastCastPacket(std::shared_ptr<PacketHeader> packetData, const std::vector<ROLE>& exclusiveRoles)
//{
//	auto userSessions = GetAllUserSessions();
//	for (auto& userSession : userSessions) {
//		if (exclusiveRoles.end() != std::find(exclusiveRoles.begin(), exclusiveRoles.end(), userSession->GetIngameRole()))
//		{
//			userSession->DoSend(packetData);
//		}
//	}
//}
//
//std::shared_ptr<CharacterObject> DeprecatedRoom::GetCharacterObject(const ROLE& role)
//{
//	if (!m_characters.count(role)) {
//		spdlog::warn("std::shared_ptr<CharacterObject> DeprecatedRoom::GetCharacterObject(const ROLE& role) - Invalid ROLE");
//		return nullptr;
//	}
//	return m_characters[role];
//}
//
//std::vector<std::shared_ptr<MonsterObject>> DeprecatedRoom::GetEnermyData()
//{
//	if (ROOM_STATE::ROOM_COMMON == m_roomState) {
//		std::vector<std::shared_ptr<MonsterObject>> monsterObjects;
//		monsterObjects.reserve(15);
//		for (auto& monster : m_smallMonsters)
//			monsterObjects.push_back(monster);
//		return monsterObjects;
//	}
//	else {
//		return std::vector<std::shared_ptr<MonsterObject>>(1, m_bossMonster);
//	}
//}
//
//void DeprecatedRoom::InitializeAllGameObject()
//{
//	static constexpr float SMALL_MONSTER_HP = 250.0f;
//	static constexpr float BOSS_HP = 2500.0f;
//
//	std::vector<std::chrono::seconds> duTime;
//	std::vector<std::chrono::seconds> coolTime;
//	//Character Initialize
//	m_characters.emplace(ROLE::WARRIOR, std::make_shared<WarriorObject>(600.0f, 50.0f, 8.0f, std::static_pointer_cast<DeprecatedRoom>(shared_from_this())));
//	m_characters.emplace(ROLE::TANKER, std::make_shared<TankerObject>(780.0f, 50.0f, 8.0f, std::static_pointer_cast<DeprecatedRoom>(shared_from_this())));
//	m_characters.emplace(ROLE::ARCHER, std::make_shared<ArcherObject>(400.0f, 50.0f, 8.0f, std::static_pointer_cast<DeprecatedRoom>(shared_from_this())));
//	m_characters.emplace(ROLE::MAGE, std::make_shared<MageObject>(500.0f, 50.0f, 8.0f, std::static_pointer_cast<DeprecatedRoom>(shared_from_this())));
//
//	auto monsterInitData = m_stageMapData->GetMonsterInitData();
//	for (int i = 0; i < monsterInitData.size(); ++i) {
//		auto monster = std::make_shared<SmallMonsterObject>(SMALL_MONSTER_HP, 50.0f, 8.0f, std::static_pointer_cast<DeprecatedRoom>(shared_from_this()), i);
//		monster->SetPosition(monsterInitData[i].position);
//		monster->Rotate(ROTATE_AXIS::Y, monsterInitData[i].eulerRotate.y);
//		monster->Rotate(ROTATE_AXIS::X, monsterInitData[i].eulerRotate.x);
//		monster->Rotate(ROTATE_AXIS::Z, monsterInitData[i].eulerRotate.z);
//		m_smallMonsters.push_back(monster);
//	}
//
//	m_bossMonster = std::make_shared<BossMonsterObject>(2500.0f, 60.0f, 30.0f, std::static_pointer_cast<DeprecatedRoom>(shared_from_this()));
//	m_bossMonster->Initialize();
//	//ProjectileObject Initialize
//	for (auto& character : m_characters) {
//		character.second->SetStagePosition(m_roomState);
//	}
//}
//
//void DeprecatedRoom::SetBossStage()
//{
//	static constexpr std::chrono::seconds BOSS_START_AFTER_TIME = std::chrono::seconds(11);
//	if (ROOM_STATE::ROOM_COMMON != m_roomState) return;
//	m_roomState = ROOM_STATE::ROOM_BOSS;
//	m_bossStartTime = std::chrono::high_resolution_clock::now() + BOSS_START_AFTER_TIME;
//
//	auto stageBossPacket = std::make_shared<SERVER_PACKET::BossStageInitPacket>();
//	int characterCnt = 0;
//	for (auto& character : m_characters) {
//		character.second->ForceStopMove();
//		character.second->SetStagePosition(ROOM_STATE::ROOM_BOSS);
//		character.second->ResetSkillCoolTime();
//		character.second->SetFullHp();
//		stageBossPacket->userState[characterCnt].role = character.first;
//		stageBossPacket->userState[characterCnt].hp = character.second->GetHp();
//		stageBossPacket->userState[characterCnt].position = character.second->GetPosition();
//		stageBossPacket->userState[characterCnt].resetShield = character.second->GetShield();
//		++characterCnt;
//	}
//	stageBossPacket->bossPosition = m_bossMonster->GetPosition();
//	stageBossPacket->bossLookVector = m_bossMonster->GetLookVector();
//	stageBossPacket->bossHp = m_bossMonster->GetMaxHp();
//	BroadCastPacket(std::static_pointer_cast<PacketHeader>(stageBossPacket));
//}
//
//void DeprecatedRoom::PlayerHeal()
//{
//	static constexpr float HEAL_HP = 75.0f;
//	if (!m_isContinueHeal)return;
//	auto healPacket = std::make_shared<SERVER_PACKET::NotifyHealPacket>(static_cast<char>(SERVER_PACKET::TYPE::NOTIFY_HEAL_HP));
//	int i = 0;
//	for (auto& character : m_characters) {
//		character.second->Heal(HEAL_HP);
//		healPacket->applyHealPlayerInfo[i].role = character.first;
//		healPacket->applyHealPlayerInfo[i].hp = character.second->GetHp();
//		++i;
//	}
//	BroadCastPacket(std::static_pointer_cast<PacketHeader>(healPacket));
//	InsertTimerEvent(TIMER_EVENT_TYPE::EV_HEAL, std::chrono::milliseconds(1000));
//}
//
//void DeprecatedRoom::PlayerApplyShield()
//{
//	for (auto& character : m_characters)
//		character.second->SetShield(true);
//	auto shieldPacket = std::make_shared<SERVER_PACKET::NotifyPacket>(static_cast<char>(SERVER_PACKET::TYPE::SHIELD_START));
//	BroadCastPacket(shieldPacket);
//
//}
//
//void DeprecatedRoom::PlayerRemoveShield()
//{
//	for (auto& character : m_characters)
//		character.second->SetShield(false);
//	auto shieldPacket = std::make_shared<SERVER_PACKET::NotifyPacket>(static_cast<char>(SERVER_PACKET::TYPE::SHIELD_END));
//	BroadCastPacket(shieldPacket);
//}
//
//void DeprecatedRoom::RainArrowAttack()
//{
//	//UpdateAfterEvent는 어차피 concurrent라 문제 없음
//	std::static_pointer_cast<ArcherObject>(m_characters[ROLE::ARCHER])->AttackRainArrow();
//}
//
//void DeprecatedRoom::SetRoomEndState()
//{
//	m_roomState = ROOM_STATE::ROOM_END;
//	auto gameEndPacket = std::make_shared<SERVER_PACKET::NotifyPacket>(static_cast<char>(SERVER_PACKET::TYPE::GAME_END));
//	BroadCastPacket(gameEndPacket);
//}

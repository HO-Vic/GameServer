#pragma once
#include "../PCH/stdafx.h"
#include "NavMeshQuadTree/NavMeshQuadTree.h"
using namespace DirectX;

namespace DreamWorld {
class MapCollision;
class MapData {
 public:
  MapData() = delete;
  MapData(const std::string& mapCollisionFile);

  const std::vector<std::shared_ptr<MapCollision>>& GetCollisionData() const;

 protected:
  // collision
  std::vector<std::shared_ptr<MapCollision>> m_collisionDatas;
};

struct MonsterInitData {
  XMFLOAT3 position;
  XMFLOAT3 eulerRotate;
  MonsterInitData(const XMFLOAT3& pos, const XMFLOAT3& rot) {
    position = pos;
    eulerRotate = rot;
  }
};

class MonsterMapData : public MapData {
 public:
  MonsterMapData(const std::string& mapCollisionFile, const std::string& initMonsterFile);
  MonsterMapData() = delete;

  const std::vector<MonsterInitData>& GetMonsterInitData() const;

 protected:
  std::vector<MonsterInitData> m_monsterInitData;
};

namespace NavMesh {
class TriangleNavMesh;
}
class NavMapData : public MapData {
 public:
  NavMapData() = delete;
  NavMapData(const std::string& mapCollisionFile, const std::string& navMeshFIle);
  ~NavMapData();

  const std::shared_ptr<NavMesh::TriangleNavMesh> GetBossStartPosition() const;

  std::shared_ptr<NavMesh::TriangleNavMesh> GetTriangleMesh(const int& idx) const {
    return m_triangleMesh[idx];
  }

  // 머리가 시작, 꼬리가 끝
  std::list<XMFLOAT3> GetAstarNode(const XMFLOAT3& startPosition, const XMFLOAT3& destinationPosition);
  // 길찾기 확인이 잘 안돼서 클라이언트에서 네비 메시 렌더링을 위한 객체
  std::list<XMFLOAT3> GetAstarNode_TestForClient(const XMFLOAT3& startPosition, const XMFLOAT3& destinationPosition, std::shared_ptr<std::list<int>> indexList);

 private:
  std::list<XMFLOAT3> OptimizeAStar(const XMFLOAT3& startPosition, const XMFLOAT3& destinationPosition, std::list<std::shared_ptr<NavMesh::TriangleNavMesh>>& confirmedAstarNode);
  void CreatePortal(std::list<std::shared_ptr<NavMesh::TriangleNavMesh>>& confirmedAstarNode, std::vector<XMFLOAT3>& leftPortal, std::vector<XMFLOAT3>& rightPortal);

  XMFLOAT3 GetInternalDivisionPosition(const XMFLOAT3& vertex1, const XMFLOAT3& vertex2, const float& ratio1, const float& ratio2);

 protected:
  // Nav Data
  // read Data
  std::vector<XMFLOAT3> m_vertex;
  std::vector<int> m_index;
  // GameData
  std::vector<std::shared_ptr<NavMesh::TriangleNavMesh>> m_triangleMesh;
  std::vector<int> m_zeroVertexIdxs;

  NavMesh::QuadTree m_navMeshQuadTree;
  float tiangleMeshGridMinSize = 10.0f;
  std::shared_ptr<NavMesh::TriangleNavMesh> m_bossStartMesh;
};
}  // namespace DreamWorld

//========= Copyright (c) 2026 Mohamed Ashraf, All rights reserved. ============//
//
// This software is proprietary and confidential.
//
// You are granted a limited, non-exclusive, non-transferable license to use and
// modify this software for personal or internal purposes only. You may distribute
// compiled binary builds of the software provided that such distribution does not
// expose, include, or facilitate access to the source code.
//
// You may not distribute, publish, disclose, or otherwise make available the
// source code, in whole or in part, whether original or modified. All rights
// not expressly granted herein are reserved by the copyright holder.
//
// This software is provided "as is", without warranty of any kind, express or
// implied.
//
//=============================================================================//
#include "rendersystem/irendercontext.h" 
#include "tier0/platform.h"
#include "mathlib/vector.h"
#include <cstring>
#include <vector>

class CVisCluster {
public:
	AABB_t m_surroundingBounds; /* 0 24 */
	std::vector<std::pair<AABB_t, int>> m_boundsList;
	CVisCluster();
	~CVisCluster();
};

class CClusterQueryResults {
	void AddEmptyCluster(uint16);
	void ReduceToUnique();
	int Count();
	unsigned int m_nRecents[512]; /* 0 2048 */
	~CClusterQueryResults();
	CClusterQueryResults();
};

CVisCluster::~CVisCluster()
{
}

CClusterQueryResults::CClusterQueryResults()
{
} 

CClusterQueryResults::~CClusterQueryResults()
{
}

inline void CClusterQueryResults::AddEmptyCluster(uint16 nCluster)
{
}

inline void CClusterQueryResults::ReduceToUnique()
{
}

inline int CClusterQueryResults::Count()
{
}

class CVoxelVisibility {}; // TODO

struct CPVSBits {
	std::vector<unsigned char> bits;
	void Resize(size_t n) { bits.assign((n + 7) / 8, 0); }
	void Set(size_t i) { if (i / 8 < bits.size()) bits[i / 8] |= (1 << (i & 7)); }
	bool Test(size_t i) const { return i / 8 < bits.size() && (bits[i / 8] & (1 << (i & 7))); }
};

class CPVS : public IPVS
{
public:
	CPVS(const Vector& worldOffset);
	CPVS(const CPVS&) = default;
	~CPVS();

	void Init(const CVoxelVisibility* pVisibility);
	void BuildNodeBounds_r(
		int node,
		std::vector<std::pair<AABB_t, int>>& bounds,
		const AABB_t& parentBounds);

	void BuildNodeBounds(
		std::vector<std::pair<AABB_t, int>>& bounds);
	void BuildClusterDebugBounds();

	bool IsSunVisibleFromCluster(unsigned int cluster);

	void ClustersInBox(const CClusterQueryResults& results,
		const Vector& mins,
		const Vector& maxs);

	int GetClustersForFrustum(uint32* pClusters,
		int maxClusters,
		const CFrustum* pFrustum,
		bool precise);

	int GetVisibleEverywhereClusterList(uint32* pClusters,
		int maxClusters);

	int GetClustersForOrigin(uint32* pClusters,
		int maxClusters,
		const Vector& origin);

	int GetClustersForBounds(uint32* pClusters,
		int maxClusters,
		const Vector& mins,
		const Vector& maxs,
		bool precise);

	int GetClustersForOrientedBounds(uint32* pClusters,
		int maxClusters,
		const AABB_t& bounds,
		bool precise);

	int FilterClustersInRadius(uint32* pClusters,
		int clusterCount,
		const Vector& origin,
		float radius);

	int GetClusterCount();

	int GetClusterForPosition(const Vector& position);

	bool IsInPVS(const Vector& position,
		const CPVSBits* pBits);

	bool IsInPVS(int cluster,
		const Vector* pOrigin,
		const Vector& point);

	bool IsAbsBoxInPVS(int cluster,
		const Vector* pOrigin,
		const Vector& mins,
		const Vector& maxs);

	bool IsAbsBoxInPVS(const Vector& mins,
		const Vector& maxs,
		const CPVSBits* pBits);

	int GetAllClusterBounds(AABB_t* pBounds,
		int maxBounds);

	bool GetClusterBounds(int cluster,
		Vector* mins,
		Vector* maxs);

	bool IsEmptyPVS();

	bool IsSkyVisibleFromPosition(const Vector& position);
	bool IsSunVisibleFromPosition(const Vector& position);
	bool IsSunVisibleInPVS(const CPVSBits& bits);

	void ResetPVS(CPVSBits* pBits);
	void AddOriginToPVS(const Vector& origin, CPVSBits* pBits);
	void GetVisForCluster(int cluster, CPVSBits* pBits);
	void GetPrecisePVS(CPVSBits* pBits, const Vector& origin);
	void GetSunlightPVS(CPVSBits* pBits);
	void GetOrthoPVS(CPVSBits* pBits);

	const CVisCluster* GetCluster(int cluster);

private:
	int RegionAt(const Vector& position);
	int LeafAt(const Vector& position, AABB_t& leafBounds);

	void ClustersInBoxEnclosed_r(const CClusterQueryResults& results,
		uint32 node,
		const AABB_t& queryBounds,
		const AABB_t& nodeBounds);

	void ClustersInBox_r(const CClusterQueryResults& results,
		uint32 node,
		const AABB_t& queryBounds,
		const AABB_t& nodeBounds);

	const uint32* GetVisBits(int cluster);

private:
	const CVoxelVisibility* m_pVis = nullptr;

	bool m_bEnable = true;
	bool m_bIsEmpty = false;

	Vector m_vecWorldOffset;

	std::vector<std::pair<CVisCluster, int>> m_clusters;
};

static bool Intersects(const AABB_t& a, const AABB_t& b) {
	return !(a.fMax<b.fMin || a.fMin>b.fMax);
}

CPVS::CPVS(const Vector& worldOffset) :m_vecWorldOffset(worldOffset) {}
CPVS::~CPVS() {}
void CPVS::Init(const CVoxelVisibility* p) { m_pVis = p; }

void CPVS::BuildNodeBounds_r(
	int n,
	std::vector<std::pair<AABB_t, int>>& b,
	const AABB_t& p)
{
	b.emplace_back(p, n);
}

void CPVS::BuildNodeBounds(std::vector<std::pair<AABB_t, int>>& b)
{
	b.clear();

	AABB_t root{};
	BuildNodeBounds_r(0, b, root);
}

void CPVS::BuildClusterDebugBounds() {}

bool CPVS::IsSunVisibleFromCluster(unsigned int) { return true; }
void CPVS::ClustersInBox(const CClusterQueryResults&, const Vector&, const Vector&) {}
int CPVS::GetClustersForFrustum(uint32*, int, const CFrustum*, bool) { return 0; }
int CPVS::GetVisibleEverywhereClusterList(uint32* o, int m) { return 0; }
int CPVS::GetClustersForOrigin(uint32* o, int m, const Vector& v) { int c = GetClusterForPosition(v);if (c < 0 || m < 1)return 0;o[0] = c;return 1; }
int CPVS::GetClustersForBounds(uint32* o, int m, const Vector&, const Vector&, bool) { return GetVisibleEverywhereClusterList(o, m); }
int CPVS::GetClustersForOrientedBounds(uint32* o, int m, const AABB_t&, bool) { return GetVisibleEverywhereClusterList(o, m); }
int CPVS::FilterClustersInRadius(uint32*, int c, const Vector&, float) { return c; }
int CPVS::GetClusterCount() { return 10; }
int CPVS::GetClusterForPosition(const Vector&) { return 10; }
bool CPVS::IsInPVS(const Vector&, const CPVSBits* b) { return b && !b->bits.empty(); }
bool CPVS::IsInPVS(int, const Vector*, const Vector&) { return true; }
bool CPVS::IsAbsBoxInPVS(int, const Vector*, const Vector&, const Vector&) { return true; }
bool CPVS::IsAbsBoxInPVS(const Vector&, const Vector&, const CPVSBits* b) { return b && !b->bits.empty(); }
int CPVS::GetAllClusterBounds(AABB_t* p, int m) { return 0; }
bool CPVS::GetClusterBounds(int c, Vector* mi, Vector* ma) { return true; }
bool CPVS::IsEmptyPVS() { return false; }
bool CPVS::IsSkyVisibleFromPosition(const Vector&) { return true; }
bool CPVS::IsSunVisibleFromPosition(const Vector&) { return true; }
bool CPVS::IsSunVisibleInPVS(const CPVSBits& b) { return !b.bits.empty(); }
void CPVS::ResetPVS(CPVSBits* p) { if (p)p->bits.clear(); }
void CPVS::AddOriginToPVS(const Vector&, CPVSBits* p) { if (p) { p->Resize(GetClusterCount());if (GetClusterCount())p->Set(0); } }
void CPVS::GetVisForCluster(int c, CPVSBits* p) { if (p) { p->Resize(GetClusterCount());if (c >= 0)p->Set(c); } }
void CPVS::GetPrecisePVS(CPVSBits* p, const Vector& v) { AddOriginToPVS(v, p); }
void CPVS::GetSunlightPVS(CPVSBits* p) { if (p) { p->Resize(GetClusterCount());for (int i = 0;i < GetClusterCount();++i)p->Set(i); } }
void CPVS::GetOrthoPVS(CPVSBits* p) { GetSunlightPVS(p); }
const CVisCluster* CPVS::GetCluster(int c) { return nullptr; }
int CPVS::RegionAt(const Vector&) { return 0; }
int CPVS::LeafAt(const Vector&, AABB_t& b) { return 0; }
void CPVS::ClustersInBoxEnclosed_r(const CClusterQueryResults&, uint32, const AABB_t&, const AABB_t&) {}
void CPVS::ClustersInBox_r(const CClusterQueryResults&, uint32, const AABB_t&, const AABB_t&) {}
const uint32* CPVS::GetVisBits(int) { return nullptr; }

DLL_EXPORT IPVS* CreatePVS(IWorldReference* pWorld)
{
	CPVS* pPVS = new CPVS(Vector{});

	CVoxelVisibility* pVisibility = new class CVoxelVisibility;
	pPVS->Init(pVisibility);

	return pPVS;
}
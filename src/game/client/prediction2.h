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
#pragma once
#include "isource2_clientprediction.h"
#include "../../public/networkvar.h"

class CPrediction2 : public CBaseAppSystem<IPrediction2> {
public:
	DECLARE_CLASS_GAMEROOT(CPrediction2, IPrediction2);

			CPrediction2(void) {}
	virtual	~CPrediction2(void) {}

	virtual InitReturnVal_t	Init() { return INIT_OK; }
	virtual void			Shutdown() {}

	virtual void Update(int startframe, bool validframe, int incoming_acknowledged, int outgoing_command) {}
	virtual void PreEntityPacketReceived(class CSplitScreenSlot* pSlot, int commands_acknowledged, int current_world_update_packet) {}
	virtual void NetUpdateStart(class CSplitScreenSlot* pSlot) {}
	virtual void PostEntityPacketReceived(class CSplitScreenSlot* pSlot) {}
	virtual void PostNetworkDataReceived(class CSplitScreenSlot* pSlot, int commands_acknowledged) {}
	virtual void OnReceivedUncompressedPacket() {}

	virtual void GetViewOrigin(class CSplitScreenSlot* pSlot, class Vector* org) {}
	virtual void GetViewAngles(class CSplitScreenSlot* pSlot, class QAngle* ang) {}
	virtual void SetViewOriginAngles(class CSplitScreenSlot* pSlot, class Vector* org, class QAngle* ang) {}
	virtual void GetLocalViewAngles(class CSplitScreenSlot* pSlot, class QAngle* ang) {}
	virtual void SetLocalViewAngles(class CSplitScreenSlot* pSlot, class QAngle* ang) {}
	virtual bool HasAnyPredictables() { return false; }

	virtual bool InPrediction() const { return false; }
	virtual bool IsFirstTimePredicted() const { return false; }

	virtual int GetIncomingPacketNumber() { return 0; }
	virtual void CheckMovingGround(class C_BasePlayer* pPlayer, double frametime) {}
	virtual void RunCommand(class C_BasePlayer* pPlayer, class CUserCmd* pCmd, class IMoveHelper* pMoveHelper) {}
	virtual void SetupMove(class C_BasePlayer* pPlayer, class CUserCmd* pCmd, class IMoveHelper* pMoveHelper, class CMoveData* pMoveData) {}
	virtual void FinishMove(class C_BasePlayer* pPlayer, class CUserCmd* pCmd, class CMoveData* move) {}

	virtual void CheckError(class CSplitScreenSlot* pSlot, class C_BasePlayer* pPlayer, int commands_acknowledged) {}
	virtual void _Update(class CSplitScreenSlot* pSlot, bool bReceivedNewWorldUpdate, bool bValidFrame, int incoming_acknowledged, int outgoing_command) {}

};
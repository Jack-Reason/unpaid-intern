// Copyright (c) 2026 Jack Reason. MIT License.
#include "UIBuildGunController.h"
#include "GameFramework/PlayerController.h"

void UUIBuildGunController::BindToLocalPlayer(APlayerController* PC)
{
	BoundPC = PC;
	// TODO(M2): Resolve AFGBuildGun from AFGCharacterPlayer / equipment.
}

bool UUIBuildGunController::PlaceMinerOnNode(const FString& NodeId, int32 Tier, bool bConfirm, bool bFreePlacement, FString& OutError)
{
	(void)Tier;
	if (!BoundPC.IsValid())
	{
		OutError = TEXT("no_local_player");
		return false;
	}
	if (NodeId.IsEmpty())
	{
		OutError = TEXT("missing_node_id");
		return false;
	}
	// Policy stubs mirroring brain mock until FG APIs are wired.
	if (!bFreePlacement)
	{
		// Real impl: check intern pioneer inventory vs Recipe_MinerMk* cost.
	}
	if (!bConfirm)
	{
		// Real impl: if node occupied → OutError = needs_confirm; return false.
	}
	UE_LOG(LogTemp, Log, TEXT("UnpaidIntern BuildGunController stub: place miner on %s (confirm=%d free=%d)"),
		*NodeId, bConfirm ? 1 : 0, bFreePlacement ? 1 : 0);
	OutError.Reset();
	return true;
}

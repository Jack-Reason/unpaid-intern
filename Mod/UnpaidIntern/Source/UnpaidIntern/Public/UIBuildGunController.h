// Copyright (c) 2026 Jack Reason. MIT License.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UIBuildGunController.generated.h"

class APlayerController;

/**
 * Drives AFGBuildGun on the local pioneer: recipe select → hologram → multi-step place.
 * TODO(M2): Include FG BuildGun / hologram headers from Starter Project;
 *           call into build-gun state machine (DoMultiStepPlacement pattern) rather than SpawnActor cheats.
 */
UCLASS()
class UNPAIDINTERN_API UUIBuildGunController : public UObject
{
	GENERATED_BODY()

public:
	void BindToLocalPlayer(APlayerController* PC);

	/**
	 * Attempt to place a miner on a resource node.
	 * @return true if placement started / succeeded (stub always logs and returns policy result).
	 */
	bool PlaceMinerOnNode(const FString& NodeId, int32 Tier, bool bConfirm, bool bFreePlacement, FString& OutError);

private:
	UPROPERTY()
	TWeakObjectPtr<APlayerController> BoundPC;
};

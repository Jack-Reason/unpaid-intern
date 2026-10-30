// Copyright (c) 2026 Jack Reason. MIT License.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UIInternSettings.generated.h"

/**
 * Runtime / editor settings for unpaid-intern.
 * Bind address is loopback-only by default (safety).
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Unpaid Intern"))
class UNPAIDINTERN_API UUIInternSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** HTTP/WS listen port on 127.0.0.1 only. */
	UPROPERTY(EditAnywhere, Config, Category = "Bridge")
	int32 BridgePort = 8765;

	/** follow distance in centimetres (UE units). */
	UPROPERTY(EditAnywhere, Config, Category = "Behaviour")
	float FollowDistanceCm = 400.f;

	/**
	 * Chat mode:
	 * 0 = command-only (/intern …) — default
	 * 1 = always-listen (all player chat forwarded to brain)
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Chat")
	int32 ChatMode = 0;

	/**
	 * Dev-only: place miners without consuming materials.
	 * Must stay false for any public / ficsit.app build.
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Dev")
	bool bFreePlacement = false;
};

// Copyright (c) 2026 Jack Reason. MIT License.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UIInternBridge.generated.h"

class UUIInternSubsystem;

/**
 * Loopback-only HTTP bridge (127.0.0.1).
 * TODO(M1): Implement with UE HTTP server or third-party (see FRM / ALWebsocketLib patterns).
 * Endpoints sketched:
 *   GET  /health
 *   GET  /observe
 *   POST /action   { "name": "follow"|"goto"|"say"|"mine"|"build_miner"|"hold"|"cancel", "arguments": {...} }
 *
 * This file is a scaffold — it has NOT been compiled or run in-game.
 */
UCLASS()
class UNPAIDINTERN_API UUIInternBridge : public UObject
{
	GENERATED_BODY()

public:
	void Start(UUIInternSubsystem* Owner, int32 Port);
	void Stop();
	bool IsRunning() const { return bRunning; }

	/** Build JSON observation payload for GET /observe. */
	FString BuildObservationJson() const;

	/** Handle one action JSON body; returns result JSON. */
	FString HandleActionJson(const FString& Body);

private:
	UPROPERTY()
	TWeakObjectPtr<UUIInternSubsystem> Subsystem;

	int32 ListenPort = 8765;
	bool bRunning = false;
};

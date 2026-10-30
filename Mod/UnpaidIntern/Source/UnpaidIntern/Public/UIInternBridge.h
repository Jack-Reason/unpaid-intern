// Copyright (c) 2026 Jack Reason. MIT License.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UIInternBridge.generated.h"

class UUIInternSubsystem;

/**
 * Loopback-only HTTP bridge (127.0.0.1) on the intern machine.
 * Endpoints: GET /health, GET /observe, POST /action
 */
UCLASS()
class UNPAIDINTERN_API UUIInternBridge : public UObject
{
	GENERATED_BODY()

public:
	void Start(UUIInternSubsystem* Owner, int32 Port);
	void Stop();
	bool IsRunning() const { return bRunning; }

	FString BuildObservationJson() const;
	FString HandleActionJson(const FString& Body);

private:
	UPROPERTY()
	TWeakObjectPtr<UUIInternSubsystem> Subsystem;

	int32 ListenPort = 8765;
	bool bRunning = false;
};

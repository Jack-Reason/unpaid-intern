// Copyright (c) 2026 Jack Reason. MIT License.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UIInputDriver.generated.h"

class APlayerController;

/**
 * Injects movement / look / interact through Enhanced Input on the LOCAL pioneer.
 * NOT screen scraping; NOT OS SendInput.
 * TODO(M2): Bind FG Enhanced Input actions (Move, Look, Jump, Interact) once Starter Project headers resolve.
 * Docs intent: ADR-0006; UE EnhancedInput LocalPlayerSubsystem + AFGPlayerController mapping contexts.
 */
UCLASS()
class UNPAIDINTERN_API UUIInputDriver : public UObject
{
	GENERATED_BODY()

public:
	void BindToLocalPlayer(APlayerController* PC);

	/** High-level intents from the brain / subsystem. */
	void StartFollow(AActor* Target, float DistanceCm);
	void Hold();
	void Goto(FVector WorldLocation);
	void ClearIntents();

	FString GetJobName() const { return CurrentJob; }
	void SetJobName(const FString& Name) { CurrentJob = Name; }

	void TickDriver(float DeltaSeconds);

private:
	UPROPERTY()
	TWeakObjectPtr<APlayerController> BoundPC;

	UPROPERTY()
	TWeakObjectPtr<AActor> FollowTarget;

	float FollowDistanceCm = 400.f;
	TOptional<FVector> GotoTarget;
	FString CurrentJob = TEXT("idle");

	/** Stub: AddMovementInput / look toward goal until Enhanced Input inject is wired. */
	void SteerToward(const FVector& WorldGoal, float StopDistanceCm);
};

// Copyright (c) 2026 Jack Reason. MIT License.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UIInternCharacter.generated.h"

/**
 * Pioneer-like pawn for the unpaid intern.
 * TODO(M1): Wire mesh/anim from Starter Project placeholders; pathfinding via AIController.
 * This class has NOT been compiled against FactoryGame headers yet.
 */
UCLASS()
class UNPAIDINTERN_API AUIInternCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AUIInternCharacter();

	UFUNCTION(BlueprintCallable, Category = "UnpaidIntern")
	void StartFollow(AActor* Target, float DistanceCm);

	UFUNCTION(BlueprintCallable, Category = "UnpaidIntern")
	void HoldPosition();

	UFUNCTION(BlueprintCallable, Category = "UnpaidIntern")
	void GotoLocation(FVector WorldLocation);

	UFUNCTION(BlueprintCallable, Category = "UnpaidIntern")
	FString GetDisplayName() const { return DisplayName; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UnpaidIntern")
	FString DisplayName = TEXT("UNPAID INTERN");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UnpaidIntern")
	FString CurrentJob = TEXT("idle");

protected:
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY()
	TWeakObjectPtr<AActor> FollowTarget;

	float FollowDistanceCm = 400.f;
	bool bHolding = true;
	TOptional<FVector> GotoTarget;
};

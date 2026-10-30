// Copyright (c) 2026 Jack Reason. MIT License.
#include "UIInternCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AUIInternCharacter::AUIInternCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// TODO(M1): Configure capsule, movement, mesh after Starter Project import.
}

void AUIInternCharacter::StartFollow(AActor* Target, float DistanceCm)
{
	FollowTarget = Target;
	FollowDistanceCm = DistanceCm;
	bHolding = false;
	GotoTarget.Reset();
	CurrentJob = TEXT("follow");
}

void AUIInternCharacter::HoldPosition()
{
	bHolding = true;
	FollowTarget.Reset();
	GotoTarget.Reset();
	CurrentJob = TEXT("hold");
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
	}
}

void AUIInternCharacter::GotoLocation(FVector WorldLocation)
{
	GotoTarget = WorldLocation;
	bHolding = false;
	FollowTarget.Reset();
	CurrentJob = TEXT("goto");
	// TODO(M1): Use UAIBlueprintHelperLibrary::SimpleMoveToLocation or nav mesh.
}

void AUIInternCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bHolding)
	{
		return;
	}

	// Stub locomotion: simple MoveTo toward follow/goto without nav yet.
	FVector Goal = GetActorLocation();
	bool bHasGoal = false;

	if (GotoTarget.IsSet())
	{
		Goal = GotoTarget.GetValue();
		bHasGoal = true;
		if (FVector::DistSquared(GetActorLocation(), Goal) < FMath::Square(100.f))
		{
			HoldPosition();
			return;
		}
	}
	else if (AActor* Target = FollowTarget.Get())
	{
		Goal = Target->GetActorLocation();
		bHasGoal = true;
		const float DistSq = FVector::DistSquared(GetActorLocation(), Goal);
		if (DistSq < FMath::Square(FollowDistanceCm))
		{
			return;
		}
	}

	if (bHasGoal)
	{
		const FVector Dir = (Goal - GetActorLocation()).GetSafeNormal2D();
		AddMovementInput(Dir, 1.f);
	}
}

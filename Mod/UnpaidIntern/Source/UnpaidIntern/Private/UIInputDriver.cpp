// Copyright (c) 2026 Jack Reason. MIT License.
#include "UIInputDriver.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UUIInputDriver::BindToLocalPlayer(APlayerController* PC)
{
	BoundPC = PC;
	// TODO(M2): UEnhancedInputLocalPlayerSubsystem::AddMappingContext for FG contexts;
	//           cache UInputAction* for Move/Look/Interact from AFGPlayerController.
}

void UUIInputDriver::StartFollow(AActor* Target, float DistanceCm)
{
	FollowTarget = Target;
	FollowDistanceCm = DistanceCm;
	GotoTarget.Reset();
	CurrentJob = TEXT("follow");
}

void UUIInputDriver::Hold()
{
	FollowTarget.Reset();
	GotoTarget.Reset();
	CurrentJob = TEXT("hold");
	if (APlayerController* PC = BoundPC.Get())
	{
		if (ACharacter* Char = Cast<ACharacter>(PC->GetPawn()))
		{
			if (UCharacterMovementComponent* Move = Char->GetCharacterMovement())
			{
				Move->StopMovementImmediately();
			}
		}
	}
}

void UUIInputDriver::Goto(FVector WorldLocation)
{
	GotoTarget = WorldLocation;
	FollowTarget.Reset();
	CurrentJob = TEXT("goto");
}

void UUIInputDriver::ClearIntents()
{
	Hold();
	CurrentJob = TEXT("idle");
}

void UUIInputDriver::TickDriver(float DeltaSeconds)
{
	(void)DeltaSeconds;
	if (CurrentJob == TEXT("hold") || CurrentJob == TEXT("idle"))
	{
		return;
	}
	if (GotoTarget.IsSet())
	{
		SteerToward(GotoTarget.GetValue(), 100.f);
		if (APlayerController* PC = BoundPC.Get())
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				if (FVector::DistSquared(Pawn->GetActorLocation(), GotoTarget.GetValue()) < FMath::Square(100.f))
				{
					Hold();
				}
			}
		}
		return;
	}
	if (AActor* Target = FollowTarget.Get())
	{
		SteerToward(Target->GetActorLocation(), FollowDistanceCm);
	}
}

void UUIInputDriver::SteerToward(const FVector& WorldGoal, float StopDistanceCm)
{
	APlayerController* PC = BoundPC.Get();
	if (!PC)
	{
		return;
	}
	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		return;
	}
	const float DistSq = FVector::DistSquared(Pawn->GetActorLocation(), WorldGoal);
	if (DistSq < FMath::Square(StopDistanceCm))
	{
		return;
	}
	const FVector Dir = (WorldGoal - Pawn->GetActorLocation()).GetSafeNormal2D();
	// Stub until Enhanced Input inject: Character movement input mimics human stick.
	Pawn->AddMovementInput(Dir, 1.f);
	// TODO(M2): Inject Look input so the pioneer faces the goal (build gun / interact).
}

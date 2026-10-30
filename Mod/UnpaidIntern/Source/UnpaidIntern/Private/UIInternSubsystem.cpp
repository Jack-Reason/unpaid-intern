// Copyright (c) 2026 Jack Reason. MIT License.
#include "UIInternSubsystem.h"
#include "UIInternBridge.h"
#include "UIInternCharacter.h"
#include "UIInternSettings.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Dom/JsonObject.h"

void UUIInternSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UUIInternSettings* Settings = GetDefault<UUIInternSettings>();
	Bridge = NewObject<UUIInternBridge>(this);
	Bridge->Start(this, Settings ? Settings->BridgePort : 8765);

	// Defer spawn until world has a player — Tick/timer in real impl.
	// TODO(M1): Hook OnPostLoadMap or first player ready.
	UE_LOG(LogTemp, Log, TEXT("UnpaidIntern subsystem initialized (spawn deferred — TODO)"));
}

void UUIInternSubsystem::Deinitialize()
{
	DespawnIntern();
	if (Bridge)
	{
		Bridge->Stop();
		Bridge = nullptr;
	}
	Super::Deinitialize();
}

bool UUIInternSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// Client+listen host; skip dedicated-server-only until M2+.
	if (const UWorld* World = Cast<UWorld>(Outer))
	{
		return World->IsGameWorld();
	}
	return false;
}

AUIInternCharacter* UUIInternSubsystem::SpawnInternNearPlayer()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}
	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC || !PC->GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("UnpaidIntern: no player pawn to spawn beside"));
		return nullptr;
	}

	const FVector Origin = PC->GetPawn()->GetActorLocation();
	const FVector Offset = PC->GetPawn()->GetActorRightVector() * 300.f;
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AUIInternCharacter* NewIntern = World->SpawnActor<AUIInternCharacter>(
		AUIInternCharacter::StaticClass(),
		Origin + Offset,
		PC->GetPawn()->GetActorRotation(),
		Params);

	Intern = NewIntern;
	if (NewIntern)
	{
		const UUIInternSettings* Settings = GetDefault<UUIInternSettings>();
		NewIntern->StartFollow(PC->GetPawn(), Settings ? Settings->FollowDistanceCm : 400.f);
		UE_LOG(LogTemp, Log, TEXT("UnpaidIntern: spawned beside player"));
	}
	return NewIntern;
}

void UUIInternSubsystem::DespawnIntern()
{
	if (AUIInternCharacter* Char = Intern.Get())
	{
		Char->Destroy();
	}
	Intern.Reset();
}

int64 UUIInternSubsystem::GetWorldTick() const
{
	if (const UWorld* World = GetWorld())
	{
		return static_cast<int64>(World->GetTimeSeconds() * 1000.0);
	}
	return 0;
}

void UUIInternSubsystem::SayInChat(const FString& Text)
{
	// TODO(M1): Use SML scripted chat / FGChatManager so the name shows as UNPAID INTERN.
	// Precedent: FRM sendChatMessage; SML 3.12 sender-colour fixes.
	UE_LOG(LogTemp, Log, TEXT("[UNPAID INTERN] %s"), *Text);
}

void UUIInternSubsystem::HandleInternChatCommand(const TArray<FString>& Args)
{
	if (Args.Num() == 0)
	{
		SayInChat(TEXT("Usage: /intern spawn|despawn|follow|hold|mine|listen|commands"));
		return;
	}
	const FString Sub = Args[0].ToLower();
	if (Sub == TEXT("spawn"))
	{
		SpawnInternNearPlayer();
	}
	else if (Sub == TEXT("despawn"))
	{
		DespawnIntern();
	}
	else if (Sub == TEXT("follow"))
	{
		if (AUIInternCharacter* Char = Intern.Get())
		{
			if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
			{
				Char->StartFollow(PC->GetPawn(), GetDefault<UUIInternSettings>()->FollowDistanceCm);
			}
		}
	}
	else if (Sub == TEXT("hold"))
	{
		if (AUIInternCharacter* Char = Intern.Get())
		{
			Char->HoldPosition();
		}
	}
	else if (Sub == TEXT("mine"))
	{
		MineNearestNode();
	}
	else if (Sub == TEXT("listen"))
	{
		// Toggle always-listen vs command-only — persisted via settings in real impl.
		SayInChat(TEXT("Chat mode toggle is owned by the bridge/brain config for now (command-only default)."));
	}
	else
	{
		SayInChat(TEXT("Unknown subcommand. Try: spawn despawn follow hold mine"));
	}
}

bool UUIInternSubsystem::DispatchAction(const FString& Name, const TSharedPtr<FJsonObject>& Args)
{
	AUIInternCharacter* Char = Intern.Get();
	if (Name == TEXT("say"))
	{
		const FString Text = Args.IsValid() ? Args->GetStringField(TEXT("text")) : FString();
		SayInChat(Text);
		return true;
	}
	if (Name == TEXT("follow"))
	{
		if (!Char) return false;
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (!PC) return false;
		const float Dist = Args.IsValid() && Args->HasField(TEXT("distance_m"))
			? static_cast<float>(Args->GetNumberField(TEXT("distance_m")) * 100.0)
			: GetDefault<UUIInternSettings>()->FollowDistanceCm;
		Char->StartFollow(PC->GetPawn(), Dist);
		return true;
	}
	if (Name == TEXT("hold") || Name == TEXT("cancel"))
	{
		if (!Char) return false;
		Char->HoldPosition();
		return true;
	}
	if (Name == TEXT("goto"))
	{
		if (!Char || !Args.IsValid()) return false;
		const FVector Loc(
			Args->GetNumberField(TEXT("x")),
			Args->GetNumberField(TEXT("y")),
			Args->GetNumberField(TEXT("z")));
		Char->GotoLocation(Loc);
		return true;
	}
	if (Name == TEXT("mine"))
	{
		return MineNearestNode();
	}
	if (Name == TEXT("build_miner"))
	{
		const bool bConfirm = Args.IsValid() && Args->HasField(TEXT("confirm")) && Args->GetBoolField(TEXT("confirm"));
		return BuildMinerOnNearestNode(bConfirm);
	}
	return false;
}

bool UUIInternSubsystem::MineNearestNode()
{
	// TODO(M1): Query AFGResourceNode actors in radius; play hand-mine; push items into intern inventory.
	if (AUIInternCharacter* Char = Intern.Get())
	{
		Char->CurrentJob = TEXT("mine");
	}
	SayInChat(TEXT("Mining stub — nearest node lookup not wired yet."));
	return true;
}

bool UUIInternSubsystem::BuildMinerOnNearestNode(bool bConfirm)
{
	const UUIInternSettings* Settings = GetDefault<UUIInternSettings>();
	// Materials come from the intern's own inventory (Jack decision).
	// bFreePlacement is dev-only and must never ship enabled.
	if (Settings && Settings->bFreePlacement)
	{
		SayInChat(TEXT("DEV free-placement: would spawn miner without materials."));
		return true;
	}
	// TODO(M1): Check intern inventory for Miner recipe cost; place hologram-valid miner; require confirm if occupied.
	if (!bConfirm)
	{
		SayInChat(TEXT("build_miner needs materials in my pockets — and confirm:true if overwriting."));
	}
	SayInChat(TEXT("Build miner stub — placement API not wired yet."));
	return true;
}

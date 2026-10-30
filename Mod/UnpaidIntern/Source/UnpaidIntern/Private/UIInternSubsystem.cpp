// Copyright (c) 2026 Jack Reason. MIT License.
#include "UIInternSubsystem.h"
#include "UIInternBridge.h"
#include "UIInputDriver.h"
#include "UIBuildGunController.h"
#include "UIObservationBuilder.h"
#include "UIChatListener.h"
#include "UIInternSettings.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Dom/JsonObject.h"

void UUIInternSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UUIInternSettings* Settings = GetDefault<UUIInternSettings>();
	InputDriver = NewObject<UUIInputDriver>(this);
	BuildGun = NewObject<UUIBuildGunController>(this);
	ObservationBuilder = NewObject<UUIObservationBuilder>(this);
	ChatListener = NewObject<UUIChatListener>(this);
	Bridge = NewObject<UUIInternBridge>(this);
	Bridge->Start(this, Settings ? Settings->BridgePort : 8765);

	UE_LOG(LogTemp, Log, TEXT("UnpaidIntern driver subsystem initialized (client-side; no pawn spawn)"));
}

void UUIInternSubsystem::Deinitialize()
{
	if (Bridge)
	{
		Bridge->Stop();
		Bridge = nullptr;
	}
	InputDriver = nullptr;
	BuildGun = nullptr;
	ObservationBuilder = nullptr;
	ChatListener = nullptr;
	Super::Deinitialize();
}

bool UUIInternSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// Client-side driver only — skip dedicated server world.
	if (const UWorld* World = Cast<UWorld>(Outer))
	{
		if (!World->IsGameWorld())
		{
			return false;
		}
		return World->GetNetMode() != NM_DedicatedServer;
	}
	return false;
}

TStatId UUIInternSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UUIInternSubsystem, STATGROUP_Tickables);
}

void UUIInternSubsystem::Tick(float DeltaTime)
{
	EnsureBoundToLocalPlayer();
	if (InputDriver)
	{
		InputDriver->TickDriver(DeltaTime);
	}
}

void UUIInternSubsystem::EnsureBoundToLocalPlayer()
{
	UWorld* World = GetWorld();
	if (!World || !InputDriver)
	{
		return;
	}
	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (PC && PC->IsLocalController())
	{
		InputDriver->BindToLocalPlayer(PC);
		if (BuildGun)
		{
			BuildGun->BindToLocalPlayer(PC);
		}
	}
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
	if (ChatListener)
	{
		ChatListener->ReplyAsIntern(Text);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[UNPAID INTERN] %s"), *Text);
	}
}

void UUIInternSubsystem::HandleInternChatCommand(const TArray<FString>& Args)
{
	if (Args.Num() == 0)
	{
		SayInChat(TEXT("Usage: /intern follow|hold|mine|listen|status — I am a real pioneer (second licence), not a spawned pawn."));
		return;
	}
	const FString Sub = Args[0].ToLower();
	EnsureBoundToLocalPlayer();
	if (Sub == TEXT("follow"))
	{
		// Follow Jack: other player / first remote pawn — stub uses player index 0 host when we are client.
		// TODO(M2): resolve Jack's AFGCharacterPlayer from PlayerArray.
		SayInChat(TEXT("Follow intent queued — resolve Jack pawn in M2."));
		if (InputDriver)
		{
			if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
			{
				// Until Jack's pawn is resolved, hold-follow self is a no-op distance; job name still updates for observe.
				InputDriver->StartFollow(PC->GetPawn(), GetDefault<UUIInternSettings>()->FollowDistanceCm);
			}
		}
	}
	else if (Sub == TEXT("hold") || Sub == TEXT("cancel"))
	{
		if (InputDriver)
		{
			InputDriver->Hold();
		}
	}
	else if (Sub == TEXT("mine"))
	{
		MineNearestNode();
	}
	else if (Sub == TEXT("listen"))
	{
		SayInChat(TEXT("Chat mode toggle is owned by settings (command-only default)."));
	}
	else if (Sub == TEXT("status"))
	{
		const FString Job = InputDriver ? InputDriver->GetJobName() : TEXT("?");
		SayInChat(FString::Printf(TEXT("Driver job=%s bridge=%s"), *Job, Bridge && Bridge->IsRunning() ? TEXT("up") : TEXT("down")));
	}
	else if (Sub == TEXT("spawn") || Sub == TEXT("despawn"))
	{
		SayInChat(TEXT("No spawn — I am already a pioneer on this client (ADR-0006)."));
	}
	else
	{
		// Forward remainder to brain via chat listener.
		if (ChatListener)
		{
			ChatListener->EnqueueFromJack(TEXT("Jack"), FString::Join(Args, TEXT(" ")));
		}
		SayInChat(TEXT("Queued for brain."));
	}
}

bool UUIInternSubsystem::DispatchAction(const FString& Name, const TSharedPtr<FJsonObject>& Args)
{
	EnsureBoundToLocalPlayer();
	if (Name == TEXT("say"))
	{
		const FString Text = Args.IsValid() ? Args->GetStringField(TEXT("text")) : FString();
		SayInChat(Text);
		return true;
	}
	if (Name == TEXT("follow"))
	{
		if (!InputDriver) return false;
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (!PC) return false;
		const float Dist = Args.IsValid() && Args->HasField(TEXT("distance_m"))
			? static_cast<float>(Args->GetNumberField(TEXT("distance_m")) * 100.0)
			: GetDefault<UUIInternSettings>()->FollowDistanceCm;
		// TODO(M2): Target = Jack's pioneer, not self.
		InputDriver->StartFollow(PC->GetPawn(), Dist);
		return true;
	}
	if (Name == TEXT("hold") || Name == TEXT("cancel"))
	{
		if (!InputDriver) return false;
		InputDriver->Hold();
		return true;
	}
	if (Name == TEXT("goto"))
	{
		if (!InputDriver || !Args.IsValid()) return false;
		const FVector Loc(
			Args->GetNumberField(TEXT("x")),
			Args->GetNumberField(TEXT("y")),
			Args->GetNumberField(TEXT("z")));
		InputDriver->Goto(Loc);
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
	if (InputDriver)
	{
		InputDriver->SetJobName(TEXT("mine"));
	}
	SayInChat(TEXT("Mining stub — input-drive approach + hand interact not wired yet."));
	return true;
}

bool UUIInternSubsystem::BuildMinerOnNearestNode(bool bConfirm)
{
	const UUIInternSettings* Settings = GetDefault<UUIInternSettings>();
	FString Err;
	const FString NodeId = TEXT("nearest"); // TODO(M3): from observe / args
	const bool bOk = BuildGun && BuildGun->PlaceMinerOnNode(
		NodeId,
		/*Tier*/ 0,
		bConfirm,
		Settings && Settings->bFreePlacement,
		Err);
	if (!bOk)
	{
		SayInChat(Err.IsEmpty() ? TEXT("build_miner failed") : Err);
		return false;
	}
	if (Settings && Settings->bFreePlacement)
	{
		SayInChat(TEXT("DEV free-placement: would place miner without materials."));
	}
	else if (!bConfirm)
	{
		SayInChat(TEXT("build_miner stub — materials from my pioneer inventory; confirm:true if overwriting."));
	}
	return true;
}

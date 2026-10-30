// Copyright (c) 2026 Jack Reason. MIT License.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UIInternSubsystem.generated.h"

class UUIInternBridge;
class UUIInputDriver;
class UUIBuildGunController;
class UUIObservationBuilder;
class UUIChatListener;

/**
 * Client-side driver hub (ADR-0006): no spawned pawn — drives the LOCAL pioneer.
 * Jack's vanilla client never loads this mod (RequiredOnRemote: false).
 */
UCLASS()
class UNPAIDINTERN_API UUIInternSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	UUIInputDriver* GetInputDriver() const { return InputDriver; }
	UUIChatListener* GetChatListener() const { return ChatListener; }
	UUIObservationBuilder* GetObservationBuilder() const { return ObservationBuilder; }
	int64 GetWorldTick() const;

	/** Dispatch a named action from the bridge. */
	bool DispatchAction(const FString& Name, const TSharedPtr<FJsonObject>& Args);

	UFUNCTION(BlueprintCallable, Category = "UnpaidIntern")
	void SayInChat(const FString& Text);

	UFUNCTION(BlueprintCallable, Category = "UnpaidIntern")
	void HandleInternChatCommand(const TArray<FString>& Args);

private:
	UPROPERTY()
	TObjectPtr<UUIInternBridge> Bridge;

	UPROPERTY()
	TObjectPtr<UUIInputDriver> InputDriver;

	UPROPERTY()
	TObjectPtr<UUIBuildGunController> BuildGun;

	UPROPERTY()
	TObjectPtr<UUIObservationBuilder> ObservationBuilder;

	UPROPERTY()
	TObjectPtr<UUIChatListener> ChatListener;

	void EnsureBoundToLocalPlayer();
	bool MineNearestNode();
	bool BuildMinerOnNearestNode(bool bConfirm);
};

// Copyright (c) 2026 Jack Reason. MIT License.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UIInternSubsystem.generated.h"

class AUIInternCharacter;
class UUIInternBridge;

/**
 * World subsystem: spawns the intern near the host player, owns the bridge, dispatches actions.
 * TODO(M1): Register as GameInstance/World subsystem via Starter Project module rules;
 *           GameWorldModule Blueprint should also register /intern chat command.
 */
UCLASS()
class UNPAIDINTERN_API UUIInternSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	UFUNCTION(BlueprintCallable, Category = "UnpaidIntern")
	AUIInternCharacter* SpawnInternNearPlayer();

	UFUNCTION(BlueprintCallable, Category = "UnpaidIntern")
	void DespawnIntern();

	AUIInternCharacter* GetIntern() const { return Intern.Get(); }
	int64 GetWorldTick() const;

	/** Dispatch a named action from the bridge. Arguments may be null. */
	bool DispatchAction(const FString& Name, const TSharedPtr<FJsonObject>& Args);

	UFUNCTION(BlueprintCallable, Category = "UnpaidIntern")
	void SayInChat(const FString& Text);

	UFUNCTION(BlueprintCallable, Category = "UnpaidIntern")
	void HandleInternChatCommand(const TArray<FString>& Args);

private:
	UPROPERTY()
	TWeakObjectPtr<AUIInternCharacter> Intern;

	UPROPERTY()
	TObjectPtr<UUIInternBridge> Bridge;

	bool MineNearestNode();
	bool BuildMinerOnNearestNode(bool bConfirm);
};

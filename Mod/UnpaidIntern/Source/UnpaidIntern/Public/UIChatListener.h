// Copyright (c) 2026 Jack Reason. MIT License.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UIChatListener.generated.h"

/**
 * Reads in-game chat for Jack's orders; queues text for the brain; posts replies as this pioneer.
 * TODO(M2): Hook FG/SML chat receive; FRM getChatMessages is behavioural precedent only.
 */
UCLASS()
class UNPAIDINTERN_API UUIChatListener : public UObject
{
	GENERATED_BODY()

public:
	/** Enqueue a line that should reach the brain (command-only or always-listen). */
	void EnqueueFromJack(const FString& FromName, const FString& Text);

	/** Drain pending inbox into JSON-friendly structs (clears queue). */
	void DrainInbox(TArray<TPair<FString, FString>>& OutFromText);

	/** Reply visible as the intern pioneer in normal chat. */
	void ReplyAsIntern(const FString& Text);

	bool ShouldForward(const FString& Text, int32 ChatMode) const;

private:
	TArray<TPair<FString, FString>> Pending;
};

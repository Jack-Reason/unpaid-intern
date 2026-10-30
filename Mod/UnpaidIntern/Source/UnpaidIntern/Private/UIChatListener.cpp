// Copyright (c) 2026 Jack Reason. MIT License.
#include "UIChatListener.h"

void UUIChatListener::EnqueueFromJack(const FString& FromName, const FString& Text)
{
	Pending.Emplace(FromName, Text);
}

void UUIChatListener::DrainInbox(TArray<TPair<FString, FString>>& OutFromText)
{
	OutFromText = MoveTemp(Pending);
	Pending.Reset();
}

void UUIChatListener::ReplyAsIntern(const FString& Text)
{
	// TODO(M2): SML scripted chat / FG chat send as local player.
	UE_LOG(LogTemp, Log, TEXT("[UNPAID INTERN] %s"), *Text);
}

bool UUIChatListener::ShouldForward(const FString& Text, int32 ChatMode) const
{
	const FString Lower = Text.ToLower();
	if (Lower.StartsWith(TEXT("/intern")) || Lower.StartsWith(TEXT("/unpaid")) || Lower.Contains(TEXT("@intern")))
	{
		return true;
	}
	// ChatMode 1 = always-listen
	return ChatMode == 1;
}

// Copyright (c) 2026 Jack Reason. MIT License.
#pragma once

#include "CoreMinimal.h"
// TODO(M2): Include SML ChatCommandInstance.h and subclass AChatCommandInstance.
// Docs: https://docs.ficsit.app/satisfactory-modding/latest/Development/ModLoader/ChatCommands.html
//
//   Command Name: intern
//   Aliases: unpaid, ui
//   Usage: /intern <follow|hold|mine|listen|status> …
//   Note: spawn/despawn removed — intern IS the local pioneer (ADR-0006).
//   Also: UIChatListener reads Jack's normal chat for orders.

#include "UIInternChatCommand.generated.h"

UCLASS()
class UNPAIDINTERN_API UUIInternChatCommandDocs : public UObject
{
	GENERATED_BODY()
};

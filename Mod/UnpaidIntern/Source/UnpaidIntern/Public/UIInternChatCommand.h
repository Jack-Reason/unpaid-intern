// Copyright (c) 2026 Jack Reason. MIT License.
#pragma once

#include "CoreMinimal.h"
// TODO(M1): Include SML/ChatCommand/ChatCommandInstance.h from Starter Project and subclass AChatCommandInstance.
// Docs: https://docs.ficsit.app/satisfactory-modding/latest/Development/ModLoader/ChatCommands.html
//
// Until the Starter Project is wired, this header documents the intended registration:
//   Command Name: intern
//   Aliases: unpaid, ui
//   Usage: /intern <spawn|despawn|follow|hold|mine|listen> …
//   Register on RootGameWorld_UnpaidIntern M Chat Commands array (Blueprint or C++ GameWorldModule).

#include "UIInternChatCommand.generated.h"

UCLASS()
class UNPAIDINTERN_API UUIInternChatCommandDocs : public UObject
{
	GENERATED_BODY()
};

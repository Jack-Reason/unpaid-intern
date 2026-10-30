// Copyright (c) 2026 Jack Reason. MIT License.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UIObservationBuilder.generated.h"

class UUIInternSubsystem;

/** Builds SPEC observation JSON from local FG state on the intern client. */
UCLASS()
class UNPAIDINTERN_API UUIObservationBuilder : public UObject
{
	GENERATED_BODY()

public:
	FString BuildJson(const UUIInternSubsystem* Sys) const;
};

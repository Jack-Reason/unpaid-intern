// Copyright (c) 2026 Jack Reason. MIT License.

#include "UnpaidInternModule.h"

#define LOCTEXT_NAMESPACE "FUnpaidInternModule"

void FUnpaidInternModule::StartupModule()
{
	// Module loaded; world subsystem / game-world module do the real work.
}

void FUnpaidInternModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnpaidInternModule, UnpaidIntern)

// Copyright (c) 2026 Jack Reason. MIT License.
// TODO(M1): Compile only after copying this plugin into Mods/ of the SML Starter Project.

#pragma once

#include "Modules/ModuleManager.h"

class FUnpaidInternModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

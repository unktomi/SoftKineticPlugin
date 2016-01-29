// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SoftKineticPluginPrivatePCH.h"

#define LOCTEXT_NAMESPACE "FSoftKineticPluginModule"

void FSoftKineticPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FSoftKineticPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSoftKineticPluginModule, SoftKineticPlugin)
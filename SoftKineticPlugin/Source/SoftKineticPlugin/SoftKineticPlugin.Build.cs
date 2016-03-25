// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class SoftKineticPlugin : ModuleRules
{
	private string ModulePath
	{
		get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
	}

	private string ThirdPartyPath
	{
		get
		{
			string result = Path.GetFullPath(Path.Combine(ModulePath, "../../Thirdparty/"));
			return result;
		}
	}

	public SoftKineticPlugin(TargetInfo Target)
	{
		// Enable RTTI (prop. of superclass ModuleRules defined in UnrealEngine/Engine/Source/Programs/UnrealBuildTool/System/RulesCompiler.cs )
		bUseRTTI = true;
		// Enable C++ Exceptions for this module
		bEnableExceptions = true;
		// eventually needed as well
		UEBuildConfiguration.bForceEnableExceptions = true;
		string DEPTHSENSE_SDK_DIR = Utils.ResolveEnvironmentVariable("%DEPTHSENSESDK64%");
		string CILIB_SDK_DIR = Utils.ResolveEnvironmentVariable("%DEPTHSENSE_CILIB_64_SDK_DIR%");

		DEPTHSENSE_SDK_DIR = DEPTHSENSE_SDK_DIR.Replace("\\", "/");
		CILIB_SDK_DIR = CILIB_SDK_DIR.Replace("\\", "/");


		PublicIncludePaths.AddRange(
			new string[] {
				"SoftKineticPlugin/Public"
				// ... add public include paths required here ...
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				"SoftKineticPlugin/Private",
				// ... add other private include paths required here ...
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
					"CoreUObject",
					"Engine",
					"RHI",
					"RenderCore",
					"ProceduralMeshComponent",
					"ImageCore",
					"Media",
					"MediaAssets",
				// ... add private dependencies that you statically link with here ...	
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		PrivateIncludePaths.Add(Path.Combine(DEPTHSENSE_SDK_DIR, "include"));
		PrivateIncludePaths.Add(Path.Combine(CILIB_SDK_DIR, "include"));
		PrivateIncludePaths.Add(Path.Combine(ThirdPartyPath, "snappy-windows-1.1.1.8", "include"));
		LoadLibs(DEPTHSENSE_SDK_DIR, CILIB_SDK_DIR, Target);
	}

	public bool LoadLibs(string DEPTHSENSE_SDK_DIR, string CILIB_SDK_DIR, TargetInfo Target)
	{
		bool isLibrarySupported = false;

		if ((Target.Platform == UnrealTargetPlatform.Win64))
		{
			isLibrarySupported = true;
			//Thirdparty/DepthSenseSDK/DepthSense.lib
			string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
			PublicAdditionalLibraries.Add(Path.Combine(DEPTHSENSE_SDK_DIR, "lib", "DepthSense.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(CILIB_SDK_DIR, "lib", "CIlib.lib"));
			PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "snappy-windows-1.1.1.8", "src", PlatformString, "Release", "Snappy-static-lib.lib"));
		}

		Definitions.Add(string.Format("WITH_SOFTKINECTIC_PLUGIN_BINDING={0}", isLibrarySupported ? 1 : 0));

		return isLibrarySupported;
	}
}

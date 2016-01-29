// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SoftKineticEditorPlugin : ModuleRules
{
	public SoftKineticEditorPlugin(TargetInfo Target)
	{
		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				"AssetTools",
				"WorkspaceMenuStructure",
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				"SoftKineticEditorPlugin/Private",
				"SoftKineticEditorPlugin/Private/AssetTools",
				"SoftKineticEditorPlugin/Private/Factories",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"ContentBrowser",
				"Core",
				"CoreUObject",
				"EditorStyle",
				"Engine",
				"InputCore",
				"MediaAssets",
				"PropertyEditor",
				"RenderCore",
				"RHI",
				"ShaderCore",
				"Slate",
				"SlateCore",
				"TextureEditor",
				"UnrealEd",
                                "SoftKineticPlugin",
			}
		);

		PrivateIncludePathModuleNames.AddRange(
			new string[] {
				"AssetTools",
				"UnrealEd",
                                "SoftKineticPlugin",
				"WorkspaceMenuStructure",
			}
		);
	}
}

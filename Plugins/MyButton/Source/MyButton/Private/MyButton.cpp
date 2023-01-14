// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyButton.h"
#include "MyButtonStyle.h"
#include "MyButtonCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

#include "Engine/StaticMeshActor.h"

static const FName MyButtonTabName("MyButton");

#define LOCTEXT_NAMESPACE "FMyButtonModule"

void FMyButtonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FMyButtonStyle::Initialize();
	FMyButtonStyle::ReloadTextures();

	FMyButtonCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMyButtonCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FMyButtonModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMyButtonModule::RegisterMenus));
}

void FMyButtonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMyButtonStyle::Shutdown();

	FMyButtonCommands::Unregister();
}

void FMyButtonModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::FromString("Generate 3D Map");
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);

	SpawnStaticMeshActor(FVector::ZeroVector, FRotator::ZeroRotator, FVector::OneVector);
}

void FMyButtonModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMyButtonCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMyButtonCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

void FMyButtonModule::SpawnStaticMeshActor(const FVector& Location, const FRotator& Rotation, const FVector& Scale)
{
	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (IsValid(World))
	{
		AStaticMeshActor* Actor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
		Actor->SetMobility(EComponentMobility::Stationary);
		Actor->SetActorLocation(Location);
		Actor->SetActorRotation(Rotation);
		Actor->SetActorScale3D(Scale);
		UStaticMeshComponent* StaticMeshComponent = Actor->GetStaticMeshComponent();
		if (IsValid(StaticMeshComponent))
		{
			const FString AssetPath = TEXT("/Engine/BasicShapes/Cube.Cube");
			UStaticMesh* StaticMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *AssetPath));
			StaticMeshComponent->SetStaticMesh(StaticMesh);
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMyButtonModule, MyButton)
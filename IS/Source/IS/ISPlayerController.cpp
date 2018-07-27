// Fill out your copyright notice in the Description page of Project Settings.

#include "ISPlayerController.h"
#include "ISPlayerCharacter.h"
#include "ISSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "DynamicCamera.h"
#include "Blueprint/UserWidget.h"

AISPlayerController::AISPlayerController()
{
	bAutoManageActiveCameraTarget = false;
}

void AISPlayerController::BeginPlay()
{
	Super::BeginPlay();

	LoadedData = LoadSettings();

	FovChanged(LoadedData.SettingsFieldOfView);

}

void AISPlayerController::ShowPauseMenu(bool bDisplayMenu)
{
	if (!wPauseMenu){return;}
	if (bDisplayMenu)
	{
		PauseMenu = CreateWidget<UUserWidget>(this, wPauseMenu);

		if (PauseMenu)
		{
			PauseMenu->AddToViewport();
			SetInputMode(FInputModeUIOnly());
		}

		bShowMouseCursor = true;
	}
	else
	{
		if (PauseMenu)
		{
			PauseMenu->RemoveFromParent();
			SetInputMode(FInputModeGameOnly());
		}
		bShowMouseCursor = false;
	}
}

void AISPlayerController::SaveSettings(FLoadedData WidgetLoadedData)
{
	//Create instance of save game class
	UISSaveGame* SaveGameInstance = Cast<UISSaveGame>(UGameplayStatics::CreateSaveGameObject(UISSaveGame::StaticClass()));

	//Set save game instance variables to match the settings menu
	SaveGameInstance->bAimAccelerationEnabled = WidgetLoadedData.bSettingsAimAccelerationEnabled;
	SaveGameInstance->bPlayerControlledCamera = WidgetLoadedData.bSettingsPlayerControlledCamera;
	SaveGameInstance->bToggleCrouch = WidgetLoadedData.bSettingsToggleCrouch;
	SaveGameInstance->bToggleSprint = WidgetLoadedData.bSettingsToggleSprint;
	SaveGameInstance->AimResponseCurve = WidgetLoadedData.SettingsAimResponseCurve;
	SaveGameInstance->FieldOfView = WidgetLoadedData.SettingsFieldOfView;
	SaveGameInstance->bFPHeadBob = WidgetLoadedData.bSettingsFPHeadBob;

	//Save the game instance
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("MainSaveSlot"), 0);

	//Make sure the loaded data is up to date with the save data
	LoadedData.bSettingsAimAccelerationEnabled = WidgetLoadedData.bSettingsAimAccelerationEnabled;
	LoadedData.bSettingsPlayerControlledCamera = WidgetLoadedData.bSettingsPlayerControlledCamera;
	LoadedData.bSettingsToggleCrouch = WidgetLoadedData.bSettingsToggleCrouch;
	LoadedData.bSettingsToggleSprint = WidgetLoadedData.bSettingsToggleSprint;
	LoadedData.SettingsAimResponseCurve = WidgetLoadedData.SettingsAimResponseCurve;
	LoadedData.SettingsFieldOfView = WidgetLoadedData.SettingsFieldOfView;
	LoadedData.bSettingsFPHeadBob = WidgetLoadedData.bSettingsFPHeadBob;

	UE_LOG(LogTemp, Log, TEXT("Data saved successfully"));
	return;
}

FLoadedData AISPlayerController::LoadSettings()
{
	//Create instance of save game class and load the save game into it
	UISSaveGame* SaveGameInstance = Cast<UISSaveGame>(UGameplayStatics::LoadGameFromSlot("MainSaveSlot", 0));
	FLoadedData TempLoadedData;

	//Make sure there is save data before trying to load it
	if (!UGameplayStatics::DoesSaveGameExist("MainSaveSlot", 0))
	{
		//pass defaults if no save data
		TempLoadedData.bSettingsAimAccelerationEnabled = true;
		TempLoadedData.bSettingsPlayerControlledCamera = false;
		TempLoadedData.bSettingsToggleCrouch = true;
		TempLoadedData.bSettingsToggleSprint = false;
		TempLoadedData.SettingsAimResponseCurve = 0;
		TempLoadedData.SettingsFieldOfView = 90.f;
		TempLoadedData.bSettingsFPHeadBob = true;

		LoadedData = TempLoadedData;
		UE_LOG(LogTemp, Log, TEXT("Data loaded unsuccessfully, save data has been set to defaults"));
		return TempLoadedData;
	}

	TempLoadedData.bSettingsAimAccelerationEnabled = SaveGameInstance->bAimAccelerationEnabled;
	TempLoadedData.bSettingsPlayerControlledCamera = SaveGameInstance->bPlayerControlledCamera;
	TempLoadedData.bSettingsToggleCrouch = SaveGameInstance->bToggleCrouch;
	TempLoadedData.bSettingsToggleSprint = SaveGameInstance->bToggleSprint;
	TempLoadedData.SettingsAimResponseCurve = SaveGameInstance->AimResponseCurve;
	TempLoadedData.SettingsFieldOfView = SaveGameInstance->FieldOfView;
	TempLoadedData.bSettingsFPHeadBob = SaveGameInstance->bFPHeadBob;

	LoadedData = TempLoadedData;
	UE_LOG(LogTemp, Log, TEXT("Data loaded successfully"));
	return TempLoadedData;
}

void AISPlayerController::FovChanged(float PercentageFov)
{
	PlayerCameraManager->SetFOV(ConvertAlphaToFov(PercentageFov));
	return;
}

void AISPlayerController::PlayerControlledCameraChanged(bool bPlayerControlledCamera)
{
	AISPlayerCharacter* Player = Cast<AISPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Player->InsideCount <= 0)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADynamicCamera::StaticClass(), FoundActors);
		for (AActor* Camera : FoundActors)
		{
			ADynamicCamera* DynamicCamera = Cast<ADynamicCamera>(Camera);

			if (DynamicCamera != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Setting to false"));
				DynamicCamera->bIsFirstPerson = false;
				Player->EnvironmentSwitchCamera();
				break;
			}
		}
	}
	else
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADynamicCamera::StaticClass(), FoundActors);
		for (AActor* Camera : FoundActors)
		{
			ADynamicCamera* DynamicCamera = Cast<ADynamicCamera>(Camera);

			if (DynamicCamera != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Setting to true"));
				DynamicCamera->bIsFirstPerson = true;
				Player->EnvironmentSwitchCamera();
				break;
			}
		}
	}
	/*
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADynamicCamera::StaticClass(), FoundActors);
	for (AActor* Camera : FoundActors)
	{
		ADynamicCamera* DynamicCamera = Cast<ADynamicCamera>(Camera);

		if (DynamicCamera != nullptr)
		{
			DynamicCamera->bIsFirstPerson =;
			break;
		}
	}
	*/
	return;
}

void AISPlayerController::ResetSettingsToDefaults()
{
	FLoadedData TempLoadedData;

	//Make sure there is save data before trying to load it
	if (!UGameplayStatics::DoesSaveGameExist("MainSaveSlot", 0))
	{
		//pass defaults if no save data
		TempLoadedData.bSettingsAimAccelerationEnabled = true;
		TempLoadedData.bSettingsPlayerControlledCamera = false;
		TempLoadedData.bSettingsToggleCrouch = true;
		TempLoadedData.bSettingsToggleSprint = false;
		TempLoadedData.SettingsAimResponseCurve = 0;
		TempLoadedData.SettingsFieldOfView = 90.f;
		TempLoadedData.bSettingsFPHeadBob = true;

		LoadedData = TempLoadedData;
		UE_LOG(LogTemp, Log, TEXT("Data loaded unsuccessfully, save data has been set to defaults"));
		return;
	}

	TempLoadedData.bSettingsAimAccelerationEnabled = true;
	TempLoadedData.bSettingsPlayerControlledCamera = false;
	TempLoadedData.bSettingsToggleCrouch = true;
	TempLoadedData.bSettingsToggleSprint = false;
	TempLoadedData.SettingsAimResponseCurve = 0;
	TempLoadedData.SettingsFieldOfView = 90.f;
	TempLoadedData.bSettingsFPHeadBob = true;

	LoadedData = TempLoadedData;
	UE_LOG(LogTemp, Log, TEXT("Data loaded successfully"));
	return;
}

float AISPlayerController::ConvertFovToAlpha(float Fov)
{
	return ((Fov - MinFOV) / (MaxFOV - MinFOV));
}

float AISPlayerController::ConvertAlphaToFov(float Alpha)
{
	return ((Alpha * (MaxFOV - MinFOV)) + MinFOV);
}
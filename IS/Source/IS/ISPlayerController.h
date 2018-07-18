// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ISPlayerController.generated.h"

USTRUCT(BlueprintType)
struct FLoadedData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "LoadedData")
	bool bSettingsAimAccelerationEnabled;

	UPROPERTY(BlueprintReadWrite, Category = "LoadedData")
	bool bSettingsPlayerControlledCamera;

	UPROPERTY(BlueprintReadWrite, Category = "LoadedData")
	bool bSettingsToggleSprint;

	UPROPERTY(BlueprintReadWrite, Category = "LoadedData")
	bool bSettingsToggleCrouch;

	UPROPERTY(BlueprintReadWrite, Category = "LoadedData")
	int32 SettingsAimResponseCurve;

	UPROPERTY(BlueprintReadWrite, Category = "LoadedData")
	float SettingsFieldOfView;
};

/**
 * 
 */
UCLASS()
class IS_API AISPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AISPlayerController();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wPauseMenu;

	UUserWidget* PauseMenu;
	UUserWidget* SettingsMenu;

	virtual void BeginPlay() override;

	void ShowPauseMenu(bool bDisplayMenu);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SaveSettings(FLoadedData LoadedData);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FLoadedData LoadSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void FovChanged(float PercentageFov);

	FLoadedData LoadedData;

private:
	float ConvertFovToAlpha(float Fov);
	float ConvertAlphaToFov(float Alpha);

	float MinFOV = 50.f;
	float MaxFOV = 130.f;
};
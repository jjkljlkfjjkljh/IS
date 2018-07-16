// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ISPlayerController.generated.h"

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
};
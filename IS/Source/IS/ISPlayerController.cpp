// Fill out your copyright notice in the Description page of Project Settings.

#include "ISPlayerController.h"
#include "Blueprint/UserWidget.h"

AISPlayerController::AISPlayerController()
{
	bAutoManageActiveCameraTarget = false;
}

void AISPlayerController::BeginPlay()
{
	Super::BeginPlay();
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
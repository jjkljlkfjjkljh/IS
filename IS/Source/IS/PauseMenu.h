// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenu.generated.h"

/**
 * 
 */
UCLASS()
class IS_API UPauseMenu : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	
public:
	static void ShowPauseMenu();
};

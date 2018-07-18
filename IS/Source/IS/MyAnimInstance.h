// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

///Forward Declaratioons
class AISPlayerCharacter;

/**
 * 
 */
UCLASS()
class IS_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsWalking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsRunning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MovementSpeed;

	UFUNCTION(BlueprintCallable, Category = "UpdateAnimationProperties")
	void UpdateAnimationProperties();

private:
	AISPlayerCharacter* Player = nullptr;
	
	
};

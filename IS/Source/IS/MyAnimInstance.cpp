// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ISPlayerCharacter.h"

void UMyAnimInstance::UpdateAnimationProperties()
{
	
	Player = Cast<AISPlayerCharacter>(TryGetPawnOwner());

	if (Player)
	{
		bIsFalling = Player->GetMovementComponent()->IsFalling();
		MovementSpeed = Player->GetVelocity().Size();
		bIsCrouching = Player->bIsPlayerCrouched;
	}
}

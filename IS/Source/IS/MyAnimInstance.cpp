// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/CharacterMovementComponent.h"
#include "MyAnimInstance.h"

void UMyAnimInstance::UpdateAnimationProperties()
{
	APawn* Pawn = TryGetPawnOwner();

	if (Pawn)
	{
		bIsFalling = Pawn->GetMovementComponent()->IsFalling();
		MovementSpeed = Pawn->GetVelocity().Size();
	}
}

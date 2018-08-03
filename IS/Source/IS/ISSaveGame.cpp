// Fill out your copyright notice in the Description page of Project Settings.

#include "ISSaveGame.h"


UISSaveGame::UISSaveGame()
{
	bAimAccelerationEnabled = false;
	bPlayerControlledCamera = false;
	bToggleSprint = false;
	bToggleCrouch = false;
	AimResponseMaxSpeedPercentage = 0.5;
	FieldOfView = 90.f;
}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/*
 Contains functions that are used to check if different transforms and rotations
 are close to a secondary transform or rotation.
 */
class IS_API TransformCheck
{
public:

	/*
	Returns true if two rotations are within a margin of error on every axis.
	Returns false if any axis is not within the margin of error
	*/
	static bool RotationIsWithinLimit(FRotator Rotation1, FRotator Rotation2, float MarginOfError);

	//TODO finish method
	static bool LocationIsWithinLimit(FVector Location1, FVector Location2, float MarginOferror);
};

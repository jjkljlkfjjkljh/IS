// Fill out your copyright notice in the Description page of Project Settings.

#include "TransformCheck.h"

bool TransformCheck::RotationIsWithinLimit(FRotator Rotation1, FRotator Rotation2, float MarginOfError)
{
	float X1 = Rotation1.Roll;
	float Y1 = Rotation1.Pitch;
	float Z1 = Rotation1.Yaw;

	float X2 = Rotation2.Roll;
	float Y2 = Rotation2.Pitch;
	float Z2 = Rotation2.Yaw;

	//Check if all the axis fall within the margin of error. If they don't return false
	if ((X1 + MarginOfError) < X2)
	{
		return false;
	}
	if ((X1 - MarginOfError) > X2)
	{
		return false;
	}
	if ((Y1 + MarginOfError) < Y2)
	{
		return false;
	}
	if ((Y1 - MarginOfError) > Y2)
	{
		return false;
	}
	if ((Z1 + MarginOfError) < Z2)
	{
		return false;
	}
	if ((Z1 - MarginOfError) > Z2)
	{
		return false;
	}

	//returns true of all of the axis checks show the two rotations fall within the margin of error
	return true;
}

bool TransformCheck::LocationIsWithinLimit(FVector Location1, FVector Location2, float MarginOferror)
{
	return false;
}

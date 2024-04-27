// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/ShooterCameraShake.h"

UShooterCameraShake::UShooterCameraShake()
{
	OscillationDuration = 0.1f;
	
	RotOscillation.Pitch.Amplitude = -.5f;
	RotOscillation.Pitch.Frequency = 50.f;
	RotOscillation.Pitch.InitialOffset = EOO_OffsetZero;

	RotOscillation.Yaw.Amplitude = -.5f;
	RotOscillation.Yaw.Frequency = 50.f;

	RotOscillation.Roll.Amplitude = -.5f;
	RotOscillation.Roll.Frequency = 50.f;
	
	LocOscillation.Z.Amplitude = -0.5f;
	LocOscillation.Z.Frequency = 50.f;
}

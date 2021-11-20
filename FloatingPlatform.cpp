// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	StartPoint = FVector(0.f);
	EndPoint = FVector(0.f);

	bInterping = false;

	InterpSpeed = 4.0f;
	InterpTime = 1.f;
	
	//removed worldtimermanager from constructor to beginplay
	//vector to scalar distance
	Distance = (EndPoint - StartPoint).Size();
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	StartPoint = GetActorLocation();
	EndPoint += StartPoint;

	bInterping = false;
	GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterp, InterpTime);
	
	Distance = (EndPoint - StartPoint).Size();
	
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bInterping)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);
		SetActorLocation(Interp);
		// instance location - start point for midway locations
		float DistanceTravelled = (GetActorLocation() - StartPoint).Size();

		if (Distance - DistanceTravelled <= 1.f)
		{
			ToggleInterp();// stop interp very near to the destination

			GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterp, InterpTime);
			// swap vectors for reverse interp
			SwapVectors(StartPoint, EndPoint);
		}
	}
	
}
//interp toggle function 
void AFloatingPlatform::ToggleInterp()
{
	bInterping = !bInterping;
}
// swap vectors
void AFloatingPlatform::SwapVectors(FVector& VOne, FVector& VTwo)
{
	FVector Temp = VOne;
	VOne = VTwo;
	VTwo = Temp;
}
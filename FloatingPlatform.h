// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class FIRSTPROJECT_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();
	// add mesh property for platform
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
	class UStaticMeshComponent* Mesh;
	//start point vector
	UPROPERTY(EditAnywhere, Category = "Locations")
	FVector StartPoint;
	//end point vector
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"), Category = "Locations")
	FVector EndPoint;
	//movement speed for platform
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float InterpSpeed;
	//time default
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float InterpTime;
	//distance
	float Distance;
	//timer for inter movement
	FTimerHandle InterpTimer;
	// bool check
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	bool bInterping;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ToggleInterp();
	// swap after reach destination to start reverse interp
	void SwapVectors(FVector& VOne, FVector& VTwo);

};

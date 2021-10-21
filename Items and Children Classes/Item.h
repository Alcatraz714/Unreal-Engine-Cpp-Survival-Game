// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class FIRSTPROJECT_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();
	// base shape for collision for pickups 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Collision")
	class USphereComponent* CollisionVolume;
	//base mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Mesh")
	class UStaticMeshComponent* Mesh;

	//idle particle system
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Paricles System")
	class UParticleSystemComponent* IdleParticlesComponent;//component because it can be constructed

	//interaction particle system
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Paricles")
	class UParticleSystem* OverlapParticles;

	//sound que system
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sounds")
	class USoundCue* OverlapSound;

	//should rotate or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Rotation")
	bool bRotate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Rotation")
	float RotationRate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//overlap begin and end 
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



};

// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "FirstSaveGame.h"
#include "ItemStorage.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// camera boom create pulls up to player when damage
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; // arm length 
	CameraBoom->bUsePawnControlRotation = true; // control the pawn controller 

	//Capsule mesh for collision 
	GetCapsuleComponent()->SetCapsuleSize(34.f, 88.f);


	//Follow camera create 
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // spring arm rotates and camera is attached to it already 

	//Base turnrate and lookuprate
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	//Fix camera rotation with arrow keys 
	// only affect camera not character
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;

	// fix for camera x y not moving 
	GetCharacterMovement()->bOrientRotationToMovement = true; // character movement moves where camera faces...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...yaw rate for camera move only forward backward 
	GetCharacterMovement()->JumpZVelocity = 650.f; // jump velocity in Z Direction 
	GetCharacterMovement()->AirControl = 0.1f;

	//default stats
	MaxHealth = 100.f;
	Health = 65.f;
	MaxStamina = 150.f;// value before 350
	Stamina = 120.f;
	Coins = 0;

	//run and sprint
	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;
	bShiftKeyDown = false;

	//LMB down bool and equip 
	bLMBDown = false;

	//Escape down and up bool
	bESCDown = false;

	//initialize Enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.f;
	MinsprintStamina = 20.f;

	//interp values
	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;
	
	//move bools
	bMovingForward = false;
	bMovingRight = false;
	
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	//check the player controller before laod
	MainPlayerController = Cast<AMainPlayerController>(GetController());

	FString Map = GetWorld()->GetMapName();
	Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	// check to see in 1st level
	/**
	if (Map != "SunTemple")
	{
		LoadGameNoSwitch();
		if (MainPlayerController)
		{
			MainPlayerController->GameModeOnly();//call the essentials to allow movement and mouse movement set in load game
		}
	}
	*/

	LoadGameNoSwitch();
	if (MainPlayerController)
	{
		MainPlayerController->GameModeOnly();//call the essentials to allow movement and mouse movement set in load game
	}

}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) return;//retrun if dead
	
	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= MinsprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -= DeltaStamina;
			}
			else
			{
				Stamina -= DeltaStamina;
			}
			if (bMovingForward || bMovingRight)
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			
		}
		else //shift key not pressed
		{
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;

	case EStaminaStatus::ESS_BelowMinimum:
		if (bShiftKeyDown)
		{
			if (Stamina - DeltaStamina <= 0)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				Stamina -= DeltaStamina;
				if (bMovingForward || bMovingRight)
				{
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else
				{
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
			}
		}
		else // shift key not pressed
		{
			if (Stamina + DeltaStamina >= MinsprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
		
	case EStaminaStatus::ESS_Exhausted:
		if (bShiftKeyDown)
		{
			Stamina = 0.f;
		}
		else // Shift key not pressed
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStamina >= MinsprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		}
		else
		{
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	default:
		;
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);//interp rotation for each frame

		SetActorRotation(InterpRotation);//moves smoothly to face enemy
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		//update location for enemy health bar
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}

//rotator function lock on to target
FRotator AMainCharacter::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);//finds rotation angle between two points
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;//return the angle required
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	//Action Mapping
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::ShiftKeyUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMainCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMainCharacter::LMBUp);

	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMainCharacter::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMainCharacter::ESCUp);

	//Axis Mapping
	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);
}

bool AMainCharacter::CanMove(float Value)
{
	if (MainPlayerController)
	{
		return (Controller != nullptr) && (Value != 0.0f) && (!bAttacking) && (MovementStatus != EMovementStatus::EMS_Dead) && !MainPlayerController->bPauseMenuVisible;
	}
	return false;
}

void AMainCharacter::Turn(float Value)
{
	if (CanMove(Value))
	{
		AddControllerYawInput(Value);
	}
}

void AMainCharacter::LookUp(float Value)
{
	if (CanMove(Value))
	{
		AddControllerPitchInput(Value);
	}
}

void AMainCharacter::MoveForward(float Value)
{
	bMovingForward = false;//set to false after key release
	if (CanMove(Value))
	{
		//way forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);// X local axis for forward and backward
		AddMovementInput(Direction, Value);

		bMovingForward = true;//true and can call sprint
	}
}

void AMainCharacter::MoveRight(float Value)
{
	bMovingRight = false;
	if (CanMove(Value))
	{
		//way forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);// Y local for right left look
		AddMovementInput(Direction, Value);

		bMovingRight = true;//true and can call sprint
	}

}

void AMainCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LMBDown()
{
	bLMBDown = true;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;//return if dead does not allow attacking

	if (MainPlayerController) if (MainPlayerController->bPauseMenuVisible) return;//return if pause menu visible

	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);//equip current weapon after overlap
			SetActiveOverlappingItem(nullptr); //can be used to equip new item
		}
	}
	else if (EquippedWeapon)
	{
		Attack();
	}
}

void AMainCharacter::LMBUp()
{
	bLMBDown = false;
}

//esacpe key function
void AMainCharacter::ESCDown()
{
	bESCDown = true;

	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}

void AMainCharacter::ESCUp()
{
	bESCDown = false;
}

// health and other attributes
void AMainCharacter::DecrementHealth(float Amount)
{
	if (Health - Amount <= 0.f)
	{
		Health -= Amount;
		Die();
	}
	else
	{
		Health -= Amount;
	}
}

void AMainCharacter::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return;//prevent multiple death anims after death
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

void AMainCharacter::Jump()
{
	if (MainPlayerController) if (MainPlayerController->bPauseMenuVisible) return;//return if pause menu visible

	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		ACharacter::Jump();
	}
}

void AMainCharacter::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMainCharacter::IncrementCoins(int32 Amount)
{
	Coins += Amount;
}

void AMainCharacter::IncrementHealth(float Amount)
{
	if (Health + Amount >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health += Amount;
	}
}

void  AMainCharacter::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}

}

void AMainCharacter::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMainCharacter::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void AMainCharacter::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}

	EquippedWeapon = WeaponToSet;
}

void AMainCharacter::ShowPickupLocations()
{
	// Debug Sphere
	for (int32 i = 0; i < PickUpLocations.Num(); i++)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, PickUpLocations[i], 25.f, 12, FLinearColor::Green, 10.f, .25f);
	}
}

// Attack 
void AMainCharacter::Attack()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)// check to see if attack is already playing to prevent loop stuck while spamming
	{
		bAttacking = true;
		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			int32 Selection = FMath::RandRange(0, 1);
			switch (Selection)
			{
			case 0:
				AnimInstance->Montage_Play(CombatMontage, 2.0f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				break;

			case 1:
				AnimInstance->Montage_Play(CombatMontage, 2.0f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				break;

			default:
				;
			}
		}
	}
}

//end attack for reset
void AMainCharacter::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);//so player only rotates when attacking
	if (bLMBDown)
	{
		Attack();
	}
}

// sound sword swing
void AMainCharacter::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

//rotate towards enemy
void AMainCharacter::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

//take damage override 
float AMainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die();
		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false;
			}
		}
	}
	else
	{
		Health -= DamageAmount;
	}
	//DecrementHealth(DamageAmount); disabled after above 

	return DamageAmount;
}

//update target
void AMainCharacter::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0)
	{
		if (MainPlayerController)
		{
			MainPlayerController->RemoveEnemyHealthBar();
		}
		return;//return and stop generation of health bar
	}
	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);
	if (ClosestEnemy)
	{
		FVector Location = GetActorLocation();
		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();//return vector to float
		for (auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
				if (DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}
		if (MainPlayerController)
		{
			MainPlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
	
}

//level change
void AMainCharacter::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();

		FName CurrentLevelName(*CurrentLevel);// * used to convert the literal to Fname
		if (CurrentLevelName != LevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

//load and save functions
void AMainCharacter::SaveGame()
{
	UFirstSaveGame* SaveGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));//call and extract save game pointer

	//save current stats
	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
	SaveGameInstance->CharacterStats.Coins = Coins;

	//get level name
	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);//removes prefix for saving level name only
	SaveGameInstance->CharacterStats.LevelName = MapName;//save to struct

	//weapon save
	if (EquippedWeapon)
	{
		SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name;
	}

	//save current player position
	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
}

void AMainCharacter::LoadGame(bool SetPosition)
{
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));//load the slot

	//player stats
	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;

	if (WeaponStorage)
	{
		AItemStorage* Weapon = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);//spwan the stored weapon
		if (Weapon)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;
			//spawn new bp for the saved weapon
			if (WeaponName != TEXT(""))
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapon->WeaponMap[WeaponName]);
				WeaponToEquip->Equip(this);//equip the weapon
			}
		}
	}

	//player spawn location
	if (SetPosition)
	{
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	}
	//movement status
	SetMovementStatus(EMovementStatus::EMS_Normal);//after reload
	GetMesh()->bPauseAnims = false;// resume animation
	GetMesh()->bNoSkeletonUpdate = false;// resume skeleton updates

	if (LoadGameInstance->CharacterStats.LevelName != TEXT(""))
	{
		FName LevelName = (*LoadGameInstance->CharacterStats.LevelName);

		SwitchLevel(LevelName);
	}
}

//no switch load game
void AMainCharacter::LoadGameNoSwitch()
{
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));//load the slot
	if (LoadGameInstance)
	{
		//player stats
		Health = LoadGameInstance->CharacterStats.Health;
		MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
		Stamina = LoadGameInstance->CharacterStats.Stamina;
		MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
		Coins = LoadGameInstance->CharacterStats.Coins;

		if (WeaponStorage)
		{
			AItemStorage* Weapon = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);//spwan the stored weapon
			if (Weapon)
			{
				FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;
				//spawn new bp for the saved weapon
				if (WeaponName != TEXT(""))
				{
					AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapon->WeaponMap[WeaponName]);
					WeaponToEquip->Equip(this);//equip the weapon
				}
			}
		}

		//movement status
		SetMovementStatus(EMovementStatus::EMS_Normal);//after reload
		GetMesh()->bPauseAnims = false;// resume animation
		GetMesh()->bNoSkeletonUpdate = false;// resume skeleton updates
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawn.h"
#include <Components/StaticMeshComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Math/UnrealMathUtility.h>
#include <Kismet/KismetMathLibrary.h>
#include <Components/ArrowComponent.h>

#include "Tankogeddon.h"
#include "TankPlayerController.h"
#include "Cannon.h"

// Sets default values
ATankPawn::ATankPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank body"));
	RootComponent = BodyMesh;

    TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank turret"));
    TurretMesh->SetupAttachment(BodyMesh);

    CannonSetupPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Cannon setup point"));
    CannonSetupPoint->AttachToComponent(TurretMesh, FAttachmentTransformRules::KeepRelativeTransform);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
    SpringArm->SetupAttachment(BodyMesh);
    SpringArm->bDoCollisionTest = false;
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritYaw = false;
    SpringArm->bInheritRoll = false;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
}

void ATankPawn::MoveForward(float AxisValue)
{
    TargetForwardAxisValue = AxisValue;
}

void ATankPawn::MoveRight(float AxisValue)
{
    TargetRightAxisValue = AxisValue;
}

void ATankPawn::RotateRight(float AxisValue)
{
    TargetRotateRightAxisValue = AxisValue;
}

// Called when the game starts or when spawned
void ATankPawn::BeginPlay()
{
	Super::BeginPlay();
	
    TankController = Cast<ATankPlayerController>(GetController());
    SetupCannon();
}

void ATankPawn::SetupCannon()
{
    if (Cannon)
    {
        Cannon->Destroy();
        Cannon = nullptr;
    }

    FActorSpawnParameters Params;
    Params.Instigator = this;
    Params.Owner = this;
    Cannon = GetWorld()->SpawnActor<ACannon>(CannonClass, Params);
    Cannon->AttachToComponent(CannonSetupPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

// Called every frame
void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    FVector CurrentLocation = GetActorLocation();
    FVector ForwardVector = GetActorForwardVector();
    FVector RightVector = GetActorRightVector();
    FVector MovePosition = CurrentLocation + ForwardVector * MoveSpeed * TargetForwardAxisValue * DeltaTime;
    SetActorLocation(MovePosition, true);
    FVector MovePosition2 = MovePosition + RightVector * MoveSpeed * TargetRightAxisValue * DeltaTime;
    SetActorLocation(MovePosition2, true);
   
    //Tank rotation
    CurrentRotateRightAxisValue = FMath::Lerp(CurrentRotateRightAxisValue, TargetRotateRightAxisValue, RotationSnootheness);
    
    UE_LOG(LogTankogeddon, Verbose, TEXT("CurrentRotateRightAxisValue = %f TargetRotateRightAxisValue = %f"), CurrentRotateRightAxisValue, TargetRotateRightAxisValue);
   
    FRotator CurrentRotation = GetActorRotation();
    float YawRotation = CurrentRotateRightAxisValue * RotationSpeed * DeltaTime;
    YawRotation += CurrentRotation.Yaw;

    FRotator NewRotation = FRotator(0.f, YawRotation, 0.f);
    SetActorRotation(NewRotation);

    //Turret rotation
    if (TankController)
    {
        FVector MousePos = TankController->GetMousePos();
        FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), MousePos);
        FRotator CurrRotation = TurretMesh->GetComponentRotation();
        TargetRotation.Pitch = CurrRotation.Pitch;
        TargetRotation.Roll = CurrRotation.Roll;
        TurretMesh->SetWorldRotation(FMath::Lerp(CurrRotation, TargetRotation, TurretRotationSmootheness));
    }
}

    void ATankPawn::Fire()
    {
        if (Cannon) 
        {
            Cannon->Fire();
        }
    }

    void ATankPawn::FireSpecial()
    {
        if (Cannon)
        {
            Cannon->FireSpecial();
        }
    }

    


#include "CPlayer.h"
#include "Global.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/CAttributeComponent.h"
#include "Components/COptionComponent.h"
#include "Components/CMontagesComponent.h"
#include "Components/CActionComponent.h"
#include "Actions/CActionData.h"
#include "Demo/CBoxBase_Chest.h"
#include "UI/CKeyWidget.h"

ACPlayer::ACPlayer()
{
	//Spring Arm Comp
	CHelpers::CreateSceneComponent(this, &SpringArmComp, "SpringArmComp", GetMesh());
	SpringArmComp->SetRelativeLocation(FVector(0, 0, 140));
	SpringArmComp->AddRelativeRotation(FRotator(0, 90, 0));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->bEnableCameraLag = true;

	//Camera Comp
	CHelpers::CreateSceneComponent(this, &CameraComp, "CameraComp", SpringArmComp);

	//Mesh Comp
	USkeletalMesh* MeshAsset;
	CHelpers::GetAsset(&MeshAsset, "/Game/Character/Mesh/SK_Mannequin");
	GetMesh()->SetSkeletalMesh(MeshAsset);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	TSubclassOf<UAnimInstance> AnimClass;
	CHelpers::GetClass(&AnimClass, "/Game/Player/ABP_CPlayer");
	GetMesh()->SetAnimInstanceClass(AnimClass);

	//Action Comp
	CHelpers::CreateActorComponent(this, &ActionComp, "ActionComp");

	//Montages Comp
	CHelpers::CreateActorComponent(this, &MontagesComp, "MontagesComp");

	//Attribute Comp
	CHelpers::CreateActorComponent(this, &AttributeComp, "AttributeComp");

	//Option Comp
	CHelpers::CreateActorComponent(this, &OptionComp, "OptionComp");

	//State Comp
	CHelpers::CreateActorComponent(this, &StateComp, "StateComp");

	//Movement Comp
	GetCharacterMovement()->MaxWalkSpeed = AttributeComp->GetSprintSpeed();
	GetCharacterMovement()->RotationRate = FRotator(0, 720, 0);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;


	bReadyToOpenChest = false;

	//CHelpers::GetClass(&KeyWidgetClass, "/Game/UI/WB_Color_Key");
}

void ACPlayer::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &ACPlayer::BeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ACPlayer::EndOverlap);

	//Set DYnamic Material
	BodyMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), nullptr);
	LogoMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(1), nullptr);

	GetMesh()->SetMaterial(0, BodyMaterial);
	GetMesh()->SetMaterial(1, LogoMaterial);

	//On StateType Changed
	StateComp->OnStateTypeChanged.AddDynamic(this, &ACPlayer::OnStateTypeChanged);
	ActionComp->SetUnarmedMode();

	if (KeyWidgetClass)
	{
		KeyWidget = CreateWidget<UCKeyWidget>(GetController<APlayerController>(), KeyWidgetClass);
		KeyWidget->AddToViewport();
		KeyWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void ACPlayer::SetBodyColor(FLinearColor InColor)
{
	BodyMaterial->SetVectorParameterValue("BodyColor", InColor);
	LogoMaterial->SetVectorParameterValue("BodyColor", InColor);
}

void ACPlayer::SetDoOpenChest()
{
	bReadyToOpenChest = true;
}

void ACPlayer::SetDoNotOpenChest()
{
	bReadyToOpenChest = false;
}

void ACPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACPlayer::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACPlayer::OnMoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ACPlayer::OnTurn);
	PlayerInputComponent->BindAxis("LookUp", this, &ACPlayer::OnLookUp);
	PlayerInputComponent->BindAxis("Zoom", this, &ACPlayer::OnZoom);

	PlayerInputComponent->BindAction("Evade", IE_Pressed, this, &ACPlayer::OnEvade);
	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &ACPlayer::OnWalk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &ACPlayer::OffWalk);

	PlayerInputComponent->BindAction("Fist", IE_Pressed, this, &ACPlayer::OnFist);
	PlayerInputComponent->BindAction("OneHand", IE_Pressed, this, &ACPlayer::OnOneHand);
	PlayerInputComponent->BindAction("TwoHand", IE_Pressed, this, &ACPlayer::OnTwoHand);
	PlayerInputComponent->BindAction("MagicBall", IE_Pressed, this, &ACPlayer::OnMagicBall);
	PlayerInputComponent->BindAction("Warp", IE_Pressed, this, &ACPlayer::OnWarp);
	PlayerInputComponent->BindAction("WhirlWind", IE_Pressed, this, &ACPlayer::OnWhirlWind);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ACPlayer::OnInteract);
}

void ACPlayer::OnMoveForward(float Axis)
{
	CheckFalse(AttributeComp->IsCanMove());

	FRotator ControlRot = FRotator(0, GetControlRotation().Yaw, 0);
	FVector Direction = FQuat(ControlRot).GetForwardVector();

	AddMovementInput(Direction, Axis);
}

void ACPlayer::OnMoveRight(float Axis)
{
	CheckFalse(AttributeComp->IsCanMove());

	FRotator ControlRot = FRotator(0, GetControlRotation().Yaw, 0);
	FVector Direction = FQuat(ControlRot).GetRightVector();

	AddMovementInput(Direction, Axis);
}

void ACPlayer::OnTurn(float Axis)
{
	float Rate = Axis * OptionComp->GetMouseXSpeed()* GetWorld()->GetDeltaSeconds();

	AddControllerYawInput(Rate);
}

void ACPlayer::OnLookUp(float Axis)
{
	float Rate = Axis * OptionComp->GetMouseYSpeed() * GetWorld()->GetDeltaSeconds();

	AddControllerPitchInput(Rate);
}

void ACPlayer::OnZoom(float Axis)
{
	float Rate = OptionComp->GetZoomSpeed()* Axis* GetWorld()->GetDeltaSeconds();

	SpringArmComp->TargetArmLength += Rate;
	SpringArmComp->TargetArmLength = FMath::Clamp(SpringArmComp->TargetArmLength, OptionComp->GetZoomMin(), OptionComp->GetZoomMax());
}

void ACPlayer::OnEvade()
{
	CheckFalse(StateComp->IsIdleMode());
	CheckFalse(AttributeComp->IsCanMove());

	if (InputComponent->GetAxisValue("MoveForward") < 0)
	{
		StateComp->SetBackstempMode();
		return;
	}

	StateComp->SetRollMode();
}

void ACPlayer::OnWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = AttributeComp->GetWalkSpeed();
}

void ACPlayer::OffWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = AttributeComp->GetSprintSpeed();
}

void ACPlayer::OnFist()
{
	CheckFalse(StateComp->IsIdleMode());

	ActionComp->SetFistMode();
}

void ACPlayer::OnOneHand()
{
	CheckFalse(StateComp->IsIdleMode());

	ActionComp->SetOneHandMode();
}

void ACPlayer::OnTwoHand()
{
	CheckFalse(StateComp->IsIdleMode());

	ActionComp->SetTwoHandMode();
}

void ACPlayer::OnMagicBall()
{
	CheckFalse(StateComp->IsIdleMode());

	ActionComp->SetMagicBallMode();
}

void ACPlayer::OnWarp()
{
	CheckFalse(StateComp->IsIdleMode());

	ActionComp->SetWarpMode();
}

void ACPlayer::OnWhirlWind()
{
	CheckFalse(StateComp->IsIdleMode());

	ActionComp->SetWhirlWindMode();
}

void ACPlayer::OnInteract()
{
	FLinearColor Color;
	FString String;
	String = IsRedKey() ? "Red" : "null";
	String.Append(" : ");
	String.Append(IsGreenKey() ? "Green" : "null");
	String.Append(" : ");
	String.Append(IsBlueKey() ? "Blue" : "null");

	CLog::Print(String);

	if (bReadyToOpenChest)
	{
		if (Chest)
		{
			if (!Chest->IsOpened())
			{
				Chest->Open();
				Color = Chest->GetEmissive();
				if (Color == FVector(50, 0, 0))
				{
					bRedKey = true;
					//CLog::Print("Red!");
				}
				else if (Color == FVector(0, 50, 0))
				{
					bGreenKey = true;
					//CLog::Print("Green!");
				}
				else if (Color == FVector(0, 0, 50))
				{
					bBlueKey = true;
					//CLog::Print("Blue!");
				}
			}
		}

	}
}


void ACPlayer::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	Chest = Cast<ACBoxBase_Chest>(OtherActor);
}

void ACPlayer::EndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	Chest = nullptr;
}

void ACPlayer::Begin_Roll()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	RollingRotation();

	MontagesComp->PlayRoll();
}

void ACPlayer::Begin_Backstep()
{
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	MontagesComp->PlayBackstep();
}

void ACPlayer::RollingRotation()
{
	FVector Start, Target;
	FRotator Rotation;

	Start = GetActorLocation();

	if (GetVelocity().IsNearlyZero())
	{
		const FRotator& ControlRotation = FRotator(0, GetControlRotation().Yaw, 0);
		const FVector& ControlForward = FQuat(ControlRotation).GetForwardVector();

		Target = Start + ControlForward;
	}
	else
	{
		Target = Start + GetVelocity().GetSafeNormal2D();
	}

	Rotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
	SetActorRotation(Rotation);
}

void ACPlayer::End_Roll()
{
	UCActionData* CurrentDA = ActionComp->GetCurrentDataAsset();

	if (CurrentDA && CurrentDA->EquipmentData.bUseControlRotation)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	StateComp->SetIdleMode();
}

void ACPlayer::End_Backstep()
{
	UCActionData* CurrentDA = ActionComp->GetCurrentDataAsset();

	if (CurrentDA && !CurrentDA->EquipmentData.bUseControlRotation)
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}

	StateComp->SetIdleMode();
}

void ACPlayer::OnStateTypeChanged(EStateType InPrevType, EStateType InNewType)
{
	switch (InNewType)
	{
	case EStateType::Roll:
	{
		Begin_Roll();
	}
	break;

	case EStateType::Backstep:
	{
		Begin_Backstep();
	}
	break;
	}
}
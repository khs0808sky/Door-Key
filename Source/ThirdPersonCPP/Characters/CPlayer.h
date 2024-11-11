#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CStateComponent.h"
#include "Interfaces/CCharacterInterface.h"
#include "CPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCAttributeComponent;
class UCOptionComponent;
class UCStateComponent;
class UCMontagesComponent;
class UCActionComponent;
class UMaterialInstanceDynamic;
class ACBoxBase_Chest;
class UCKeyWidget;

UCLASS()
class THIRDPERSONCPP_API ACPlayer : public ACharacter, public ICCharacterInterface
{
	GENERATED_BODY()

public:
	ACPlayer();

	void SetBodyColor(FLinearColor InColor)	override;

	FORCEINLINE bool IsReadyToOpenChest() { return bReadyToOpenChest; }
	FORCEINLINE bool IsRedKey() { return bRedKey; }
	FORCEINLINE bool IsGreenKey() { return bGreenKey; }
	FORCEINLINE bool IsBlueKey() { return bBlueKey; }

	void SetDoOpenChest();
	void SetDoNotOpenChest();

protected:
	virtual void BeginPlay() override;


public:	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	void OnMoveForward(float Axis);
	void OnMoveRight(float Axis);
	void OnTurn(float Axis);
	void OnLookUp(float Axis);
	void OnZoom(float Axis);

	void OnEvade();
	void OnWalk();
	void OffWalk();

	void OnFist();
	void OnOneHand();
	void OnTwoHand();
	void OnMagicBall();
	void OnWarp();
	void OnWhirlWind();

	void OnInteract();
	
private:

	UFUNCTION()
		void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
		void EndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void Begin_Roll();
	void Begin_Backstep();

	void RollingRotation();

public:
	void End_Roll();
	void End_Backstep();

private:
	UFUNCTION()
	void OnStateTypeChanged(EStateType InPrevType, EStateType InNewType);

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UCAttributeComponent* AttributeComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UCOptionComponent* OptionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UCStateComponent* StateComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UCMontagesComponent* MontagesComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UCActionComponent* ActionComp;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UCKeyWidget> KeyWidgetClass;

private:
	UMaterialInstanceDynamic* BodyMaterial;
	UMaterialInstanceDynamic* LogoMaterial;

	bool bReadyToOpenChest;

	ACBoxBase_Chest* Chest;
	UCKeyWidget* KeyWidget;

protected:
	UPROPERTY(BlueprintReadOnly)
		bool bRedKey;

	UPROPERTY(BlueprintReadOnly)
		bool bGreenKey;

	UPROPERTY(BlueprintReadOnly)
		bool bBlueKey;
};
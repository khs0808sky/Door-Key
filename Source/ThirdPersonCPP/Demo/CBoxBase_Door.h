#pragma once

#include "CoreMinimal.h"
#include "Collision/CBoxBase.h"
#include "CBoxBase_Door.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;

UCLASS()
class THIRDPERSONCPP_API ACBoxBase_Door : public ACBoxBase
{
	GENERATED_BODY()
	
public:
	ACBoxBase_Door();

	FORCEINLINE bool IsOpened() { return bOpened; }
	FORCEINLINE FLinearColor GetBaseColor() { return BaseColor; }

	void Open();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

private:

	UFUNCTION()
		void BeginOverlap
		(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);

	UFUNCTION()
		void EndOverlap
		(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex
		);
		
		
protected:
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UBoxComponent* Box;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* Door;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* DoorFrame;

private:
	UPROPERTY(EditInstanceOnly, Category = "BaseColor")
	FLinearColor BaseColor;


private:
	UMaterialInstanceDynamic* DynamicMaterial;

	bool bOpened;
};

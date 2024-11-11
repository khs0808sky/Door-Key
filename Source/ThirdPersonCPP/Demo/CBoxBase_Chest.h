#pragma once

#include "CoreMinimal.h"
#include "Collision/CBoxBase.h"
#include "CBoxBase_Chest.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;

UCLASS()
class THIRDPERSONCPP_API ACBoxBase_Chest : public ACBoxBase
{
	GENERATED_BODY()

public:
	ACBoxBase_Chest();

	FORCEINLINE bool IsOpened() { return bOpened; }
	FORCEINLINE FLinearColor GetEmissive() { return Emissive; }

	void Open();


protected:
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
		UStaticMeshComponent* Top;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Components")
		UStaticMeshComponent* Bottom;

	UPROPERTY(EditAnywhere, Category = "Color")
		FLinearColor Emissive;

private:
	UMaterialInstanceDynamic* EmissiveColor;

	bool bOpened;

};

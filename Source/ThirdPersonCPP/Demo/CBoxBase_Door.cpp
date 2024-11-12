#include "CBoxBase_Door.h"
#include "Global.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Characters/CPlayer.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"

ACBoxBase_Door::ACBoxBase_Door()
{
	BaseColor = FLinearColor(0, 0, 0);

	CHelpers::CreateSceneComponent(this, &Box, "Box");
	Box->SetRelativeScale3D(FVector(4));
	Box->bHiddenInGame = false;

	CHelpers::CreateSceneComponent(this, &Door, "Door", Box);
	CHelpers::CreateSceneComponent(this, &DoorFrame, "DoorFrame", Box);

	UStaticMesh* DoorMesh;
	CHelpers::GetAsset(&DoorMesh, "/Game/Material/SM_Door");
	UStaticMesh* DoorFrameMesh;
	CHelpers::GetAsset(&DoorFrameMesh, "/Game/Material/SM_DoorFrame");

	Door->SetStaticMesh(DoorMesh);
	DoorFrame->SetStaticMesh(DoorFrameMesh);

	Door->SetRelativeScale3D(FVector(0.25f));
	DoorFrame->SetRelativeScale3D(FVector(0.25f));

	Door->SetRelativeLocation(FVector(0, 11.5f, -32.5f));
	DoorFrame->SetRelativeLocation(FVector(0, 0, -32.5f));

	bOpened = false;
}

void ACBoxBase_Door::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UMaterialInterface* Material;
	UMaterialInterface* Material2;
	CHelpers::GetAssetDynamic(&Material, "/Game/Material/MI_DoorFrame");
	CHelpers::GetAssetDynamic(&Material2, "/Game/Material/MI_Door");

	if (Material)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(Material, nullptr);

		if (DynamicMaterial)
		{
			DoorFrame->SetMaterial(0, DynamicMaterial);

			DynamicMaterial->SetVectorParameterValue("BaseColor", BaseColor);
		}
	}
}

void ACBoxBase_Door::Open()
{
	if (bOpened == false)
	{
		bOpened = true;
		Door->SetRelativeRotation(FRotator(0, -90, 0));
		CLog::Print("Open!");
	}
}

void ACBoxBase_Door::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &ACBoxBase_Door::BeginOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &ACBoxBase_Door::EndOverlap);
}

void ACBoxBase_Door::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACPlayer* Player = Cast<ACPlayer>(OtherActor);

	//Close
	if (!bOpened)
	{
		CLog::Print("Ready to Open Door!", -1);
		Player->SetDoOpenDoor();
		if (BaseColor == FVector(1, 0, 0))
		{
			if (Player->IsRedKey())
			{
				DrawDebugString(GetWorld(), FVector(50, 0, 0), "Press 'F' to open", this, FColor::White, 3.f, true, 1.f);
				return;
			}
			else
			{
				DrawDebugString(GetWorld(), FVector(50, 0, 0), "You don't have a red key!", this, FColor::White, 3.f, true, 1.f);
				return;
			}
		}
		else if (BaseColor == FVector(0, 1, 0))
		{
			if (Player->IsGreenKey())
			{
				DrawDebugString(GetWorld(), FVector(50, 0, 0), "Press 'F' to open", this, FColor::White, 3.f, true, 1.f);
				return;
			}
			else
			{
				DrawDebugString(GetWorld(), FVector(50, 0, 0), "You don't have a green key!", this, FColor::White, 3.f, true, 1.f);
				return;
			}
		}
		else if (BaseColor == FVector(0, 0, 1))
		{
			if (Player->IsBlueKey())
			{
				DrawDebugString(GetWorld(), FVector(50, 0, 0), "Press 'F' to open", this, FColor::White, 3.f, true, 1.f);
				return;
			}
			else
			{
				DrawDebugString(GetWorld(), FVector(50, 0, 0), "You don't have a blue key!", this, FColor::White, 3.f, true, 1.f);
				return;
			}
		}
	}
}

void ACBoxBase_Door::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACPlayer* Player = Cast<ACPlayer>(OtherActor);
	Player->SetDoNotOpenDoor();

	if (!bOpened)
	{
		CLog::Print("Not Ready to Open Door!", -1);
	}

}
#include "CBoxBase_Chest.h"
#include "Global.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Characters/CPlayer.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"

ACBoxBase_Chest::ACBoxBase_Chest()
{
	SymbolColor = FLinearColor(0, 0, 0);

	CHelpers::CreateSceneComponent(this, &Box, "Box");
	Box->SetRelativeScale3D(FVector(4));
	Box->bHiddenInGame = false;

	CHelpers::CreateSceneComponent(this, &Top, "Top", Box);
	CHelpers::CreateSceneComponent(this, &Bottom, "Bottom", Box);

	UStaticMesh* TopMesh;
	CHelpers::GetAsset(&TopMesh, "/Game/Material/SM_ChestTop");
	UStaticMesh* BottomMesh;
	CHelpers::GetAsset(&BottomMesh, "/Game/Material/SM_ChestBottom");

	Top->SetStaticMesh(TopMesh);
	Bottom->SetStaticMesh(BottomMesh);

	Top->SetRelativeScale3D(FVector(0.2f));
	Bottom->SetRelativeScale3D(FVector(0.2f));

	Top->SetRelativeLocation(FVector(-10, 0, -20));
	Bottom->SetRelativeLocation(FVector(-10, 0, -20));

	bOpened = false;
}

void ACBoxBase_Chest::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UMaterialInterface* Material;
	CHelpers::GetAssetDynamic(&Material, "/Game/Material/MI_Chest");

	if (Material)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(Material, nullptr);

		if (DynamicMaterial)
		{
			Top->SetMaterial(0, DynamicMaterial);
			Bottom->SetMaterial(0, DynamicMaterial);

			DynamicMaterial->SetVectorParameterValue("SymbolColor", SymbolColor);
		}
	}
}

void ACBoxBase_Chest::Open()
{
	if (bOpened == false)
	{
		bOpened = true;
		Top->SetRelativeRotation(FRotator(110, 0, 0));
		CLog::Print("Open!");
	}
}

void ACBoxBase_Chest::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &ACBoxBase_Chest::BeginOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &ACBoxBase_Chest::EndOverlap);

	//Set Dynamic Material
	/*EmissiveColor = UMaterialInstanceDynamic::Create(Top->GetMaterial(0), nullptr);

	EmissiveColor->SetVectorParameterValue("SymbolColor", Emissive);

	Top->SetMaterial(0, EmissiveColor);
	Bottom->SetMaterial(0, EmissiveColor);*/
}

void ACBoxBase_Chest::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACPlayer* Player = Cast<ACPlayer>(OtherActor);

	if (!bOpened)
	{
		Player->SetDoOpenChest();
		DrawDebugString(GetWorld(), FVector(50, 0, 0), "Press 'F' to open", this, FColor::White, 3.f, true, 1.f);
		CLog::Print(Player->IsReadyToOpenChest() ? "Ready!" : "Not Ready!", -1);
	}
}

void ACBoxBase_Chest::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACPlayer* Player = Cast<ACPlayer>(OtherActor);

	if (!bOpened)
	{
		Player->SetDoNotOpenChest();
		CLog::Print(Player->IsReadyToOpenChest() ? "Ready!" : "Not Ready!", -1);
	}

}


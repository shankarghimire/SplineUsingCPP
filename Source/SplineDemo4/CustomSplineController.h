// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "CustomSplineController.generated.h"

UENUM(BlueprintType)
enum class ESplineMeshType:uint8
{
	DEFAULT		UMETA(DisplayName = "Default Mesh"),
	START		UMETA(DisplayName = "Starting Mesh"),
	END			UMETA(DisplayName = "Ending Mesh"),
};

USTRUCT(BlueprintType)
struct FSplineMeshDetails : public FTableRowBase
{
	GENERATED_BODY()

	

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class UMaterialInterface* DefaultMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class UMaterialInterface* AlternativeMaterial;
	FSplineMeshDetails() : ForwardAxis(ESplineMeshAxis::Type::X)
	{
	}
		
};



UCLASS()
class SPLINEDEMO4_API ACustomSplineController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACustomSplineController();

	void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
public:
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "CustomSplineController")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomSplineController")
		TMap<ESplineMeshType, FSplineMeshDetails> SplineMeshMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,  Category = "CustomSplineController")
		USplineComponent* SplineComponent;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "CustomSplineController")
		TSubclassOf<class AActor>ActorToMoveClass;
	class AActor* ActorToMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomSplineController")
		float TotalPathTimeController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomSplineController")
		bool bSplineInLoop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomSplineController")
		bool bCanMoveActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomSplineController")
		float StartTime;

};

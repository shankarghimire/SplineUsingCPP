// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasicSplineController.generated.h"

UCLASS()
class SPLINEDEMO4_API ABasicSplineController : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SplineController")
		USceneComponent* SceneComponentRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SplineController")
		class USplineComponent* Spline;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SplineController")
		TSubclassOf<class AActor> ActorToMoveClass;
		class AActor* ActorToMove;

protected:
	UPROPERTY(EditAnywhere, Category = "SplineController")
		float TotalPathTimeController;
	UPROPERTY(EditAnywhere, Category = "SplineController")
		bool bSplineInLoop;
	UPROPERTY(EditAnywhere, Category = "SplineController")
		bool bCanMoveActor;
	UPROPERTY(EditAnywhere, Category = "SplineController")
		float StartTime;

public:	
	// Sets default values for this actor's properties
	ABasicSplineController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

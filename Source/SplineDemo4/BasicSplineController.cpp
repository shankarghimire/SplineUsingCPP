// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicSplineController.h"
#include "Components/SplineComponent.h"

// Sets default values
ABasicSplineController::ABasicSplineController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponentRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneComponentRoot;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(SceneComponentRoot);
	Spline->Duration = TotalPathTimeController;
	Spline->bDrawDebug = true;

	TotalPathTimeController = 10.0f;
	bCanMoveActor = false;

}

// Called when the game starts or when spawned
void ABasicSplineController::BeginPlay()
{
	Super::BeginPlay();
	
	if (ActorToMoveClass != nullptr)
	{
		ActorToMove = GetWorld()->SpawnActor<AActor>(ActorToMoveClass, Spline->GetComponentTransform());

		if (ActorToMove != nullptr)
		{
			StartTime = GetWorld()->GetTimeSeconds();
			bCanMoveActor = true;
		}
	}
}

// Called every frame
void ABasicSplineController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ((ActorToMove != nullptr) && (bCanMoveActor))
	{
		float CurrentSplineTime = (GetWorld()->GetTimeSeconds() - StartTime) / TotalPathTimeController;
		float Distance = Spline->GetSplineLength() * CurrentSplineTime;

		//Set World Position
		FVector Position = Spline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		ActorToMove->SetActorLocation(Position);

		//Set World Rotation 
		FVector Direction = Spline->GetDirectionAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

		//Create Rotator given a vector direction
		FRotator Rotator = FRotationMatrix::MakeFromX(Direction).Rotator();
		ActorToMove->SetActorRotation(Rotator);

		//Reach the end
		if (CurrentSplineTime >= 1.0f)
		{
			if (bSplineInLoop)
			{
				bCanMoveActor = true;
				StartTime = true;
				StartTime = GetWorld()->GetTimeSeconds();
				CurrentSplineTime = (GetWorld()->GetTimeSeconds() - StartTime) / TotalPathTimeController;
			}
		}
	}
}


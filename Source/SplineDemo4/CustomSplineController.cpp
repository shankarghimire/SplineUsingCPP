// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomSplineController.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

// Sets default values
ACustomSplineController::ACustomSplineController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	if (SceneRoot != nullptr)
	{
		RootComponent = SceneRoot;

		SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
		if (SplineComponent != nullptr)
		{
			SplineComponent->SetupAttachment(SceneRoot);

			//Setup up some properties
			TotalPathTimeController = 10.f;
			SpeedController_01 = 10.f;
			SpeedController_02 = 5.f;
			bCanMoveActor = true;
			StartTime = 0.f;

		}
		
	}

	//if (SplineComponent)
	//{
	//	SetRootComponent(SplineComponent);
	//}

}

// Called when the game starts or when spawned
void ACustomSplineController::BeginPlay()
{
	Super::BeginPlay();
	if (ActorToMoveClass != nullptr)
	{
		ActorToMove = GetWorld()->SpawnActor<AActor>(ActorToMoveClass, SplineComponent->GetComponentTransform());

		if (ActorToMove != nullptr)
		{
			//Time when the press Play button
			StartTime = GetWorld()->GetTimeSeconds();
			bCanMoveActor = true;

			//Get Spline Length
			SplineTotalLength = SplineComponent->GetSplineLength();
			UE_LOG(LogTemp, Warning, TEXT("Total Spline Length is %f "), SplineTotalLength);

			int32 TotalSplinePoints = SplineComponent->GetNumberOfSplinePoints();
			UE_LOG(LogTemp, Warning, TEXT("Total Spline Points is %d "), TotalSplinePoints);

			int32 TotalSplineSegments = SplineComponent->GetNumberOfSplineSegments();
			UE_LOG(LogTemp, Warning, TEXT("Total Spline Segments is %d "), TotalSplineSegments);
		}
	}
	
	

}

// Called every frame
void ACustomSplineController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//To Move the Actor along the spline track
	if (ActorToMove != nullptr && bCanMoveActor)
	{
		float TimeVal1 = (GetWorld()->GetTimeSeconds() - StartTime);
		UE_LOG(LogTemp, Warning, TEXT("Time Val1 %f"), TimeVal1);
		CurrentSplineTime = (TimeVal1 / TotalPathTimeController);
	/*	if (CurrentSplineTime <= 0.5)
		{

			CurrentSplineTime = (TimeVal1 / TotalPathTimeController) ;
		}
		else
		{
			CurrentSplineTime = TimeVal1 / TotalPathTimeController + (TimeVal1 / SpeedController_01);
		}*/
		//CurrentSplineTime = (TimeVal1 / TotalPathTimeController) + (TimeVal1 / 5.0f);
		//UE_LOG(LogTemp, Warning, TEXT("TotalPathTimeController %f"), TotalPathTimeController);
		UE_LOG(LogTemp, Warning, TEXT("CurrentSplineTime %f"), CurrentSplineTime);

		float Val1 = SplineComponent->GetSplineLength();
		UE_LOG(LogTemp, Warning, TEXT("Val1 %f"), Val1);

		Distance = Val1 * CurrentSplineTime;

		UE_LOG(LogTemp, Warning, TEXT("Current Distance %f"), Distance);

		FVector Position = SplineComponent->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

		ActorToMove->SetActorLocation(Position);

		FVector Direction = SplineComponent->GetDirectionAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);

		FRotator Rotator = FRotationMatrix::MakeFromX(Direction).Rotator();

		ActorToMove->SetActorRotation(Rotator);

		//Reach the end
		if (CurrentSplineTime >= 1.f)
		{
			if (bSplineInLoop)
			{
				bCanMoveActor = true;
				StartTime = GetWorld()->GetTimeSeconds();
				CurrentSplineTime = (GetWorld()->GetTimeSeconds() - StartTime) / TotalPathTimeController;
			}
		}
	}

}


void ACustomSplineController::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (SplineComponent && SplineMeshMap.Num() > 0)
	{
		// lookup all pertinent values
		FSplineMeshDetails* StartMeshDetails = nullptr;
		if (SplineMeshMap.Contains(ESplineMeshType::START))
		{
			StartMeshDetails = SplineMeshMap.Find(ESplineMeshType::START);
		}

		FSplineMeshDetails* EndMeshDetails = nullptr;
		if (SplineMeshMap.Contains(ESplineMeshType::END))
		{
			EndMeshDetails = SplineMeshMap.Find(ESplineMeshType::END);
		}

		FSplineMeshDetails* DefaultMeshDetails = nullptr;
		if (SplineMeshMap.Contains(ESplineMeshType::DEFAULT))
		{
			DefaultMeshDetails = SplineMeshMap.Find(ESplineMeshType::DEFAULT);
		}
		else
		{
			// exit if we don't have a default mesh to work with
			return;
		}

		const int32 SplinePoints = SplineComponent->GetNumberOfSplinePoints();

		for (int SplineCount = 0; SplineCount < (SplinePoints - 1); SplineCount++)
		{
			USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());

			UStaticMesh* StaticMesh = DefaultMeshDetails->Mesh;
			UMaterialInterface* Material = nullptr;
			ESplineMeshAxis::Type ForwardAxis = DefaultMeshDetails->ForwardAxis;

			// start mesh
			if (StartMeshDetails && StartMeshDetails->Mesh && SplineCount == 0)
			{
				StaticMesh = StartMeshDetails->Mesh;
				ForwardAxis = StartMeshDetails->ForwardAxis;

				if (StartMeshDetails->DefaultMaterial)
				{
					Material = StartMeshDetails->DefaultMaterial;
				}
			}
			else if (EndMeshDetails && EndMeshDetails->Mesh && SplinePoints > 2 && SplineCount == (SplinePoints - 2))
			{
				// end mesh
				StaticMesh = EndMeshDetails->Mesh;
				ForwardAxis = EndMeshDetails->ForwardAxis;

				if (EndMeshDetails->DefaultMaterial)
				{
					Material = EndMeshDetails->DefaultMaterial;
				}
			}
			else
			{
				// default assignment - middle mesh
				if (DefaultMeshDetails->AlternativeMaterial && SplineCount > 0 && SplineCount % 2 == 0)
				{
					Material = DefaultMeshDetails->AlternativeMaterial;
				}
				else if (DefaultMeshDetails->DefaultMaterial)
				{
					Material = DefaultMeshDetails->DefaultMaterial;
				}
			}

			// update mesh details
			SplineMesh->SetStaticMesh(StaticMesh);
			SplineMesh->SetForwardAxis(ForwardAxis, true);
			SplineMesh->SetMaterial(0, Material);


			// initialize the object
			SplineMesh->RegisterComponentWithWorld(GetWorld());

			SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			SplineMesh->SetMobility(EComponentMobility::Movable);

			SplineMesh->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

			// define the positions of the points and tangents
			const FVector StartPoint = SplineComponent->GetLocationAtSplinePoint(SplineCount, ESplineCoordinateSpace::Type::Local);
			const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(SplineCount, ESplineCoordinateSpace::Type::Local);
			const FVector EndPoint = SplineComponent->GetLocationAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Type::Local);
			const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Type::Local);
			SplineMesh->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, true);

			// query physics
			SplineMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		}
	}
}

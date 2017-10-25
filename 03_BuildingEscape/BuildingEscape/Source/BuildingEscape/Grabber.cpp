// Copyright Remi Talbot

#include "Grabber.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Public/DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

/// Look for attached Physic Handle
void UGrabber::FindPhysicsHandleComponent()
{
	PhysicHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("Physics Handle not found on %s"), *GetOwner()->GetName());
	}
}

/// Look for the attached input component (only appear at runtime)
void UGrabber::SetupInputComponent()
{
	InputComponentHandle = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponentHandle)
	{
		InputComponentHandle->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponentHandle->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Input Handle isn't found on %s"), *GetOwner()->GetName());
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//If the physics handle is attached
	if (PhysicHandle->GrabbedComponent)
	{
		// move the object that we're holding
		PhysicHandle->SetTargetLocation(GetReachLineEnd());
	}
}

void UGrabber::Grab()
{
	/// Line trace and reach any actors with physics body collision channel set
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent(); // gets the mesh that we are moving
	AActor* ActorHit = HitResult.GetActor();

	/// If we hit something, then attach a physics handle
	if (ActorHit)
	{
		PhysicHandle->GrabComponent(
			ComponentToGrab,
			NAME_None, // no bones needed
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true // allow rotation
			);
	}
}

void UGrabber::Release()
{
	PhysicHandle->ReleaseComponent();
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	/// Line-trace (Ray-cast) out to reach distance
	FHitResult Hit;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	///See what we hit; NOTE When getting actor; if actor is NULL, UNREAL WILL CRASH
	if (Hit.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("You just hit: %s"), *Hit.GetActor()->GetName());
	}

	return Hit;
}

FVector UGrabber::GetReachLineStart()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation;
}

FVector UGrabber::GetReachLineEnd()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}
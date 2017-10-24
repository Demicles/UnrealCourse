// Copyright Remi Talbot

#include "Grabber.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Public/DrawDebugHelpers.h"

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
	if (PhysicHandle)
	{
		//Physics handle is found
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Physics Handle not found on %s"), *GetOwner()->GetName());
	}
}

/// Look for the input component
void UGrabber::SetupInputComponent()
{
	InputComponentHandle = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponentHandle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Input Handle found!"));
		///Bind the input axis
		InputComponentHandle->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponentHandle->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Input Handle isn't found on %s"), *GetOwner()->GetName());
	}
}


void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab pressed!"));

	/// Line trace and reach any actors with physics body collision channel set
	GetFirstPhysicsBodyInReach();

	/// If we hit something, then attach a physics handle
	// TODO Attach physics handle
}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab released!"));
	// TODO release physics handle
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//If the physics handle is attached
		// move the object that we're holding


}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	/// Get the player view point this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	/// Setup query parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	/// Line-trace (Ray-cast) out to reach distance
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	///See what we hit; NOTE When getting actor; if actor is NULL, UNREAL WILL CRASH
	if (Hit.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("You just hit: %s"), *Hit.GetActor()->GetName());
	}

	return FHitResult();
}

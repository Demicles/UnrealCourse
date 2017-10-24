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

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	
	/// Look for attached Physic Handle
	PhysicHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicHandle)
	{
		//Physics handle is found
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Physics Handle not found on %s"), *GetOwner()->GetName());
	}

	/// Look for the input component
	InputComponentHandle = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponentHandle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Input Handle found!"));
		///Bind the input axis
		InputComponentHandle->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Input Handle isn't found on %s"), *GetOwner()->GetName());
	}

}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab pressed!"));
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/// Get the player view point this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	//Log out to test
	/*UE_LOG(LogTemp, Warning, TEXT("Player ViewPoint is Location: %s Rotation: %s"),
		*PlayerViewPointLocation.ToString(),
		*PlayerViewPointRotation.ToString()
	);*/

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	///Draw a red trace in the world to visualize if in debug mode
	if (bDebugActive == true)
	{
		DrawDebugLine(
			GetWorld(),
			PlayerViewPointLocation,
			LineTraceEnd,
			FColor(255, 0, 0),
			false,
			0.f,
			0.f,
			10.f
		);
	}

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
}


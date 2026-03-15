// Copyright Epic Games, Inc. All Rights Reserved.

#include "GD_2D_prj1Character.h"
#include "EnhancedInputSubsystems.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "PickUpActor.h"
#include "Camera/CameraComponent.h"



DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);

//////////////////////////////////////////////////////////////////////////
// AGD_2D_prj1Character

AGD_2D_prj1Character::AGD_2D_prj1Character()
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	Inventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(
		this,
		&AGD_2D_prj1Character::OnOverlapBegin
	);

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	

	// Create an orthographic camera (no perspective) and attach it to the boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	SideViewCameraComponent->OrthoWidth = 2048.0f;
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	CameraBoom->SetUsingAbsoluteRotation(true);
	SideViewCameraComponent->bUsePawnControlRotation = false;
	SideViewCameraComponent->bAutoActivate = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

    // 	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
    // 	TextComponent->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));
    // 	TextComponent->SetRelativeLocation(FVector(35.0f, 5.0f, 20.0f));
    // 	TextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    // 	TextComponent->SetupAttachment(RootComponent);

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;
}

//////////////////////////////////////////////////////////////////////////
// Animation
void AGD_2D_prj1Character::CoyoteJump()
{
	if (GetCharacterMovement()->IsMovingOnGround() || bCanUseCoyoteTime)
	{
		// Reset Coyote Time
		bCanUseCoyoteTime = false;
		TimeSinceLeftGround = 0.0f;

		// Perform the jump
		LaunchCharacter(FVector(0.f, 0.f, GetCharacterMovement()->JumpZVelocity), false, true);
	}
}
void AGD_2D_prj1Character::UpdateCoyoteTime(float DeltaTime)
{
	if (!GetCharacterMovement()->IsMovingOnGround())
	{
		TimeSinceLeftGround += DeltaTime;
		bCanUseCoyoteTime = (TimeSinceLeftGround <= CoyoteTimeDuration);
	}
	else
	{
		TimeSinceLeftGround = 0.0f;
		bCanUseCoyoteTime = false;
	}
}
void AGD_2D_prj1Character::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	TimeSinceLeftGround = 0.0f;
	bCanUseCoyoteTime = false;
}
void AGD_2D_prj1Character::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	// Are we moving or standing still?
	UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? RunningAnimation : IdleAnimation;
	if( GetSprite()->GetFlipbook() != DesiredAnimation 	)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void AGD_2D_prj1Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateCharacter();
	UpdateCoyoteTime(DeltaSeconds);
}


//////////////////////////////////////////////////////////////////////////
// Input

void AGD_2D_prj1Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// 1. Get the Player Controller
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		// 2. Get the Enhanced Input Local Player Subsystem
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// 3. Add the Mapping Context
			if (InputMapping)
			{
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(InputMapping, 0);
			}
		}
	}

	// 4. Bind the Actions
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Movement
		if (IaMove)
		{
			EnhancedInput->BindAction(IaMove, ETriggerEvent::Triggered, this, &AGD_2D_prj1Character::MoveRight);
		}

		// Jump (Standard Character Jump)
		if (IaJump)
		{
			EnhancedInput->BindAction(IaJump, ETriggerEvent::Started, this, &AGD_2D_prj1Character::CoyoteJump);
			EnhancedInput->BindAction(IaJump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
	}
}

void AGD_2D_prj1Character::MoveRight(const FInputActionValue& Value)
{
	/*UpdateChar();*/

	// Apply the input to the character motion
	float MovementValue = Value.Get<float>();

	AddMovementInput(FVector(1.f, 0.f, 0.f), MovementValue);
}

void AGD_2D_prj1Character::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Jump on any touch
	CoyoteJump();
}

void AGD_2D_prj1Character::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Cease jumping once touch stopped
	StopJumping();
}

void AGD_2D_prj1Character::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector PlayerVelocity = GetVelocity();	
	float TravelDirection = PlayerVelocity.X;
	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}
void AGD_2D_prj1Character::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapped"));

	if (!OtherActor || OtherActor == this) return;

	// Print all tags
	for (const FName& Tag : OtherActor->Tags)
	{
		UE_LOG(LogTemp, Warning, TEXT("Collided with tag: %s"), *Tag.ToString());
	}

	// === PICKUP LOGIC ===
	if (OtherActor->ActorHasTag("Pickup"))
	{
		APickUpActor* Pickup = Cast<APickUpActor>(OtherActor);
		if (Pickup && Pickup->PickUpAsset)
		{
			bool success = Inventory->AddItem(Pickup->PickUpAsset->ItemName);
			UE_LOG(LogTemp, Warning,
				TEXT("Was the item added to inventory: %s : %s"),
				*Pickup->PickUpAsset->ItemName,
				success ? TEXT("true") : TEXT("false"));

			Pickup->Destroy();
		}
	}

	// === ENEMY LOGIC ===
	if (OtherActor->ActorHasTag("Enemy"))
	{
		if (Inventory && Inventory->CheckHasItem("HolyWater", 1))
		{
			// Remove one Holy Water
			bool removed = Inventory->RemoveItem("HolyWater", 1);

			// Destroy skeleton
			OtherActor->Destroy();

			int remaining = Inventory->GetQuantityOfItem("HolyWater");
			UE_LOG(LogTemp, Warning,
				TEXT("Destroyed skeleton using Holy Water! Holy Water left: %d, removed successfully: %s"),
				remaining, removed ? TEXT("true") : TEXT("false"));
		}
		else
		{
			// Player does not have Holy Water: bounce back
			FVector BounceDirection = GetActorLocation() - OtherActor->GetActorLocation();
			BounceDirection.Normalize();
			FVector Impulse = BounceDirection * 2500.0f;
			GetCharacterMovement()->AddImpulse(Impulse, true);

			UE_LOG(LogTemp, Warning, TEXT("Hit skeleton without Holy Water! Bouncing back."));
		}
	}
}


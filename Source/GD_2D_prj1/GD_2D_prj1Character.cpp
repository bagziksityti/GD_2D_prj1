// Copyright Epic Games, Inc. All Rights Reserved.

#include "GD_2D_prj1Character.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
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
	// Enable overlap events
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	// Bind overlap event
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(
		this,
		&AGD_2D_prj1Character::OnOverlapBegin
	);
	Inventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));

}

//////////////////////////////////////////////////////////////////////////
// Animation
void AGD_2D_prj1Character::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}
}

void AGD_2D_prj1Character::UpdateAnimation(UPaperFlipbook* animation)
{
	if (GetSprite()->GetFlipbook() != animation)
	{
		GetSprite()->SetFlipbook(animation);
	}
}

void AGD_2D_prj1Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// update the players state
	UpdateState();
	// update the state functionality
	HandleState();
	//UpdateCharacter();	
}



//////////////////////////////////////////////////////////////////////////
// Input

void AGD_2D_prj1Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(IaMove, ETriggerEvent::Triggered, this, &AGD_2D_prj1Character::MoveRight);
		EnhancedInput->BindAction(IaJump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInput->BindAction(IaJump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
}

void AGD_2D_prj1Character::MoveRight(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>(); // Extract the float from the InputActionValue
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisValue);
}

void AGD_2D_prj1Character::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Jump on any touch
	Jump();
}

void AGD_2D_prj1Character::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Cease jumping once touch stopped
	StopJumping();
}

void AGD_2D_prj1Character::UpdateCharacter()
{
	// Update animation to match the motion
	//UpdateAnimation();

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
void AGD_2D_prj1Character::UpdateState()
{
	const FVector PlayerVelocity = GetVelocity();
	float HorizontalSpeed = FMath::Abs(PlayerVelocity.X);

	switch (CharacterState)
	{
	case ECharacterState::Idle:
		if (PlayerVelocity.Z > 0.1) CharacterState = ECharacterState::Jumping;
		else if (HorizontalSpeed > 0.1) CharacterState = ECharacterState::Running;
		break;

	case ECharacterState::Running:
		if (PlayerVelocity.Z > 0.1) CharacterState = ECharacterState::Jumping;
		else if (PlayerVelocity.Z < -0.1) CharacterState = ECharacterState::Falling;
		else if (HorizontalSpeed < 0.1) CharacterState = ECharacterState::Idle;
		break;

	case ECharacterState::Jumping:
		if (PlayerVelocity.Z < -0.1) CharacterState = ECharacterState::Falling;
		else if (PlayerVelocity.Z < 0.1) CharacterState = ECharacterState::Idle;
		break;

	case ECharacterState::Falling:
		if (FMath::Abs(PlayerVelocity.Z) < 0.1) CharacterState = ECharacterState::Idle;
		break;

	case ECharacterState::Dead:
		break;
	}
}
void AGD_2D_prj1Character::HandleState()
{
	switch (CharacterState)
	{
	case ECharacterState::Idle:
		UpdateAnimation(IdleAnimation);
		break;

	case ECharacterState::Running:
		UpdateAnimation(RunningAnimation);
		UpdateCharacter();  // Handles movement
		break;

	case ECharacterState::Jumping:
		UpdateAnimation(JumpingAnimation);
		UpdateCharacter();
		break;

	case ECharacterState::Falling:
		UpdateAnimation(FallingAnimation);
		UpdateCharacter();
		break;

	case ECharacterState::Dead:
		UpdateAnimation(DeadAnimation);
		break;
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
	if (OtherActor && (OtherActor != this))
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap detected"));

		// Print tags (debug)
		for (const FName& Tag : OtherActor->Tags)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tag: %s"), *Tag.ToString());
		}

		// ENEMY COLLISION
		if (OtherActor->ActorHasTag("Enemy"))
		{
			// Calculate bounce direction
			FVector BounceDirection = GetActorLocation() - OtherActor->GetActorLocation();
			BounceDirection.Normalize();

			// Apply force
			FVector Impulse = BounceDirection * 2500.0f;

			// Push player
			GetCharacterMovement()->AddImpulse(Impulse, true);
		}
	}
}
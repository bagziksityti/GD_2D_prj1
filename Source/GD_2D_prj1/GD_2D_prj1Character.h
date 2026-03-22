// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "InputActionValue.h"          
#include "InputMappingContext.h" 
#include "Components/PrimitiveComponent.h"
#include "Inventory.h"
#include "GD_2D_prj1Character.generated.h"

class UTextRenderComponent;
class UPaperFlipbook;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;

/** Enum to track character states */
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    Idle       UMETA(DisplayName = "Idle"),
    Running    UMETA(DisplayName = "Running"),
    Jumping    UMETA(DisplayName = "Jumping"),
    Falling    UMETA(DisplayName = "Falling"),
    Dead       UMETA(DisplayName = "Dead")
};

UCLASS(config = Game)
class AGD_2D_prj1Character : public APaperCharacter
{
    GENERATED_BODY()

public:
    AGD_2D_prj1Character();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerAttributes|Inventory")
    UInventory* Inventory;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* IaMove;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* IaJump;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnhancedInput")
    UInputMappingContext* InputMapping;

    /** Returns SideViewCameraComponent subobject **/
    FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }

    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    /** Character’s current state */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    ECharacterState CharacterState;

protected:
    /** Side view camera */
    virtual void BeginPlay() override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* SideViewCameraComponent;

    /** Camera boom positioning the camera beside the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    /** Optional text component */
    UTextRenderComponent* TextComponent;

    /** Called every frame */
    virtual void Tick(float DeltaSeconds) override;

    /** Animation flipbooks */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
    UPaperFlipbook* IdleAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
    UPaperFlipbook* RunningAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
    UPaperFlipbook* JumpingAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
    UPaperFlipbook* FallingAnimation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
    UPaperFlipbook* DeadAnimation;

    /** Update the animation based on the current state */
    void UpdateAnimation(UPaperFlipbook* animation);

    /** Side-to-side input */
    void MoveRight(const FInputActionValue& Value);

    /** Update rotation, movement, and facing direction */
    void UpdateCharacter();

    /** Handle touch inputs */
    void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);
    void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

    /** Setup player inputs */
    virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

private:
    /** Update the current state based on velocity or conditions */
    void UpdateState();

    /** Handle behavior and animations for the current state */
    void HandleState();
};
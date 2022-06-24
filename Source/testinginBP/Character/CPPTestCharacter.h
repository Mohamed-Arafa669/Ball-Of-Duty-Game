// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "LockOnTargetComponent.h"
#include "testinginBP/Ball/CPPBall.h"
#include "NiagaraComponent.h"
#include "testinginBP/HUD/UI/UI_RespawnWidget.h"
#include "CPPTestCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);


UCLASS()
class TESTINGINBP_API ACPPTestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACPPTestCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //Replication
	virtual void PostInitializeComponents() override;
	virtual void Jump() override;;

	
	UPROPERTY(Replicated)
	bool bDoingAbility;
	
	UPROPERTY(Replicated)
		bool bKnocked;


	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, EditDefaultsOnly, Category = "Health")
		float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, VisibleAnywhere, Category = "Health")
		float CurrentHealth;


	void Knocked(FVector ImpulseDirection, bool bPlayerLeftGame);

	virtual void LockTarget();
	void ClearTarget();
	void DoEmotes(UAnimMontage* Emote);

	void PlayThrowMontage();

	UPROPERTY(EditAnywhere, Category = Camera)
		class UCameraComponent* followCamera;
	UPROPERTY(EditAnywhere, Category = Camera)
		class USpringArmComponent* cameraBoom;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
		class ULockOnTargetComponent* lockOnTargets;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
		class UTargetingHelperComponent* targetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		AActor* FoundTarget;

	UFUNCTION(BlueprintCallable)
	void ThrowButtonPressed();

	//Lock On Target
	ULockOnTargetComponent* LockOnTargetComponent;

	UPROPERTY(VisibleAnywhere)
		class UCombatComponent* combat;
	UFUNCTION(Client, Reliable)
		void ClientRespawnCountDown(float seconds);

	UPROPERTY()
	class ACPPPlayerController* CPPPlayerController;

	UPROPERTY()
	class AGameHUD* GameHUD;

	UPROPERTY()
	class AMyPlayerState* MyPlayerState;

	UFUNCTION()
		void ResetHealthHUD(float timer);

	UFUNCTION()
		void ResetHealthTest();

protected:
	virtual void BeginPlay() override;

	virtual void Falling() override;
	void MoveForward(float value);
	void MoveRight(float value);
	void Turn(float value);
	void LookUp(float value);
	//void EquipButtonPressed();
	void Catch();
	void Dash();
	void CanDash();
	void CanCatch();
	void ThrowButtonReleased();
	void StunCoolDown();
	void OnHealthUpdate();
	void UpdateHUDHealth();
	void Elim();
	UFUNCTION(NetMulticast, Unreliable,WithValidation)
	void MultiKnocked(FVector ImpulseDirection ,bool bPlayerLeftGame);
	auto MultiKnocked_Validate(FVector ImpulseDirection,bool bPlayerLeftGame) -> bool;
	void MultiKnocked_Implementation(FVector ImpulseDirection,bool bPlayerLeftGame);
	// Poll for any relivant classes and inits the HUD
	void PollInit();

	

	UPROPERTY(EditAnywhere)
	float DashCoolDownDuration = 0.7f;

	UPROPERTY(Replicated)
	bool bIsTargeting;

	USkeletalMeshComponent* CharacterMesh;
	
	UPROPERTY(EditAnywhere, Category = "CameraShake")
	TSubclassOf<class UCameraShakeBase> CamShake;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* overHeadWidget;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* throwAnim;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* DashAnim;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* CatchAnim;
	

	//UPROPERTY(ReplicatedUsing = OnRep_OverlappingBall) //Replication
	//	class ACPPBall* overlappingBall;

	void CallDestroy();

	UFUNCTION()
	void OnRep_CurrentHealth();

	//UFUNCTION()
	//void OnRep_OverlappingBall(ACPPBall* lastBall); //Replication

	UPROPERTY(VisibleAnywhere)
	class UGameplayStatics* gameStatic;

	UPROPERTY(VisibleAnywhere)
	class UWorld* world;

	/*UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();*/

	UFUNCTION(Server, Reliable)
		void ServerCatch();

	UFUNCTION(Server, Reliable)
		void ServerThrowButtonReleased();

	UFUNCTION(Server, Reliable)
	void DashButtonPressed(FVector DashDir);

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
			bool bFromSweep, const FHitResult& SweepResult);

public:

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* GetHitAnim;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Animations, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* ClappingAnim;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Emotes, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* Emote1;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Emotes, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* Emote2;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Emotes, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* Emote3;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Emotes, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* Emote4;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Emotes, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* Emote5;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Emotes, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* Emote6;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadOnly, Category = Emotes, meta = (AllowPrivateAccess = "true"))
		class UAnimMontage* Emote7;

	void DoEmote1();
	void DoEmote2();
	void DoEmote3();
	void DoEmote4();
	void DoEmote5();
	void DoEmote6();
	void DoEmote7();


	UFUNCTION(Server, Unreliable, WithValidation, Category = Animation)
		void ServerPlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	UFUNCTION(NetMulticast, Unreliable, WithValidation, Category = Animation)
		void MulticastPlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	UFUNCTION(Server, Unreliable, WithValidation, Category = Animation)
		void ServerStopAnimMontage();

	UFUNCTION(NetMulticast, Unreliable, WithValidation, Category = Animation)
		void MulticastStopAnimMontage();

	UFUNCTION(Server, Unreliable, WithValidation, Category = Effects)
		void ServerPlayNiagara(UNiagaraComponent* fx, bool state);

	UFUNCTION(NetMulticast, Unreliable, WithValidation, Category = Effects)
		void MulticastPlayNiagara(UNiagaraComponent* fx, bool state);

	FTimerHandle ElimTimer;

	void DoEmote();

	UPROPERTY(EditDefaultsOnly)
		float ElimDelay = 3.f;
	void ElimTimerFinished();

	bool bLeftGame = false;

public:

	UFUNCTION(Server, Reliable)
		void ServerLeaveGame();

	FOnLeftGame OnLeftGame;
	
	void Stunned();

	/* void SetOverlappingBall(ACPPBall* cppBall);*/

	
	 bool IsBallEquipped();

	UPROPERTY(Replicated)
	 bool bIsSpawnInvincible;

	UPROPERTY(Replicated)
		bool Dancing;

	void SetSpawnInvincibility();

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetIsSpawnInvincibleState() const { return bIsSpawnInvincible; }

	UPROPERTY(EditAnywhere, Category = "Respawn")
		float SpawnInvincibilityDuration = 2.f;

	UPROPERTY(EditAnywhere, Category = "Effects")
		UNiagaraComponent* HitEffect;

	 bool bCanDash;

	 bool bThrown;

	 bool bCanThrow;

	UPROPERTY(Replicated)
	 bool bCatching;

	UPROPERTY(Replicated)
		bool bSteal;

	UPROPERTY(EditAnywhere, Replicated)
		bool bIsDashing;

	 UPROPERTY(Replicated)
	 bool bEquipped;
	 UPROPERTY(Replicated)
	 bool bStunned;

	 UPROPERTY(Replicated)
		 bool bSlerps;

	 UPROPERTY(Replicated)
		 bool bisLocked;


	 UFUNCTION()
		 virtual void MyThrow();

	 void StopThrow();

	 bool IsAllowedToMove();

	 UFUNCTION(BlueprintPure, Category = "Health")
		 FORCEINLINE float GetMaxHealth() const { return MaxHealth; }


	 UFUNCTION(BlueprintPure, Category = "Health")
		 FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	 UFUNCTION(BlueprintPure, Category = "Abilities")
		 FORCEINLINE bool GetStunnedState() const { return bStunned; }

	 UFUNCTION(BlueprintPure, Category = "Abilities")
		 FORCEINLINE bool GetDashingState() const { return bIsDashing; }

	 void SetCurrentHealth(float healthValue);

	 UFUNCTION(BlueprintCallable, Category = "Health")
	virtual float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
  
	UPROPERTY(EditAnywhere, Replicated, Category = "Movement")
	 float DashDistance = 6000.f;

	FVector testVect;

	UPROPERTY(EditAnywhere, Category = "Throw")
		float throwPower = 500.0f;
	UPROPERTY(EditAnywhere)
		float HitImpulse = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Catching")
		float CatchCooldown = 1.f;

	UPROPERTY(EditAnywhere, Category = "Abilities")
		float StunDuration = 10.f;

	UPROPERTY(Replicated)
	FVector ballHitDirection;

	
	UPROPERTY(EditAnywhere)
		TSubclassOf<UUI_RespawnWidget> RespawingCountWidgetClass;

	UPROPERTY()
	UUI_RespawnWidget* RespawingWidget;

	UFUNCTION()
	void RemoveWidget(UUI_RespawnWidget* MsgToRemove);

	UPROPERTY(EditAnywhere)
		float DashAnimDuration = 0.3;
	void SetDashingAnimOff();
	//void SpawnActors();

public:

		FORCEINLINE UCombatComponent* GetCombat() const { return combat; }

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* DashFX;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* LockFX;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* AbilityFX;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* StunFX;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* DustFX;

	UPROPERTY(VisibleAnywhere, Category = Shaders)
		UMaterialInstanceDynamic* DynamicInvincibleMatInst;

	UPROPERTY(EditAnywhere, Category = Shaders)
		UMaterialInstance* InvincibleMaterialInstance;

	UPROPERTY(VisibleAnywhere, Category = Shaders)
		UMaterialInstanceDynamic* DynamicLockedMatInst;

	UPROPERTY(EditAnywhere, Category = Shaders)
		UMaterialInstance* LockedMaterialInstance;

	UMaterialInterface* OriginalMat1;
	UMaterialInterface* OriginalMat2;
	UMaterialInterface* OriginalMat3;

	UMaterialInterface* TargetOriginalMat1;
	UMaterialInterface* TargetOriginalMat2;
	UMaterialInterface* TargetOriginalMat3;

	UPROPERTY(EditAnywhere, Category = "Aiming")
		float SensetivityX = 40;

	UPROPERTY(EditAnywhere, Category = "Aiming")
		float SensetivityY = 40.f;


	///Sound Cue
	UPROPERTY(EditAnywhere, Category = "SFX")
		USoundCue* Hit_Cue;

	UPROPERTY(EditAnywhere, Category = "SFX")
		USoundCue* AbilityWithBall;

	UPROPERTY(EditAnywhere, Category = "SFX")
		USoundCue* AbilityWithoutBall;

	UFUNCTION()
		void PlaySounds(USoundCue* Cue, FVector Location);

	UFUNCTION(Server, Unreliable)
		void ServerPlaySounds(USoundCue* Cue, FVector Location);

	UFUNCTION(NetMulticast, Unreliable, WithValidation)
		void MulticastPlaySounds(USoundCue* Cue, FVector Location);

	UPROPERTY(EditAnywhere, Category = "Movement")
		float StrafeMultiplier = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Movement")
		float BackwardMultiplier = 0.4f;



};




// Fill out your copyright notice in the Description page of Project Settings.


#include "RestartWidget.h"
#include "DodgeballPlayerController.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void URestartWidget::OnRestartClicked()
{
	ADodgeballPlayerController* PlayerController = Cast<ADodgeballPlayerController>(GetOwningPlayer());
	if (PlayerController != nullptr)
	{
		PlayerController->HideRestartWidget();
	}
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}

void URestartWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (RestartButton != nullptr)
	{
		RestartButton->OnClicked.AddDynamic(this, &URestartWidget::OnRestartClicked);
	}
	if (ExitButton != nullptr)
	{
		ExitButton->OnClicked.AddDynamic(this, &URestartWidget::OnExitClicked);
	}
}

void URestartWidget::OnExitClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
}

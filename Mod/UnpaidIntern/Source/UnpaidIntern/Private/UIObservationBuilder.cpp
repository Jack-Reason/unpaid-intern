// Copyright (c) 2026 Jack Reason. MIT License.
#include "UIObservationBuilder.h"
#include "UIInternSubsystem.h"
#include "UIInputDriver.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

FString UUIObservationBuilder::BuildJson(const UUIInternSubsystem* Sys) const
{
	TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetNumberField(TEXT("tick"), Sys ? Sys->GetWorldTick() : 0);

	TSharedRef<FJsonObject> Session = MakeShared<FJsonObject>();
	Session->SetStringField(TEXT("mode"), TEXT("client"));
	Session->SetStringField(TEXT("topology"), TEXT("second_licence"));
	Session->SetStringField(TEXT("bridge"), TEXT("stub"));
	Root->SetObjectField(TEXT("session"), Session);

	TSharedRef<FJsonObject> Intern = MakeShared<FJsonObject>();
	Intern->SetStringField(TEXT("id"), TEXT("unpaid-intern-1"));
	if (Sys)
	{
		if (UUIInputDriver* Driver = Sys->GetInputDriver())
		{
			Intern->SetStringField(TEXT("job"), Driver->GetJobName());
		}
		if (UWorld* World = Sys->GetWorld())
		{
			if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
			{
				if (APawn* Pawn = PC->GetPawn())
				{
					const FVector Loc = Pawn->GetActorLocation();
					TSharedRef<FJsonObject> Location = MakeShared<FJsonObject>();
					Location->SetNumberField(TEXT("x"), Loc.X);
					Location->SetNumberField(TEXT("y"), Loc.Y);
					Location->SetNumberField(TEXT("z"), Loc.Z);
					Intern->SetObjectField(TEXT("location"), Location);
				}
			}
		}
	}
	Root->SetObjectField(TEXT("intern"), Intern);

	// TODO(M2): host pose from other player states; nearby AFGResourceNode; inventory; chat via UIChatListener.
	Root->SetObjectField(TEXT("host"), MakeShared<FJsonObject>());
	Root->SetArrayField(TEXT("nearby_nodes"), TArray<TSharedPtr<FJsonValue>>());
	Root->SetArrayField(TEXT("chat_inbox"), TArray<TSharedPtr<FJsonValue>>());

	FString Out;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
	FJsonSerializer::Serialize(Root, Writer);
	return Out;
}

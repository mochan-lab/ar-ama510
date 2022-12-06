// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameGrabberActor.h"

#if WITH_EDITOR
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "IAssetViewport.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(GrabImage, Display, All);

// Sets default values
AFrameGrabberActor::AFrameGrabberActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFrameGrabberActor::BeginPlay()
{
	Super::BeginPlay();
	Myuint = 0;
	//StartFrameGrab();
	ConnectPipe();
}

void AFrameGrabberActor::BeginDestroy()
{
	Super::BeginDestroy();

	ReleaseFrameGrabber();

	//if (CaptureFrameTexture)
	//{
	//	CaptureFrameTexture->ConditionalBeginDestroy();
	//	CaptureFrameTexture = nullptr;
	//}
}

// Called every frame
void AFrameGrabberActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Myuint == 1) {
		StartFrameGrab();
	}

	if (FrameGrabber.IsValid())
	{
		Capture();
	}
	Myuint++;
}

void AFrameGrabberActor::Capture()
{
	//if (FrameGrabber.IsValid() && CaptureFrameTexture)
	if (FrameGrabber.IsValid())
	{
		FrameGrabber->CaptureThisFrame(FFramePayloadPtr());
		TArray<FCapturedFrameData> Frames = FrameGrabber->GetCapturedFrames();

		if (Frames.Num())
		{
			FCapturedFrameData& LastFrame = Frames.Last();

			CaptureFrameData.Empty();

			for (int32 i = 0; i < LastFrame.ColorBuffer.Num(); i++)
			{
				//FColor (R,G,B,A)->BGRA
				CaptureFrameData.Add(LastFrame.ColorBuffer[i].R);
				CaptureFrameData.Add(LastFrame.ColorBuffer[i].G);
				CaptureFrameData.Add(LastFrame.ColorBuffer[i].B);
				//CaptureFrameData.Add(LastFrame.ColorBuffer[i].A);
			}

			//UE_LOG(GrabImage, Display, TEXT("%s"), L"myLog");

			//auto Region = new FUpdateTextureRegion2D(0, 0, 0, 0, LastFrame.BufferSize.X, LastFrame.BufferSize.Y);
			//CaptureFrameTexture->UpdateTextureRegions(0, 1, Region, 4 * LastFrame.BufferSize.X, 4, CaptureFrameData.GetData());
			if (TestPipe != INVALID_HANDLE_VALUE)
			{
				if (WritePipe() == false)
				{
					FPlatformMisc::RequestExit(false);
				}
			}
		}
	}
}

bool AFrameGrabberActor::StartFrameGrab()
{
	FString key = "resw";
	resw = GetCommandLine(key);
	key = "resh";
	resh = GetCommandLine(key);

	TSharedPtr<FSceneViewport> SceneViewport;

	//Get SceneViewport
	// ( quoted from FRemoteSessionHost::OnCreateChannels() )
#if WITH_EDITOR
	if (GIsEditor)
	{
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			if (Context.WorldType == EWorldType::PIE)
			{
				FSlatePlayInEditorInfo* SlatePlayInEditorSession = GEditor->SlatePlayInEditorMap.Find(Context.ContextHandle);
				if (SlatePlayInEditorSession)
				{
					if (SlatePlayInEditorSession->DestinationSlateViewport.IsValid())
					{
						TSharedPtr<IAssetViewport> DestinationLevelViewport = SlatePlayInEditorSession->DestinationSlateViewport.Pin();
						SceneViewport = DestinationLevelViewport->GetSharedActiveViewport();
					}
					else if (SlatePlayInEditorSession->SlatePlayInEditorWindowViewport.IsValid())
					{
						SceneViewport = SlatePlayInEditorSession->SlatePlayInEditorWindowViewport;
					}
				}
			}
		}
	}
	else
#endif
	{
		UGameEngine* GameEngine = Cast<UGameEngine>(GEngine);
		SceneViewport = GameEngine->SceneViewport;
	}
	if (!SceneViewport.IsValid())
	{
		return false;
	}

	//Setup Texture
	//if (!CaptureFrameTexture)
	//{
	//	CaptureFrameTexture = UTexture2D::CreateTransient(SceneViewport.Get()->GetSize().X, SceneViewport.Get()->GetSize().Y, PF_B8G8R8A8);
	//	CaptureFrameTexture->UpdateResource();

	//	//MaterialInstanceDynamic->SetTextureParameterValue(FName("Texture"), CaptureFrameTexture);
	//}

	// Start Capture
	ReleaseFrameGrabber();
	FrameGrabber = MakeShareable(new FFrameGrabber(SceneViewport.ToSharedRef(), SceneViewport->GetSize()));
	FrameGrabber->StartCapturingFrames();

	return true;
}

void AFrameGrabberActor::StopFrameGrab()
{
	ReleaseFrameGrabber();
}

void AFrameGrabberActor::ReleaseFrameGrabber()
{
	if (FrameGrabber.IsValid())
	{
		FrameGrabber->StopCapturingFrames();
		FrameGrabber->Shutdown();
		FrameGrabber.Reset();
	}
}

void AFrameGrabberActor::SetMaterialInstanceDynamic(UMaterialInstanceDynamic* MI)
{
	//MaterialInstanceDynamic = MI;
}

int32 AFrameGrabberActor::GetCommandLine(FString& myKey)
{
	FString val;
	bool isSuccess = FParse::Value(FCommandLine::Get(), *myKey, val);
	if (!isSuccess)
	{
		if (myKey == "resw")val = "1280";
		else if (myKey == "resh")val = "720";
		else if (myKey == "pipe")val = "0";
	}

	return FCString::Atoi(*val);
}

void AFrameGrabberActor::ConnectPipe()
{
	LPCWSTR CPipeName;
	FString key = "pipe";
	switch (GetCommandLine(key))
	{
	case 0:
		CPipeName = TEXT("\\\\.\\pipe\\testpipe");
		break;
	case 1:
		CPipeName = TEXT("\\\\.\\pipe\\pipe1");
		break;
	case 2:
		CPipeName = TEXT("\\\\.\\pipe\\pipe2");
		break;
	case 3:
		CPipeName = TEXT("\\\\.\\pipe\\pipe3");
		break;
	case 4:
		CPipeName = TEXT("\\\\.\\pipe\\pipe4");
		break;
	case 5:
		CPipeName = TEXT("\\\\.\\pipe\\pipe5");
		break;
	default:
		CPipeName = TEXT("\\\\.\\pipe\\testpipe");
		break;
	}

	TestPipe = INVALID_HANDLE_VALUE;
	
	while (1)
	{
		TestPipe = CreateFile(
			CPipeName,
			GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

		if (TestPipe != INVALID_HANDLE_VALUE)
			break;
		if (GetLastError() != ERROR_PIPE_BUSY)
			return;
	}
	return;
}

bool AFrameGrabberActor::WritePipe()
{
	BOOL fSuccess = false;
	fSuccess = WriteFile(
		TestPipe,
		CaptureFrameData.GetData(),
		//CaptureFrameData.Num() * sizeof(uint8),
		resw*resh*3* sizeof(uint8),
		NULL,
		NULL
	);
	return fSuccess;
}

void AFrameGrabberActor::ClosePipe()
{

}
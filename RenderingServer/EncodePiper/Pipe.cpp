#include "Pipe.h"

std::shared_ptr<HANDLE> thisPipe;
bool isPipeClose;

int ConnectPipe(int resw, int resh, int pipeNum, int portNum)
{
    bool isPipeConnect;

    thisPipe = std::make_shared<HANDLE>(INVALID_HANDLE_VALUE);

    LPCTSTR pipeName;
    switch (pipeNum)
    {
    case 0:
        pipeName = TEXT("\\\\.\\pipe\\testpipe");
        break;
    case 1:
        pipeName = TEXT("\\\\.\\pipe\\pipe1");
        break;
    case 2:
        pipeName = TEXT("\\\\.\\pipe\\pipe2");
        break;
    case 3:
        pipeName = TEXT("\\\\.\\pipe\\pipe3");
        break;
    case 4:
        pipeName = TEXT("\\\\.\\pipe\\pipe4");
        break;
    case 5:
        pipeName = TEXT("\\\\.\\pipe\\pipe5");
        break;
    default:
        pipeName = TEXT("\\\\.\\pipe\\testpipe");
        break;
    }

    //BOOL isPipeConnect = FALSE;
    LPDWORD cBytesRead = 0;
    std::vector<std::uint8_t> receiveBytes(resw*resh*3*sizeof(std::uint8_t));
 
    GstInit();
    MyGst a(resw,resh, portNum);
    //a.SetFilter();
    a.SetImageData(receiveBytes);

    while (1)
    {
        *thisPipe = CreateNamedPipe(
            pipeName,
            PIPE_ACCESS_INBOUND,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
            PIPE_UNLIMITED_INSTANCES,
            0,
            0,
            0,
            NULL);

        //Success create the pipe 
        if (*thisPipe != INVALID_HANDLE_VALUE)
            break;

        if (GetLastError() != ERROR_PIPE_BUSY)
        {
            printf("Could not open pipe. GLE=%d\n", GetLastError());
            return -1;
        }
    }

    printf("Waiting client connection.\n");
    isPipeConnect = ConnectNamedPipe(*thisPipe, NULL) ?
        TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (isPipeConnect)
    {
        a.StartPlay();
    }
    isPipeClose = false;
    while (isPipeConnect && !isPipeClose)
    {
        isPipeConnect = ReadFile(
            *thisPipe,
            receiveBytes.data(),
            receiveBytes.size(),
            cBytesRead,
            NULL);

        if (!isPipeConnect)
        {
            if (GetLastError() == ERROR_BROKEN_PIPE)
                printf("Client disconneted.\n");
            else
                printf("ReadFile failed, GLE=%d\n", GetLastError());

            break;
        }

        //printf("%c", receiveBytes[1]);
    }

    printf("The pipe disconncted.\n");
    CloseHandle(*thisPipe);

    return 0;
}

int ForceClosePipe()
{
    CloseHandle(*thisPipe);
    printf("Cut the pipe.\n");
    return 0;
}

void disPipeConnect()
{
    isPipeClose = true;
}
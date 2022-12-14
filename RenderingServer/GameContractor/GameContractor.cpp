#include "GameContractor.h"

std::string MyPromoter::PromoterIP = "APIServer_IP";
unsigned short MyPromoter::PromoterPort = 80;
std::string MyPromoter::PromoterHost = "APIServer_Host";

std::string MySkyway::apikey = "SkyWay_APIkey";
std::string MySkyway::skywayHost = "SkyWay_GateWay";
std::string MyUserClass::EncodePiperUrl = "path_to_EncodePiper.exe";

std::string MyUserClass::GameEngineUrlNight = "path_to_GameEngineForNight.exe";

std::string MyUserClass::GameEngineUrlDay = "path_to_GameEngineForDay.exe";


int main()
{
	int LoopStatus = 1;
	std::vector<MyUserClass> Clients;
	int startRecvPort = 64002;
	std::vector<int> openNums{ -1,0,0,0,0,0 };

	int serverID = 0;
	std::cout << "Please input your Rserver ID" << std::endl;
	std::cin >> serverID;
	std::cout << "Start Contractor with RserverID : " << serverID << "\n" << std::endl; 

	MyHttp::StartWSA();

	MyHttp Gateway("127.0.0.1", 8000);
	MyHttp Promoter(MyPromoter::PromoterIP, MyPromoter::PromoterPort);

	std::string newstartstatus = "START";
	MyPromoter::RserverStatusUpdate(serverID, newstartstatus, Clients.size(), Promoter);

	std::thread connectThread([&]
		{
			int switchNum = 1;

			//MyHttp Gateway("127.0.0.1", 8000);
			//MyHttp Promoter(MyPromoter::PromoterIP, MyPromoter::PromoterPort);

			while (LoopStatus)
			{
				MyHttp Gateway("127.0.0.1", 8000);
				MyHttp Promoter(MyPromoter::PromoterIP, MyPromoter::PromoterPort);

				if (switchNum == 1) { //waitingUser
					MyUserClass newclient;
					int check = 0;
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
					std::string newstatus = "WAITING_NEW_CLIENT";
					MyPromoter::RserverStatusUpdate(serverID, newstatus, Clients.size(), Promoter);
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
					switchNum = MyPromoter::WaitingUser(serverID, Promoter, newclient);

					if (switchNum == 10) {
						Clients.push_back(newclient);

						for (int i = 1; i < 6; i++) {
							if (openNums[i] == 0) {
								openNums[i] = 1;
								Clients.back().SetDRecvPort(startRecvPort + (i*2));
								Clients.back().SetPipeNum(i);
								break;
							}
						}

						check += MySkyway::MMakePeer(Gateway, Clients.back());
						check += MySkyway::DMakePeer(Gateway, Clients.back());
						check += MySkyway::MWaitBindPeer(Gateway, Clients.back());
						check += MySkyway::DWaitBindPeer(Gateway, Clients.back());
						check += MySkyway::MOpenSendPort(Gateway, Clients.back());
						check += MySkyway::DOpenSendPort(Gateway, Clients.back());

						MyPromoter::PleaseCall(serverID, Promoter, Clients.back());

						if (check != 0) {
							newstatus = "LOOKING_FOR_RSERVER";	//error please clean up
							MyPromoter::StatusChange(serverID, newstatus, Promoter, Clients.back());
							MySkyway::MCloseStream(Gateway, Clients.back());
							MySkyway::DCloseConnect(Gateway, Clients.back());
							openNums[Clients.back().PipeNum()] = 0;
							Clients.pop_back();
							
							//newstatus = "WAITING_NEW_CLIENT";
							//MyPromoter::RserverStatusUpdate(serverID, newstatus, Clients.size(), Promoter);
							std::printf("WARN: Gateway open port ERROR\n");
							switchNum = -1;
							continue;
						}

						if (MySkyway::MWaitCall(Gateway, Clients.back())) {
							newstatus = "LOOKING_FOR_RSERVER";	//error please clean up
							MyPromoter::StatusChange(serverID, newstatus, Promoter, Clients.back());
							MySkyway::MCloseStream(Gateway, Clients.back());
							MySkyway::DCloseConnect(Gateway, Clients.back());
							openNums[Clients.back().PipeNum()] = 0;
							Clients.pop_back();
							//newstatus = "WAITING_NEW_CLIENT";
							//MyPromoter::RserverStatusUpdate(serverID, newstatus, Clients.size(), Promoter);
							std::printf("WARN: Gateway open port ERROR\n");
							switchNum = -1;
							continue;
						}

						check += MySkyway::DWaitConnect(Gateway, Clients.back());

						Clients.back().CreateEncodePiper();
						Clients.back().CreateGameEngine();

						check += MySkyway::MAnswerCall(Gateway, Clients.back());
						check += MySkyway::MStartStream(Gateway, Clients.back());
						check += MySkyway::DWaitOpen(Gateway, Clients.back());
						check += MySkyway::DPutParams(Gateway, Clients.back());
						if (check != 0) {
							newstatus = "LOOKING_FOR_RSERVER";  //error please clean up
							MyPromoter::StatusChange(serverID, newstatus, Promoter, Clients.back());
							Clients.back().CloseEncodePiper();
							Clients.back().CloseGameEngine();
							MySkyway::MCloseStream(Gateway, Clients.back());
							MySkyway::DCloseConnect(Gateway, Clients.back());
							openNums[Clients.back().PipeNum()] = 0;
							Clients.pop_back();
							//newstatus = "WAITING_NEW_CLIENT";
							//MyPromoter::RserverStatusUpdate(serverID, newstatus, Clients.size(), Promoter);
							std::printf("WARN: Error soon before start AR\n");
							switchNum = -1;
							continue;
						}
						newstatus = "PERFORMING_AR";
						MyPromoter::StatusChange(serverID, newstatus, Promoter, Clients.back());
						//newstatus = "WAITING_NEW_CLIENT";
						//MyPromoter::RserverStatusUpdate(serverID, newstatus, Clients.size(), Promoter);
						switchNum = -1;
						continue;
					}
					else
					{
						switchNum = -1;
						continue;
					}
				}
				else if(switchNum == -1) // waiting quit
				{
					std::string newdeletestatus = "DELETING_CLIENT";
					MyPromoter::RserverStatusUpdate(serverID, newdeletestatus, Clients.size(), Promoter);
					
					switchNum = MyPromoter::WaitingQuit(serverID, Promoter, Gateway, Clients, openNums);	

					continue;
				}

				switchNum = 1;
				continue;
			}
		});


	while (1)
	{
		std::string line;
		std::cin >> line;
		if (line == "quit")
		{
			std::printf("Please Re-Enter \"quit\"");
			std::string line;
			std::cin >> line;
			if (line == "quit") {
				LoopStatus = 0;
				break;
			}
		}
		std::cout << "unknown command" << std::endl;
	}

	std::printf("STOPPING THE SERVER...\n");

	connectThread.join();

	MyHttp GatewayEnd("127.0.0.1", 8000);
	MyHttp PromoterEnd(MyPromoter::PromoterIP, MyPromoter::PromoterPort);

	MyPromoter::RserverStatusUpdate(serverID, "STOP", Clients.size(), PromoterEnd);
	for (int i = 0; i < Clients.size(); i++) {
		Clients[i].CloseEncodePiper();
		Clients[i].CloseGameEngine();
		MySkyway::MCloseStream(GatewayEnd, Clients[i]);
		MySkyway::DCloseConnect(GatewayEnd, Clients[i]);
		std::string newquitstatus = "CLIENT_QUITED_INVALID";
		MyPromoter::StatusChange(serverID, newquitstatus, PromoterEnd, Clients[i]);
	}
	Clients.clear();
	MyHttp::CleanWSA();

	std::printf("ServerID:%d success stop! Please check the database when you re-start!", serverID);
}
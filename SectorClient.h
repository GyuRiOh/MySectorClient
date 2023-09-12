#pragma once
#include "NetRoot/NetClient/NetClient.h"
#include "MonitorProtocol.h"

#define MESSAGE_MAX 512
#define SECTOR_X_MAX 50
#define SECTOR_Y_MAX 50
#define SECTOR_MONITOR_X_MAX 25
#define SECTOR_MONITOR_Y_MAX 25
#define SECTOR_MONITOR 2
#define GRID_SIZE 30

namespace server_baby
{
	class SectorClient : public NetClient
	{
	public:
		bool OnRecv(NetPacketSet* const packetList)
		{
			switch (packetList->GetType())
			{
			case eNET_RECVED_PACKET_SET:
			{
				while (packetList->GetSize() > 0)
				{
					NetDummyPacket* packet = nullptr;
					if (packetList->Dequeue(&packet) == false)
						CrashDump::Crash();

					if (!PacketProc(packetList->GetSessionID(), packet))
						return false;
				}
				break;
			}
			default:
				CrashDump::Crash();
				break;
			}

			NetPacketSet::Free(packetList);

			return true;
		}

		void OnConnect()
		{
			NetPacket* packet = NetPacket::Alloc();

			*packet << (WORD)en_PACKET_CS_SECTOR_TOOL_REQ_LOGIN;

			SendPacket(packet);
			NetPacket::Free(packet);

		}

		bool PacketProc(NetSessionID sessionID, NetDummyPacket* packet)
		{
			WORD type;
			*packet >> type;
			switch (type)
			{
			case en_PACKET_CS_SECTOR_TOOL_RES_LOGIN:
			{
				return true;
			}

			case en_PACKET_CS_MONITOR_TOOL_DATA_UPDATE:
			{

				BYTE sectorBuffer[SECTOR_Y_MAX][SECTOR_X_MAX] = { 0 };
				packet->DeqData((char*)sectorBuffer, SECTOR_X_MAX * SECTOR_Y_MAX);


				for (int i = 0; i < SECTOR_Y_MAX; i++)
				{
					for (int j = 0; j < SECTOR_X_MAX; j++)
					{
						sectorMap_[i][j] = sectorBuffer[i][j];
					}
				}

				return true;
			}

			}
			return false;
		}

	public:
		//¼½ÅÍ ¸Ê
		DWORD sectorMap_[SECTOR_Y_MAX][SECTOR_X_MAX] = { 0 };

	};
}

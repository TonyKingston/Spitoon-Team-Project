#include <iostream>
#include <unordered_map>

#include "Server_Net_Common.h"

class GameServer : public NCL::net::server_interface<GameMsg>
{
public:
	GameServer(uint16_t nPort) : NCL::net::server_interface<GameMsg>(nPort)
	{
	}

	std::unordered_map<uint32_t, sPlayerDescription> m_mapPlayerRoster;
	std::vector<uint32_t> m_vGarbageIDs;

protected:
	bool OnClientConnect(std::shared_ptr<NCL::net::connection<GameMsg>> client) override
	{
		// For now we will allow all 
		return true;
	}

	void OnClientValidated(std::shared_ptr<NCL::net::connection<GameMsg>> client) override
	{
		// Client passed validation check, so send them a message informing
		// them they can continue to communicate
		NCL::net::message<GameMsg> msg;
		msg.header.id = GameMsg::Client_Accepted;
		client->Send(msg);
	}

	void OnClientDisconnect(std::shared_ptr<NCL::net::connection<GameMsg>> client) override
	{
		if (client)
		{
			if (m_mapPlayerRoster.find(client->GetID()) == m_mapPlayerRoster.end())
			{
				// client never added to roster, so just let it disappear
			}
			else
			{
				auto& pd = m_mapPlayerRoster[client->GetID()];
				std::cout << "[UNGRACEFUL REMOVAL]:" + std::to_string(pd.nUniqueID) + "\n";
				m_mapPlayerRoster.erase(client->GetID());
				m_vGarbageIDs.push_back(client->GetID());
			}
		}

	}

	void OnMessage(std::shared_ptr<NCL::net::connection<GameMsg>> client, NCL::net::message<GameMsg>& msg) override
	{
		if (!m_vGarbageIDs.empty())
		{
			for (auto pid : m_vGarbageIDs)
			{
				NCL::net::message<GameMsg> m;
				m.header.id = GameMsg::Game_RemovePlayer;
				m << pid;
				std::cout << "Removing " << pid << "\n";
				MessageAllClients(m);
			}
			m_vGarbageIDs.clear();
		}



		switch (msg.header.id)
		{
		case GameMsg::Client_RegisterWithServer:
		{
			sPlayerDescription desc;
			msg >> desc;
			desc.nUniqueID = client->GetID();
			m_mapPlayerRoster.insert_or_assign(desc.nUniqueID, desc);

			NCL::net::message<GameMsg> msgSendID;
			msgSendID.header.id = GameMsg::Client_AssignID;
			msgSendID << desc.nUniqueID;
			MessageClient(client, msgSendID);

			NCL::net::message<GameMsg> msgAddPlayer;
			msgAddPlayer.header.id = GameMsg::Game_AddPlayer;
			msgAddPlayer << desc;
			MessageAllClients(msgAddPlayer);

			for (const auto& player : m_mapPlayerRoster)
			{
				NCL::net::message<GameMsg> msgAddOtherPlayers;
				msgAddOtherPlayers.header.id = GameMsg::Game_AddPlayer;
				msgAddOtherPlayers << player.second;
				MessageClient(client, msgAddOtherPlayers);
			}

			break;
		}

		case GameMsg::Client_UnregisterWithServer:
		{
			break;
		}

		case GameMsg::Game_UpdatePlayer:
		{
			// Simply bounce update to everyone except incoming client
			MessageAllClients(msg, client);
			break;
		}

		}

	}

};



int main()
{
	GameServer server(60000);
	server.Start();

	while (1)
	{
		server.Update(-1, true);
	}
	return 0;
}

#include "NetworkManager.h"
#include <iostream>
#include <thread>

namespace Archura {

NetworkManager& NetworkManager::Get() {
    static NetworkManager instance;
    return instance;
}

bool NetworkManager::Init() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return false;
    }
    m_Initialized = true;
    return true;
}

void NetworkManager::Shutdown() {
    if (m_Socket != INVALID_SOCKET) {
        closesocket(m_Socket);
        m_Socket = INVALID_SOCKET;
    }
    
    for (SOCKET client : m_ClientSockets) {
        closesocket(client);
    }
    m_ClientSockets.clear();

    if (m_Initialized) {
        WSACleanup();
        m_Initialized = false;
    }
}

bool NetworkManager::StartServer(int port) {
    if (!m_Initialized) Init();

    m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_Socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        return false;
    }

    // Non-blocking mode
    u_long mode = 1;
    ioctlsocket(m_Socket, FIONBIO, &mode);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(m_Socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(m_Socket);
        return false;
    }

    if (listen(m_Socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(m_Socket);
        return false;
    }

    m_IsServer = true;
    m_IsConnected = true;

    return true;
}

void NetworkManager::UpdateServer() {
    if (!m_IsServer) return;

    // Accept new connections
    SOCKET clientSocket = accept(m_Socket, NULL, NULL);
    if (clientSocket != INVALID_SOCKET) {

        
        // Non-blocking for client
        u_long mode = 1;
        ioctlsocket(clientSocket, FIONBIO, &mode);
        
        m_ClientSockets.push_back(clientSocket);
    }

    // Process clients
    char buffer[1024];
    for (auto it = m_ClientSockets.begin(); it != m_ClientSockets.end(); ) {
        SOCKET client = *it;
        int bytesReceived = recv(client, buffer, sizeof(buffer), 0);
        
        if (bytesReceived > 0) {
            // Broadcast to other clients
            for (SOCKET other : m_ClientSockets) {
                if (other != client) {
                    send(other, buffer, bytesReceived, 0);
                }
            }
            
            // Process locally
            if (bytesReceived >= sizeof(PacketHeader)) {
                PacketHeader* header = (PacketHeader*)buffer;
                if (header->type == PacketType::PlayerUpdate) {
                    if (m_OnPlayerUpdate && bytesReceived >= sizeof(PacketHeader) + sizeof(PlayerUpdatePacket)) {
                        PlayerUpdatePacket* packet = (PlayerUpdatePacket*)(buffer + sizeof(PacketHeader));
                        m_OnPlayerUpdate(*packet);
                    }
                } else if (header->type == PacketType::PlayerShoot) {
                    if (m_OnPlayerShoot && bytesReceived >= sizeof(PacketHeader) + sizeof(PlayerShootPacket)) {
                        PlayerShootPacket* packet = (PlayerShootPacket*)(buffer + sizeof(PacketHeader));
                        m_OnPlayerShoot(*packet);
                    }
                }
            }
            ++it;
        }
        else if (bytesReceived == 0 || (bytesReceived == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)) {

            closesocket(client);
            it = m_ClientSockets.erase(it);
        }
        else {
            ++it;
        }
    }
}

bool NetworkManager::Connect(const std::string& ip, int port) {
    if (!m_Initialized) Init();

    m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_Socket == INVALID_SOCKET) return false;

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (connect(m_Socket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(m_Socket);
        return false;
    }

    // Set non-blocking AFTER connect
    u_long mode = 1;
    ioctlsocket(m_Socket, FIONBIO, &mode);

    m_IsServer = false;
    m_IsConnected = true;

    return true;
}

void NetworkManager::UpdateClient() {
    if (m_IsServer || !m_IsConnected) return;

    char buffer[1024];
    int bytesReceived = recv(m_Socket, buffer, sizeof(buffer), 0);

    if (bytesReceived > 0) {
        if (bytesReceived >= sizeof(PacketHeader)) {
            PacketHeader* header = (PacketHeader*)buffer;
            if (header->type == PacketType::PlayerUpdate) {
                if (m_OnPlayerUpdate && bytesReceived >= sizeof(PacketHeader) + sizeof(PlayerUpdatePacket)) {
                    PlayerUpdatePacket* packet = (PlayerUpdatePacket*)(buffer + sizeof(PacketHeader));
                    m_OnPlayerUpdate(*packet);
                }
            } else if (header->type == PacketType::PlayerShoot) {
                if (m_OnPlayerShoot && bytesReceived >= sizeof(PacketHeader) + sizeof(PlayerShootPacket)) {
                    PlayerShootPacket* packet = (PlayerShootPacket*)(buffer + sizeof(PacketHeader));
                    m_OnPlayerShoot(*packet);
                }
            }
        }
    }
    else if (bytesReceived == 0 || (bytesReceived == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)) {

        m_IsConnected = false;
        closesocket(m_Socket);
        m_Socket = INVALID_SOCKET;
    }
}

void NetworkManager::SendPlayerUpdate(const PlayerUpdatePacket& packet) {
    if (!m_IsConnected) return;

    std::vector<char> buffer;
    PacketHeader header;
    header.type = PacketType::PlayerUpdate;
    header.size = sizeof(PlayerUpdatePacket);

    buffer.resize(sizeof(PacketHeader) + sizeof(PlayerUpdatePacket));
    memcpy(buffer.data(), &header, sizeof(PacketHeader));
    memcpy(buffer.data() + sizeof(PacketHeader), &packet, sizeof(PlayerUpdatePacket));

    if (m_IsServer) {
        // Server sends to all clients (except sender, but here we are the sender)
        // In this simple implementation, server player is also a client to itself logically
        // But for network, we send to all connected clients
        for (SOCKET client : m_ClientSockets) {
            send(client, buffer.data(), (int)buffer.size(), 0);
        }
    } else {
        // Client sends to server
        send(m_Socket, buffer.data(), (int)buffer.size(), 0);
    }
}

void NetworkManager::SetOnPlayerUpdate(std::function<void(const PlayerUpdatePacket&)> callback) {
    m_OnPlayerUpdate = callback;
}

void NetworkManager::SendPlayerShoot(const PlayerShootPacket& packet) {
    if (!m_IsConnected) return;

    std::vector<char> buffer;
    PacketHeader header;
    header.type = PacketType::PlayerShoot;
    header.size = sizeof(PlayerShootPacket);

    buffer.resize(sizeof(PacketHeader) + sizeof(PlayerShootPacket));
    memcpy(buffer.data(), &header, sizeof(PacketHeader));
    memcpy(buffer.data() + sizeof(PacketHeader), &packet, sizeof(PlayerShootPacket));

    if (m_IsServer) {
        for (SOCKET client : m_ClientSockets) {
            send(client, buffer.data(), (int)buffer.size(), 0);
        }
    } else {
        send(m_Socket, buffer.data(), (int)buffer.size(), 0);
    }
}

void NetworkManager::SetOnPlayerShoot(std::function<void(const PlayerShootPacket&)> callback) {
    m_OnPlayerShoot = callback;
}

} // namespace Archura

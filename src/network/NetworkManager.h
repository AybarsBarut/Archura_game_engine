#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

// SDK Version Control - Must be included BEFORE any other Windows headers
#include <sdkddkver.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00 // Windows 10
#endif

// Prevent Windows.h from defining min/max macros and reduce header bloat
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

// Include Winsock2 headers (winsock2.h includes windows.h internally)
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace Archura {

enum class PacketType : uint8_t {
    Connect = 0,
    Disconnect,
    PlayerUpdate,
    PlayerShoot,
    WorldState
};

#pragma pack(push, 1)
struct PacketHeader {
    PacketType type;
    uint32_t size; // Payload size
};

struct PlayerUpdatePacket {
    uint32_t id;
    float x, y, z;      // Position
    float yaw, pitch;   // Rotation
};

struct PlayerShootPacket {
    uint32_t id;        // Shooter ID
    float originX, originY, originZ;
    float dirX, dirY, dirZ;
    int weaponType;
};
#pragma pack(pop)

class NetworkManager {
public:
    static NetworkManager& Get();

    bool Init();
    void Shutdown();

    bool IsServer() const { return m_IsServer; }
    bool IsConnected() const { return m_IsConnected; }

    // Server functions
    bool StartServer(int port);
    void UpdateServer();

    // Client functions
    bool Connect(const std::string& ip, int port);
    void UpdateClient();
    void SendPlayerUpdate(const PlayerUpdatePacket& packet);
    void SendPlayerShoot(const PlayerShootPacket& packet);

    // Callbacks
    void SetOnPlayerUpdate(std::function<void(const PlayerUpdatePacket&)> callback);
    void SetOnPlayerShoot(std::function<void(const PlayerShootPacket&)> callback);

private:
    NetworkManager() = default;
    ~NetworkManager() = default;

    bool m_Initialized = false;
    bool m_IsServer = false;
    bool m_IsConnected = false;
    
    SOCKET m_Socket = INVALID_SOCKET;
    std::vector<SOCKET> m_ClientSockets;

    std::function<void(const PlayerUpdatePacket&)> m_OnPlayerUpdate;
    std::function<void(const PlayerShootPacket&)> m_OnPlayerShoot;
};

} // namespace Archura

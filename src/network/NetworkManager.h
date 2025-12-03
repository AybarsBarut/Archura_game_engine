#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace Archura {

enum class PacketType : uint8_t {
    Connect = 0,
    Disconnect,
    PlayerUpdate,
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



    // Callbacks
    void SetOnPlayerUpdate(std::function<void(const PlayerUpdatePacket&)> callback);

private:
    NetworkManager() = default;
    ~NetworkManager() = default;

    bool m_Initialized = false;
    bool m_IsServer = false;
    bool m_IsConnected = false;
    
    SOCKET m_Socket = INVALID_SOCKET;
    std::vector<SOCKET> m_ClientSockets;

    std::function<void(const PlayerUpdatePacket&)> m_OnPlayerUpdate;
};

} // namespace Archura

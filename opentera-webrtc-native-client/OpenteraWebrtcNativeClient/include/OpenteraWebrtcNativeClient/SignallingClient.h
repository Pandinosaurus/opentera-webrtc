#ifndef OPENTERA_WEBRTC_NATIVE_CLIENT_SIGNALING_CLIENT_H
#define OPENTERA_WEBRTC_NATIVE_CLIENT_SIGNALING_CLIENT_H

#include <OpenteraWebrtcNativeClient/Utils/ClassMacro.h>
#include <OpenteraWebrtcNativeClient/Utils/Client.h>
#include <OpenteraWebrtcNativeClient/Utils/IceServer.h>
#include <OpenteraWebrtcNativeClient/Handlers/PeerConnectionHandler.h>

#include <sio_client.h>

#include <api/peer_connection_interface.h>
#include <api/scoped_refptr.h>

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace introlab
{
    class SignalingClient
    {
        std::string m_url;
        std::string m_clientName;
        sio::message::ptr m_clientData;
        std::string m_room;
        std::string m_password;
        std::vector<IceServer> m_iceServers;

        std::recursive_mutex m_sioMutex;

        sio::client m_sio;
        bool m_hasClosePending;

        std::map<std::string, Client> m_roomClientsById;
        std::vector<std::string> m_alreadyAcceptedCalls;

        rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_peerConnectionFactory;

        std::function<void()> m_onSignallingConnectionOpen;
        std::function<void()> m_onSignallingConnectionClosed;
        std::function<void(const std::string&)> m_onSignallingConnectionError;

        std::function<void(const std::vector<RoomClient>&)> m_onRoomClientsChanged;

        std::function<bool(const Client&)> m_callAcceptor;
        std::function<void(const Client&)> m_onCallRejected;

        std::function<void(const Client&)> m_onClientConnected;
        std::function<void(const Client&)> m_onClientDisconnected;

        std::function<void(const std::string& error)> m_onError;

    protected:
        std::recursive_mutex m_peerConnectionMutex;
        std::recursive_mutex m_callbackMutex;

        std::map<std::string, std::unique_ptr<PeerConnectionHandler>> m_peerConnectionsHandlerById;

    public:
        SignalingClient(const std::string& url, const std::string& clientName, const sio::message::ptr& clientData,
                const std::string& room, const std::string& password, const std::vector<IceServer>& iceServers);
        virtual ~SignalingClient();

        DECLARE_NOT_COPYABLE(SignalingClient);
        DECLARE_NOT_MOVABLE(SignalingClient);

        void connect();
        void close();
        void closeSync();

        void callAll();
        void callIds(const std::vector<std::string>& ids);

        void hangUpAll();
        void closeAllRoomPeerConnections();

        bool isConnected();
        bool isRtcConnected();
        std::string id();

        std::vector<std::string> getConnectedRoomClientIds();

        RoomClient getRoomClient(const std::string& id);
        std::vector<RoomClient> getRoomClients();

        void setOnSignallingConnectionOpen(const std::function<void()>& callback);
        void setOnSignallingConnectionClosed(const std::function<void()>& callback);
        void setOnSignallingConnectionError(const std::function<void(const std::string&)>& callback);

        void setOnRoomClientsChanged(const std::function<void(const std::vector<RoomClient>&)>& callback);

        void setCallAcceptor(const std::function<bool(const Client&)>& callback);
        void setOnCallRejected(const std::function<void(const Client&)>& callback);

        void setOnClientConnected(const std::function<void(const Client&)>& callback);
        void setOnClientDisconnected(const std::function<void(const Client&)>& callback);

        void setOnError(const std::function<void(const std::string& error)>& callback);

    protected:
        template<class T, class ... Types>
        void invokeIfCallable(const std::function<T>& f, Types... args);

        virtual std::unique_ptr<PeerConnectionHandler> createPeerConnectionHandler(const std::string& id,
                const Client& peerClient, bool isCaller) = 0;

        std::function<void(const std::string&, sio::message::ptr)> getSendEventFunction();
        std::function<void(const std::string&)> getOnErrorFunction();

    private:
        void connectSioEvents();

        void onSioConnectEvent();
        void onSioErrorEvent();
        void onSioDisconnectEvent(const sio::client::close_reason& reason);

        void onJoinRoomCallback(const sio::message::list& message);

        void onRoomClientsEvent(sio::event& event);

        void onMakePeerCallEvent(sio::event& event);
        void makePeerCall(const std::string& id);

        void onPeerCallReceivedEvent(sio::event& event);
        void onPeerCallAnswerReceivedEvent(sio::event& event);
        void onCloseAllPeerConnectionsRequestReceivedEvent(sio::event& event);

        void onIceCandidateReceivedEvent(sio::event& event);
        void closeAllConnections();

        bool getCallAcceptance(const std::string& id);

        std::unique_ptr<PeerConnectionHandler> createConnection(const std::string& peerId, bool isCaller);
        void removeConnection(const std::string& id);
    };

    inline bool SignalingClient::isConnected()
    {
        std::lock_guard<std::recursive_mutex> lock(m_sioMutex);
        return m_sio.opened();
    }

    inline bool SignalingClient::isRtcConnected()
    {
        std::lock_guard<std::recursive_mutex> lock(m_peerConnectionMutex);
        return !m_peerConnectionsHandlerById.empty();
    }

    inline std::string SignalingClient::id()
    {
        std::lock_guard<std::recursive_mutex> lock(m_sioMutex);
        return m_hasClosePending ? "" : m_sio.get_sessionid();
    }

    inline RoomClient SignalingClient::getRoomClient(const std::string& id)
    {
        std::lock_guard<std::recursive_mutex> lockSio(m_sioMutex);
        std::lock_guard<std::recursive_mutex> lockPeerConnection(m_peerConnectionMutex);

        const Client& client = m_roomClientsById.at(id);
        bool isConnected = m_peerConnectionsHandlerById.find(client.id()) != m_peerConnectionsHandlerById.end() ||
                client.id() == this->id();
        return RoomClient(client, isConnected);
    }

    inline void SignalingClient::setOnSignallingConnectionOpen(const std::function<void()>& callback)
    {
        std::lock_guard<std::recursive_mutex> lock(m_callbackMutex);
        m_onSignallingConnectionOpen = callback;
    }

    inline void SignalingClient::setOnSignallingConnectionClosed(const std::function<void()>& callback)
    {
        std::lock_guard<std::recursive_mutex> lock(m_callbackMutex);
        m_onSignallingConnectionClosed = callback;
    }

    inline void SignalingClient::setOnSignallingConnectionError(const std::function<void(const std::string&)>& callback)
    {
        std::lock_guard<std::recursive_mutex> lock(m_callbackMutex);
        m_onSignallingConnectionError = callback;
    }

    inline void SignalingClient::setOnRoomClientsChanged(
            const std::function<void(const std::vector<RoomClient>&)>& callback)
    {
        std::lock_guard<std::recursive_mutex> lock(m_callbackMutex);
        m_onRoomClientsChanged = callback;
    }

    inline void SignalingClient::setCallAcceptor(const std::function<bool(const Client&)>& callback)
    {
        std::lock_guard<std::recursive_mutex> lock(m_callbackMutex);
        m_callAcceptor = callback;
    }

    inline void SignalingClient::setOnCallRejected(const std::function<void(const Client&)>& callback)
    {
        std::lock_guard<std::recursive_mutex> lock(m_callbackMutex);
        m_onCallRejected = callback;
    }

    inline void SignalingClient::setOnClientConnected(const std::function<void(const Client&)>& callback)
    {
        std::lock_guard<std::recursive_mutex> lock(m_callbackMutex);
        m_onClientConnected = callback;
    }

    inline void SignalingClient::setOnClientDisconnected(const std::function<void(const Client&)>& callback)
    {
        std::lock_guard<std::recursive_mutex> lock(m_callbackMutex);
        m_onClientDisconnected = callback;
    }

    inline void SignalingClient::setOnError(const std::function<void(const std::string& error)>& callback)
    {
        std::lock_guard<std::recursive_mutex> lock(m_callbackMutex);
        m_onError = callback;
    }

    template<class T, class ... Types>
    void SignalingClient::invokeIfCallable(const std::function<T>& f, Types... args)
    {
        std::lock_guard<std::recursive_mutex> lock(m_callbackMutex);
        if (f)
        {
            f(args...);
        }
    }
}

#endif

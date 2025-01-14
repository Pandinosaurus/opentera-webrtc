#ifndef OPENTERA_WEBRTC_NATIVE_CLIENT_CONFIGURATIONS_SIGNALING_SERVER_CONFIGURATION_H
#define OPENTERA_WEBRTC_NATIVE_CLIENT_CONFIGURATIONS_SIGNALING_SERVER_CONFIGURATION_H

#include <sio_message.h>

#include <string>

namespace opentera
{
    /**
     * @brief Represents a signaling server configuration.
     */
    class SignalingServerConfiguration
    {
        std::string m_url;
        std::string m_clientName;
        sio::message::ptr m_clientData;
        std::string m_room;
        std::string m_password;

        SignalingServerConfiguration(
            std::string&& url,
            std::string&& clientName,
            sio::message::ptr&& clientData,
            std::string&& room,
            std::string&& password);

    public:
        SignalingServerConfiguration(const SignalingServerConfiguration& other) = default;
        SignalingServerConfiguration(SignalingServerConfiguration&& other) = default;
        virtual ~SignalingServerConfiguration() = default;

        static SignalingServerConfiguration create(std::string url, std::string clientName, std::string room);
        static SignalingServerConfiguration
            create(std::string url, std::string clientName, sio::message::ptr clientData, std::string room);
        static SignalingServerConfiguration
            create(std::string url, std::string clientName, std::string room, std::string password);
        static SignalingServerConfiguration create(
            std::string url,
            std::string clientName,
            sio::message::ptr clientData,
            std::string room,
            std::string password);

        const std::string& url() const;
        const std::string& clientName() const;
        sio::message::ptr clientData() const;
        const std::string& room() const;
        const std::string& password() const;

        SignalingServerConfiguration& operator=(const SignalingServerConfiguration& other) = default;
        SignalingServerConfiguration& operator=(SignalingServerConfiguration&& other) = default;
    };

    /**
     * @brief Creates an signaling server configuration with the specified values.
     *
     * @param url The signaling server URL
     * @param clientName The client name
     * @param room The room name
     * @return A signaling server configuration with the specified values
     */
    inline SignalingServerConfiguration
        SignalingServerConfiguration::create(std::string url, std::string clientName, std::string room)
    {
        return SignalingServerConfiguration(
            std::move(url),
            std::move(clientName),
            sio::null_message::create(),
            std::move(room),
            "");
    }

    /**
     * @brief Creates an signaling server configuration with the specified values.
     *
     * @param url The signaling server URL
     * @param clientName The client name
     * @param clientData The client data
     * @param room The room name
     * @return A signaling server configuration with the specified values
     */
    inline SignalingServerConfiguration SignalingServerConfiguration::create(
        std::string url,
        std::string clientName,
        sio::message::ptr clientData,
        std::string room)
    {
        return SignalingServerConfiguration(
            std::move(url),
            std::move(clientName),
            std::move(clientData),
            std::move(room),
            "");
    }

    /**
     * @brief Creates an signaling server configuration with the specified values.
     *
     * @param url The signaling server URL
     * @param clientName The client name
     * @param room The room name
     * @param password The signaling server password
     * @return A signaling server configuration with the specified values
     */
    inline SignalingServerConfiguration SignalingServerConfiguration::create(
        std::string url,
        std::string clientName,
        std::string room,
        std::string password)
    {
        return SignalingServerConfiguration(
            std::move(url),
            std::move(clientName),
            sio::null_message::create(),
            std::move(room),
            std::move(password));
    }

    /**
     * @brief Creates an signaling server configuration with the specified values.
     *
     * @param url The signaling server URL
     * @param clientName The client name
     * @param clientData The client data
     * @param room The room name
     * @param password The signaling server password
     * @return A signaling server configuration with the specified values
     */
    inline SignalingServerConfiguration SignalingServerConfiguration::create(
        std::string url,
        std::string clientName,
        sio::message::ptr clientData,
        std::string room,
        std::string password)
    {
        return SignalingServerConfiguration(
            std::move(url),
            std::move(clientName),
            std::move(clientData),
            std::move(room),
            std::move(password));
    }

    /**
     * @brief Returns the signaling server URL.
     * @return The signaling server URL
     */
    inline const std::string& SignalingServerConfiguration::url() const { return m_url; }

    /**
     * @brief Returns the client name.
     * @return The client name
     */
    inline const std::string& SignalingServerConfiguration::clientName() const { return m_clientName; }

    /**
     * @brief Returns the client data.
     * @return The client data
     */
    inline sio::message::ptr SignalingServerConfiguration::clientData() const { return m_clientData; }

    /**
     * @brief Returns the room name.
     * @return The room name
     */
    inline const std::string& SignalingServerConfiguration::room() const { return m_room; }

    /**
     * @brief Returns the signaling server password.
     * @return The signaling server password
     */
    inline const std::string& SignalingServerConfiguration::password() const { return m_password; }
}

#endif

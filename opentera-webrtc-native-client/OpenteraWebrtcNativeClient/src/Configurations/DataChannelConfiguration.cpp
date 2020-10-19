#include <OpenteraWebrtcNativeClient/Configurations/DataChannelConfiguration.h>

using namespace introlab;
using namespace std;

DataChannelConfiguration::DataChannelConfiguration(bool ordered, const std::optional<int>& maxPacketLifeTime,
        const std::optional<int>& maxRetransmits, const std::string& protocol) :
        m_ordered(ordered), m_maxPacketLifeTime(maxPacketLifeTime), m_maxRetransmits(maxRetransmits),
        m_protocol(protocol)
{
}

DataChannelConfiguration::operator webrtc::DataChannelInit() const
{
    webrtc::DataChannelInit configuration;
    configuration.ordered = m_ordered;
    if (m_maxPacketLifeTime)
    {
        configuration.maxRetransmitTime = m_maxPacketLifeTime.value();
    }
    if (m_maxRetransmits)
    {
        configuration.maxRetransmits = m_maxRetransmits.value();
    }
    configuration.protocol = m_protocol;

    return configuration;
}

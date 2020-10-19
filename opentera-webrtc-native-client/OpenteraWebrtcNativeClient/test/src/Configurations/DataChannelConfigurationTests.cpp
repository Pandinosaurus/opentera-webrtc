#include <OpenteraWebrtcNativeClient/Configurations/DataChannelConfiguration.h>

#include <gtest/gtest.h>

using namespace introlab;
using namespace std;

TEST(DataChannelConfigurationTests, create_shouldSetTheAttributes)
{
    DataChannelConfiguration testee = DataChannelConfiguration::create();

    EXPECT_EQ(testee.ordered(), true);
    EXPECT_EQ(testee.maxPacketLifeTime(), nullopt);
    EXPECT_EQ(testee.maxRetransmits(), nullopt);
    EXPECT_EQ(testee.protocol(), "");
}

TEST(DataChannelConfigurationTests, create_ordered_shouldSetTheAttributes)
{
    DataChannelConfiguration testee = DataChannelConfiguration::create(false);

    EXPECT_EQ(testee.ordered(), false);
    EXPECT_EQ(testee.maxPacketLifeTime(), nullopt);
    EXPECT_EQ(testee.maxRetransmits(), nullopt);
    EXPECT_EQ(testee.protocol(), "");
}

TEST(DataChannelConfigurationTests, createProtocol_shouldSetTheAttributes)
{
    DataChannelConfiguration testee = DataChannelConfiguration::createProtocol("a");

    EXPECT_EQ(testee.ordered(), true);
    EXPECT_EQ(testee.maxPacketLifeTime(), nullopt);
    EXPECT_EQ(testee.maxRetransmits(), nullopt);
    EXPECT_EQ(testee.protocol(), "a");
}

TEST(DataChannelConfigurationTests, create_orderedProtocol_shouldSetTheAttributes)
{
    DataChannelConfiguration testee = DataChannelConfiguration::create(false, "a");

    EXPECT_EQ(testee.ordered(), false);
    EXPECT_EQ(testee.maxPacketLifeTime(), nullopt);
    EXPECT_EQ(testee.maxRetransmits(), nullopt);
    EXPECT_EQ(testee.protocol(), "a");
}

TEST(DataChannelConfigurationTests, createMaxPacketLifeTime_shouldSetTheAttributes)
{
    DataChannelConfiguration testee = DataChannelConfiguration::createMaxPacketLifeTime(10);

    EXPECT_EQ(testee.ordered(), true);
    EXPECT_EQ(testee.maxPacketLifeTime(), 10);
    EXPECT_EQ(testee.maxRetransmits(), nullopt);
    EXPECT_EQ(testee.protocol(), "");
}

TEST(DataChannelConfigurationTests, createMaxPacketLifeTime_ordered_shouldSetTheAttributes)
{
    DataChannelConfiguration testee = DataChannelConfiguration::createMaxPacketLifeTime(false, 10);

    EXPECT_EQ(testee.ordered(), false);
    EXPECT_EQ(testee.maxPacketLifeTime(), 10);
    EXPECT_EQ(testee.maxRetransmits(), nullopt);
    EXPECT_EQ(testee.protocol(), "");
}

TEST(DataChannelConfigurationTests, createMaxPacketLifeTime_protocol_shouldSetTheAttributes)
{
    DataChannelConfiguration testee = DataChannelConfiguration::createMaxPacketLifeTime(10, "a");

    EXPECT_EQ(testee.ordered(), true);
    EXPECT_EQ(testee.maxPacketLifeTime(), 10);
    EXPECT_EQ(testee.maxRetransmits(), nullopt);
    EXPECT_EQ(testee.protocol(), "a");
}

TEST(DataChannelConfigurationTests, createMaxPacketLifeTime_orderedProtocol_shouldSetTheAttributes)
{
    DataChannelConfiguration testee = DataChannelConfiguration::createMaxPacketLifeTime(false, 10, "a");

    EXPECT_EQ(testee.ordered(), false);
    EXPECT_EQ(testee.maxPacketLifeTime(), 10);
    EXPECT_EQ(testee.maxRetransmits(), nullopt);
    EXPECT_EQ(testee.protocol(), "a");
}

TEST(DataChannelConfigurationTests, createMaxRetransmits_shouldSetTheAttributes)
{
    DataChannelConfiguration testee = DataChannelConfiguration::createMaxRetransmits(10);

    EXPECT_EQ(testee.ordered(), true);
    EXPECT_EQ(testee.maxPacketLifeTime(), nullopt);
    EXPECT_EQ(testee.maxRetransmits(), 10);
    EXPECT_EQ(testee.protocol(), "");
}

TEST(DataChannelConfigurationTests, createMaxRetransmits_ordered_shouldSetTheAttributes)
{
    DataChannelConfiguration testee = DataChannelConfiguration::createMaxRetransmits(false, 10);

    EXPECT_EQ(testee.ordered(), false);
    EXPECT_EQ(testee.maxPacketLifeTime(), nullopt);
    EXPECT_EQ(testee.maxRetransmits(), 10);
    EXPECT_EQ(testee.protocol(), "");
}

TEST(DataChannelConfigurationTests, createMaxRetransmits_protocol_shouldSetTheAttributes)
{
    DataChannelConfiguration testee = DataChannelConfiguration::createMaxRetransmits(10, "a");

    EXPECT_EQ(testee.ordered(), true);
    EXPECT_EQ(testee.maxPacketLifeTime(), nullopt);
    EXPECT_EQ(testee.maxRetransmits(), 10);
    EXPECT_EQ(testee.protocol(), "a");
}

TEST(DataChannelConfigurationTests, createMaxRetransmits_orderedProtocol_shouldSetTheAttributes)
{
    DataChannelConfiguration testee = DataChannelConfiguration::createMaxRetransmits(false, 10, "a");

    EXPECT_EQ(testee.ordered(), false);
    EXPECT_EQ(testee.maxPacketLifeTime(), nullopt);
    EXPECT_EQ(testee.maxRetransmits(), 10);
    EXPECT_EQ(testee.protocol(), "a");
}

TEST(DataChannelConfigurationTests, operator_webrtcDataChannelInit_shouldSetTheAttributes)
{
    auto testee1 = static_cast<webrtc::DataChannelInit>(DataChannelConfiguration::createMaxPacketLifeTime(false, 10,
            "a"));
    auto testee2 = static_cast<webrtc::DataChannelInit>(DataChannelConfiguration::createMaxRetransmits(false, 10, "a"));

    EXPECT_EQ(testee1.ordered, false);
    EXPECT_EQ(testee1.maxRetransmitTime, 10);
    EXPECT_EQ(testee1.maxRetransmits, nullopt);
    EXPECT_EQ(testee1.protocol, "a");

    EXPECT_EQ(testee2.ordered, false);
    EXPECT_EQ(testee2.maxRetransmitTime, nullopt);
    EXPECT_EQ(testee2.maxRetransmits, 10);
    EXPECT_EQ(testee2.protocol, "a");

}

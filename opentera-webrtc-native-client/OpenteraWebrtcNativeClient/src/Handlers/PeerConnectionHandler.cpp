#include <OpenteraWebrtcNativeClient/Handlers/PeerConnectionHandler.h>

using namespace opentera;
using namespace std;

class DummySetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver
{
public:
    static DummySetSessionDescriptionObserver* Create()
    {
        return new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
    }

    void OnSuccess() override {}
    void OnFailure(webrtc::RTCError error) override {}
};

void CreateSessionDescriptionObserverHelper::OnSuccess(webrtc::SessionDescriptionInterface* desc)
{
    OnCreateSessionDescriptionObserverSuccess(desc);
}

void CreateSessionDescriptionObserverHelper::OnFailure(webrtc::RTCError error)
{
    OnCreateSessionDescriptionObserverFailure(error);
}

void SetSessionDescriptionObserverHelper::OnSuccess()
{
    OnSetSessionDescriptionObserverSuccess();
}

void SetSessionDescriptionObserverHelper::OnFailure(webrtc::RTCError error)
{
    OnSetSessionDescriptionObserverFailure(error);
}

PeerConnectionHandler::PeerConnectionHandler(
    string&& id,
    Client&& peerClient,
    bool isCaller,
    function<void(const string&, const sio::message::ptr&)>&& sendEvent,
    function<void(const string&)>&& onError,
    function<void(const Client&)>&& onClientConnected,
    function<void(const Client&)>&& onClientDisconnected)
    : m_id(move(id)),
      m_peerClient(move(peerClient)),
      m_isCaller(isCaller),
      m_sendEvent(move(sendEvent)),
      m_onError(move(onError)),
      m_onClientConnected(move(onClientConnected)),
      m_onClientDisconnected(move(onClientDisconnected)),
      m_onClientDisconnectedCalled(true)
{
}

PeerConnectionHandler::~PeerConnectionHandler()
{
    if (m_peerConnection)
    {
        m_sendEvent = [](const string&, const sio::message::ptr&) {};
        m_onError = [](const string&) {};
        m_onClientConnected = [](const Client&) {};
        m_onClientDisconnected = [](const Client&) {};
        m_peerConnection->Close();

        if (!m_onClientDisconnectedCalled)
        {
            m_onClientDisconnected(m_peerClient);
            m_onClientDisconnectedCalled = true;
        }
    }
}

void PeerConnectionHandler::setPeerConnection(const rtc::scoped_refptr<webrtc::PeerConnectionInterface>& peerConnection)
{
    m_peerConnection = peerConnection;
}

void PeerConnectionHandler::makePeerCall()
{
    m_peerConnection->CreateOffer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
}

void PeerConnectionHandler::receivePeerCall(const string& sdp)
{
    webrtc::SdpParseError error;
    auto desc = webrtc::CreateSessionDescription("offer", sdp, &error);
    if (desc)
    {
        m_peerConnection->SetRemoteDescription(this, desc);
    }
    else
    {
        m_onError(error.line + " - " + error.description);
    }
}

void PeerConnectionHandler::receivePeerCallAnswer(const string& sdp)
{
    webrtc::SdpParseError error;
    auto desc = webrtc::CreateSessionDescription("answer", sdp, &error);
    if (desc)
    {
        m_peerConnection->SetRemoteDescription(DummySetSessionDescriptionObserver::Create(), desc);
    }
    else
    {
        m_onError(error.line + " - " + error.description);
    }
}

void PeerConnectionHandler::receiveIceCandidate(const string& sdpMid, int sdpMLineIndex, const string& sdp)
{
    webrtc::SdpParseError error;
    auto candidate = webrtc::CreateIceCandidate(sdpMid, sdpMLineIndex, sdp, &error);
    if (candidate)
    {
        m_peerConnection->AddIceCandidate(candidate);
        delete candidate;
    }
    else if (error.line != "")
    {
        m_onError(error.line + " - " + error.description);
    }
}

void PeerConnectionHandler::OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState newState)
{
    switch (newState)
    {
        case webrtc::PeerConnectionInterface::PeerConnectionState::kConnected:
            m_onClientConnected(m_peerClient);
            m_onClientDisconnectedCalled = false;
            break;

        case webrtc::PeerConnectionInterface::PeerConnectionState::kDisconnected:
        case webrtc::PeerConnectionInterface::PeerConnectionState::kFailed:
        case webrtc::PeerConnectionInterface::PeerConnectionState::kClosed:
            m_onClientDisconnected(m_peerClient);
            m_onClientDisconnectedCalled = true;
            break;

        default:
            break;
    }
}

void PeerConnectionHandler::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
{
    string sdp;
    candidate->ToString(&sdp);

    auto candidateMessage = sio::object_message::create();
    candidateMessage->get_map()["sdpMid"] = sio::string_message::create(candidate->sdp_mid());
    candidateMessage->get_map()["sdpMLineIndex"] = sio::int_message::create(candidate->sdp_mline_index());
    candidateMessage->get_map()["candidate"] = sio::string_message::create(sdp);

    auto data = sio::object_message::create();
    data->get_map()["toId"] = sio::string_message::create(m_peerClient.id());
    data->get_map()["candidate"] = candidateMessage;

    m_sendEvent("send-ice-candidate", data);
}

void PeerConnectionHandler::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel) {}

void PeerConnectionHandler::OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) {}

void PeerConnectionHandler::OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) {}

void PeerConnectionHandler::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState newState) {}

void PeerConnectionHandler::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState newState) {}

void PeerConnectionHandler::OnCreateSessionDescriptionObserverSuccess(webrtc::SessionDescriptionInterface* desc)
{
    m_peerConnection->SetLocalDescription(DummySetSessionDescriptionObserver::Create(), desc);

    string sdp;
    desc->ToString(&sdp);

    auto offer = sio::object_message::create();
    offer->get_map()["sdp"] = sio::string_message::create(sdp);

    auto data = sio::object_message::create();
    data->get_map()["toId"] = sio::string_message::create(m_peerClient.id());

    if (m_isCaller)
    {
        offer->get_map()["type"] = sio::string_message::create("offer");
        data->get_map()["offer"] = offer;
        m_sendEvent("call-peer", data);
    }
    else
    {
        offer->get_map()["type"] = sio::string_message::create("answer");
        data->get_map()["answer"] = offer;
        m_sendEvent("make-peer-call-answer", data);
    }
}

void PeerConnectionHandler::OnCreateSessionDescriptionObserverFailure(webrtc::RTCError error)
{
    m_onError(error.message());
}

void PeerConnectionHandler::OnSetSessionDescriptionObserverSuccess()
{
    if (!m_isCaller)
    {
        createAnswer();
    }
}

void PeerConnectionHandler::OnSetSessionDescriptionObserverFailure(webrtc::RTCError error)
{
    m_onError(error.message());
}

void PeerConnectionHandler::AddRef() const {}

rtc::RefCountReleaseStatus PeerConnectionHandler::Release() const
{
    return rtc::RefCountReleaseStatus::kOtherRefsRemained;
}

void PeerConnectionHandler::createAnswer()
{
    m_peerConnection->CreateAnswer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
}

void opentera::setTransceiverDirection(
    const rtc::scoped_refptr<webrtc::RtpTransceiverInterface>& transceiver,
    webrtc::RtpTransceiverDirection direction)
{
    transceiver->SetDirectionWithError(direction);
}

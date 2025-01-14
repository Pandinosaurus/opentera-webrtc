#include <OpenteraWebrtcNativeClientPython/PyBindUtils.h>
#include <OpenteraWebrtcNativeClientPython/SignalingClientPython.h>

#include <OpenteraWebrtcNativeClient/SignalingClient.h>

#include <pybind11/functional.h>
#include <pybind11/stl.h>

using namespace opentera;
using namespace std;
namespace py = pybind11;

void opentera::initSignalingClientPython(pybind11::module& m)
{
    py::class_<SignalingClient>(m, "SignalingClient")
        .def("connect", &SignalingClient::connect, "Connects the client the signaling server.")
        .def("close", &SignalingClient::close, "Closes all client connections (asynchronous).")
        .def(
            "close_sync",
            &SignalingClient::closeSync,
            py::call_guard<py::gil_scoped_release>(),
            "Closes all client connections (synchronous).")

        .def("call_all", &SignalingClient::callAll, "Calls all room clients.")
        .def("call_ids", &SignalingClient::callIds, "Calls the specified clients.", py::arg("ids"))

        .def("hang_up_all", &SignalingClient::hangUpAll, "Hangs up all clients.")
        .def(
            "close_all_room_peer_connections",
            &SignalingClient::closeAllRoomPeerConnections,
            "Closes all room peer connections.")

        .def_property_readonly(
            "is_connected",
            GilScopedRelease<SignalingClient>::guard(&SignalingClient::isConnected),
            "Indicates if the client is connected to the signaling server.\n"
            ":return: True if the client is connected to the signaling server")
        .def_property_readonly(
            "is_rtc_connected",
            GilScopedRelease<SignalingClient>::guard(&SignalingClient::isRtcConnected),
            "Indicates if the client is connected to a least "
            "one client (RTCPeerConnection).\n"
            ":return: True if the client is connected to a "
            "least one client (RTCPeerConnection)")
        .def_property_readonly(
            "id",
            GilScopedRelease<SignalingClient>::guard(&SignalingClient::id),
            "Returns the client id.\n"
            ":return: The client id")

        .def_property_readonly(
            "connected_room_client_ids",
            GilScopedRelease<SignalingClient>::guard(&SignalingClient::getConnectedRoomClientIds),
            "Returns the connected room client ids.\n"
            ":return: The connected room client ids")

        .def(
            "get_room_client",
            &SignalingClient::getRoomClient,
            py::call_guard<py::gil_scoped_release>(),
            "Returns the room client that matches with the specified id.\n"
            "If no room client matches with the id, a default room client is "
            "returned.\n"
            "\n"
            ":param id: The room client id\n"
            ":return: The room client that matches with the specified id",
            py::arg("id"))
        .def_property_readonly(
            "room_clients",
            GilScopedRelease<SignalingClient>::guard(&SignalingClient::getRoomClients),
            "Returns the room clients\n"
            ":return: The room clients")

        .def_property(
            "on_signaling_connection_opened",
            nullptr,
            GilScopedRelease<SignalingClient>::guard(&SignalingClient::setOnSignalingConnectionOpened),
            "Sets the callback that is called when the signaling "
            "connection opens.\n"
            "\n"
            "The callback is called from the internal client thread. "
            "The callback should not block.\n"
            "\n"
            ":param callback: The callback")
        .def_property(
            "on_signaling_connection_closed",
            nullptr,
            GilScopedRelease<SignalingClient>::guard(&SignalingClient::setOnSignalingConnectionClosed),
            "Sets the callback that is called when the signaling "
            "connection closes.\n"
            "\n"
            "The callback is called from the internal client thread. "
            "The callback should not block.\n"
            "\n"
            ":param callback: The callback")
        .def_property(
            "on_signaling_connection_error",
            nullptr,
            GilScopedRelease<SignalingClient>::guard(&SignalingClient::setOnSignalingConnectionError),
            "Sets the callback that is called when a signaling "
            "connection error occurs.\n"
            "\n"
            "The callback is called from the internal client thread. "
            "The callback should not block.\n"
            "\n"
            "Callback parameters:\n"
            " - error: The error message\n"
            "\n"
            ":param callback: The callback")

        .def_property(
            "on_room_clients_changed",
            nullptr,
            GilScopedRelease<SignalingClient>::guard(&SignalingClient::setOnRoomClientsChanged),
            "Sets the callback that is called when the room client changes.\n"
            "\n"
            "The callback is called from the internal client thread. The "
            "callback should not block.\n"
            "\n"
            "Callback parameters:\n"
            " - room_clients: The room clients\n"
            "\n"
            ":param callback: The callback")

        .def_property(
            "call_acceptor",
            nullptr,
            GilScopedRelease<SignalingClient>::guard(&SignalingClient::setCallAcceptor),
            "Sets the callback that is used to accept or reject a call.\n"
            "\n"
            "The callback is called from the internal client thread. The "
            "callback should not block.\n"
            "\n"
            "Callback parameters:\n"
            " - client: The client the call is from\n"
            "\n"
            "Callback return value:\n"
            " - True to accept the call, False to reject the call\n"
            "\n"
            ":param callback: The callback")
        .def_property(
            "on_call_rejected",
            nullptr,
            GilScopedRelease<SignalingClient>::guard(&SignalingClient::setOnCallRejected),
            "Sets the callback that is called when a call is rejected.\n"
            "\n"
            "The callback is called from the internal client thread. The "
            "callback should not block.\n"
            "\n"
            "Callback parameters:\n"
            " - client: The client that rejects the call\n"
            "\n"
            ":param callback: The callback")

        .def_property(
            "on_client_connected",
            nullptr,
            GilScopedRelease<SignalingClient>::guard(&SignalingClient::setOnClientConnected),
            "Sets the callback that is called when a client peer "
            "connection opens.\n"
            "\n"
            "The callback is called from the internal client thread. "
            "The callback should not block.\n"
            "\n"
            "Callback parameters:\n"
            " - client: The client that is connected\n"
            "\n"
            ":param callback: The callback")
        .def_property(
            "on_client_disconnected",
            nullptr,
            GilScopedRelease<SignalingClient>::guard(&SignalingClient::setOnClientDisconnected),
            "Sets the callback that is called when a client peer "
            "connection closes.\n"
            "\n"
            "The callback is called from the internal client thread. "
            "The callback should not block.\n"
            "\n"
            "Callback parameters:\n"
            " - client: The client that is disconnected\n"
            "\n"
            ":param callback: The callback")

        .def_property(
            "on_error",
            nullptr,
            GilScopedRelease<SignalingClient>::guard(&SignalingClient::setOnError),
            "Sets the callback that is called when an error occurs.\n"
            "\n"
            "The callback is called from the internal client thread. "
            "The callback should not block.\n"
            "\n"
            "Callback parameters:\n"
            " - error: The error message\n"
            "\n"
            ":param callback: The callback")

        .def_property(
            "logger",
            nullptr,
            GilScopedRelease<SignalingClient>::guard(&SignalingClient::setLogger),
            "Sets the callback that is used to log information.\n"
            "\n"
            "The callback is called from the internal client thread. The "
            "callback should not block.\n"
            "\n"
            "Callback parameters:\n"
            " - message: The message\n"
            "\n"
            ":param callback: The callback")

        .def_property(
            "tls_verification_enabled",
            nullptr,
            &SignalingClient::setTlsVerificationEnabled,
            "Enable or disable the TLS verification. By default, the "
            "TLS verification is enabled.\n"
            ":param: is_enabled");
}

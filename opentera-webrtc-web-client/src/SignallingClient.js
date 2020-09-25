import io from 'socket.io-client';


class SignallingClient {
  constructor(signallingServerConfiguration, hasRtcPeerConnection, getRtcPeerConnection, getAllRtcPeerConnection) {
    if (!window.RTCSessionDescription) {
      throw new Error('RTCSessionDescription is not supported.');
    }

    this._signallingServerConfiguration = signallingServerConfiguration;
    this._hasRtcPeerConnection = hasRtcPeerConnection;
    this._getRtcPeerConnection = getRtcPeerConnection;
    this._getAllRtcPeerConnection = getAllRtcPeerConnection;

    this._socket = null;

    this._clients = [];
    this._clientNamesById = {};

    this._onConnectionOpen = () => {};
    this._onConnectionClose = () => {};
    this._onConnectionError = () => {};
    this._onRoomClientsChanged = () => {};
  }
  
  async connect() {
    this._socket = io(this._signallingServerConfiguration.url);
    this._connectEvents();

    await new Promise((resolve, reject) => {
      this._socket.on('connect', () => resolve());
      this._socket.on('connect_error', error => reject(error));
      this._socket.on('connect_timeout', error => reject(error));
    });
    
    let data = {
      name: this._signallingServerConfiguration.name,
      room: this._signallingServerConfiguration.room,
      password: this._signallingServerConfiguration.password
    };
    this._socket.emit('join-room', data, isJoined => {
      if (isJoined) {
        this._onConnectionOpen();
      }
      else {
        this.close();
        this._onConnectionError('Invalid password');
      }
    });
  }  

  _connectEvents() {
    this._socket.on('disconnect', () => this._disconnect());

    this._socket.on('room-clients', clients => {
      this._clients = clients;
      this._updateClientNamesById(clients);
      this._onRoomClientsChanged(this._addConnectionStateToClients(this._clients));
    });

    this._socket.on('make-peer-call', async ids => await this._makePeerCall(ids));
    this._socket.on('peer-call-received', async data => await this._peerCallReceived(data));
    this._socket.on('peer-call-answer-received', async data => await this._peerCallAnswerReceived(data));

    this._socket.on('ice-candidate-received', async data => await this._addIceCandidate(data));
  }

  _disconnectEvents() {
    this._socket.off('connect');
    this._socket.off('connect_error');
    this._socket.off('connect_timeout');
    this._socket.off('disconnect');

    this._socket.off('room-clients');

    this._socket.off('make-peer-call');
    this._socket.off('peer-call-received');
    this._socket.off('peer-call-answer-received');

    this._getAllRtcPeerConnection().forEach(c => c.onicecandidate = () => {});
    this._socket.off('ice-candidate');
  }

  _disconnect() {
    this._disconnectEvents();
    this._socket.close();
    this._socket = null;
    this._onConnectionClose();
  }

  async _peerCallReceived(data) {
    let rtcPeerConnection = this._getRtcPeerConnection(data.fromId, false);
    this._connectOnIceCandidateEvent(data.fromId, rtcPeerConnection);

    await rtcPeerConnection.setRemoteDescription(new window.RTCSessionDescription(data.offer));
    
    let answer = await rtcPeerConnection.createAnswer();    
    await rtcPeerConnection.setLocalDescription(new window.RTCSessionDescription(answer));

    data = { toId: data.fromId, answer: answer };
    this._socket.emit('make-peer-call-answer', data);
  }

  async _peerCallAnswerReceived(data) {
    let _rtcPeerConnection =  this._getRtcPeerConnection(data.fromId, false);
    await _rtcPeerConnection.setRemoteDescription(new window.RTCSessionDescription(data.answer));
  }

  async _addIceCandidate(data) {
    if (data && data.candidate) {
      this._getRtcPeerConnection(data.fromId).addIceCandidate(data.candidate);
    }
  }

  async _makePeerCall(ids) {
    ids = ids.filter(id => id != this._socket.id);
    ids.forEach(async id => {
      if (this._hasRtcPeerConnection(id)) {
        return;
      }

      let rtcPeerConnection = this._getRtcPeerConnection(id, true);
      this._connectOnIceCandidateEvent(id, rtcPeerConnection);

      let offer = await rtcPeerConnection.createOffer();
      await rtcPeerConnection.setLocalDescription(new RTCSessionDescription(offer));

      let data = { toId: id, offer: offer };
      this._socket.emit('call-peer', data);
    });
  }

  _connectOnIceCandidateEvent(id, rtcPeerConnection) {
    rtcPeerConnection.onicecandidate = event => {
      let data = { toId: id, candidate: event.candidate };
      this._socket.emit('send-ice-candidate', data);
    };
  }

  _addConnectionStateToClients(clients) {
    let newClients = [];
    clients.forEach(client => {
      newClients.push({
        id: client.id,
        name: client.name,
        isConnected: this._hasRtcPeerConnection(client.id) || client.id == this.id
      });
    });

    return newClients;
  }

  _updateClientNamesById(clients) {
    this._clientNamesById = {};
    clients.forEach(client => {
      this._clientNamesById[client.id] = client.name;
    });
  }

  updateRoomClients() {
    this._onRoomClientsChanged(this._addConnectionStateToClients(this._clients));
  }

  close() {
    if (this._socket !== null) {
      this._disconnect();
    }
  }

  callAll() {
    this._socket.emit('call-all');
  }

  callIds(ids) {
    this._socket.emit('call-ids', ids);
  }

  hangUpAll() {
    this._getAllRtcPeerConnection().forEach(c => c.onicecandidate = () => {});
  }

  getClientName(id) {
    return this._clientNamesById[id];
  }

  get id() {
    return this._socket.id;
  }

  get clients() {
    return this._addConnectionStateToClients(this._clients);
  }

  set onConnectionOpen(onConnectionOpen) {
    this._onConnectionOpen = onConnectionOpen;
  }

  set onConnectionClose(onConnectionClose) {
    this._onConnectionClose = onConnectionClose;
  }

  set onConnectionError(onConnectionError) {
    this._onConnectionError = onConnectionError;
  }

  set onRoomClientsChanged(onRoomClientsChanged) {
    this._onRoomClientsChanged = onRoomClientsChanged;
  }
}

export default SignallingClient;

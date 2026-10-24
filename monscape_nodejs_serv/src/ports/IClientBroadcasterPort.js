/**
 * Driving/Driven Port: IClientBroadcasterPort
 * Interface for pushing real-time events to connected Game Master clients.
 */
class IClientBroadcasterPort {
  broadcast(event, payload) {
    throw new Error('broadcast() must be implemented by adapter');
  }

  onClientCommand(handler) {
    throw new Error('onClientCommand() must be implemented by adapter');
  }
}

module.exports = IClientBroadcasterPort;

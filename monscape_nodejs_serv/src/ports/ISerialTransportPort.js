/**
 * Driven Port: ISerialTransportPort
 * Interface for serial communication (RS485 hardware on RPi, or mock on dev PC).
 */
class ISerialTransportPort {
  open() {
    throw new Error('open() must be implemented by adapter');
  }

  send(data, callback) {
    throw new Error('send() must be implemented by adapter');
  }

  onData(handler) {
    throw new Error('onData() must be implemented by adapter');
  }

  close() {
    throw new Error('close() must be implemented by adapter');
  }

  isOpen() {
    throw new Error('isOpen() must be implemented by adapter');
  }
}

module.exports = ISerialTransportPort;

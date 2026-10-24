const ISerialTransportPort = require('../../ports/ISerialTransportPort');

class MockSerialAdapter extends ISerialTransportPort {
  constructor({ autoRespond = true } = {}) {
    super();
    this.autoRespond = autoRespond;
    this.isOpenState = false;
    this.dataHandlers = [];
    this.sentMessages = [];
  }

  open() {
    this.isOpenState = true;
    console.log('[MockSerialAdapter] Mock serial port opened.');
    return Promise.resolve();
  }

  send(data, callback) {
    const raw = typeof data === 'string' ? data : JSON.stringify(data);
    this.sentMessages.push(raw);
    console.log('[MockSerialAdapter] Sent:', raw);

    if (this.autoRespond) {
      this.handleAutoResponse(raw);
    }

    if (typeof callback === 'function') {
      callback(null);
    }
    return Promise.resolve();
  }

  handleAutoResponse(raw) {
    try {
      const parsed = JSON.parse(raw);
      if (parsed.A && parsed.C === 'I') {
        // Auto-respond with status
        setTimeout(() => {
          this.simulateIncoming(JSON.stringify({
            Nom: parsed.A,
            Adr: parsed.A,
            stat: 4
          }));
        }, 50);
      }
    } catch (e) {
      // Ignore parse errors on raw strings
    }
  }

  simulateIncoming(line) {
    console.log('[MockSerialAdapter] Simulating incoming line:', line);
    for (const handler of this.dataHandlers) {
      handler(line);
    }
  }

  onData(handler) {
    this.dataHandlers.push(handler);
  }

  close() {
    this.isOpenState = false;
    console.log('[MockSerialAdapter] Mock serial port closed.');
    return Promise.resolve();
  }

  isOpen() {
    return this.isOpenState;
  }
}

module.exports = MockSerialAdapter;

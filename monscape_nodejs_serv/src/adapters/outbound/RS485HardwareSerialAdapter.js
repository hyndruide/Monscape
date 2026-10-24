const ISerialTransportPort = require('../../ports/ISerialTransportPort');

class RS485HardwareSerialAdapter extends ISerialTransportPort {
  constructor({ device = '/dev/ttyS0', baudRate = 74880, gpioRe = 17, gpioDe = 27 } = {}) {
    super();
    this.device = device;
    this.baudRate = baudRate;
    this.gpioRePin = gpioRe;
    this.gpioDePin = gpioDe;
    this.port = null;
    this.parser = null;
    this.rsRe = null;
    this.rsDe = null;
    this.dataHandlers = [];
    this.initGpio();
  }

  initGpio() {
    try {
      const { Gpio } = require('onoff');
      this.rsRe = new Gpio(this.gpioRePin, 'out');
      this.rsDe = new Gpio(this.gpioDePin, 'out');
      this.switchRxMode();
      console.log(`[RS485HardwareSerialAdapter] GPIOs initialized on pins RE=${this.gpioRePin}, DE=${this.gpioDePin}`);
    } catch (err) {
      console.warn(`[RS485HardwareSerialAdapter] Warning: GPIO (onoff) not available: ${err.message}. Running in soft-GPIO mode.`);
      this.rsRe = { writeSync: () => {}, write: (val, cb) => cb && cb(null) };
      this.rsDe = { writeSync: () => {}, write: (val, cb) => cb && cb(null) };
    }
  }

  switchRxMode() {
    if (this.rsRe) this.rsRe.writeSync(0);
    if (this.rsDe) this.rsDe.writeSync(0);
  }

  switchTxMode(callback) {
    if (this.rsRe) this.rsRe.writeSync(1);
    if (this.rsDe && typeof this.rsDe.write === 'function') {
      this.rsDe.write(1, callback);
    } else {
      if (this.rsDe) this.rsDe.writeSync(1);
      if (typeof callback === 'function') callback(null);
    }
  }

  open() {
    return new Promise((resolve, reject) => {
      try {
        let SerialPortLib = null;
        try {
          const spModule = require('serialport');
          SerialPortLib = spModule.SerialPort || spModule;
        } catch (e) {
          return reject(new Error(`serialport package not installed: ${e.message}`));
        }

        const Readline = require('@serialport/parser-readline').Readline ||
                         require('@serialport/parser-readline');

        this.port = new SerialPortLib({
          path: this.device,
          baudRate: this.baudRate
        }, (err) => {
          if (err) {
            console.error(`[RS485HardwareSerialAdapter] Error opening serial port ${this.device}:`, err.message);
            return reject(err);
          }
        });

        this.parser = this.port.pipe(new Readline({ delimiter: '\r\n' }));

        this.port.on('open', () => {
          console.log(`[RS485HardwareSerialAdapter] Serial port open on ${this.device} @ ${this.baudRate}`);
          this.switchRxMode();
          resolve();
        });

        this.parser.on('data', (line) => {
          console.log('[RS485HardwareSerialAdapter] Received data:', line);
          for (const handler of this.dataHandlers) {
            handler(line);
          }
        });

        this.port.on('error', (err) => {
          console.error('[RS485HardwareSerialAdapter] Serial error:', err.message);
        });
      } catch (err) {
        reject(err);
      }
    });
  }

  send(data, callback) {
    return new Promise((resolve, reject) => {
      if (!this.port || !this.port.isOpen) {
        const err = new Error('Serial port is not open');
        if (typeof callback === 'function') callback(err);
        return reject(err);
      }

      this.switchTxMode((gpioErr) => {
        if (gpioErr) {
          console.error('[RS485HardwareSerialAdapter] Error setting TX mode:', gpioErr.message);
        }

        let value = typeof data === 'string' ? data : JSON.stringify(data);
        console.log('[RS485HardwareSerialAdapter] Sending:', value);
        value = value + '\n';

        this.port.write(value, (writeErr) => {
          if (writeErr) {
            console.error('[RS485HardwareSerialAdapter] Error on write:', writeErr.message);
            if (typeof callback === 'function') callback(writeErr);
            return reject(writeErr);
          }

          setTimeout(() => {
            this.switchRxMode();
            if (typeof callback === 'function') callback(null);
            resolve();
          }, 15);
        });
      });
    });
  }

  onData(handler) {
    this.dataHandlers.push(handler);
  }

  close() {
    return new Promise((resolve) => {
      this.switchRxMode();
      if (this.port && this.port.isOpen) {
        this.port.close(() => {
          console.log('[RS485HardwareSerialAdapter] Serial port closed.');
          resolve();
        });
      } else {
        resolve();
      }
    });
  }

  isOpen() {
    return Boolean(this.port && this.port.isOpen);
  }
}

module.exports = RS485HardwareSerialAdapter;

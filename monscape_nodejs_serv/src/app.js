const path = require('path');
const config = require('./config');
const PuzzleRegistry = require('./domain/PuzzleRegistry');
const RS485HardwareSerialAdapter = require('./adapters/outbound/RS485HardwareSerialAdapter');
const MockSerialAdapter = require('./adapters/outbound/MockSerialAdapter');
const ExpressHttpAdapter = require('./adapters/inbound/ExpressHttpAdapter');
const SocketIoAdapter = require('./adapters/inbound/SocketIoAdapter');

class MonscapeApp {
  constructor(options = {}) {
    this.config = { ...config, ...options };
    this.puzzleRegistry = new PuzzleRegistry(this.config.initialPuzzles);

    // Instantiate Serial Adapter (Hardware or Mock)
    if (this.config.useMockSerial) {
      console.log('[MonscapeApp] Using MockSerialAdapter for serial transport.');
      this.serialAdapter = new MockSerialAdapter();
    } else {
      console.log('[MonscapeApp] Using RS485HardwareSerialAdapter for serial transport.');
      this.serialAdapter = new RS485HardwareSerialAdapter(this.config.serial);
    }

    // Instantiate HTTP & Socket.IO Adapters
    const baseDir = path.resolve(__dirname, '..');
    this.httpAdapter = new ExpressHttpAdapter({
      puzzleRegistry: this.puzzleRegistry,
      baseDir
    });

    this.broadcaster = new SocketIoAdapter(this.httpAdapter.getHttpServer());

    this.wirePipelines();
  }

  wirePipelines() {
    // Pipeline: Serial -> Domain Registry -> WebSocket Broadcast
    this.serialAdapter.onData((line) => {
      console.log('[Pipeline] Inbound Serial Data:', line);
      this.puzzleRegistry.processIncomingTrame(line);
      this.broadcaster.broadcast('message', line);
    });

    // Pipeline: WebSocket Client Command -> Serial Outbound
    this.broadcaster.onClientCommand((commandData) => {
      console.log('[Pipeline] Outbound Command to Serial:', commandData);
      this.serialAdapter.send(commandData, (err) => {
        if (err) {
          console.error('[Pipeline] Error sending command to serial:', err.message);
        }
      });
    });
  }

  async start() {
    try {
      await this.serialAdapter.open();
    } catch (err) {
      console.warn(`[MonscapeApp] Serial open failed: ${err.message}. Falling back to MockSerialAdapter.`);
      this.serialAdapter = new MockSerialAdapter();
      await this.serialAdapter.open();
      // Re-register pipeline on new adapter
      this.serialAdapter.onData((line) => {
        this.puzzleRegistry.processIncomingTrame(line);
        this.broadcaster.broadcast('message', line);
      });
    }

    return new Promise((resolve) => {
      this.httpAdapter.listen(this.config.port, () => {
        console.log(`[MonscapeApp] Server running on http://localhost:${this.config.port}`);
        resolve();
      });
    });
  }

  async stop() {
    await this.broadcaster.close();
    await this.httpAdapter.close();
    await this.serialAdapter.close();
  }
}

module.exports = MonscapeApp;

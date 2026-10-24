const { Server } = require('socket.io');
const IClientBroadcasterPort = require('../../ports/IClientBroadcasterPort');

class SocketIoAdapter extends IClientBroadcasterPort {
  constructor(httpServer) {
    super();
    this.io = new Server(httpServer);
    this.commandHandlers = [];
    this.setupListeners();
  }

  setupListeners() {
    this.io.on('connection', (socket) => {
      console.log(`[SocketIoAdapter] Client connected: ${socket.id}`);

      socket.on('Communication', (data) => {
        console.log(`[SocketIoAdapter] Received 'Communication' from ${socket.id}:`, data);
        for (const handler of this.commandHandlers) {
          handler(data, socket);
        }
      });

      socket.on('disconnect', () => {
        console.log(`[SocketIoAdapter] Client disconnected: ${socket.id}`);
      });
    });
  }

  broadcast(event, payload) {
    this.io.emit(event, payload);
  }

  onClientCommand(handler) {
    this.commandHandlers.push(handler);
  }

  close() {
    return new Promise((resolve) => {
      this.io.close(() => {
        console.log('[SocketIoAdapter] Socket.io server closed.');
        resolve();
      });
    });
  }
}

module.exports = SocketIoAdapter;

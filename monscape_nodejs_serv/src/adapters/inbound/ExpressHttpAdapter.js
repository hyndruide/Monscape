const express = require('express');
const http = require('http');
const path = require('path');

class ExpressHttpAdapter {
  constructor({ puzzleRegistry, baseDir }) {
    this.puzzleRegistry = puzzleRegistry;
    this.baseDir = baseDir || path.resolve(__dirname, '../../..');
    this.app = express();
    this.server = http.createServer(this.app);
    this.setupMiddleware();
    this.setupRoutes();
  }

  setupMiddleware() {
    this.app.use(express.json());
    this.app.use('/static', express.static(path.join(this.baseDir, 'public')));
    this.app.set('views', path.join(this.baseDir, 'views'));
    this.app.set('view engine', 'ejs');
  }

  setupRoutes() {
    this.app.get('/', (req, res) => {
      const data = this.puzzleRegistry.getForView();
      res.setHeader('Content-Type', 'text/html');
      res.render('tesjs.ejs', { data });
    });

    this.app.get('/api/puzzles', (req, res) => {
      res.json(this.puzzleRegistry.getAll().map(p => p.toJSON()));
    });
  }

  getHttpServer() {
    return this.server;
  }

  listen(port, callback) {
    return this.server.listen(port, callback);
  }

  close() {
    return new Promise((resolve) => {
      this.server.close(() => {
        resolve();
      });
    });
  }
}

module.exports = ExpressHttpAdapter;

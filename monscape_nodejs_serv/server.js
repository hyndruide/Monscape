/**
 * Monscape Conductor Server (Hexagonal Architecture Entrypoint)
 */
const MonscapeApp = require('./src/app');

const app = new MonscapeApp();

if (require.main === module) {
  app.start().catch((err) => {
    console.error('[Fatal Error] Failed to start Monscape server:', err);
    process.exit(1);
  });
}

module.exports = app;

const test = require('node:test');
const assert = require('node:assert/strict');
const http = require('node:http');

const MonscapeApp = require('../src/app');

test('MonscapeApp - start, HTTP endpoints & stop in mock mode', async () => {
  const testPort = 8181;
  const app = new MonscapeApp({
    port: testPort,
    useMockSerial: true,
    initialPuzzles: [{ name: 'PL1', type: 'Light', light: 1 }]
  });

  await app.start();

  // Test GET /
  const html = await new Promise((resolve, reject) => {
    http.get(`http://localhost:${testPort}/`, (res) => {
      assert.equal(res.statusCode, 200);
      let data = '';
      res.on('data', chunk => { data += chunk; });
      res.on('end', () => resolve(data));
    }).on('error', reject);
  });
  assert.ok(html.includes('PL1'));

  // Test GET /api/puzzles
  const puzzles = await new Promise((resolve, reject) => {
    http.get(`http://localhost:${testPort}/api/puzzles`, (res) => {
      assert.equal(res.statusCode, 200);
      let data = '';
      res.on('data', chunk => { data += chunk; });
      res.on('end', () => resolve(JSON.parse(data)));
    }).on('error', reject);
  });
  assert.equal(puzzles.length, 1);
  assert.equal(puzzles[0].name, 'PL1');

  // Test simulating serial data incoming
  app.serialAdapter.simulateIncoming(JSON.stringify({
    Nom: 'PL1',
    Adr: 'PL1',
    stat: 12
  }));

  const updatedPuzzle = app.puzzleRegistry.getByAddress('PL1');
  assert.equal(updatedPuzzle.state, 12);
  assert.equal(updatedPuzzle.isWin(), true);

  await app.stop();
});

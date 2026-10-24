const test = require('node:test');
const assert = require('node:assert/strict');

const Puzzle = require('../src/domain/Puzzle');
const PuzzleRegistry = require('../src/domain/PuzzleRegistry');
const CommandBuilder = require('../src/domain/CommandBuilder');

test('Puzzle Entity - state & trame updates', () => {
  const puzzle = new Puzzle({ name: 'PL1', address: 'PL1', type: 'Light' });
  assert.equal(puzzle.name, 'PL1');
  assert.equal(puzzle.address, 'PL1');
  assert.equal(puzzle.state, Puzzle.STATES.IDLE);
  assert.equal(puzzle.isWin(), false);
  assert.equal(puzzle.isBypass(), false);

  // Update with log trame
  puzzle.updateFromTrame(JSON.stringify({
    Nom: 'PL1',
    Adr: 'PL1',
    stat: 12
  }));
  assert.equal(puzzle.state, Puzzle.STATES.WIN);
  assert.equal(puzzle.isWin(), true);
  assert.equal(puzzle.isResolved(), true);

  // Update with init trame
  puzzle.updateFromTrame({
    Nom: 'PL1',
    Adr: 'PL1',
    V_G: '2.0',
    Desc: 'Light puzzle',
    Win: '9999'
  });
  assert.equal(puzzle.versionGame, '2.0');
  assert.equal(puzzle.desc, 'Light puzzle');
  assert.equal(puzzle.winCode, '9999');
});

test('PuzzleRegistry - registration and auto-discovery', () => {
  const registry = new PuzzleRegistry([
    { name: 'PG1', address: 'PG1', type: 'Keypad' }
  ]);

  assert.equal(registry.getAll().length, 1);
  assert.equal(registry.getByAddress('PG1').type, 'Keypad');

  // Discover new puzzle via incoming trame
  const discovered = registry.processIncomingTrame(JSON.stringify({
    Nom: 'PG2',
    Adr: 'PG2',
    Desc: 'Knob',
    stat: 6
  }));

  assert.ok(discovered);
  assert.equal(registry.getAll().length, 2);
  assert.equal(registry.getByAddress('PG2').name, 'PG2');
  assert.equal(registry.getByAddress('PG2').state, Puzzle.STATES.START);

  const viewData = registry.getForView();
  assert.equal(viewData.length, 2);
  assert.equal(viewData[0].name, 'PG1');
  assert.equal(viewData[1].name, 'PG2');
});

test('CommandBuilder - command factory & serialization', () => {
  const resetCmd = CommandBuilder.createReset('PG1');
  assert.deepEqual(resetCmd, { A: 'PG1', C: 'R' });

  const bypassCmd = CommandBuilder.createBypass('PG2');
  assert.deepEqual(bypassCmd, { A: 'PG2', C: 'B' });

  const infoCmd = CommandBuilder.createInfoRequest('PL1');
  assert.deepEqual(infoCmd, { A: 'PL1', C: 'I' });

  const serialized = CommandBuilder.serialize(bypassCmd);
  assert.equal(serialized, '{"A":"PG2","C":"B"}');

  const parsed = CommandBuilder.parse(serialized);
  assert.deepEqual(parsed, { A: 'PG2', C: 'B' });
  assert.equal(CommandBuilder.parse('invalid json'), null);
});

const Puzzle = require('./Puzzle');

/**
 * Domain Service / Repository: PuzzleRegistry
 * Manages the collection of active puzzles in the escape room.
 */
class PuzzleRegistry {
  constructor(initialPuzzles = []) {
    this.puzzles = new Map();
    for (const p of initialPuzzles) {
      this.register(p);
    }
  }

  register(puzzleConfig) {
    const puzzle = (puzzleConfig instanceof Puzzle)
      ? puzzleConfig
      : new Puzzle(puzzleConfig);
    this.puzzles.set(puzzle.address, puzzle);
    return puzzle;
  }

  getByAddress(address) {
    if (!address) return null;
    return this.puzzles.get(address) || null;
  }

  getByName(name) {
    for (const puzzle of this.puzzles.values()) {
      if (puzzle.name === name) return puzzle;
    }
    return null;
  }

  getAll() {
    return Array.from(this.puzzles.values());
  }

  getForView() {
    return this.getAll().map(p => ({
      name: p.name,
      type: p.type,
      light: p.light,
      state: p.state,
      address: p.address
    }));
  }

  processIncomingTrame(rawOrParsed) {
    let trame = rawOrParsed;
    if (typeof rawOrParsed === 'string') {
      try {
        trame = JSON.parse(rawOrParsed);
      } catch (e) {
        return null;
      }
    }

    if (!trame || typeof trame !== 'object') return null;

    const address = trame.Adr || trame.Nom;
    if (!address) return null;

    let puzzle = this.getByAddress(address) || this.getByName(address);
    if (!puzzle) {
      puzzle = this.register({
        name: trame.Nom || address,
        address: address,
        type: trame.Desc || 'Light',
        state: trame.stat !== undefined ? Number(trame.stat) : Puzzle.STATES.IDLE
      });
    }

    puzzle.updateFromTrame(trame);
    return puzzle;
  }
}

module.exports = PuzzleRegistry;

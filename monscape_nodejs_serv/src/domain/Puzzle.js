/**
 * Domain Entity: Puzzle
 * Represents an escape game puzzle node connected to the Monscape bus.
 */
class Puzzle {
  static STATES = {
    IDLE: 4,
    START: 6,
    BUZY: 9,
    BYPASS: 11,
    WIN: 12
  };

  constructor({ name, address, type = 'Light', light = 1, desc = '', winCode = '', state = Puzzle.STATES.IDLE }) {
    this.name = name || address || 'Unknown';
    this.address = address || name || 'UNK';
    this.type = type;
    this.light = light;
    this.desc = desc;
    this.winCode = winCode;
    this.state = state;
    this.versionGame = '1.0';
    this.versionMonscape = '0.2';
    this.lastInput = '';
    this.input = '';
    this.lastSeen = new Date();
  }

  updateFromTrame(data) {
    if (typeof data === 'string') {
      try {
        data = JSON.parse(data);
      } catch (e) {
        return false;
      }
    }

    if (!data || typeof data !== 'object') return false;

    this.lastSeen = new Date();

    if (data.Nom) this.name = data.Nom;
    if (data.Adr) this.address = data.Adr;
    if (data.V_G) this.versionGame = data.V_G;
    if (data.V_mon) this.versionMonscape = data.V_mon;
    if (data.Desc) this.desc = data.Desc;
    if (data.Win) this.winCode = data.Win;
    if (data.stat !== undefined) this.state = Number(data.stat);
    if (data.last_input !== undefined) this.lastInput = data.last_input;
    if (data.input !== undefined) this.input = data.input;

    return true;
  }

  isWin() {
    return this.state === Puzzle.STATES.WIN;
  }

  isBypass() {
    return this.state === Puzzle.STATES.BYPASS;
  }

  isResolved() {
    return this.isWin() || this.isBypass();
  }

  toJSON() {
    return {
      name: this.name,
      address: this.address,
      type: this.type,
      light: this.light,
      desc: this.desc,
      winCode: this.winCode,
      state: this.state,
      versionGame: this.versionGame,
      versionMonscape: this.versionMonscape,
      lastInput: this.lastInput,
      input: this.input,
      lastSeen: this.lastSeen
    };
  }
}

module.exports = Puzzle;

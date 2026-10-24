/**
 * Domain Service: CommandBuilder
 * Encapsulates the wire format and construction of commands sent to microcontrollers.
 */
class CommandBuilder {
  static createReset(targetAddress) {
    return { A: targetAddress, C: 'R' };
  }

  static createBypass(targetAddress) {
    return { A: targetAddress, C: 'B' };
  }

  static createInfoRequest(targetAddress) {
    return { A: targetAddress, C: 'I' };
  }

  static createDumpEeprom(targetAddress) {
    return { A: targetAddress, C: 'X' };
  }

  static createSpecial(targetAddress, commandCode, data) {
    return { A: targetAddress, C: commandCode || 'S', D: data };
  }

  static serialize(commandObj) {
    return JSON.stringify(commandObj);
  }

  static parse(rawString) {
    if (!rawString || typeof rawString !== 'string') return null;
    try {
      return JSON.parse(rawString.trim());
    } catch (e) {
      return null;
    }
  }
}

module.exports = CommandBuilder;

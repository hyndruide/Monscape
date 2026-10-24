const fs = require('fs');

const serialDevice = process.env.SERIAL_DEVICE || '/dev/ttyS0';
const isDevicePresent = fs.existsSync(serialDevice);

const config = {
  port: parseInt(process.env.PORT || '8080', 10),
  serial: {
    device: serialDevice,
    baudRate: parseInt(process.env.BAUD_RATE || '74880', 10),
    gpioRe: parseInt(process.env.GPIO_RE || '17', 10),
    gpioDe: parseInt(process.env.GPIO_DE || '27', 10)
  },
  useMockSerial: process.env.USE_MOCK_SERIAL === 'true' ||
                 process.env.NODE_ENV === 'test' ||
                 !isDevicePresent,
  initialPuzzles: [
    { name: "PL1", type: "Light", light: 1 }
  ]
};

module.exports = config;

const fs = require('fs');
const html = fs.readFileSync('index.html', 'utf-8');
const newCode = fs.readFileSync('day-render.js', 'utf-8');

const startMarker = '// ============================================================\n// LEARN PAGE';
const endMarker = '// ============================================================\n// CLOZE PAGE (短文完型)';

const startIdx = html.indexOf(startMarker);
const endIdx = html.indexOf(endMarker);

if (startIdx === -1 || endIdx === -1) {
  console.error('Boundary not found. start:', startIdx, 'end:', endIdx);
  process.exit(1);
}

const before = html.substring(0, startIdx);
const after = html.substring(endIdx);
const result = before + newCode + '\n' + after;

fs.writeFileSync('index.html', result, 'utf-8');
console.log('Replaced from', startIdx, 'to', endIdx);
console.log('New size:', result.length.toLocaleString(), 'bytes');
console.log('Done!');

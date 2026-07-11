const fs = require('fs');
const html = fs.readFileSync('index.html', 'utf-8');

// Extract WORDS
const match = html.match(/const WORDS = \[([\s\S]*?)\];/);
if (!match) { console.error('WORDS not found'); process.exit(1); }

let WORDS;
eval('WORDS = [' + match[1] + '];');

// Build standalone lookup
const wordDefMap = {};
WORDS.forEach(w => {
  if (w.meanings && w.meanings.length > 0) {
    wordDefMap[w.word.toLowerCase()] = w.meanings[0].def;
  }
});

// Simulate our lookup function
function getDerivativeDef(derivWord, parentWord) {
  const lower = derivWord.toLowerCase();
  if (wordDefMap[lower]) return wordDefMap[lower];
  if (parentWord && parentWord.notes) {
    const notes = parentWord.notes.toLowerCase();
    const idx = notes.indexOf(lower);
    if (idx !== -1) return '(found in notes)';
  }
  return null;
}

const missing = [];
WORDS.forEach(w => {
  if (!w.derivatives || w.derivatives.length === 0) return;
  w.derivatives.forEach(d => {
    if (!d.def && !getDerivativeDef(d.word, w)) {
      missing.push({ deriv: d.word, pos: d.pos, parent: w.word, parentMeanings: (w.meanings || []).map(m => m.def).join(', ') });
    }
  });
});

console.log('Total derivatives with missing defs: ' + missing.length);
missing.forEach(m => {
  console.log(`  ${m.deriv} (${m.pos}) ← ${m.parent} [${m.parentMeanings}]`);
});

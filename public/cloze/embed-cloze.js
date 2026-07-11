// embed-cloze.js — replace CLOZE_TESTS placeholder in index.html with real data
const fs = require('fs');

const clozeData = fs.readFileSync('cloze_tests.js', 'utf-8');
const indexHtml = fs.readFileSync('index.html', 'utf-8');

// Extract just the array content (strip module.exports)
let clozeContent = clozeData
  .replace(/^const CLOZE_TESTS = /, '')
  .replace(/\nif \(typeof module !== 'undefined' && module\.exports\) \{\n  module\.exports = \{ CLOZE_TESTS \};\n\}$/, '')
  .trim();

// clozeContent is now just the array literal: [\n  {...},\n  ...\n];
// We need to replace:
//   const CLOZE_TESTS = [];
//   // PLACEHOLDER_CLOZE — will be replaced with full parsed data
// with:
//   const CLOZE_TESTS = <clozeContent>;

const placeholder = `const CLOZE_TESTS = [];
// PLACEHOLDER_CLOZE — will be replaced with full parsed data`;

const replacement = 'const CLOZE_TESTS = ' + clozeContent;

if (!indexHtml.includes(placeholder)) {
  console.error('ERROR: Placeholder not found in index.html');
  console.error('Looking for:', placeholder.substring(0, 50) + '...');
  process.exit(1);
}

const result = indexHtml.replace(placeholder, replacement);
fs.writeFileSync('index.html', result, 'utf-8');

// Stats
const testCount = (clozeContent.match(/\bid:\s*\d+/g) || []).length;
const qCount = (clozeContent.match(/answer:\s*'[A-D]'/g) || []).length;
const blankCount = (clozeContent.match(/__BLANK_\d+__/g) || []).length;
console.log(`Embedded: ${testCount} tests, ${qCount} questions, ${blankCount} blank markers`);
console.log(`index.html: ${result.length.toLocaleString()} bytes`);
console.log('Done!');

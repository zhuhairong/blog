const fs = require('fs');
const html = fs.readFileSync('index.html', 'utf-8');
const exercises = fs.readFileSync('day-exercises.js', 'utf-8');

const marker = '</script>';
const idx = html.lastIndexOf(marker);
if (idx === -1) { console.error('</script> not found'); process.exit(1); }

const result = html.substring(0, idx) + '\n// ============================================================\n// DAY EXERCISES (auto-generated)\n// ============================================================\n' + exercises + '\n' + html.substring(idx);

fs.writeFileSync('index.html', result, 'utf-8');
console.log('Embedded DAY_EXERCISES. New size:', result.length.toLocaleString(), 'bytes');
console.log('Done!');

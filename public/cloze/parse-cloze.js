// parse-cloze.js — Parse 短文.md + 答案.txt to generate CLOZE_TESTS array
const fs = require('fs');

const passageMd = fs.readFileSync('短文.md', 'utf-8').replace(/\r\n/g, '\n');
const answerTxt = fs.readFileSync('答案.txt', 'utf-8').replace(/\r\n/g, '\n');

// Parse answers
const answerMap = {}; // "真题N" -> { num: answerLetter }
let currentExam = null;
for (const line of answerTxt.split('\n')) {
  const examMatch = line.match(/## 真题(\d+) 答案/);
  if (examMatch) {
    currentExam = `真题${examMatch[1]}`;
    answerMap[currentExam] = answerMap[currentExam] || {};
    continue;
  }
  const ansMatch = line.match(/\|\s*(\d+|\(\d+\))\s*\|\s*([A-D])\s*\|/);
  if (ansMatch && currentExam) {
    answerMap[currentExam][ansMatch[1]] = ansMatch[2];
  }
}

// Split by ## 真题N headers
const passageBlocks = [];
let currentBlock = null;
let currentTitle = '';
let examNum = 0;

for (const line of passageMd.split('\n')) {
  const examMatch = line.match(/^## 真题(\d+)/);
  if (examMatch) {
    if (currentBlock) passageBlocks.push({ num: examNum, title: currentTitle, lines: currentBlock });
    examNum = parseInt(examMatch[1]);
    currentTitle = '';
    currentBlock = [];
    continue;
  }
  if (examNum > 0) {
    // Skip meta lines
    if (line.startsWith('# ')) continue;
    if (/^阅读|^先阅读|^通读|^根据|^Think/.test(line.trim())) continue;

    // Extract title from ### heading
    if (line.startsWith('### ') && !currentTitle) {
      currentTitle = line.replace('### ', '').trim();
      continue;
    }
    currentBlock.push(line);
  }
}
if (currentBlock) passageBlocks.push({ num: examNum, title: currentTitle, lines: currentBlock });

function parsePassage(block) {
  const lines = block.lines;
  const questions = [];
  const textLines = [];

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];

    // Check if this line is a question line: **XX.** A. ... B. ... C. ... D. ...
    const qMatch = line.match(/^\*\*(\d+|\(\d+\))\.?\*\*\s+(.*)/);

    if (qMatch) {
      let qNum = qMatch[1].replace(/[()]/g, '');
      qNum = parseInt(qNum);
      const optionText = qMatch[2];

      // Gather options from this line + next few lines
      let allOptionText = optionText;
      let j = i + 1;
      while (j < lines.length && !lines[j].match(/^\*\*(\d+|\(\d+\))\.?\*\*\s+/) && j < i + 5) {
        const nl = lines[j].trim();
        if (nl && !nl.startsWith('##') && !nl.startsWith('#')) {
          allOptionText += ' ' + nl;
        }
        j++;
      }

      // Parse individual options: A. xxx B. xxx ...
      const options = {};
      // Split on [A-D]. patterns
      const parts = allOptionText.split(/([A-D])\.\s*/).filter(Boolean);
      for (let k = 0; k < parts.length; k += 2) {
        if (parts[k].match(/^[A-D]$/) && parts[k+1]) {
          options[parts[k]] = parts[k+1].trim().replace(/\s+/g, ' ');
        }
      }

      if (Object.keys(options).length >= 2) {
        questions.push({ number: qNum, options });
      }
      continue;
    }

    // Regular text line
    textLines.push(line);
  }

  // Clean text: remove trailing empty lines
  while (textLines.length && !textLines[textLines.length-1].trim()) textLines.pop();
  while (textLines.length && !textLines[0].trim()) textLines.shift();

  let text = textLines.join('\n').trim();
  // Collapse multiple blank lines
  text = text.replace(/\n{3,}/g, '\n\n');
  // Replace **XX** and **XX.** with __BLANK_XX__
  text = text.replace(/\*\*(\d+)\.?\*\*/g, '__BLANK_$1__');
  // Replace **XX** within text (not at line start)
  text = text.replace(/\*\*(\d+)\*\*/g, '__BLANK_$1__');

  return { questions, text };
}

// Generate output
let jsOutput = 'const CLOZE_TESTS = [\n';

for (const block of passageBlocks) {
  const { questions, text } = parsePassage(block);
  if (questions.length === 0) continue;

  const ansKey = answerMap[`真题${block.num}`] || {};

  // Map answers
  const items = [];
  for (const q of questions) {
    const answer = ansKey[String(q.number)] || ansKey[`(${q.number})`] || '';
    if (answer) {
      items.push({ number: q.number, options: q.options, answer });
    }
  }
  if (items.length === 0) continue;

  // Fix title for 真题3 which has sub-parts
  let title = block.title || `真题${block.num}`;
  // For 真题3, just check if title includes both parts
  if (title.startsWith('(') && block.num === 3) title = 'Bobonaro + Remy';

  // Escape backticks, backslashes, and single quotes in text/options
  const esc = (s) => s.replace(/\\/g, '\\\\').replace(/'/g, "\\'").replace(/\n/g, '\\n').replace(/\r/g, '');

  jsOutput += `  {\n`;
  jsOutput += `    id: ${block.num},\n`;
  jsOutput += `    title: '${esc(title)}',\n`;
  jsOutput += `    questionCount: ${items.length},\n`;
  jsOutput += `    sections: [\n`;
  jsOutput += `      { type: 'paragraph', text: '${esc(text)}' },\n`;
  jsOutput += `      { type: 'question_block', items: [\n`;

  for (const item of items) {
    jsOutput += `        { number: ${item.number}, options: {`;
    const opts = [];
    for (const [k, v] of Object.entries(item.options)) {
      opts.push(`'${k}': '${esc(v)}'`);
    }
    jsOutput += opts.join(', ');
    jsOutput += `}, answer: '${item.answer}' },\n`;
  }

  jsOutput += `      ]}\n`;
  jsOutput += `    ]\n`;
  jsOutput += `  },\n`;
}

jsOutput += '];\n';

fs.writeFileSync('cloze-data-generated.js', jsOutput, 'utf-8');
const count = jsOutput.split('id:').length - 1;
const totalQ = jsOutput.split('answer:').length - 1;
console.log(`Generated ${count} cloze tests with ${totalQ} questions`);

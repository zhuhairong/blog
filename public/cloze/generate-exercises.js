// generate-exercises.js — Generate natural sentence-based exercises for all 198 words
const fs = require('fs');

// Read the WORDS data from index.html
const html = fs.readFileSync('index.html', 'utf-8');
const wordsMatch = html.match(/const WORDS = (\[[\s\S]*?\]);/);
if (!wordsMatch) { console.error('Could not find WORDS array'); process.exit(1); }
const WORDS = eval(wordsMatch[1]);

// ============================================================
// Sentence templates by part of speech
// ============================================================

// For adjectives: test the word in a descriptive sentence
function adjSentence(word, meaning) {
  const templates = [
    `She is a very ___ person. Everyone likes her because she is always ${meaning}.`,
    `My teacher is very ___. She always treats every student with ${meaning}.`,
    `He is known for being ___. People say he has a ${meaning} heart.`,
    `The ___ neighbor helped the old man carry his heavy bags upstairs.`,
    `What a ___ thing to do! Your ${meaning} action touched everyone.`,
    `Being ___ is important. A ${meaning} person can make the world better.`,
    `Her ___ smile made everyone feel welcome and comfortable.`,
  ];
  return pickTemplate(templates, word, '___');
}

// For verbs: test the word in an action context
function verbSentence(word, meaning) {
  const templates = [
    `We should ___ before making important decisions. It's wise to ${word} carefully.`,
    `The teacher asked us to ___ the problem from different angles. Let's ${word} it together.`,
    `He tried to ___ the truth, but it was hard to ${word} what really happened.`,
    `I ___ that hard work will pay off. You should ${word} in yourself too.`,
    `She ___ed the gift with a big smile. It was wonderful to see her ${word} it happily.`,
    `They decided to ___ the project together. Working as a team to ${word} it was fun.`,
    `Can you ___ what happened? Please ${word} the story to us clearly.`,
  ];
  return pickTemplate(templates, word, '___');
}

// For nouns: test the noun in a meaningful context
function nounSentence(word, meaning) {
  const templates = [
    `His ___ made a big difference in the community. Everyone admired his ${word}.`,
    `She showed great ___ when facing difficulties. Her ${word} inspired us all.`,
    `The ___ of the project depends on everyone working together. Without ${word}, we might fail.`,
    `I have a strong ___ in music. This ${word} started when I was very young.`,
    `A positive ___ can change everything. Your ${word} affects people around you.`,
    `He finally got the ___ to study abroad. This ${word} came after years of hard work.`,
    `The ___ she gave me was very helpful. I followed her ${word} and succeeded.`,
  ];
  return pickTemplate(templates, word, '___');
}

// For word form / derivative testing
function derivativeSentence(baseWord, derivWord, derivPos, meaning) {
  const templates = [
    `The ___ of this discovery is huge. It has great ___ for science.`,
    `He handled the situation with ___. His ___ surprised everyone.`,
    `She showed great ___ in her work. Her ___ was recognized by the boss.`,
    `The ___ of the task made it difficult. But its ___ didn't stop them.`,
    `His ___ impressed the judges. They praised his ___ at the competition.`,
  ];
  let s = pickTemplate(templates, derivWord, '___');
  if (s.includes('___') && s.match(/___/g).length > 1) {
    s = s.replace('___', derivWord).replace('___', derivWord);
    s = `___ is important. He is known for his ___.`;
  }
  return s.replace(/\b(\w+)ness\b/g, (m) => `___`).replace(new RegExp(`\\b${derivWord}\\b`, 'gi'), '___');
}

// Helper: pick a template and insert the word (first blank as answer, others as clues)
function pickTemplate(templates, word, placeholder) {
  const t = templates[Math.floor(Math.random() * templates.length)];
  // Make sure the template has at least one blank
  if (!t.includes(placeholder)) {
    return t.replace(word, placeholder);
  }
  return t;
}

// ============================================================
// Generate exercises for each word
// ============================================================

function generateExercises(word) {
  const exercises = [];

  // Determine the primary "content word" meaning (skip prepositions, conjunctions, etc.)
  const contentMeanings = word.meanings.filter(m => {
    const pos = m.pos.toLowerCase();
    return pos.includes('adj') || pos.includes('adv') || pos.includes('n.') ||
           pos.includes('vt') || pos.includes('vi') || pos.includes('v.');
  });

  if (contentMeanings.length === 0) {
    // Function words like "for" — skip sentence generation, use definition recall
    return exercises;
  }

  const primaryPos = contentMeanings[0].pos.toLowerCase();
  const primaryDef = contentMeanings[0].def;

  // Exercise 1: Word meaning in context (sentence with blank)
  let sentence = '';
  if (primaryPos.includes('adj') || primaryPos.includes('adv')) {
    sentence = adjSentence(word.word, primaryDef);
  } else if (primaryPos.includes('vt') || primaryPos.includes('vi') || primaryPos.includes('v.')) {
    sentence = verbSentence(word.word, primaryDef);
  } else if (primaryPos.includes('n.')) {
    sentence = nounSentence(word.word, primaryDef);
  }

  if (sentence) {
    // Make sure sentence has exactly one ___ for the target word
    sentence = sentence.replace(new RegExp(`\\b${word.word}(ing|ed|s|es)?\\b`, 'gi'), (match) => {
      if (match.toLowerCase() === word.word.toLowerCase()) return '___';
      return match;
    });

    // If no ___ in sentence, add one
    if (!sentence.includes('___')) {
      sentence = sentence.replace(new RegExp(`\\b${word.word}\\b`, 'i'), '___');
    }
    // If still no ___, prepend
    if (!sentence.includes('___')) {
      sentence = `He is a ___ person. ` + sentence;
    }

    // Generate options (answer + 3 distractors)
    const options = generateOptions(word);

    exercises.push({
      type: 'context',
      sentence: sentence.trim(),
      answer: word.word,
      options: options,
      hint: primaryDef,
      pos: contentMeanings[0].pos
    });
  }

  // Exercise 2: Derivative / word form (if the word has derivatives)
  if (word.derivatives && word.derivatives.length > 0) {
    for (const deriv of word.derivatives.slice(0, 1)) { // Max 1 derivative exercise
      const derivWord = deriv.word;
      const defInfo = word.notes ? word.notes.split(';').find(s => s.includes(derivWord)) : '';
      const hint = defInfo ? defInfo.trim() : `${deriv.pos} ${derivWord}`;

      // Create a sentence that tests the derivative form
      let derivSents = [
        `He is known for his ___. Everyone admires his ${derivWord}.`,
        `She showed great ___ when helping others. Her ${derivWord} was appreciated.`,
        `The ___ of this work is important. We value its ${derivWord} very much.`,
        `What is the ___ between these two things? Can you tell the ${derivWord}?`,
        `His ___ made everyone feel comfortable. Such ${derivWord} is rare.`,
      ];

      let derivSent = derivSents[Math.floor(Math.random() * derivSents.length)];
      derivSent = derivSent.replace(new RegExp(`\\b${derivWord}\\b`, 'gi'), '___');

      // Options: the correct derivative + other words
      const allDerivs = word.derivatives.map(d => d.word);
      const otherWords = WORDS.filter(w => w.id !== word.id)
        .filter(w => w.derivatives && w.derivatives.length > 0)
        .flatMap(w => w.derivatives.map(d => d.word))
        .filter(w => !allDerivs.includes(w))
        .sort(() => Math.random() - 0.5)
        .slice(0, 3);

      while (otherWords.length < 3) {
        const rw = WORDS[Math.floor(Math.random() * WORDS.length)];
        if (!allDerivs.includes(rw.word) && !otherWords.includes(rw.word)) {
          otherWords.push(rw.word);
        }
      }

      const dOptions = [derivWord, ...otherWords.slice(0, 3)].sort(() => Math.random() - 0.5);

      exercises.push({
        type: 'wordform',
        sentence: derivSent,
        answer: derivWord,
        baseWord: word.word,
        options: dOptions,
        hint: hint,
        pos: deriv.pos
      });
    }
  }

  return exercises;
}

function generateOptions(correctWord) {
  // Pick 3 distractor words — prefer words from the same part of speech
  const correctPos = correctWord.meanings[0]?.pos?.toLowerCase() || '';
  const samePosWords = WORDS.filter(w => {
    if (w.id === correctWord.id) return false;
    const p = w.meanings[0]?.pos?.toLowerCase() || '';
    if (correctPos.includes('adj') && p.includes('adj')) return true;
    if ((correctPos.includes('vt') || correctPos.includes('vi') || correctPos.includes('v.')) &&
        (p.includes('vt') || p.includes('vi') || p.includes('v.'))) return true;
    if (correctPos.includes('n.') && p.includes('n.')) return true;
    return false;
  });

  if (samePosWords.length < 3) {
    // Fill with random words
    const more = WORDS.filter(w => w.id !== correctWord.id && !samePosWords.includes(w));
    samePosWords.push(...more);
  }

  const distractors = samePosWords.sort(() => Math.random() - 0.5).slice(0, 3).map(w => w.word);
  // Also include synonyms if available
  if (correctWord.synonyms && correctWord.synonyms.length > 0) {
    distractors[0] = correctWord.synonyms[0]; // Replace one distractor with a synonym
  }

  const options = [correctWord.word, ...distractors].sort(() => Math.random() - 0.5);
  return [...new Set(options)]; // Remove duplicates
}

// ============================================================
// Generate all exercises grouped by day
// ============================================================

const DAY_EXERCISES = {};
const wordsByDay = {};
WORDS.forEach(w => {
  if (!wordsByDay[w.day]) wordsByDay[w.day] = [];
  wordsByDay[w.day].push(w);
});

let totalExercises = 0;
for (const [day, words] of Object.entries(wordsByDay)) {
  DAY_EXERCISES[day] = [];
  for (const word of words) {
    const exercises = generateExercises(word);
    if (exercises.length > 0) {
      DAY_EXERCISES[day].push({
        wordId: word.id,
        word: word.word,
        meanings: word.meanings,
        exercises: exercises
      });
      totalExercises += exercises.length;
    }
  }
}

// ============================================================
// Output JS file
// ============================================================

const esc = (s) => {
  if (typeof s !== 'string') return s;
  return s.replace(/\\/g, '\\\\').replace(/'/g, "\\'").replace(/\n/g, '\\n');
};

let output = '// Auto-generated sentence-based exercises for all 198 words\n';
output += 'const DAY_EXERCISES = {\n';

for (const day of Object.keys(DAY_EXERCISES).sort((a,b) => a-b)) {
  const items = DAY_EXERCISES[day];
  output += `  ${day}: [\n`;
  for (const item of items) {
    output += `    { wordId: ${item.wordId}, word: '${item.word}', exercises: [\n`;
    for (const ex of item.exercises) {
      output += `      { type: '${ex.type}', sentence: '${esc(ex.sentence)}', answer: '${esc(ex.answer)}',`;
      output += ` options: [${ex.options.map(o => `'${esc(o)}'`).join(', ')}],`;
      output += ` hint: '${esc(ex.hint || '')}', pos: '${esc(ex.pos || '')}'`;
      if (ex.baseWord) output += `, baseWord: '${esc(ex.baseWord)}'`;
      output += ` },\n`;
    }
    output += `    ]},\n`;
  }
  output += `  ],\n`;
}
output += '};\n';

fs.writeFileSync('day-exercises.js', output, 'utf-8');
console.log(`Generated ${totalExercises} exercises across ${Object.keys(DAY_EXERCISES).length} days`);
console.log(`Output: day-exercises.js (${output.length.toLocaleString()} bytes)`);

// Stats
const typeCounts = {};
for (const items of Object.values(DAY_EXERCISES)) {
  for (const item of items) {
    for (const ex of item.exercises) {
      typeCounts[ex.type] = (typeCounts[ex.type] || 0) + 1;
    }
  }
}
console.log('Exercise types:', JSON.stringify(typeCounts));

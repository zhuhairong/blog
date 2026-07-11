// restructure.js — Replace old tab-based rendering with day-based architecture
const fs = require('fs');

const NEW_CODE = `
// ============================================================
// DAY-BASED LEARNING STATE
// ============================================================
let currentView = 'map';    // 'map' | 'detail'
let currentDay = 1;
let currentStep = 1;        // 1=vocab, 2=fillblank, 3=quiz, 4=spell, 5=match
let stepState = {};         // per-day per-step state

// Step icons and labels
const STEP_INFO = [
  { icon: '📖', label: '词汇记忆', key: 'learn' },
  { icon: '✍️', label: '语境填空', key: 'fillblank' },
  { icon: '🎯', label: '选择练习', key: 'quiz' },
  { icon: '📝', label: '拼写练习', key: 'spell' },
  { icon: '🔗', label: '配对游戏', key: 'match' }
];

function getDayProgress(dayNum) {
  if (!appState.dayProgress[dayNum]) {
    appState.dayProgress[dayNum] = {
      unlocked: dayNum === 1,
      completed: false,
      stars: 0,
      currentStep: 1,
      steps: {}
    };
  }
  return appState.dayProgress[dayNum];
}

function isDayUnlocked(dayNum) {
  if (dayNum === 1) return true;
  return getDayProgress(dayNum).unlocked;
}

function unlockNextDay(currentDayNum) {
  const next = currentDayNum + 1;
  if (next <= 19) {
    const dp = getDayProgress(next);
    dp.unlocked = true;
    saveState();
  }
}

function getDayWords(dayNum) {
  return WORDS.filter(w => w.day === dayNum);
}

function getDayExerciseData(dayNum) {
  return (typeof DAY_EXERCISES !== 'undefined' && DAY_EXERCISES[dayNum]) ? DAY_EXERCISES[dayNum] : [];
}

// ============================================================
// DAY MAP (Home Page)
// ============================================================
function renderDayMap() {
  currentView = 'map';
  const container = document.getElementById('page-daily-learn');
  const wordsByDay = getWordsByDay();
  const days = Object.keys(wordsByDay).map(Number).sort((a,b) => a-b);
  const totalLearned = getLearnedCount();
  const totalMastered = getMasteredCount();
  const completedDays = Object.values(appState.dayProgress).filter(d => d.completed).length;

  let html = '';

  // Header
  html += '<div class="day-map-header">';
  html += '<h2>📅 每日学习</h2>';
  html += '<p class="subtitle">每天学习一组词汇，循序渐进，逐天解锁</p>';
  html += '</div>';

  // Stats
  html += '<div class="day-map-stats">';
  html += `<div class="stat-item"><div class="stat-num">${totalLearned}</div><div class="stat-label">已学单词 / 198</div></div>`;
  html += `<div class="stat-item"><div class="stat-num">${totalMastered}</div><div class="stat-label">已掌握</div></div>`;
  html += `<div class="stat-item"><div class="stat-num">${completedDays}</div><div class="stat-label">已完成天数 / ${days.length}</div></div>`;
  html += `<div class="stat-item"><div class="stat-num">🔥 ${appState.streak.current}</div><div class="stat-label">连续天数</div></div>`;
  html += '</div>';

  // Day grid
  html += '<div class="day-grid">';
  for (const d of days) {
    const dp = getDayProgress(d);
    const dayWords = wordsByDay[d];
    const unlocked = dp.unlocked;
    const completed = dp.completed;
    const stars = dp.stars || 0;
    const inProgress = unlocked && !completed && (dp.currentStep > 1 || Object.keys(dp.steps).length > 0);

    let cardClass = '';
    let badgeHtml = '';
    if (!unlocked) {
      cardClass = 'locked';
      badgeHtml = '<span class="day-badge" style="background:#FEE2E2;color:#DC2626">🔒</span>';
    } else if (completed) {
      cardClass = 'completed';
    } else if (inProgress) {
      cardClass = 'current';
      badgeHtml = '<span class="day-badge doing">进行中</span>';
    } else if (unlocked) {
      cardClass = '';
      badgeHtml = '<span class="day-badge new">新!</span>';
    }

    const starsStr = completed ? ('⭐'.repeat(Math.max(1, stars)) + '☆'.repeat(3 - Math.max(1, stars))) : '☆☆☆';

    html += `<div class="day-card ${cardClass}" onclick="${unlocked ? "openDay("+d+")" : "showToast('请先完成前一天的练习','info')"}" style="animation:slideUp ${0.3 + d*0.03}s ease both">`;
    html += badgeHtml;
    html += `<div class="day-num">Day ${d}</div>`;
    html += `<div class="day-label">${dayWords.length} 个单词</div>`;
    html += `<div class="day-stars" style="color:${completed?'#F59E0B':'#D1D5DB'}">${starsStr}</div>`;
    html += '</div>';
  }
  html += '</div>';

  // Find current in-progress day for quick continue
  let activeDay = null;
  for (const d of days) {
    const dp = getDayProgress(d);
    if (dp.unlocked && !dp.completed) { activeDay = d; break; }
  }
  if (activeDay) {
    const dp = getDayProgress(activeDay);
    const stepLabel = dp.currentStep > 1 && dp.currentStep <= 5 ? STEP_INFO[dp.currentStep-1].label : '开始学习';
    html += `<div style="text-align:center;margin-top:20px">
      <button class="btn btn-primary btn-lg" onclick="openDay(${activeDay})" style="animation:pulse 2s infinite">
        ${dp.currentStep > 1 ? '▶ 继续 Day '+activeDay+' — '+stepLabel : '🚀 开始 Day '+activeDay}
      </button>
    </div>`;
  }

  container.innerHTML = html;
}

function openDay(dayNum) {
  currentDay = dayNum;
  currentView = 'detail';
  const dp = getDayProgress(dayNum);
  currentStep = dp.currentStep || 1;
  renderDayDetail(dayNum);
}

function goBackToMap() {
  currentView = 'map';
  currentStep = 1;
  renderDayMap();
}

// ============================================================
// DAY DETAIL (5-Step Learning Flow)
// ============================================================
function renderDayDetail(dayNum) {
  currentView = 'detail';
  currentDay = dayNum;
  const container = document.getElementById('page-daily-learn');
  const dp = getDayProgress(dayNum);
  const dayWords = getDayWords(dayNum);

  let html = '';

  // Header
  html += '<div class="day-detail-header">';
  html += '<button class="back-btn" onclick="goBackToMap()" title="返回">←</button>';
  html += '<div>';
  html += `<div class="day-title">Day ${dayNum}</div>`;
  html += `<div class="day-subtitle">${dayWords.length} 个单词 · 5步学习流程</div>`;
  html += '</div>';
  html += '</div>';

  // Step progress bar
  html += '<div class="step-progress-bar">';
  for (let i = 0; i < 5; i++) {
    const s = STEP_INFO[i];
    const stepNum = i + 1;
    let cls = '';
    if (dp.steps && dp.steps[stepNum] && dp.steps[stepNum].done) cls = 'done';
    else if (stepNum === currentStep) cls = 'active';

    if (i > 0) {
      const lineDone = (dp.steps && dp.steps[i] && dp.steps[i].done) ? ' done' : '';
      html += `<div class="step-line${lineDone}"></div>`;
    }
    html += `<div class="step-dot ${cls}" title="${s.label}">
      ${cls === 'done' ? '✓' : s.icon}
      <span class="step-label">${s.label}</span>
    </div>`;
  }
  html += '</div>';

  // Step content
  html += '<div class="step-content" id="stepContent">';
  html += renderStepContent(dayNum, currentStep);
  html += '</div>';

  // Navigation buttons
  html += '<div class="step-actions">';
  if (currentStep > 1) {
    html += `<button class="btn btn-outline" onclick="prevStep(${dayNum})">← 上一步</button>`;
  } else {
    html += '<span></span>';
  }
  html += `<button class="btn btn-primary" id="btnNextStep" onclick="nextStep(${dayNum})">`;
  html += currentStep < 5 ? '下一步 →' : '完成学习 🎉';
  html += '</button>';
  html += '</div>';

  container.innerHTML = html;
}

function renderStepContent(dayNum, step) {
  switch(step) {
    case 1: return renderStepVocab(dayNum);
    case 2: return renderStepFillBlank(dayNum);
    case 3: return renderStepQuiz(dayNum);
    case 4: return renderStepSpell(dayNum);
    case 5: return renderStepMatch(dayNum);
    default: return '';
  }
}

function prevStep(dayNum) {
  if (currentStep > 1) {
    currentStep--;
    const dp = getDayProgress(dayNum);
    dp.currentStep = currentStep;
    saveState();
    renderDayDetail(dayNum);
  }
}

function nextStep(dayNum) {
  const dp = getDayProgress(dayNum);

  // Save current step progress
  if (!dp.steps) dp.steps = {};
  if (!dp.steps[currentStep]) dp.steps[currentStep] = {};
  dp.steps[currentStep].done = true;

  if (currentStep < 5) {
    currentStep++;
    dp.currentStep = currentStep;
    saveState();
    renderDayDetail(dayNum);
    // Scroll to top
    document.getElementById('page-daily-learn').scrollIntoView({behavior:'smooth'});
  } else {
    // Complete the day
    completeDay(dayNum);
  }
}

function completeDay(dayNum) {
  const dp = getDayProgress(dayNum);

  // Calculate stars based on step performance
  let totalScore = 0;
  let totalMax = 0;
  for (let s = 2; s <= 5; s++) {
    if (dp.steps[s]) {
      totalScore += dp.steps[s].score || 0;
      totalMax += dp.steps[s].total || 0;
    }
  }
  const pct = totalMax > 0 ? totalScore / totalMax : 0;
  let stars = 1;
  if (pct >= 0.9) stars = 3;
  else if (pct >= 0.7) stars = 2;

  dp.completed = true;
  dp.stars = stars;
  dp.currentStep = 5;
  saveState();

  // Mark all words in this day as mastered
  getDayWords(dayNum).forEach(w => {
    if (!appState.wordMastery[w.id] || appState.wordMastery[w.id] < 2) {
      appState.wordMastery[w.id] = 2;
    }
  });

  // Unlock next day
  unlockNextDay(dayNum);

  // Update streak
  updateStreak();

  // Show completion overlay
  showDayComplete(dayNum, stars, pct);
}

function showDayComplete(dayNum, stars, pct) {
  const overlay = document.createElement('div');
  overlay.className = 'day-complete-overlay';
  overlay.id = 'dayCompleteOverlay';

  const msgs = stars === 3 ? ['太棒了！', '完美通关！', '满分学霸！'] :
              stars === 2 ? ['做得不错！', '继续加油！', '还有进步空间~'] :
              ['完成啦！', '再接再厉！', '每一天都在进步！'];
  const msg = msgs[Math.floor(Math.random() * msgs.length)];

  overlay.innerHTML = '<div class="day-complete-card">' +
    '<div class="big-emoji">🎉</div>' +
    '<h2 style="margin:8px 0">' + msg + '</h2>' +
    '<div class="stars-display">' + '⭐'.repeat(stars) + '☆'.repeat(3-stars) + '</div>' +
    '<div class="score-text">正确率: ' + Math.round(pct * 100) + '%</div>' +
    '<p style="color:var(--text-secondary);font-size:.85rem;margin:8px 0">Day ' + dayNum + ' 完成！</p>' +
    (dayNum < 19 ? '<p style="color:var(--success);font-size:.85rem">🔓 Day ' + (dayNum+1) + ' 已解锁！</p>' : '<p style="color:var(--primary);font-size:.85rem">🏆 全部19天已完成！</p>') +
    '<div style="display:flex;gap:8px;justify-content:center;margin-top:16px;flex-wrap:wrap">' +
    '<button class="btn btn-outline" onclick="closeDayComplete();openDay(' + dayNum + ')">📖 复习本天</button>' +
    '<button class="btn btn-primary" onclick="closeDayComplete();goBackToMap()">📅 返回主页</button>' +
    (dayNum < 19 ? '<button class="btn btn-success" onclick="closeDayComplete();openDay(' + (dayNum+1) + ')">▶ 挑战 Day ' + (dayNum+1) + '</button>' : '') +
    '</div></div>';

  document.body.appendChild(overlay);

  // Spawn particles
  setTimeout(() => {
    const card = overlay.querySelector('.day-complete-card');
    if (card) {
      const rect = card.getBoundingClientRect();
      spawnParticles(rect.left + rect.width/2, rect.top + 20, 15);
    }
  }, 300);
}

function closeDayComplete() {
  const overlay = document.getElementById('dayCompleteOverlay');
  if (overlay) overlay.remove();
  updateHeaderStats();
}

// ============================================================
// STEP 1: VOCABULARY LEARNING (Flashcard + Word List)
// ============================================================
let vocabCardIdx = 0;
let vocabShowFront = true;

function renderStepVocab(dayNum) {
  const words = getDayWords(dayNum);
  if (words.length === 0) return '<p>暂无词汇数据</p>';

  // Reset vocab state
  if (vocabCardIdx >= words.length) vocabCardIdx = 0;

  let html = '<div style="max-width:500px;margin:0 auto">';

  // Flashcard
  const w = words[vocabCardIdx];
  const mainDef = w.meanings && w.meanings.length > 0 ? w.meanings[0].def : '';
  const allDefs = (w.meanings || []).map(m => `<span class="tag tag-primary" style="margin:2px">${m.pos} ${m.def}</span>`).join(' ');

  html += '<div class="flashcard-container" onclick="flipVocabCard()" id="vocabFlashcard" style="height:260px">';
  html += '<div class="flashcard" id="vocabFlashcardInner">';
  // Front
  html += '<div class="flashcard-face flashcard-front">';
  html += '<div class="fc-word">' + w.word + '</div>';
  html += '<div class="fc-hint">点击翻转查看释义</div>';
  html += '</div>';
  // Back
  html += '<div class="flashcard-face flashcard-back">';
  html += '<div style="font-size:1.6rem;font-weight:700;margin-bottom:8px;color:var(--primary)">' + w.word + '</div>';
  html += '<div style="font-size:.95rem;color:var(--text)">' + allDefs + '</div>';
  if (w.derivatives && w.derivatives.length > 0) {
    html += '<div style="font-size:.8rem;color:var(--text-secondary);margin-top:8px">派生: ' + w.derivatives.map(d => d.word + ' (' + d.pos + ')').join(', ') + '</div>';
  }
  if (w.phrases && w.phrases.length > 0) {
    html += '<div style="font-size:.75rem;color:var(--text-light);margin-top:4px">' + w.phrases.slice(0,3).join(' | ') + '</div>';
  }
  html += '</div>';
  html += '</div></div>';

  // Card counter + nav
  html += '<div style="display:flex;justify-content:space-between;align-items:center;margin-top:12px">';
  html += '<button class="btn btn-sm btn-ghost" onclick="prevVocabCard(' + dayNum + ')" ' + (vocabCardIdx===0?'disabled':'') + '>◀</button>';
  html += '<span style="font-size:.85rem;color:var(--text-secondary)">' + (vocabCardIdx+1) + ' / ' + words.length + '</span>';
  html += '<button class="btn btn-sm btn-ghost" onclick="nextVocabCard(' + dayNum + ')" ' + (vocabCardIdx>=words.length-1?'disabled':'') + '>▶</button>';
  html += '</div>';

  // Word list (compact)
  html += '<div style="margin-top:16px;max-height:200px;overflow-y:auto">';
  html += '<div style="font-size:.8rem;font-weight:700;color:var(--text-secondary);margin-bottom:8px">本天词汇列表 (点击切换)</div>';
  for (let i = 0; i < words.length; i++) {
    const ww = words[i];
    const def = ww.meanings && ww.meanings.length > 0 ? ww.meanings[0].def : '';
    const active = i === vocabCardIdx;
    html += '<div style="padding:6px 10px;cursor:pointer;border-radius:6px;' + (active?'background:var(--primary-bg);font-weight:700':'') + '" onclick="jumpVocabCard(' + dayNum + ',' + i + ')">';
    html += '<span style="font-family:monospace;font-weight:600">' + ww.word + '</span>';
    html += ' <span style="color:var(--text-secondary);font-size:.8rem">' + def + '</span>';
    html += '</div>';
  }
  html += '</div>';

  html += '</div>';
  return html;
}

function flipVocabCard() {
  const inner = document.getElementById('vocabFlashcardInner');
  if (inner) {
    vocabShowFront = !vocabShowFront;
    inner.classList.toggle('flipped', !vocabShowFront);
  }
}

function prevVocabCard(dayNum) {
  if (vocabCardIdx > 0) {
    vocabCardIdx--;
    vocabShowFront = true;
    refreshVocabStep(dayNum);
  }
}

function nextVocabCard(dayNum) {
  const words = getDayWords(dayNum);
  if (vocabCardIdx < words.length - 1) {
    vocabCardIdx++;
    vocabShowFront = true;
    refreshVocabStep(dayNum);
  }
}

function jumpVocabCard(dayNum, idx) {
  vocabCardIdx = idx;
  vocabShowFront = true;
  refreshVocabStep(dayNum);
}

function refreshVocabStep(dayNum) {
  const el = document.getElementById('stepContent');
  if (el) el.innerHTML = renderStepVocab(dayNum);
}

// ============================================================
// STEP 2: SENTENCE FILL-BLANK (语境填空)
// ============================================================
let fillblankState = {}; // {dayNum: {idx, answers:{}, submitted:bool, score:int}}

function renderStepFillBlank(dayNum) {
  const exData = getDayExerciseData(dayNum);
  const words = getDayWords(dayNum);

  // Initialize state
  if (!fillblankState[dayNum]) {
    fillblankState[dayNum] = { idx: 0, answers: {}, submitted: false, score: 0, results: {} };
  }
  const fb = fillblankState[dayNum];

  // Build exercises: use DAY_EXERCISES if available, otherwise build from words
  let exercises = [];
  if (exData.length > 0) {
    exData.forEach(item => {
      item.exercises.filter(e => e.type === 'context').forEach(e => {
        exercises.push({
          wordId: item.wordId,
          word: item.word,
          sentence: e.sentence,
          answer: e.answer,
          options: e.options,
          hint: e.hint,
          pos: e.pos
        });
      });
    });
  }

  // Fallback: build simple sentence exercises from words
  if (exercises.length === 0) {
    words.forEach(w => {
      const def = w.meanings && w.meanings.length > 0 ? w.meanings[0].def : '';
      const pos = w.meanings && w.meanings.length > 0 ? w.meanings[0].pos : '';
      exercises.push({
        wordId: w.id,
        word: w.word,
        sentence: 'The word "___" means ' + def + '.',
        answer: w.word,
        options: [w.word, ...getRandomDistractors(w, 3).map(d => d.word)],
        hint: def,
        pos: pos
      });
    });
  }

  if (fb.idx >= exercises.length) fb.idx = 0;
  const ex = exercises[fb.idx];
  const userAnswer = fb.answers[fb.idx] || '';
  const result = fb.results[fb.idx];

  let html = '<div style="max-width:600px;margin:0 auto">';
  html += '<div style="font-size:.85rem;color:var(--text-secondary);margin-bottom:12px">第 ' + (fb.idx+1) + ' / ' + exercises.length + ' 题 · 根据语境填入正确的单词</div>';

  // Sentence display
  html += '<div class="sentence-exercise">';
  if (ex.pos) html += '<span class="sent-pos">' + ex.pos + '</span>';

  // Show sentence with blank filled or not
  let sentHtml = ex.sentence;
  if (fb.submitted) {
    const cls = result && result.correct ? 'correct' : 'wrong';
    sentHtml = sentHtml.replace('___', '<span class="blank filled ' + cls + '">' + (userAnswer || '___') + '</span>');
    if (!result || !result.correct) {
      sentHtml += ' <span style="color:var(--success);font-weight:700">(' + ex.answer + ')</span>';
    }
  } else {
    sentHtml = sentHtml.replace('___', '<span class="blank' + (userAnswer ? ' filled' : '') + '">' + (userAnswer || '___') + '</span>');
  }
  html += '<div class="sent-text">' + sentHtml + '</div>';
  if (ex.hint) html += '<div class="sent-hint">💡 提示: ' + ex.hint + '</div>';
  html += '</div>';

  // Input or choices
  if (!fb.submitted) {
    if (ex.options && ex.options.length >= 2) {
      html += '<div class="choice-grid">';
      ex.options.forEach(opt => {
        const sel = userAnswer === opt ? ' selected' : '';
        html += '<button class="choice-btn' + sel + '" onclick="selectFillblank(' + dayNum + ',' + fb.idx + ',\\'' + opt + '\\')">' + opt + '</button>';
      });
      html += '</div>';
    } else {
      html += '<div class="spell-input-row" style="margin-top:12px">';
      html += '<input class="input" id="fillblankInput" type="text" placeholder="输入单词..." value="' + (userAnswer||'') + '" onkeydown="if(event.key===\\'Enter\\')submitFillblank(' + dayNum + ')">';
      html += '<button class="btn btn-primary" onclick="submitFillblank(' + dayNum + ')">确认</button>';
      html += '</div>';
    }
  } else {
    // Show result + next button
    const isCorrect = result && result.correct;
    html += '<div style="text-align:center;margin-top:16px;font-size:1.1rem;' + (isCorrect?'color:var(--success)':'color:var(--danger)') + '">';
    html += isCorrect ? '✅ 正确!' : '❌ 正确答案: ' + ex.answer;
    html += '</div>';
  }

  // Navigation
  html += '<div style="display:flex;justify-content:space-between;margin-top:16px">';
  html += '<button class="btn btn-sm btn-ghost" onclick="prevFillblank(' + dayNum + ')" ' + (fb.idx===0?'disabled':'') + '>◀ 上一题</button>';
  if (fb.submitted) {
    html += '<button class="btn btn-sm btn-primary" onclick="nextFillblank(' + dayNum + ',' + exercises.length + ')">下一题 ▶</button>';
  } else if (ex.options && ex.options.length >= 2) {
    html += '<button class="btn btn-sm btn-primary" onclick="submitFillblankChoice(' + dayNum + ',' + fb.idx + ')" ' + (!userAnswer?'disabled':'') + '>提交</button>';
  }
  html += '</div>';

  html += '</div>';
  return html;
}

function selectFillblank(dayNum, idx, word) {
  if (!fillblankState[dayNum]) return;
  fillblankState[dayNum].answers[idx] = word;
  refreshFillblankStep(dayNum);
}

function submitFillblank(dayNum) {
  const input = document.getElementById('fillblankInput');
  if (!input || !fillblankState[dayNum]) return;
  const fb = fillblankState[dayNum];
  fb.answers[fb.idx] = input.value.trim();
  fb.submitted = true;
  checkFillblankAnswer(dayNum);
  refreshFillblankStep(dayNum);
}

function submitFillblankChoice(dayNum, idx) {
  if (!fillblankState[dayNum]) return;
  fillblankState[dayNum].submitted = true;
  checkFillblankAnswer(dayNum);
  refreshFillblankStep(dayNum);
}

function checkFillblankAnswer(dayNum) {
  const fb = fillblankState[dayNum];
  const exData = getDayExerciseData(dayNum);
  const words = getDayWords(dayNum);

  // Rebuild exercises same as render
  let exercises = [];
  if (exData.length > 0) {
    exData.forEach(item => {
      item.exercises.filter(e => e.type === 'context').forEach(e => {
        exercises.push({ wordId: item.wordId, word: item.word, sentence: e.sentence, answer: e.answer, options: e.options, hint: e.hint, pos: e.pos });
      });
    });
  }
  if (exercises.length === 0) {
    words.forEach(w => {
      const def = w.meanings && w.meanings.length > 0 ? w.meanings[0].def : '';
      const pos = w.meanings && w.meanings.length > 0 ? w.meanings[0].pos : '';
      exercises.push({ wordId: w.id, word: w.word, sentence: 'The word "___" means ' + def + '.', answer: w.word, options: [w.word, ...getRandomDistractors(w, 3).map(d => d.word)], hint: def, pos: pos });
    });
  }

  const ex = exercises[fb.idx];
  const userAnswer = (fb.answers[fb.idx] || '').trim().toLowerCase();
  const correct = userAnswer === ex.answer.toLowerCase();
  fb.results[fb.idx] = { correct, userAnswer };
  if (correct) fb.score++;

  // Save to day progress
  const dp = getDayProgress(dayNum);
  if (!dp.steps) dp.steps = {};
  dp.steps[2] = { done: fb.idx >= exercises.length - 1, score: fb.score, total: fb.idx + 1 };
  saveState();

  // Track wrong words
  if (!correct) {
    const w = getWordById(ex.wordId);
    if (w && !appState.wrongWords.includes(w.id)) {
      appState.wrongWords.push(w.id);
      saveState();
    }
  }
}

function prevFillblank(dayNum) {
  if (!fillblankState[dayNum]) return;
  const fb = fillblankState[dayNum];
  if (fb.idx > 0) {
    fb.idx--;
    fb.submitted = !!fb.results[fb.idx];
    refreshFillblankStep(dayNum);
  }
}

function nextFillblank(dayNum, total) {
  if (!fillblankState[dayNum]) return;
  const fb = fillblankState[dayNum];
  if (fb.idx < total - 1) {
    fb.idx++;
    fb.submitted = !!fb.results[fb.idx];
    refreshFillblankStep(dayNum);
  }
}

function refreshFillblankStep(dayNum) {
  const el = document.getElementById('stepContent');
  if (el) el.innerHTML = renderStepFillBlank(dayNum);
}

// ============================================================
// STEP 3: MULTIPLE CHOICE QUIZ
// ============================================================
let quizDayState = {}; // {dayNum: {idx, score, total, answered, results:[]}}

function renderStepQuiz(dayNum) {
  const words = getDayWords(dayNum);
  if (words.length === 0) return '<p>暂无词汇</p>';

  if (!quizDayState[dayNum]) {
    // Generate quiz questions
    const questions = [];
    words.forEach(w => {
      const def = w.meanings && w.meanings.length > 0 ? w.meanings[0].def : '';
      const mode = Math.random() > 0.5 ? 'en2zh' : 'zh2en';
      if (mode === 'en2zh') {
        const distractors = getRandomDistractors(w, 3);
        questions.push({
          wordId: w.id, word: w.word, mode: 'en2zh',
          question: w.word,
          answer: def,
          options: [def, ...distractors.map(d => (d.meanings&&d.meanings[0])?d.meanings[0].def:d.word)].sort(()=>Math.random()-0.5)
        });
      } else {
        const distractors = getRandomDistractors(w, 3);
        questions.push({
          wordId: w.id, word: w.word, mode: 'zh2en',
          question: def,
          answer: w.word,
          options: [w.word, ...distractors.map(d => d.word)].sort(()=>Math.random()-0.5)
        });
      }
    });
    quizDayState[dayNum] = { idx: 0, score: 0, total: questions.length, questions, answered: false, results: [] };
  }

  const qs = quizDayState[dayNum];
  if (qs.idx >= qs.questions.length) qs.idx = 0;
  const q = qs.questions[qs.idx];
  const result = qs.results[qs.idx];

  let html = '<div style="max-width:550px;margin:0 auto">';
  html += '<div style="font-size:.85rem;color:var(--text-secondary);margin-bottom:12px">第 ' + (qs.idx+1) + ' / ' + qs.questions.length + ' 题</div>';

  // Question
  html += '<div class="card" style="text-align:center;padding:24px;margin-bottom:16px">';
  html += '<div style="font-size:.75rem;color:var(--text-light);margin-bottom:4px">' + (q.mode==='en2zh'?'选择正确的中文释义':'选择对应的英文单词') + '</div>';
  html += '<div style="font-size:1.6rem;font-weight:700;color:var(--primary)">' + q.question + '</div>';
  html += '</div>';

  // Options
  html += '<div class="choice-grid">';
  q.options.forEach(opt => {
    let cls = '';
    if (result) {
      if (opt === q.answer) cls = ' correct';
      else if (opt === result.selected && !result.correct) cls = ' wrong';
    } else if (qs.selected === opt) {
      cls = ' selected';
    }
    html += '<button class="choice-btn' + cls + '" onclick="answerQuizDay(' + dayNum + ',\\'' + opt.replace(/'/g, "\\'") + '\\')" ' + (result?'disabled':'') + '>' + opt + '</button>';
  });
  html += '</div>';

  if (result) {
    html += '<div style="text-align:center;margin-top:16px;font-size:1.1rem;' + (result.correct?'color:var(--success)':'color:var(--danger)') + '">';
    html += result.correct ? '✅ 正确!' : '❌ 正确答案: ' + q.answer;
    html += '</div>';
  }

  // Navigation
  html += '<div style="display:flex;justify-content:space-between;margin-top:16px">';
  html += '<button class="btn btn-sm btn-ghost" onclick="prevQuizDay(' + dayNum + ')" ' + (qs.idx===0?'disabled':'') + '>◀ 上一题</button>';
  html += '<span style="font-size:.85rem;color:var(--text-secondary)">得分: ' + qs.score + ' / ' + qs.results.length + '</span>';
  html += '<button class="btn btn-sm btn-primary" onclick="nextQuizDay(' + dayNum + ')" ' + (!result?'disabled':'') + '>下一题 ▶</button>';
  html += '</div>';

  html += '</div>';
  return html;
}

function answerQuizDay(dayNum, selected) {
  const qs = quizDayState[dayNum];
  if (!qs) return;
  const q = qs.questions[qs.idx];
  const correct = selected === q.answer;
  if (correct) qs.score++;
  qs.results[qs.idx] = { selected, correct };

  if (!correct) {
    const w = getWordById(q.wordId);
    if (w && !appState.wrongWords.includes(w.id)) {
      appState.wrongWords.push(w.id);
      saveState();
    }
  }

  // Save progress
  const dp = getDayProgress(dayNum);
  if (!dp.steps) dp.steps = {};
  dp.steps[3] = { done: qs.results.length >= qs.questions.length, score: qs.score, total: qs.results.length };
  saveState();

  refreshQuizStep(dayNum);
}

function prevQuizDay(dayNum) {
  const qs = quizDayState[dayNum];
  if (qs && qs.idx > 0) {
    qs.idx--;
    refreshQuizStep(dayNum);
  }
}

function nextQuizDay(dayNum) {
  const qs = quizDayState[dayNum];
  if (!qs) return;
  if (qs.idx < qs.questions.length - 1) {
    qs.idx++;
    refreshQuizStep(dayNum);
  }
}

function refreshQuizStep(dayNum) {
  const el = document.getElementById('stepContent');
  if (el) el.innerHTML = renderStepQuiz(dayNum);
}

// ============================================================
// STEP 4: SPELLING PRACTICE
// ============================================================
let spellDayState = {}; // {dayNum: {idx, score, total, results:[]}}

function renderStepSpell(dayNum) {
  const words = getDayWords(dayNum);
  if (words.length === 0) return '<p>暂无词汇</p>';

  if (!spellDayState[dayNum]) {
    const items = words.map(w => ({
      wordId: w.id, word: w.word,
      hint: (w.meanings&&w.meanings[0])?w.meanings[0].def:'',
      pos: (w.meanings&&w.meanings[0])?w.meanings[0].pos:''
    })).sort(() => Math.random() - 0.5);
    spellDayState[dayNum] = { idx: 0, score: 0, total: items.length, items, results: [], submitted: false };
  }

  const ss = spellDayState[dayNum];
  if (ss.idx >= ss.items.length) ss.idx = 0;
  const item = ss.items[ss.idx];
  const result = ss.results[ss.idx];

  let html = '<div style="max-width:500px;margin:0 auto">';
  html += '<div style="font-size:.85rem;color:var(--text-secondary);margin-bottom:12px">第 ' + (ss.idx+1) + ' / ' + ss.items.length + ' 题 · 根据中文释义拼写英文单词</div>';

  html += '<div class="card" style="text-align:center;padding:24px;margin-bottom:16px">';
  html += '<div style="font-size:.75rem;color:var(--text-light)">' + item.pos + '</div>';
  html += '<div style="font-size:2rem;font-weight:700;color:var(--text);margin:12px 0">' + item.hint + '</div>';
  html += '</div>';

  if (!result || !result.submitted) {
    html += '<div class="spell-input-row">';
    html += '<input class="input" id="spellDayInput" type="text" placeholder="输入英文单词..." autocomplete="off" onkeydown="if(event.key===\\'Enter\\')submitSpellDay(' + dayNum + ')">';
    html += '<button class="btn btn-primary" onclick="submitSpellDay(' + dayNum + ')">确认</button>';
    html += '</div>';
    html += '<div style="text-align:center;margin-top:8px">';
    html += '<button class="btn btn-sm btn-ghost" onclick="showSpellHint(' + dayNum + ')">💡 显示提示</button>';
    html += '<div id="spellHintArea" style="font-size:.85rem;color:var(--text-secondary);margin-top:4px"></div>';
    html += '</div>';
  } else {
    const correct = result.correct;
    html += '<div style="text-align:center;margin-top:16px">';
    html += '<div style="font-size:1.2rem;' + (correct?'color:var(--success)':'color:var(--danger)') + '">';
    html += correct ? '✅ 正确!' : '❌ 正确答案: <b>' + item.word + '</b>';
    html += '</div>';
    html += '<div style="font-size:.9rem;color:var(--text-secondary);margin-top:4px">你的输入: ' + (result.userInput||'') + '</div>';
    html += '</div>';
  }

  html += '<div style="display:flex;justify-content:space-between;margin-top:16px">';
  html += '<button class="btn btn-sm btn-ghost" onclick="prevSpellDay(' + dayNum + ')" ' + (ss.idx===0?'disabled':'') + '>◀ 上一题</button>';
  html += '<span style="font-size:.85rem;color:var(--text-secondary)">得分: ' + ss.score + ' / ' + ss.results.filter(r=>r&&r.submitted).length + '</span>';
  html += '<button class="btn btn-sm btn-primary" onclick="nextSpellDay(' + dayNum + ')" ' + (!result||!result.submitted?'disabled':'') + '>下一题 ▶</button>';
  html += '</div>';

  html += '</div>';
  return html;
}

function showSpellHint(dayNum) {
  const ss = spellDayState[dayNum];
  if (!ss) return;
  const item = ss.items[ss.idx];
  const el = document.getElementById('spellHintArea');
  if (el) {
    el.textContent = '首字母: ' + item.word[0].toUpperCase() + '... 长度: ' + item.word.length + ' 个字母';
  }
}

function submitSpellDay(dayNum) {
  const input = document.getElementById('spellDayInput');
  if (!input || !spellDayState[dayNum]) return;
  const ss = spellDayState[dayNum];
  const item = ss.items[ss.idx];
  const userInput = input.value.trim();
  const correct = userInput.toLowerCase() === item.word.toLowerCase();
  if (correct) ss.score++;
  ss.results[ss.idx] = { submitted: true, correct, userInput };

  if (!correct) {
    const w = getWordById(item.wordId);
    if (w && !appState.wrongWords.includes(w.id)) {
      appState.wrongWords.push(w.id);
      saveState();
    }
  }

  const dp = getDayProgress(dayNum);
  if (!dp.steps) dp.steps = {};
  const doneCount = ss.results.filter(r => r && r.submitted).length;
  dp.steps[4] = { done: doneCount >= ss.total, score: ss.score, total: doneCount };
  saveState();

  refreshSpellStep(dayNum);
}

function prevSpellDay(dayNum) {
  const ss = spellDayState[dayNum];
  if (ss && ss.idx > 0) { ss.idx--; refreshSpellStep(dayNum); }
}

function nextSpellDay(dayNum) {
  const ss = spellDayState[dayNum];
  if (ss && ss.idx < ss.items.length - 1) { ss.idx++; refreshSpellStep(dayNum); }
}

function refreshSpellStep(dayNum) {
  const el = document.getElementById('stepContent');
  if (el) el.innerHTML = renderStepSpell(dayNum);
}

// ============================================================
// STEP 5: MATCHING GAME
// ============================================================
let matchDayState = {}; // {dayNum: {pairs:[], selected:[], matched:[], score, total}}

function renderStepMatch(dayNum) {
  const words = getDayWords(dayNum);
  if (words.length === 0) return '<p>暂无词汇</p>';

  if (!matchDayState[dayNum]) {
    // Build pairs: English word <-> Chinese meaning
    const pairs = words.map(w => ({
      wordId: w.id,
      eng: w.word,
      chn: (w.meanings&&w.meanings[0])?w.meanings[0].def:''
    }));
    matchDayState[dayNum] = {
      pairs,
      matched: [],
      selected: null, // {idx, type:'eng'|'chn'}
      score: 0,
      total: pairs.length
    };
  }

  const ms = matchDayState[dayNum];

  // Create shuffled card array
  const cards = [];
  ms.pairs.forEach((p, i) => {
    cards.push({ pairIdx: i, type: 'eng', text: p.eng, matched: ms.matched.includes(i) });
    cards.push({ pairIdx: i, type: 'chn', text: p.chn, matched: ms.matched.includes(i) });
  });
  // Shuffle
  for (let i = cards.length - 1; i > 0; i--) {
    const j = Math.floor(Math.random() * (i + 1));
    [cards[i], cards[j]] = [cards[j], cards[i]];
  }

  let html = '<div style="max-width:650px;margin:0 auto">';
  html += '<div style="font-size:.85rem;color:var(--text-secondary);margin-bottom:12px">点击英文单词和对应的中文释义进行配对 · 已配对: ' + ms.matched.length + ' / ' + ms.total + '</div>';

  html += '<div class="match-pair-grid">';
  cards.forEach((card, i) => {
    let cls = 'match-pair-card ' + (card.type === 'eng' ? 'eng' : 'chn');
    if (card.matched) cls += ' matched';
    if (ms.selected && ms.selected.idx === i) cls += ' selected';
    html += '<div class="' + cls + '" onclick="selectMatchCard(' + dayNum + ',' + i + ',' + card.pairIdx + ',\\'' + card.type + '\\')" data-idx="' + i + '">' + card.text + '</div>';
  });
  html += '</div>';

  if (ms.matched.length >= ms.total) {
    html += '<div style="text-align:center;margin-top:20px;font-size:1.2rem;color:var(--success)">🎉 全部配对完成!</div>';
  }

  html += '</div>';
  return html;
}

function selectMatchCard(dayNum, cardIdx, pairIdx, type) {
  const ms = matchDayState[dayNum];
  if (!ms || ms.matched.includes(pairIdx)) return;

  if (!ms.selected) {
    ms.selected = { idx: cardIdx, pairIdx, type };
    refreshMatchStep(dayNum);
    return;
  }

  if (ms.selected.idx === cardIdx) {
    // Deselect
    ms.selected = null;
    refreshMatchStep(dayNum);
    return;
  }

  // Check if matching
  if (ms.selected.pairIdx === pairIdx && ms.selected.type !== type) {
    // Correct match!
    ms.matched.push(pairIdx);
    ms.score++;
    ms.selected = null;

    // Save progress
    const dp = getDayProgress(dayNum);
    if (!dp.steps) dp.steps = {};
    dp.steps[5] = { done: ms.matched.length >= ms.total, score: ms.score, total: ms.total };
    saveState();
  } else {
    // Wrong match - flash and reset
    ms.selected = null;
    refreshMatchStep(dayNum);
    // Brief wrong flash
    setTimeout(() => {
      const cards = document.querySelectorAll('.match-pair-card');
      cards.forEach(c => c.classList.remove('wrong-pair'));
    }, 400);
    return;
  }

  refreshMatchStep(dayNum);
}

function refreshMatchStep(dayNum) {
  const el = document.getElementById('stepContent');
  if (el) el.innerHTML = renderStepMatch(dayNum);
}
`;

// Read and process
const html = fs.readFileSync('index.html', 'utf-8');

// Find the boundary markers
const startMarker = '// ============================================================\n// LEARN PAGE';
const endMarker = '// ============================================================\n// CLOZE PAGE (短文完型)';

const startIdx = html.indexOf(startMarker);
const endIdx = html.indexOf(endMarker);

if (startIdx === -1 || endIdx === -1) {
  console.error('Could not find boundary markers');
  console.error('startIdx:', startIdx, 'endIdx:', endIdx);
  process.exit(1);
}

// Replace the old code with new code
const before = html.substring(0, startIdx);
const after = html.substring(endIdx);
const result = before + NEW_CODE + '\n' + after;

fs.writeFileSync('index.html', result, 'utf-8');
console.log('Replaced from position', startIdx, 'to', endIdx);
console.log('New file size:', result.length.toLocaleString(), 'bytes');
console.log('Done!');

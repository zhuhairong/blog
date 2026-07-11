
// ============================================================
// DAY-BASED LEARNING STATE
// ============================================================
let currentView = 'map';
let currentDay = 1;
let currentStep = 1;
const STEP_INFO = [
  { icon: '📖', label: '词汇记忆', key: 'learn' },
  { icon: '✍️', label: '语境填空', key: 'fillblank' },
  { icon: '🎯', label: '选择练习', key: 'quiz' },
  { icon: '📝', label: '拼写练习', key: 'spell' },
  { icon: '🔗', label: '配对游戏', key: 'match' }
];

function getDayProgress(dayNum) {
  if (!appState.dayProgress[dayNum]) {
    appState.dayProgress[dayNum] = { unlocked: dayNum === 1, completed: false, stars: 0, currentStep: 1, steps: {} };
  }
  return appState.dayProgress[dayNum];
}

function unlockNextDay(currentDayNum) {
  var next = currentDayNum + 1;
  if (next <= 19) { getDayProgress(next).unlocked = true; saveState(); }
}

function getDayExerciseData(dayNum) {
  return (typeof DAY_EXERCISES !== 'undefined' && DAY_EXERCISES[dayNum]) ? DAY_EXERCISES[dayNum] : [];
}

// ============================================================
// DAY MAP (Home Page)
// ============================================================
function renderDayMap() {
  currentView = 'map';
  var container = document.getElementById('page-daily-learn');
  var wordsByDay = getWordsByDay();
  var days = Object.keys(wordsByDay).map(Number).sort(function(a,b){return a-b;});
  var totalLearned = getLearnedCount();
  var totalMastered = getMasteredCount();
  var completedDays = 0;
  for (var dk in appState.dayProgress) { if (appState.dayProgress[dk].completed) completedDays++; }

  var h = '';
  h += '<div class="day-map-header"><h2>📅 每日学习</h2><p class="subtitle">每天学习一组词汇，循序渐进，逐天解锁</p></div>';
  h += '<div class="day-map-stats">';
  h += '<div class="stat-item"><div class="stat-num">' + totalLearned + '</div><div class="stat-label">已学单词 / 198</div></div>';
  h += '<div class="stat-item"><div class="stat-num">' + totalMastered + '</div><div class="stat-label">已掌握</div></div>';
  h += '<div class="stat-item"><div class="stat-num">' + completedDays + '</div><div class="stat-label">已完成 / ' + days.length + ' 天</div></div>';
  h += '<div class="stat-item"><div class="stat-num">🔥 ' + appState.streak.current + '</div><div class="stat-label">连续天数</div></div>';
  h += '</div>';

  h += '<div class="day-grid">';
  for (var di = 0; di < days.length; di++) {
    var d = days[di];
    var dp = getDayProgress(d);
    var dayWords = wordsByDay[d];
    var unlocked = dp.unlocked;
    var completed = dp.completed;
    var stars = dp.stars || 0;
    var inProgress = unlocked && !completed && (dp.currentStep > 1 || Object.keys(dp.steps).length > 0);
    var cardClass = '';
    var badgeHtml = '';
    if (!unlocked) {
      cardClass = 'locked';
      badgeHtml = '<span class="day-badge" style="background:#FEE2E2;color:#DC2626">🔒</span>';
    } else if (completed) {
      cardClass = 'completed';
    } else if (inProgress) {
      cardClass = 'current';
      badgeHtml = '<span class="day-badge doing">进行中</span>';
    } else {
      badgeHtml = '<span class="day-badge new">新!</span>';
    }
    var starsStr = completed ? ('⭐'.repeat(Math.max(1, stars)) + '☆'.repeat(Math.max(0, 3 - Math.max(1, stars)))) : '☆☆☆';
    var animDelay = (0.3 + di * 0.03).toFixed(2);
    h += '<div class="day-card ' + cardClass + '" style="animation:slideUp ' + animDelay + 's ease both" onclick="' + (unlocked ? "openDay(" + d + ")" : "showToast('请先完成前一天的练习','info')") + '">';
    h += badgeHtml;
    h += '<div class="day-num">Day ' + d + '</div>';
    h += '<div class="day-label">' + dayWords.length + ' 个单词</div>';
    h += '<div class="day-stars" style="color:' + (completed ? '#F59E0B' : '#D1D5DB') + '">' + starsStr + '</div>';
    h += '</div>';
  }
  h += '</div>';

  var activeDay = null;
  for (var adi = 0; adi < days.length; adi++) {
    var add = days[adi];
    var adp = getDayProgress(add);
    if (adp.unlocked && !adp.completed) { activeDay = add; break; }
  }
  if (activeDay) {
    var adp2 = getDayProgress(activeDay);
    var stepLabel = (adp2.currentStep > 1 && adp2.currentStep <= 5) ? STEP_INFO[adp2.currentStep - 1].label : '开始学习';
    h += '<div style="text-align:center;margin-top:20px"><button class="btn btn-primary btn-lg" onclick="openDay(' + activeDay + ')" style="animation:pulse 2s infinite">' + (adp2.currentStep > 1 ? '▶ 继续 Day ' + activeDay + ' — ' + stepLabel : '🚀 开始 Day ' + activeDay) + '</button></div>';
  }

  container.innerHTML = h;
}

function openDay(dayNum) {
  currentDay = dayNum;
  currentView = 'detail';
  var dp = getDayProgress(dayNum);
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
  var container = document.getElementById('page-daily-learn');
  var dp = getDayProgress(dayNum);
  var dayWords = getDayWords(dayNum);

  var h = '';
  h += '<div class="day-detail-header">';
  h += '<button class="back-btn" onclick="goBackToMap()" title="返回">←</button>';
  h += '<div><div class="day-title">Day ' + dayNum + '</div>';
  h += '<div class="day-subtitle">' + dayWords.length + ' 个单词 · 5步学习流程</div></div>';
  h += '</div>';

  // Step progress bar
  h += '<div class="step-progress-bar">';
  for (var i = 0; i < 5; i++) {
    var s = STEP_INFO[i];
    var stepNum = i + 1;
    var cls = '';
    if (dp.steps && dp.steps[stepNum] && dp.steps[stepNum].done) cls = 'done';
    else if (stepNum === currentStep) cls = 'active';
    if (i > 0) {
      var lineDone = (dp.steps && dp.steps[i] && dp.steps[i].done) ? ' done' : '';
      h += '<div class="step-line' + lineDone + '"></div>';
    }
    h += '<div class="step-dot ' + cls + '" title="' + s.label + '">' + (cls === 'done' ? '✓' : s.icon) + '<span class="step-label">' + s.label + '</span></div>';
  }
  h += '</div>';

  // Step content
  h += '<div class="step-content" id="stepContent">';
  h += renderStepContent(dayNum, currentStep);
  h += '</div>';

  // Navigation
  h += '<div class="step-actions">';
  if (currentStep > 1) {
    h += '<button class="btn btn-outline" onclick="prevStep(' + dayNum + ')">← 上一步</button>';
  } else {
    h += '<span></span>';
  }
  h += '<button class="btn btn-primary" id="btnNextStep" onclick="nextStep(' + dayNum + ')">' + (currentStep < 5 ? '下一步 →' : '完成学习 🎉') + '</button>';
  h += '</div>';

  container.innerHTML = h;
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
  if (currentStep > 1) { currentStep--; var dp = getDayProgress(dayNum); dp.currentStep = currentStep; saveState(); renderDayDetail(dayNum); }
}

function nextStep(dayNum) {
  var dp = getDayProgress(dayNum);
  if (!dp.steps) dp.steps = {};
  if (!dp.steps[currentStep]) dp.steps[currentStep] = {};
  dp.steps[currentStep].done = true;
  if (currentStep < 5) {
    currentStep++;
    dp.currentStep = currentStep;
    saveState();
    renderDayDetail(dayNum);
    document.getElementById('page-daily-learn').scrollIntoView({behavior:'smooth'});
  } else {
    completeDay(dayNum);
  }
}

function completeDay(dayNum) {
  var dp = getDayProgress(dayNum);
  var totalScore = 0, totalMax = 0;
  for (var s = 2; s <= 5; s++) {
    if (dp.steps[s]) { totalScore += dp.steps[s].score || 0; totalMax += dp.steps[s].total || 0; }
  }
  var pct = totalMax > 0 ? totalScore / totalMax : 0;
  var stars = pct >= 0.9 ? 3 : (pct >= 0.7 ? 2 : 1);
  dp.completed = true;
  dp.stars = stars;
  dp.currentStep = 5;
  saveState();
  getDayWords(dayNum).forEach(function(w) {
    if (!appState.wordMastery[w.id] || appState.wordMastery[w.id] < 2) appState.wordMastery[w.id] = 2;
  });
  unlockNextDay(dayNum);
  updateStreak();
  showDayComplete(dayNum, stars, pct);
}

function showDayComplete(dayNum, stars, pct) {
  var overlay = document.createElement('div');
  overlay.className = 'day-complete-overlay';
  overlay.id = 'dayCompleteOverlay';
  var msgs = stars === 3 ? ['太棒了！', '完美通关！', '满分学霸！'] : (stars === 2 ? ['做得不错！', '继续加油！'] : ['完成啦！', '再接再厉！']);
  var msg = msgs[Math.floor(Math.random() * msgs.length)];
  overlay.innerHTML = '<div class="day-complete-card">' +
    '<div class="big-emoji">🎉</div>' +
    '<h2 style="margin:8px 0">' + msg + '</h2>' +
    '<div class="stars-display">' + '⭐'.repeat(stars) + '☆'.repeat(3 - stars) + '</div>' +
    '<div class="score-text">正确率: ' + Math.round(pct * 100) + '%</div>' +
    '<p style="color:var(--text-secondary);font-size:.85rem;margin:8px 0">Day ' + dayNum + ' 完成！</p>' +
    (dayNum < 19 ? '<p style="color:var(--success);font-size:.85rem">🔓 Day ' + (dayNum + 1) + ' 已解锁！</p>' : '<p style="color:var(--primary);font-size:.85rem">🏆 全部19天已完成！</p>') +
    '<div style="display:flex;gap:8px;justify-content:center;margin-top:16px;flex-wrap:wrap">' +
    '<button class="btn btn-outline" onclick="closeDayComplete();openDay(' + dayNum + ')">📖 复习本天</button>' +
    '<button class="btn btn-primary" onclick="closeDayComplete();goBackToMap()">📅 返回主页</button>' +
    (dayNum < 19 ? '<button class="btn btn-success" onclick="closeDayComplete();openDay(' + (dayNum + 1) + ')">▶ 挑战 Day ' + (dayNum + 1) + '</button>' : '') +
    '</div></div>';
  document.body.appendChild(overlay);
  setTimeout(function() {
    var card = overlay.querySelector('.day-complete-card');
    if (card) { var rect = card.getBoundingClientRect(); spawnParticles(rect.left + rect.width / 2, rect.top + 20, 15); }
  }, 300);
}

function closeDayComplete() {
  var overlay = document.getElementById('dayCompleteOverlay');
  if (overlay) overlay.remove();
  updateHeaderStats();
}

// ============================================================
// STEP 1: VOCABULARY LEARNING (Flashcard + Word List)
// ============================================================
var vocabCardIdx = 0;
var vocabShowFront = true;

function renderStepVocab(dayNum) {
  var words = getDayWords(dayNum);
  if (words.length === 0) return '<p>暂无词汇数据</p>';
  if (vocabCardIdx >= words.length) vocabCardIdx = 0;
  var w = words[vocabCardIdx];
  var allDefs = (w.meanings || []).map(function(m) { return '<span class="tag tag-primary" style="margin:2px">' + m.pos + ' ' + m.def + '</span>'; }).join(' ');
  var derivStr = '';
  if (w.derivatives && w.derivatives.length > 0) {
    derivStr = '<div style="font-size:.8rem;color:var(--text-secondary);margin-top:8px">派生: ' + w.derivatives.map(function(d) { return d.word + ' (' + d.pos + ')'; }).join(', ') + '</div>';
  }
  var phraseStr = '';
  if (w.phrases && w.phrases.length > 0) {
    phraseStr = '<div style="font-size:.75rem;color:var(--text-light);margin-top:4px">' + w.phrases.slice(0, 3).join(' | ') + '</div>';
  }

  var h = '<div style="max-width:500px;margin:0 auto">';
  h += '<div class="flashcard-container" onclick="flipVocabCard()" id="vocabFlashcard" style="height:260px">';
  h += '<div class="flashcard" id="vocabFlashcardInner">';
  h += '<div class="flashcard-face flashcard-front"><div class="fc-word">' + w.word + '</div><div class="fc-hint">点击翻转查看释义</div></div>';
  h += '<div class="flashcard-face flashcard-back"><div style="font-size:1.6rem;font-weight:700;margin-bottom:8px;color:var(--primary)">' + w.word + '</div><div style="font-size:.95rem;color:var(--text)">' + allDefs + '</div>' + derivStr + phraseStr + '</div>';
  h += '</div></div>';

  h += '<div style="display:flex;justify-content:space-between;align-items:center;margin-top:12px">';
  h += '<button class="btn btn-sm btn-ghost" onclick="prevVocabCard(' + dayNum + ')"' + (vocabCardIdx === 0 ? ' disabled' : '') + '>◀</button>';
  h += '<span style="font-size:.85rem;color:var(--text-secondary)">' + (vocabCardIdx + 1) + ' / ' + words.length + '</span>';
  h += '<button class="btn btn-sm btn-ghost" onclick="nextVocabCard(' + dayNum + ')"' + (vocabCardIdx >= words.length - 1 ? ' disabled' : '') + '>▶</button>';
  h += '</div>';

  h += '<div style="margin-top:16px;max-height:200px;overflow-y:auto">';
  h += '<div style="font-size:.8rem;font-weight:700;color:var(--text-secondary);margin-bottom:8px">本天词汇列表 (点击切换)</div>';
  for (var i = 0; i < words.length; i++) {
    var ww = words[i];
    var def = (ww.meanings && ww.meanings.length > 0) ? ww.meanings[0].def : '';
    var active = i === vocabCardIdx;
    h += '<div style="padding:6px 10px;cursor:pointer;border-radius:6px;' + (active ? 'background:var(--primary-bg);font-weight:700' : '') + '" onclick="jumpVocabCard(' + dayNum + ',' + i + ')">';
    h += '<span style="font-family:monospace;font-weight:600">' + ww.word + '</span> <span style="color:var(--text-secondary);font-size:.8rem">' + def + '</span></div>';
  }
  h += '</div></div>';
  return h;
}

function flipVocabCard() {
  var inner = document.getElementById('vocabFlashcardInner');
  if (inner) { vocabShowFront = !vocabShowFront; inner.classList.toggle('flipped', !vocabShowFront); }
}

function prevVocabCard(dayNum) {
  if (vocabCardIdx > 0) { vocabCardIdx--; vocabShowFront = true; refreshVocabStep(dayNum); }
}

function nextVocabCard(dayNum) {
  var words = getDayWords(dayNum);
  if (vocabCardIdx < words.length - 1) { vocabCardIdx++; vocabShowFront = true; refreshVocabStep(dayNum); }
}

function jumpVocabCard(dayNum, idx) {
  vocabCardIdx = idx; vocabShowFront = true; refreshVocabStep(dayNum);
}

function refreshVocabStep(dayNum) {
  var el = document.getElementById('stepContent');
  if (el) el.innerHTML = renderStepVocab(dayNum);
}

// ============================================================
// STEP 2: SENTENCE FILL-BLANK (语境填空)
// ============================================================
var fillblankState = {};

function renderStepFillBlank(dayNum) {
  var exData = getDayExerciseData(dayNum);
  var words = getDayWords(dayNum);
  if (!fillblankState[dayNum]) fillblankState[dayNum] = { idx: 0, answers: {}, submitted: false, score: 0, results: {} };
  var fb = fillblankState[dayNum];

  var exercises = [];
  if (exData.length > 0) {
    exData.forEach(function(item) {
      item.exercises.filter(function(e) { return e.type === 'context'; }).forEach(function(e) {
        exercises.push({ wordId: item.wordId, word: item.word, sentence: e.sentence, answer: e.answer, options: e.options, hint: e.hint, pos: e.pos });
      });
    });
  }
  if (exercises.length === 0) {
    words.forEach(function(w) {
      var def = (w.meanings && w.meanings.length > 0) ? w.meanings[0].def : '';
      var pos = (w.meanings && w.meanings.length > 0) ? w.meanings[0].pos : '';
      var dists = getRandomDistractors(w, 3).map(function(d) { return d.word; });
      exercises.push({ wordId: w.id, word: w.word, sentence: 'The word "___" means ' + def + '.', answer: w.word, options: [w.word].concat(dists), hint: def, pos: pos });
    });
  }

  if (fb.idx >= exercises.length) fb.idx = 0;
  var ex = exercises[fb.idx];
  var userAnswer = fb.answers[fb.idx] || '';
  var result = fb.results[fb.idx];

  var h = '<div style="max-width:600px;margin:0 auto">';
  h += '<div style="font-size:.85rem;color:var(--text-secondary);margin-bottom:12px">第 ' + (fb.idx + 1) + ' / ' + exercises.length + ' 题 · 根据语境填入正确的单词</div>';
  h += '<div class="sentence-exercise">';
  if (ex.pos) h += '<span class="sent-pos">' + ex.pos + '</span>';

  if (fb.submitted) {
    var cls = (result && result.correct) ? 'correct' : 'wrong';
    h += '<div class="sent-text">' + ex.sentence.replace('___', '<span class="blank filled ' + cls + '">' + (userAnswer || '___') + '</span>');
    if (!result || !result.correct) h += ' <span style="color:var(--success);font-weight:700">(' + ex.answer + ')</span>';
    h += '</div>';
  } else {
    h += '<div class="sent-text">' + ex.sentence.replace('___', '<span class="blank' + (userAnswer ? ' filled' : '') + '">' + (userAnswer || '___') + '</span>') + '</div>';
  }
  if (ex.hint) h += '<div class="sent-hint">💡 提示: ' + ex.hint + '</div>';
  h += '</div>';

  if (!fb.submitted) {
    if (ex.options && ex.options.length >= 2) {
      h += '<div class="choice-grid">';
      ex.options.forEach(function(opt) {
        var sel = userAnswer === opt ? ' selected' : '';
        h += '<button class="choice-btn' + sel + '" onclick="selectFillblank(' + dayNum + ',' + fb.idx + ',\'' + opt.replace(/'/g, "\\'") + '\')">' + opt + '</button>';
      });
      h += '</div>';
    } else {
      h += '<div class="spell-input-row" style="margin-top:12px"><input class="input" id="fillblankInput" type="text" placeholder="输入单词..." value="' + (userAnswer || '') + '" onkeydown="if(event.key===\'Enter\')submitFillblank(' + dayNum + ')"><button class="btn btn-primary" onclick="submitFillblank(' + dayNum + ')">确认</button></div>';
    }
  } else {
    var isCorrect = result && result.correct;
    h += '<div style="text-align:center;margin-top:16px;font-size:1.1rem;' + (isCorrect ? 'color:var(--success)' : 'color:var(--danger)') + '">' + (isCorrect ? '✅ 正确!' : '❌ 正确答案: ' + ex.answer) + '</div>';
  }

  h += '<div style="display:flex;justify-content:space-between;margin-top:16px">';
  h += '<button class="btn btn-sm btn-ghost" onclick="prevFillblank(' + dayNum + ')"' + (fb.idx === 0 ? ' disabled' : '') + '>◀ 上一题</button>';
  if (fb.submitted) {
    h += '<button class="btn btn-sm btn-primary" onclick="nextFillblank(' + dayNum + ',' + exercises.length + ')">下一题 ▶</button>';
  } else if (ex.options && ex.options.length >= 2) {
    h += '<button class="btn btn-sm btn-primary" onclick="submitFillblankChoice(' + dayNum + ',' + fb.idx + ')"' + (!userAnswer ? ' disabled' : '') + '>提交</button>';
  }
  h += '</div></div>';
  return h;
}

function selectFillblank(dayNum, idx, word) {
  if (!fillblankState[dayNum]) return;
  fillblankState[dayNum].answers[idx] = word;
  refreshFillblankStep(dayNum);
}

function submitFillblank(dayNum) {
  var input = document.getElementById('fillblankInput');
  if (!input || !fillblankState[dayNum]) return;
  var fb = fillblankState[dayNum];
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
  var fb = fillblankState[dayNum];
  var exData = getDayExerciseData(dayNum);
  var words = getDayWords(dayNum);
  var exercises = [];
  if (exData.length > 0) {
    exData.forEach(function(item) {
      item.exercises.filter(function(e) { return e.type === 'context'; }).forEach(function(e) {
        exercises.push({ wordId: item.wordId, word: item.word, sentence: e.sentence, answer: e.answer, options: e.options, hint: e.hint, pos: e.pos });
      });
    });
  }
  if (exercises.length === 0) {
    words.forEach(function(w) {
      var def = (w.meanings && w.meanings.length > 0) ? w.meanings[0].def : '';
      var pos = (w.meanings && w.meanings.length > 0) ? w.meanings[0].pos : '';
      var dists = getRandomDistractors(w, 3).map(function(d) { return d.word; });
      exercises.push({ wordId: w.id, word: w.word, sentence: 'The word "___" means ' + def + '.', answer: w.word, options: [w.word].concat(dists), hint: def, pos: pos });
    });
  }
  var ex = exercises[fb.idx];
  var userAnswer = (fb.answers[fb.idx] || '').trim().toLowerCase();
  var correct = userAnswer === ex.answer.toLowerCase();
  fb.results[fb.idx] = { correct: correct, userAnswer: userAnswer };
  if (correct) fb.score++;
  var dp = getDayProgress(dayNum);
  if (!dp.steps) dp.steps = {};
  dp.steps[2] = { done: fb.idx >= exercises.length - 1, score: fb.score, total: fb.idx + 1 };
  saveState();
  if (!correct) {
    var w = getWordById(ex.wordId);
    if (w && appState.wrongWords.indexOf(w.id) === -1) { appState.wrongWords.push(w.id); saveState(); }
  }
}

function prevFillblank(dayNum) {
  if (!fillblankState[dayNum]) return;
  var fb = fillblankState[dayNum];
  if (fb.idx > 0) { fb.idx--; fb.submitted = !!fb.results[fb.idx]; refreshFillblankStep(dayNum); }
}

function nextFillblank(dayNum, total) {
  if (!fillblankState[dayNum]) return;
  var fb = fillblankState[dayNum];
  if (fb.idx < total - 1) { fb.idx++; fb.submitted = !!fb.results[fb.idx]; refreshFillblankStep(dayNum); }
}

function refreshFillblankStep(dayNum) {
  var el = document.getElementById('stepContent');
  if (el) el.innerHTML = renderStepFillBlank(dayNum);
}

// ============================================================
// STEP 3: MULTIPLE CHOICE QUIZ
// ============================================================
var quizDayState = {};

function renderStepQuiz(dayNum) {
  var words = getDayWords(dayNum);
  if (words.length === 0) return '<p>暂无词汇</p>';
  if (!quizDayState[dayNum]) {
    var questions = [];
    words.forEach(function(w) {
      var def = (w.meanings && w.meanings.length > 0) ? w.meanings[0].def : '';
      var mode = Math.random() > 0.5 ? 'en2zh' : 'zh2en';
      if (mode === 'en2zh') {
        var distractors = getRandomDistractors(w, 3);
        questions.push({ wordId: w.id, word: w.word, mode: 'en2zh', question: w.word, answer: def, options: [def].concat(distractors.map(function(d) { return (d.meanings && d.meanings[0]) ? d.meanings[0].def : d.word; })).sort(function() { return Math.random() - 0.5; }) });
      } else {
        var dists2 = getRandomDistractors(w, 3);
        questions.push({ wordId: w.id, word: w.word, mode: 'zh2en', question: def, answer: w.word, options: [w.word].concat(dists2.map(function(d) { return d.word; })).sort(function() { return Math.random() - 0.5; }) });
      }
    });
    quizDayState[dayNum] = { idx: 0, score: 0, total: questions.length, questions: questions, results: [] };
  }
  var qs = quizDayState[dayNum];
  if (qs.idx >= qs.questions.length) qs.idx = 0;
  var q = qs.questions[qs.idx];
  var result = qs.results[qs.idx];

  var h = '<div style="max-width:550px;margin:0 auto">';
  h += '<div style="font-size:.85rem;color:var(--text-secondary);margin-bottom:12px">第 ' + (qs.idx + 1) + ' / ' + qs.questions.length + ' 题</div>';
  h += '<div class="card" style="text-align:center;padding:24px;margin-bottom:16px">';
  h += '<div style="font-size:.75rem;color:var(--text-light);margin-bottom:4px">' + (q.mode === 'en2zh' ? '选择正确的中文释义' : '选择对应的英文单词') + '</div>';
  h += '<div style="font-size:1.6rem;font-weight:700;color:var(--primary)">' + q.question + '</div></div>';
  h += '<div class="choice-grid">';
  q.options.forEach(function(opt) {
    var cls = '';
    if (result) {
      if (opt === q.answer) cls = ' correct';
      else if (opt === result.selected && !result.correct) cls = ' wrong';
    } else if (qs.selected === opt) cls = ' selected';
    h += '<button class="choice-btn' + cls + '" onclick="answerQuizDay(' + dayNum + ',\'' + opt.replace(/'/g, "\\'") + '\')"' + (result ? ' disabled' : '') + '>' + opt + '</button>';
  });
  h += '</div>';
  if (result) {
    h += '<div style="text-align:center;margin-top:16px;font-size:1.1rem;' + (result.correct ? 'color:var(--success)' : 'color:var(--danger)') + '">' + (result.correct ? '✅ 正确!' : '❌ 正确答案: ' + q.answer) + '</div>';
  }
  h += '<div style="display:flex;justify-content:space-between;margin-top:16px">';
  h += '<button class="btn btn-sm btn-ghost" onclick="prevQuizDay(' + dayNum + ')"' + (qs.idx === 0 ? ' disabled' : '') + '>◀ 上一题</button>';
  h += '<span style="font-size:.85rem;color:var(--text-secondary)">得分: ' + qs.score + ' / ' + qs.results.length + '</span>';
  h += '<button class="btn btn-sm btn-primary" onclick="nextQuizDay(' + dayNum + ')"' + (!result ? ' disabled' : '') + '>下一题 ▶</button>';
  h += '</div></div>';
  return h;
}

function answerQuizDay(dayNum, selected) {
  var qs = quizDayState[dayNum];
  if (!qs) return;
  var q = qs.questions[qs.idx];
  var correct = selected === q.answer;
  if (correct) qs.score++;
  qs.results[qs.idx] = { selected: selected, correct: correct };
  if (!correct) {
    var w = getWordById(q.wordId);
    if (w && appState.wrongWords.indexOf(w.id) === -1) { appState.wrongWords.push(w.id); saveState(); }
  }
  var dp = getDayProgress(dayNum);
  if (!dp.steps) dp.steps = {};
  dp.steps[3] = { done: qs.results.length >= qs.questions.length, score: qs.score, total: qs.results.length };
  saveState();
  refreshQuizStep(dayNum);
}

function prevQuizDay(dayNum) {
  var qs = quizDayState[dayNum];
  if (qs && qs.idx > 0) { qs.idx--; refreshQuizStep(dayNum); }
}

function nextQuizDay(dayNum) {
  var qs = quizDayState[dayNum];
  if (!qs) return;
  if (qs.idx < qs.questions.length - 1) { qs.idx++; refreshQuizStep(dayNum); }
}

function refreshQuizStep(dayNum) {
  var el = document.getElementById('stepContent');
  if (el) el.innerHTML = renderStepQuiz(dayNum);
}

// ============================================================
// STEP 4: SPELLING PRACTICE
// ============================================================
var spellDayState = {};

function renderStepSpell(dayNum) {
  var words = getDayWords(dayNum);
  if (words.length === 0) return '<p>暂无词汇</p>';
  if (!spellDayState[dayNum]) {
    var items = words.map(function(w) {
      return { wordId: w.id, word: w.word, hint: (w.meanings && w.meanings[0]) ? w.meanings[0].def : '', pos: (w.meanings && w.meanings[0]) ? w.meanings[0].pos : '' };
    }).sort(function() { return Math.random() - 0.5; });
    spellDayState[dayNum] = { idx: 0, score: 0, total: items.length, items: items, results: [], submitted: false };
  }
  var ss = spellDayState[dayNum];
  if (ss.idx >= ss.items.length) ss.idx = 0;
  var item = ss.items[ss.idx];
  var result = ss.results[ss.idx];

  var h = '<div style="max-width:500px;margin:0 auto">';
  h += '<div style="font-size:.85rem;color:var(--text-secondary);margin-bottom:12px">第 ' + (ss.idx + 1) + ' / ' + ss.items.length + ' 题 · 根据中文释义拼写英文单词</div>';
  h += '<div class="card" style="text-align:center;padding:24px;margin-bottom:16px"><div style="font-size:.75rem;color:var(--text-light)">' + item.pos + '</div><div style="font-size:2rem;font-weight:700;color:var(--text);margin:12px 0">' + item.hint + '</div></div>';

  if (!result || !result.submitted) {
    h += '<div class="spell-input-row"><input class="input" id="spellDayInput" type="text" placeholder="输入英文单词..." autocomplete="off" onkeydown="if(event.key===\'Enter\')submitSpellDay(' + dayNum + ')"><button class="btn btn-primary" onclick="submitSpellDay(' + dayNum + ')">确认</button></div>';
    h += '<div style="text-align:center;margin-top:8px"><button class="btn btn-sm btn-ghost" onclick="showSpellHint(' + dayNum + ')">💡 显示提示</button><div id="spellHintArea" style="font-size:.85rem;color:var(--text-secondary);margin-top:4px"></div></div>';
  } else {
    var correct = result.correct;
    h += '<div style="text-align:center;margin-top:16px"><div style="font-size:1.2rem;' + (correct ? 'color:var(--success)' : 'color:var(--danger)') + '">' + (correct ? '✅ 正确!' : '❌ 正确答案: <b>' + item.word + '</b>') + '</div><div style="font-size:.9rem;color:var(--text-secondary);margin-top:4px">你的输入: ' + (result.userInput || '') + '</div></div>';
  }

  h += '<div style="display:flex;justify-content:space-between;margin-top:16px">';
  h += '<button class="btn btn-sm btn-ghost" onclick="prevSpellDay(' + dayNum + ')"' + (ss.idx === 0 ? ' disabled' : '') + '>◀ 上一题</button>';
  h += '<span style="font-size:.85rem;color:var(--text-secondary)">得分: ' + ss.score + ' / ' + ss.results.filter(function(r) { return r && r.submitted; }).length + '</span>';
  h += '<button class="btn btn-sm btn-primary" onclick="nextSpellDay(' + dayNum + ')"' + (!result || !result.submitted ? ' disabled' : '') + '>下一题 ▶</button>';
  h += '</div></div>';
  return h;
}

function showSpellHint(dayNum) {
  var ss = spellDayState[dayNum];
  if (!ss) return;
  var item = ss.items[ss.idx];
  var el = document.getElementById('spellHintArea');
  if (el) el.textContent = '首字母: ' + item.word[0].toUpperCase() + '... 长度: ' + item.word.length + ' 个字母';
}

function submitSpellDay(dayNum) {
  var input = document.getElementById('spellDayInput');
  if (!input || !spellDayState[dayNum]) return;
  var ss = spellDayState[dayNum];
  var item = ss.items[ss.idx];
  var userInput = input.value.trim();
  var correct = userInput.toLowerCase() === item.word.toLowerCase();
  if (correct) ss.score++;
  ss.results[ss.idx] = { submitted: true, correct: correct, userInput: userInput };
  if (!correct) {
    var w = getWordById(item.wordId);
    if (w && appState.wrongWords.indexOf(w.id) === -1) { appState.wrongWords.push(w.id); saveState(); }
  }
  var dp = getDayProgress(dayNum);
  if (!dp.steps) dp.steps = {};
  var doneCount = ss.results.filter(function(r) { return r && r.submitted; }).length;
  dp.steps[4] = { done: doneCount >= ss.total, score: ss.score, total: doneCount };
  saveState();
  refreshSpellStep(dayNum);
}

function prevSpellDay(dayNum) {
  var ss = spellDayState[dayNum];
  if (ss && ss.idx > 0) { ss.idx--; refreshSpellStep(dayNum); }
}

function nextSpellDay(dayNum) {
  var ss = spellDayState[dayNum];
  if (ss && ss.idx < ss.items.length - 1) { ss.idx++; refreshSpellStep(dayNum); }
}

function refreshSpellStep(dayNum) {
  var el = document.getElementById('stepContent');
  if (el) el.innerHTML = renderStepSpell(dayNum);
}

// ============================================================
// STEP 5: MATCHING GAME
// ============================================================
var matchDayState = {};

function renderStepMatch(dayNum) {
  var words = getDayWords(dayNum);
  if (words.length === 0) return '<p>暂无词汇</p>';
  if (!matchDayState[dayNum]) {
    var pairs = words.map(function(w) {
      return { wordId: w.id, eng: w.word, chn: (w.meanings && w.meanings[0]) ? w.meanings[0].def : '' };
    });
    matchDayState[dayNum] = { pairs: pairs, matched: [], selected: null, score: 0, total: pairs.length, cardMap: [] };
  }
  var ms = matchDayState[dayNum];

  var cards = [];
  ms.pairs.forEach(function(p, i) {
    cards.push({ pairIdx: i, type: 'eng', text: p.eng, matched: ms.matched.indexOf(i) !== -1 });
    cards.push({ pairIdx: i, type: 'chn', text: p.chn, matched: ms.matched.indexOf(i) !== -1 });
  });
  for (var i = cards.length - 1; i > 0; i--) {
    var j = Math.floor(Math.random() * (i + 1));
    var tmp = cards[i]; cards[i] = cards[j]; cards[j] = tmp;
  }
  // Store shuffled cards for selection logic
  ms.cardMap = cards;

  var h = '<div style="max-width:650px;margin:0 auto">';
  h += '<div style="font-size:.85rem;color:var(--text-secondary);margin-bottom:12px">点击英文单词和对应的中文释义进行配对 · 已配对: ' + ms.matched.length + ' / ' + ms.total + '</div>';
  h += '<div class="match-pair-grid">';
  cards.forEach(function(card, i) {
    var cls = 'match-pair-card ' + (card.type === 'eng' ? 'eng' : 'chn');
    if (card.matched) cls += ' matched';
    if (ms.selected && ms.selected.idx === i) cls += ' selected';
    h += '<div class="' + cls + '" id="matchCard' + i + '" onclick="selectMatchCard(' + dayNum + ',' + i + ')">' + card.text + '</div>';
  });
  h += '</div>';
  if (ms.matched.length >= ms.total) {
    h += '<div style="text-align:center;margin-top:20px;font-size:1.2rem;color:var(--success)">🎉 全部配对完成!</div>';
  }
  h += '</div>';
  return h;
}

function selectMatchCard(dayNum, cardIdx) {
  var ms = matchDayState[dayNum];
  if (!ms) return;
  var card = ms.cardMap[cardIdx];
  if (!card || card.matched) return;

  if (!ms.selected) {
    ms.selected = { idx: cardIdx, pairIdx: card.pairIdx, type: card.type };
    refreshMatchStep(dayNum);
    return;
  }

  if (ms.selected.idx === cardIdx) {
    ms.selected = null;
    refreshMatchStep(dayNum);
    return;
  }

  if (ms.selected.pairIdx === card.pairIdx && ms.selected.type !== card.type) {
    ms.matched.push(card.pairIdx);
    ms.score++;
    ms.selected = null;
    var dp = getDayProgress(dayNum);
    if (!dp.steps) dp.steps = {};
    dp.steps[5] = { done: ms.matched.length >= ms.total, score: ms.score, total: ms.total };
    saveState();
    refreshMatchStep(dayNum);
  } else {
    // Wrong match — flash red briefly
    var el1 = document.getElementById('matchCard' + ms.selected.idx);
    var el2 = document.getElementById('matchCard' + cardIdx);
    if (el1) el1.classList.add('wrong-pair');
    if (el2) el2.classList.add('wrong-pair');
    var prevSelected = ms.selected;
    ms.selected = null;
    setTimeout(function() {
      if (el1) el1.classList.remove('wrong-pair');
      if (el2) el2.classList.remove('wrong-pair');
      refreshMatchStep(dayNum);
    }, 400);
  }
}

function refreshMatchStep(dayNum) {
  var el = document.getElementById('stepContent');
  if (el) el.innerHTML = renderStepMatch(dayNum);
}

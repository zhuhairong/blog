// New day-based rendering code for index.html
// This replaces the old tab-based render functions

// ============================================================
// DAY-BASED LEARNING STATE
// ============================================================
let currentView = 'map';    // 'map' | 'detail'
let currentDay = 1;
let currentStep = 1;        // 1=vocab, 2=fillblank, 3=quiz, 4=spell, 5=match

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
  var next = currentDayNum + 1;
  if (next <= 19) {
    var dp = getDayProgress(next);
    dp.unlocked = true;
    saveState();
  }
}

function getDayExerciseData(dayNum) {
  return (typeof DAY_EXERCISES !== 'undefined' && DAY_EXERCISES[dayNum]) ? DAY_EXERCISES[dayNum] : [];
}

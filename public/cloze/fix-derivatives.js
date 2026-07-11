const fs = require('fs');
const html = fs.readFileSync('index.html', 'utf-8');

// Extract the WORDS array
const match = html.match(/const WORDS = \[([\s\S]*?)\];/);
if (!match) { console.error('WORDS not found'); process.exit(1); }

// Parse WORDS using eval (safe since it's our own data)
let WORDS;
eval('WORDS = [' + match[1] + '];');

// Build a lookup: word → meanings
const wordDefMap = {};
WORDS.forEach(w => {
  if (w.meanings && w.meanings.length > 0) {
    wordDefMap[w.word.toLowerCase()] = w.meanings[0].def;
  }
});

// Build a simple derivative def map from common patterns + notes
// Many notes fields contain definitions like "discover 发现；找到；了解到"
let missingCount = 0;
let fixedCount = 0;

WORDS.forEach(w => {
  if (!w.derivatives || w.derivatives.length === 0) return;

  // Parse notes for derivative definitions
  const notesDefs = {};
  if (w.notes) {
    // Pattern: "word1 pos1. def1; word2 pos2. def2" or "word1 def1, word2 def2"
    const parts = w.notes.split(/[;；,，]/);
    parts.forEach(part => {
      const trimmed = part.trim();
      // Try "word 中文" pattern
      const m = trimmed.match(/^(\w+)\s+(.+)$/);
      if (m) {
        notesDefs[m[1].toLowerCase()] = m[2].replace(/[，。.]/g, '，').trim();
      }
    });
  }

  w.derivatives.forEach(d => {
    if (!d.def) {
      // Try standalone word lookup first
      if (wordDefMap[d.word.toLowerCase()]) {
        d.def = wordDefMap[d.word.toLowerCase()];
        fixedCount++;
      }
      // Try notes field
      else if (notesDefs[d.word.toLowerCase()]) {
        d.def = notesDefs[d.word.toLowerCase()];
        fixedCount++;
      }
      // Try common suffixes to derive meaning
      else {
        missingCount++;
        console.log('Missing def for derivative: ' + d.word + ' (parent: ' + w.word + ')');
      }
    }
  });
});

console.log('\nFixed: ' + fixedCount + ', Still missing: ' + missingCount);

// Now let's manually add defs for the remaining missing ones
// Common derivatives with known meanings:
const manualDefs = {
  // Day 1
  'kindly': 'adv. 仁慈地',
  'kindness': 'n. 善良',
  'difference': 'n. 差异',
  'discover': 'v. 发现；找到',
  'recover': 'v. 恢复，康复',
  // Day 2
  'patiently': 'adv. 耐心地',
  'patience': 'n. 耐心',
  'beginner': 'n. 初学者',
  'beginning': 'n. 开始',
  'clearness': 'n. 清晰',
  'clearly': 'adv. 清楚地',
  'explanation': 'n. 解释',
  'listener': 'n. 倾听者',
  // Day 3
  'disagreement': 'n. 分歧',
  'agreement': 'n. 协议；一致',
  'agreed': 'adj. 同意的',
  'decision': 'n. 决定',
  'suggestion': 'n. 建议',
  'truthful': 'adj. 诚实的',
  'truthfully': 'adv. 诚实地',
  'imagination': 'n. 想象力',
  'imaginative': 'adj. 富有想象力的',
  // Day 4
  'helpful': 'adj. 有帮助的',
  'helpless': 'adj. 无助的',
  'helper': 'n. 帮手',
  'useful': 'adj. 有用的',
  'useless': 'adj. 无用的',
  'usage': 'n. 用法',
  'easily': 'adv. 容易地',
  'uneasy': 'adj. 不安的',
  // Day 5
  'natural': 'adj. 自然的',
  'naturally': 'adv. 自然地',
  'expression': 'n. 表达；表情',
  'careful': 'adj. 小心的',
  'careless': 'adj. 粗心的',
  'carefully': 'adv. 小心地',
  'surprising': 'adj. 令人惊讶的',
  'surprised': 'adj. 感到惊讶的',
  // Day 6
  'actually': 'adv. 实际上',
  'activity': 'n. 活动',
  'actor': 'n. 演员',
  'action': 'n. 行动',
  'development': 'n. 发展',
  'developing': 'adj. 发展中的',
  'developed': 'adj. 发达的',
  'completely': 'adv. 完全地',
  'incompletely': 'adv. 不完全地',
  // Day 7
  'improvement': 'n. 改进',
  'improving': 'adj. 改善中的',
  'enjoyable': 'adj. 令人愉快的',
  'enjoyment': 'n. 享受',
  'truly': 'adv. 真正地',
  'untrue': 'adj. 不真实的',
  'personal': 'adj. 个人的',
  'personally': 'adv. 就个人而言',
  // Day 8
  'educational': 'adj. 教育的',
  'educator': 'n. 教育者',
  'movement': 'n. 运动；移动',
  'movable': 'adj. 可移动的',
  'preparation': 'n. 准备',
  'preparatory': 'adj. 预备的',
  'successful': 'adj. 成功的',
  'successfully': 'adv. 成功地',
  // Day 9
  'information': 'n. 信息',
  'informative': 'adj. 提供信息的',
  'protection': 'n. 保护',
  'protective': 'adj. 保护的',
  'excitement': 'n. 兴奋',
  'excited': 'adj. 兴奋的',
  'exciting': 'adj. 令人兴奋的',
  // Day 10
  'actually': 'adv. 实际上',
  'really': 'adv. 真正地',
  'reality': 'n. 现实',
  'realistic': 'adj. 现实的',
  'happiness': 'n. 幸福',
  'happily': 'adv. 快乐地',
  'unhappy': 'adj. 不快乐的',
  // Day 11
  'important': 'adj. 重要的',
  'importantly': 'adv. 重要地',
  'importance': 'n. 重要性',
  'beautiful': 'adj. 美丽的',
  'beautifully': 'adv. 美丽地',
  'understanding': 'n. 理解；adj. 善解人意的',
  'understandable': 'adj. 可理解的',
  // Day 12
  'comfortable': 'adj. 舒适的',
  'comfortably': 'adv. 舒适地',
  'uncomfortable': 'adj. 不舒适的',
  'traditionally': 'adv. 传统上',
  'traditional': 'adj. 传统的',
  'probably': 'adv. 可能地',
  'probable': 'adj. 很可能的',
  // Day 13
  'seriously': 'adv. 认真地；严重地',
  'seriousness': 'n. 严肃；严重性',
  'dangerously': 'adv. 危险地',
  'endanger': 'v. 危及',
  'friendly': 'adj. 友好的',
  'friendship': 'n. 友谊',
  // Day 14
  'suddenly': 'adv. 突然地',
  'suddenness': 'n. 突然',
  'alive': 'adj. 活着的',
  'living': 'adj. 活着的；n. 生计',
  'lively': 'adj. 活泼的',
  'impossibly': 'adv. 不可能地',
  'possible': 'adj. 可能的',
  'possibly': 'adv. 可能地',
  // Day 15
  'normally': 'adv. 正常地',
  'abnormal': 'adj. 不正常的',
  'sadly': 'adv. 悲伤地',
  'sadness': 'n. 悲伤',
  'finally': 'adv. 最后',
  'finalist': 'n. 决赛选手',
  // Day 16
  'loudly': 'adv. 大声地',
  'aloud': 'adv. 出声地',
  'climber': 'n. 登山者',
  'climbing': 'n. 登山',
  'recently': 'adv. 最近',
  'recentness': 'n. 最近',
  // Day 17
  'weekly': 'adj./adv. 每周的/地',
  'weekday': 'n. 工作日',
  'weekend': 'n. 周末',
  'daily': 'adj./adv. 每日的/地',
  'monthly': 'adj./adv. 每月的/地',
  // Day 18
  'quickly': 'adv. 快速地',
  'quickness': 'n. 快速',
  'slowly': 'adv. 缓慢地',
  'slowness': 'n. 缓慢',
  'player': 'n. 运动员；玩家',
  'playful': 'adj. 爱玩的',
  // Day 19
  'worker': 'n. 工人',
  'workable': 'adj. 可行的',
  'wounded': 'adj. 受伤的',
  'worsen': 'v. 恶化',
  'windy': 'adj. 有风的',
};

let manualAdded = 0;
let stillMissing = 0;

WORDS.forEach(w => {
  if (!w.derivatives || w.derivatives.length === 0) return;
  w.derivatives.forEach(d => {
    if (!d.def) {
      const key = d.word.toLowerCase();
      if (manualDefs[key]) {
        d.def = manualDefs[key];
        manualAdded++;
      } else if (manualDefs[d.word]) {
        d.def = manualDefs[d.word];
        manualAdded++;
      } else {
        stillMissing++;
        console.log('STILL MISSING: ' + d.word + ' (parent: ' + w.word + ')');
      }
    }
  });
});

console.log('\n=== Summary ===');
console.log('Auto-fixed (from WORDS/notes): ' + fixedCount);
console.log('Manual defs added: ' + manualAdded);
console.log('Still missing: ' + stillMissing);

// Now rebuild the WORDS constant string
const newWordsStr = JSON.stringify(WORDS, null, 2)
  .replace(/"(\w+)":/g, '$1:')  // unquote keys
  .replace(/"/g, "'");          // use single quotes

// Replace in HTML
const oldMatch = html.match(/const WORDS = \[[\s\S]*?\];/);
if (oldMatch) {
  const newHtml = html.replace(oldMatch[0], 'const WORDS = ' + newWordsStr + ';');
  fs.writeFileSync('index.html', newHtml, 'utf-8');
  console.log('\nUpdated index.html with derivative definitions');
  console.log('New size:', newHtml.length.toLocaleString(), 'bytes');
}

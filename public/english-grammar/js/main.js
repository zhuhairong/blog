/**
 * 英语语法学习网页 - 公共功能模块
 */

// ===== 常量定义 =====
const GRAMMAR_MODULES = [
  { id: 'present-perfect', name: '现在完成时', icon: '⏰', color: 'orange' },
  { id: 'past-continuous', name: '过去进行时', icon: '🕐', color: 'blue' },
  { id: 'modal-verbs', name: '情态动词', icon: '🗣️', color: 'green' },
  { id: 'passive-voice', name: '被动语态', icon: '🔄', color: 'purple' },
  { id: 'comparison', name: '比较级最高级', icon: '📊', color: 'pink' },
  { id: 'infinitive-gerund', name: '不定式和动名词', icon: '✨', color: 'yellow' },
  { id: 'conjunctions', name: '连词和状语从句', icon: '🔗', color: 'red' }
];

const LEVEL_THRESHOLDS = [
  { level: 1, points: 0, title: '英语萌新' },
  { level: 2, points: 100, title: '入门学徒' },
  { level: 3, points: 300, title: '学习达人' },
  { level: 4, points: 600, title: '语法能手' },
  { level: 5, points: 1000, title: '英语高手' },
  { level: 6, points: 1500, title: '语法专家' },
  { level: 7, points: 2200, title: '英语达人' },
  { level: 8, points: 3000, title: '语法大师' },
  { level: 9, points: 4000, title: '英语王者' },
  { level: 10, points: 5500, title: '语法至尊' }
];

const ACHIEVEMENTS = [
  { id: 'first_study', name: '初学者', icon: '🌱', condition: '完成第一次学习', points: 50 },
  { id: 'streak_3', name: '坚持者', icon: '🔥', condition: '连续学习3天', points: 100 },
  { id: 'perfect_10', name: '学霸', icon: '⭐', condition: '单次练习正确率100%', points: 150 },
  { id: 'speed_10', name: '速度王', icon: '⚡', condition: '10秒内答对一题', points: 30 },
  { id: 'combo_5', name: '连击大师', icon: '💥', condition: '连续答对5题', points: 80 },
  { id: 'all_modules', name: '语法达人', icon: '🏆', condition: '完成所有语法模块', points: 500 },
  { id: 'total_100', name: '勤奋家', icon: '📚', condition: '累计完成100道题', points: 200 },
  { id: 'module_perfect', name: '完美主义', icon: '💎', condition: '某模块全部三星', points: 100 }
];

// ===== 存储管理 =====
const Storage = {
  // 初始化存储
  init() {
    if (!localStorage.getItem('userProgress')) {
      this.setUserProgress({
        level: 1,
        totalPoints: 0,
        streakDays: 0,
        lastStudyDate: null,
        todayQuestions: 0,
        todayTime: 0
      });
    }
    if (!localStorage.getItem('grammarProgress')) {
      const initialProgress = {};
      GRAMMAR_MODULES.forEach(m => {
        initialProgress[m.id] = {
          viewed: false,
          completed: 0,
          correct: 0,
          stars: 0
        };
      });
      this.setGrammarProgress(initialProgress);
    }
    if (!localStorage.getItem('achievements')) {
      this.setAchievements([]);
    }
    if (!localStorage.getItem('settings')) {
      this.setSettings({
        soundEnabled: true,
        theme: 'cartoon'
      });
    }
  },

  // 用户进度
  getUserProgress() {
    return JSON.parse(localStorage.getItem('userProgress'));
  },
  setUserProgress(data) {
    localStorage.setItem('userProgress', JSON.stringify(data));
  },
  updateUserProgress(key, value) {
    const progress = this.getUserProgress();
    progress[key] = value;
    this.setUserProgress(progress);
  },

  // 语法模块进度
  getGrammarProgress() {
    return JSON.parse(localStorage.getItem('grammarProgress'));
  },
  setGrammarProgress(data) {
    localStorage.setItem('grammarProgress', JSON.stringify(data));
  },
  updateModuleProgress(moduleId, key, value) {
    const progress = this.getGrammarProgress();
    progress[moduleId][key] = value;
    this.setGrammarProgress(progress);
  },

  // 成就
  getAchievements() {
    return JSON.parse(localStorage.getItem('achievements'));
  },
  setAchievements(data) {
    localStorage.setItem('achievements', JSON.stringify(data));
  },
  addAchievement(achievementId) {
    const achievements = this.getAchievements();
    if (!achievements.includes(achievementId)) {
      achievements.push(achievementId);
      this.setAchievements(achievements);
      return true;
    }
    return false;
  },

  // 设置
  getSettings() {
    return JSON.parse(localStorage.getItem('settings'));
  },
  setSettings(data) {
    localStorage.setItem('settings', JSON.stringify(data));
  },

  // 清除所有数据
  clearAll() {
    localStorage.removeItem('userProgress');
    localStorage.removeItem('grammarProgress');
    localStorage.removeItem('achievements');
    localStorage.removeItem('settings');
    this.init();
  }
};

// ===== 积分系统 =====
const PointsSystem = {
  // 添加积分
  addPoints(points) {
    const progress = Storage.getUserProgress();
    progress.totalPoints += points;
    progress.level = this.calculateLevel(progress.totalPoints);
    Storage.setUserProgress(progress);
    return progress;
  },

  // 计算等级
  calculateLevel(totalPoints) {
    for (let i = LEVEL_THRESHOLDS.length - 1; i >= 0; i--) {
      if (totalPoints >= LEVEL_THRESHOLDS[i].points) {
        return LEVEL_THRESHOLDS[i].level;
      }
    }
    return 1;
  },

  // 获取等级信息
  getLevelInfo(level) {
    return LEVEL_THRESHOLDS.find(l => l.level === level);
  },

  // 获取下一等级信息
  getNextLevelInfo(level) {
    if (level >= 10) return null;
    return LEVEL_THRESHOLDS.find(l => l.level === level + 1);
  },

  // 计算到下一等级的进度百分比
  getLevelProgress() {
    const progress = Storage.getUserProgress();
    const currentLevelInfo = this.getLevelInfo(progress.level);
    const nextLevelInfo = this.getNextLevelInfo(progress.level);

    if (!nextLevelInfo) return 100;

    const currentThreshold = currentLevelInfo.points;
    const nextThreshold = nextLevelInfo.points;
    const pointsInLevel = progress.totalPoints - currentThreshold;
    const pointsNeeded = nextThreshold - currentThreshold;

    return Math.min(100, Math.round((pointsInLevel / pointsNeeded) * 100));
  }
};

// ===== 连续学习天数 =====
const StreakManager = {
  // 更新学习日期
  updateStudyDate() {
    const progress = Storage.getUserProgress();
    const today = new Date().toISOString().split('T')[0];

    if (progress.lastStudyDate === today) {
      return progress.streakDays;
    }

    if (progress.lastStudyDate) {
      const lastDate = new Date(progress.lastStudyDate);
      const todayDate = new Date(today);
      const diffDays = Math.floor((todayDate - lastDate) / (1000 * 60 * 60 * 24));

      if (diffDays === 1) {
        progress.streakDays += 1;
      } else if (diffDays > 1) {
        progress.streakDays = 1;
      }
    } else {
      progress.streakDays = 1;
    }

    progress.lastStudyDate = today;
    Storage.setUserProgress(progress);

    // 检查连续学习成就
    if (progress.streakDays >= 3) {
      AchievementManager.checkAndUnlock('streak_3');
    }

    return progress.streakDays;
  }
};

// ===== 成就管理 =====
const AchievementManager = {
  // 检查并解锁成就
  checkAndUnlock(achievementId) {
    const achievement = ACHIEVEMENTS.find(a => a.id === achievementId);
    if (!achievement) return null;

    if (Storage.addAchievement(achievementId)) {
      PointsSystem.addPoints(achievement.points);
      return achievement;
    }
    return null;
  },

  // 检查所有成就条件
  checkAllConditions() {
    const progress = Storage.getUserProgress();
    const grammarProgress = Storage.getGrammarProgress();
    const achievements = Storage.getAchievements();

    // 检查完成所有模块
    const allCompleted = Object.values(grammarProgress).every(m => m.completed > 0);
    if (allCompleted) {
      this.checkAndUnlock('all_modules');
    }

    // 检查累计题目数
    const totalCompleted = Object.values(grammarProgress).reduce((sum, m) => sum + m.completed, 0);
    if (totalCompleted >= 100) {
      this.checkAndUnlock('total_100');
    }

    // 检查某模块全部三星
    const hasPerfectModule = Object.values(grammarProgress).some(m => m.stars === 3);
    if (hasPerfectModule) {
      this.checkAndUnlock('module_perfect');
    }
  },

  // 获取已解锁成就列表
  getUnlockedAchievements() {
    const unlockedIds = Storage.getAchievements();
    return ACHIEVEMENTS.filter(a => unlockedIds.includes(a.id));
  },

  // 获取所有成就（带解锁状态）
  getAllAchievements() {
    const unlockedIds = Storage.getAchievements();
    return ACHIEVEMENTS.map(a => ({
      ...a,
      unlocked: unlockedIds.includes(a.id)
    }));
  }
};

// ===== 星级计算 =====
const StarCalculator = {
  // 根据正确率计算星级
  calculateStars(correct, total) {
    if (total === 0) return 0;
    const rate = correct / total;
    if (rate >= 0.9) return 3;
    if (rate >= 0.7) return 2;
    if (rate >= 0.5) return 1;
    return 0;
  },

  // 更新模块星级
  updateModuleStars(moduleId) {
    const progress = Storage.getGrammarProgress();
    const module = progress[moduleId];
    module.stars = this.calculateStars(module.correct, module.completed);
    Storage.setGrammarProgress(progress);
    return module.stars;
  }
};

// ===== 语音播放 =====
const SpeechHelper = {
  // 检查是否支持语音
  isSupported() {
    return 'speechSynthesis' in window;
  },

  // 播放文本
  speak(text, lang = 'en-US') {
    if (!this.isSupported()) return;

    const settings = Storage.getSettings();
    if (!settings.soundEnabled) return;

    window.speechSynthesis.cancel();

    const utterance = new SpeechSynthesisUtterance(text);
    utterance.lang = lang;
    utterance.rate = 0.8;
    utterance.pitch = 1;

    window.speechSynthesis.speak(utterance);
  },

  // 停止播放
  stop() {
    if (this.isSupported()) {
      window.speechSynthesis.cancel();
    }
  }
};

// ===== 音效播放 =====
const SoundEffects = {
  // 播放正确音效（使用简单的提示音）
  playCorrect() {
    const settings = Storage.getSettings();
    if (!settings.soundEnabled) return;
    this.playTone(800, 0.1, 'sine');
  },

  // 播放错误音效
  playWrong() {
    const settings = Storage.getSettings();
    if (!settings.soundEnabled) return;
    this.playTone(300, 0.15, 'sine');
  },

  // 播放成就音效
  playAchievement() {
    const settings = Storage.getSettings();
    if (!settings.soundEnabled) return;
    this.playTone(600, 0.1, 'sine');
    setTimeout(() => this.playTone(800, 0.1, 'sine'), 100);
    setTimeout(() => this.playTone(1000, 0.15, 'sine'), 200);
  },

  // 使用 Web Audio API 播放简单音调
  playTone(frequency, duration, type = 'sine') {
    try {
      const audioContext = new (window.AudioContext || window.webkitAudioContext)();
      const oscillator = audioContext.createOscillator();
      const gainNode = audioContext.createGain();

      oscillator.connect(gainNode);
      gainNode.connect(audioContext.destination);

      oscillator.frequency.value = frequency;
      oscillator.type = type;

      gainNode.gain.setValueAtTime(0.3, audioContext.currentTime);
      gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + duration);

      oscillator.start(audioContext.currentTime);
      oscillator.stop(audioContext.currentTime + duration);
    } catch (e) {
      console.log('Audio not supported');
    }
  }
};

// ===== UI 更新工具 =====
const UIHelper = {
  // 显示连击提示
  showCombo(comboCount) {
    const toast = document.getElementById('comboToast');
    if (!toast) return;

    toast.textContent = `🔥 连击 x${comboCount}！`;
    toast.classList.add('show');

    setTimeout(() => {
      toast.classList.remove('show');
    }, 1500);
  },

  // 显示成就弹窗
  showAchievement(achievement) {
    const modal = document.getElementById('achievementModal');
    if (!modal) return;

    document.getElementById('achievementIcon').textContent = achievement.icon;
    document.getElementById('achievementTitle').textContent = achievement.name;
    document.getElementById('achievementDesc').textContent = achievement.condition;
    document.getElementById('achievementPoints').textContent = `+${achievement.points} 积分`;

    modal.classList.add('show');
    SoundEffects.playAchievement();

    setTimeout(() => {
      modal.classList.remove('show');
    }, 3000);
  },

  // 更新进度显示
  updateProgressDisplay() {
    const progress = Storage.getUserProgress();
    const levelInfo = PointsSystem.getLevelInfo(progress.level);
    const levelProgress = PointsSystem.getLevelProgress();

    // 更新等级显示
    const levelBadge = document.querySelector('.level-badge');
    if (levelBadge) {
      levelBadge.textContent = `${progress.level}级 ${levelInfo.title}`;
    }

    // 更新积分显示
    const pointsValue = document.querySelector('.points-value');
    if (pointsValue) {
      pointsValue.textContent = progress.totalPoints;
    }

    // 更新进度条
    const progressBar = document.querySelector('.progress-bar');
    if (progressBar) {
      progressBar.style.width = `${levelProgress}%`;
    }

    // 更新连续天数
    const streakValue = document.querySelector('.stat-item.streak .stat-value');
    if (streakValue) {
      streakValue.textContent = progress.streakDays;
    }

    // 更新今日题目数
    const todayValue = document.querySelector('.stat-item.today .stat-value');
    if (todayValue) {
      todayValue.textContent = progress.todayQuestions || 0;
    }
  },

  // 生成语法卡片HTML
  generateGrammarCards() {
    const progress = Storage.getGrammarProgress();
    const modules = GRAMMAR_MODULES;

    return modules.map(module => {
      const moduleProgress = progress[module.id];
      const totalQuestions = 10; // 每模块10题
      const progressPercent = Math.round((moduleProgress.completed / totalQuestions) * 100);
      const progressClass = progressPercent < 30 ? 'low' : progressPercent < 70 ? 'medium' : 'high';

      const starsHtml = [1, 2, 3].map(star =>
        `<span class="star ${moduleProgress.stars >= star ? 'filled' : ''}">⭐</span>`
      ).join('');

      return `
        <div class="grammar-card" onclick="window.location.href='pages/grammar/${module.id}.html'">
          <div class="card-header">
            <div class="card-icon ${module.color}">${module.icon}</div>
            <div>
              <div class="card-title">${module.name}</div>
              <div class="card-subtitle">${moduleProgress.completed}/${totalQuestions} 已完成</div>
            </div>
          </div>
          <div class="card-progress">
            <div class="card-progress-bar">
              <div class="card-progress-fill ${progressClass}" style="width: ${progressPercent}%"></div>
            </div>
            <div class="card-progress-text">${progressPercent}% 完成</div>
          </div>
          <div class="card-stars">${starsHtml}</div>
          <div class="card-action" onclick="event.stopPropagation(); window.location.href='pages/practice/practice.html?module=${module.id}'">
            开始练习 →
          </div>
        </div>
      `;
    }).join('');
  }
};

// ===== 页面初始化 =====
function initPage() {
  Storage.init();
  StreakManager.updateStudyDate();

  // 如果有语法卡片容器，渲染卡片
  const gridContainer = document.querySelector('.grammar-grid');
  if (gridContainer) {
    gridContainer.innerHTML = UIHelper.generateGrammarCards();
  }

  // 更新进度显示
  UIHelper.updateProgressDisplay();

  // 检查成就条件
  AchievementManager.checkAllConditions();
}

// ===== DOM 加载完成后初始化 =====
document.addEventListener('DOMContentLoaded', initPage);

// 导出模块供其他页面使用
window.Storage = Storage;
window.PointsSystem = PointsSystem;
window.StreakManager = StreakManager;
window.AchievementManager = AchievementManager;
window.StarCalculator = StarCalculator;
window.SpeechHelper = SpeechHelper;
window.SoundEffects = SoundEffects;
window.UIHelper = UIHelper;
window.GRAMMAR_MODULES = GRAMMAR_MODULES;
window.ACHIEVEMENTS = ACHIEVEMENTS;
window.LEVEL_THRESHOLDS = LEVEL_THRESHOLDS;
// 等待页面加载完成
document.addEventListener('DOMContentLoaded', function() {
    // 初始化返回顶部按钮
    initBackToTop();

    // 初始化平滑滚动
    initSmoothScroll();

    // 初始化方法卡片展开状态（第一个默认展开）
    const firstMethod = document.querySelector('.method-card .method-header');
    if (firstMethod) {
        toggleMethod(firstMethod);
    }
});

// 切换答案显示
function toggleAnswer(element) {
    element.classList.toggle('show-answer');

    // 判断是否正确
    const answerSpan = element.querySelector('.quiz-answer');
    if (answerSpan.classList.contains('correct')) {
        element.classList.add('correct-bg');
    } else {
        element.classList.add('wrong-bg');
    }
}

// 切换解法内容展开/折叠
function toggleMethod(header) {
    const content = header.nextElementSibling;
    const icon = header.querySelector('.toggle-icon');

    if (content.classList.contains('collapsed')) {
        content.classList.remove('collapsed');
        icon.textContent = '▲';
    } else {
        content.classList.add('collapsed');
        icon.textContent = '▼';
    }
}

// 切换解答显示
function toggleSolution(element) {
    const solutionContent = element.nextElementSibling;
    const showText = element.querySelector('.show-text');

    if (solutionContent.classList.contains('show')) {
        solutionContent.classList.remove('show');
        showText.textContent = '点击查看解答 ▼';
    } else {
        solutionContent.classList.add('show');
        showText.textContent = '点击隐藏解答 ▲';
    }
}

// 切换拓展内容
function toggleExtension(card) {
    const content = card.querySelector('.extension-content');

    if (content.classList.contains('show')) {
        content.classList.remove('show');
    } else {
        content.classList.add('show');
    }
}

// 切换练习标签页
function switchTab(event, tabId) {
    // 移除所有标签页的激活状态
    const tabs = document.querySelectorAll('.tab-btn');
    tabs.forEach(tab => tab.classList.remove('active'));

    // 激活当前标签
    event.target.classList.add('active');

    // 隐藏所有内容
    const contents = document.querySelectorAll('.tab-content');
    contents.forEach(content => content.classList.remove('active'));

    // 显示当前内容
    document.getElementById(tabId).classList.add('active');
}

// 检查答案（普通练习）
function checkAnswer(button, correctAnswer) {
    const input = button.previousElementSibling;
    const feedback = button.nextElementSibling;
    const userAnswer = input.value.trim().replace(/\s+/g, '').toLowerCase();
    const correct = correctAnswer.replace(/\s+/g, '').toLowerCase();

    // 清除之前的反馈
    feedback.classList.remove('correct', 'wrong');
    feedback.textContent = '';

    // 标准化答案格式处理
    const userNormalized = normalizeAnswer(userAnswer);
    const correctNormalized = normalizeAnswer(correct);

    if (userNormalized === correctNormalized) {
        feedback.classList.add('correct');
        feedback.textContent = '✓ 正确！很好！';
        input.style.borderColor = '#28a745';
    } else {
        feedback.classList.add('wrong');
        feedback.textContent = '✗ 不正确。正确答案：' + formatAnswer(correctAnswer);
        input.style.borderColor = '#dc3545';
    }
}

// 标准化答案格式
function normalizeAnswer(answer) {
    // 处理各种格式：x1=2,x2=3 / x₁=2,x₂=3 / x=2或x=3 等
    answer = answer.replace(/x₁|x1/gi, 'x1');
    answer = answer.replace(/x₂|x2/gi, 'x2');
    answer = answer.replace(/x=/gi, 'x');
    answer = answer.replace(/或/g, ',');
    answer = answer.replace(/=/g, '');
    answer = answer.replace(/，/g, ',');

    // 处理分数格式
    answer = answer.replace(/1\/2/g, '0.5');
    answer = answer.replace(/2\/3/g, '0.667');
    answer = answer.replace(/-2\/3/g, '-0.667');
    answer = answer.replace(/-1\/2/g, '-0.5');
    answer = answer.replace(/1\/3/g, '0.333');
    answer = answer.replace(/-1\/3/g, '-0.333');

    // 分割并排序
    const parts = answer.split(',').filter(p => p).map(p => {
        // 提取数值
        const match = p.match(/-?\d*\.?\d*/);
        return match ? parseFloat(match[0]) : p;
    }).sort((a, b) => a - b);

    return parts.join(',');
}

// 格式化答案显示
function formatAnswer(answer) {
    return answer.replace(/x₁=([^,]+),x₂=([^,]+)/, 'x₁ = $1, x₂ = $2');
}

// 检查综合练习答案（包含方法选择）
function checkComprehensive(button, correctMethod, correctAnswer) {
    const methodSelect = button.previousElementSibling.previousElementSibling;
    const input = button.previousElementSibling;
    const feedback = button.nextElementSibling;

    feedback.classList.remove('correct', 'wrong');
    feedback.textContent = '';

    const selectedMethod = methodSelect.value;
    const userAnswer = input.value.trim().replace(/\s+/g, '').toLowerCase();
    const correct = correctAnswer.replace(/\s+/g, '').toLowerCase();

    const methodCorrect = selectedMethod === correctMethod;
    const answerCorrect = normalizeAnswer(userAnswer) === normalizeAnswer(correct);

    if (methodCorrect && answerCorrect) {
        feedback.classList.add('correct');
        feedback.textContent = '✓ 方法选择正确，答案正确！完美！';
        input.style.borderColor = '#28a745';
        methodSelect.style.borderColor = '#28a745';
    } else if (!methodCorrect && answerCorrect) {
        feedback.classList.add('wrong');
        feedback.textContent = '✗ 答案正确，但建议方法不对。建议方法：' + correctMethod;
        input.style.borderColor = '#28a745';
        methodSelect.style.borderColor = '#dc3545';
    } else if (methodCorrect && !answerCorrect) {
        feedback.classList.add('wrong');
        feedback.textContent = '✗ 方法选择正确，但答案不对。正确答案：' + formatAnswer(correctAnswer);
        input.style.borderColor = '#dc3545';
        methodSelect.style.borderColor = '#28a745';
    } else {
        feedback.classList.add('wrong');
        feedback.textContent = '✗ 方法选择和答案都不正确。建议方法：' + correctMethod + '，正确答案：' + formatAnswer(correctAnswer);
        input.style.borderColor = '#dc3545';
        methodSelect.style.borderColor = '#dc3545';
    }
}

// 初始化返回顶部按钮
function initBackToTop() {
    const backToTopBtn = document.getElementById('backToTop');

    window.addEventListener('scroll', function() {
        if (window.pageYOffset > 300) {
            backToTopBtn.classList.add('show');
        } else {
            backToTopBtn.classList.remove('show');
        }
    });
}

// 返回顶部
function scrollToTop() {
    window.scrollTo({
        top: 0,
        behavior: 'smooth'
    });
}

// 初始化平滑滚动
function initSmoothScroll() {
    const navLinks = document.querySelectorAll('.nav-links a');

    navLinks.forEach(link => {
        link.addEventListener('click', function(e) {
            e.preventDefault();

            const targetId = this.getAttribute('href');
            const targetElement = document.querySelector(targetId);

            if (targetElement) {
                const offsetTop = targetElement.offsetTop - 80; // 考虑导航栏高度

                window.scrollTo({
                    top: offsetTop,
                    behavior: 'smooth'
                });
            }
        });
    });
}

// 计算得分功能（可选）
function calculateScore() {
    let correctCount = 0;
    let totalCount = 0;

    const feedbacks = document.querySelectorAll('.feedback');
    feedbacks.forEach(feedback => {
        totalCount++;
        if (feedback.classList.contains('correct')) {
            correctCount++;
        }
    });

    const score = totalCount > 0 ? Math.round((correctCount / totalCount) * 100) : 0;

    return {
        correct: correctCount,
        total: totalCount,
        percentage: score
    };
}

// 显示得分（可选功能）
function showScore() {
    const score = calculateScore();
    alert(`你的得分：${score.correct}/${score.total} (${score.percentage}%)`);
}

// 添加键盘快捷键支持
document.addEventListener('keydown', function(e) {
    // ESC 键关闭所有展开的解答
    if (e.key === 'Escape') {
        const expandedSolutions = document.querySelectorAll('.solution-content.show');
        expandedSolutions.forEach(solution => {
            solution.classList.remove('show');
            const toggle = solution.previousElementSibling;
            if (toggle) {
                toggle.querySelector('.show-text').textContent = '点击查看解答 ▼';
            }
        });
    }
});

// 为移动端添加触摸优化
if ('ontouchstart' in window) {
    document.querySelectorAll('.quiz-option, .method-header, .solution-toggle, .extension-card').forEach(element => {
        element.addEventListener('touchstart', function() {
            this.style.transform = 'scale(0.98)';
        });

        element.addEventListener('touchend', function() {
            this.style.transform = 'scale(1)';
        });
    });
}
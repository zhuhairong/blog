# Demo 目录代码审核与重写 - 实现计划

## [x] Task 1: 审核 demo 目录代码质量
- **Priority**: P0
- **Depends On**: None
- **Description**: 
  - 分析 demo 目录中的所有代码文件
  - 识别代码结构、错误处理、功能展示等方面的问题
  - 确定需要优先重写的演示程序
- **Acceptance Criteria Addressed**: AC-1
- **Test Requirements**:
  - `human-judgment` TR-1.1: 完成所有 demo 文件的审核，提供问题清单
  - `human-judgment` TR-1.2: 确定至少 5 个需要优先重写的演示程序
- **Notes**: 重点关注仅打印说明文字而不实际调用库功能的演示程序

## [x] Task 2: 重写 demo_otp.c 演示程序
- **Priority**: P0
- **Depends On**: Task 1
- **Description**: 
  - 重写 demo_otp.c 以展示 OTP 库的实际功能
  - 实现 TOTP 和 HOTP 的生成和验证
  - 添加错误处理和边界情况测试
- **Acceptance Criteria Addressed**: AC-2, AC-3, AC-4
- **Test Requirements**:
  - `programmatic` TR-2.1: 程序能够编译成功
  - `programmatic` TR-2.2: 程序能够运行并展示实际 OTP 生成和验证
  - `human-judgment` TR-2.3: 代码结构清晰，注释适当
- **Notes**: 确保演示程序展示 TOTP 和 HOTP 的完整流程

## [x] Task 3: 重写其他关键演示程序
- **Priority**: P1
- **Depends On**: Task 1
- **Description**: 
  - 根据审核结果，重写其他关键演示程序
  - 确保每个重写的演示程序能够实际调用库功能
  - 保持代码风格一致
- **Acceptance Criteria Addressed**: AC-2, AC-3, AC-4
- **Test Requirements**:
  - `programmatic` TR-3.1: 所有重写的程序能够编译成功
  - `programmatic` TR-3.2: 所有重写的程序能够运行并展示功能
  - `human-judgment` TR-3.3: 代码质量符合最佳实践
- **Notes**: 优先重写那些展示核心功能的演示程序

## [x] Task 4: 测试所有演示程序
- **Priority**: P1
- **Depends On**: Task 2, Task 3
- **Description**: 
  - 编译并运行所有演示程序
  - 验证程序运行结果是否符合预期
  - 修复运行过程中发现的问题
- **Acceptance Criteria Addressed**: AC-3
- **Test Requirements**:
  - `programmatic` TR-4.1: 所有演示程序能够成功编译
  - `programmatic` TR-4.2: 所有演示程序能够正常运行
  - `human-judgment` TR-4.3: 运行结果符合预期
- **Notes**: 关注程序的输出是否清晰、功能是否完整

## [x] Task 5: 代码质量检查和优化
- **Priority**: P2
- **Depends On**: Task 4
- **Description**: 
  - 对重写后的代码进行质量检查
  - 优化代码结构和可读性
  - 确保注释和文档的完整性
- **Acceptance Criteria Addressed**: AC-4
- **Test Requirements**:
  - `human-judgment` TR-5.1: 代码符合 C 语言最佳实践
  - `human-judgment` TR-5.2: 注释清晰完整
  - `human-judgment` TR-5.3: 代码风格一致
- **Notes**: 关注代码的可维护性和可读性

# C Utils Demo Implementation - Product Requirement Document

## Overview
- **Summary**: 为c_utils库中的每个模块创建或完善demo文件，确保每个库都有实际的使用示例，而不是模拟实现。
- **Purpose**: 确保c_utils库的每个模块都有可运行的demo，方便用户理解和使用这些库。
- **Target Users**: c_utils库的使用者和维护者。

## Goals
- 确保每个c_utils库模块都有对应的demo文件
- 所有demo文件都实际调用库函数，不使用模拟实现
- 所有demo文件都能编译通过并正常运行
- 按批次实现，优先处理核心模块

## Non-Goals (Out of Scope)
- 不修改库本身的代码
- 不创建新的库功能
- 不优化现有库的实现

## Background & Context
- c_utils是一个C语言工具库，包含多个功能模块
- 目前大部分模块已有demo文件，但可能存在一些模块缺少demo或demo是模拟实现
- 需要确保每个模块都有实际调用库函数的demo

## Functional Requirements
- **FR-1**: 识别缺少demo的库模块
- **FR-2**: 为缺少demo的模块创建实际的demo文件
- **FR-3**: 确保所有demo文件都能编译通过
- **FR-4**: 确保所有demo文件都能正常运行

## Non-Functional Requirements
- **NFR-1**: Demo文件应该简洁明了，展示库的核心功能
- **NFR-2**: Demo文件应该包含必要的错误处理
- **NFR-3**: Demo文件应该有清晰的注释和说明
- **NFR-4**: 实现应该分批进行，优先处理核心模块

## Constraints
- **Technical**: 使用C语言，遵循现有的代码风格和结构
- **Dependencies**: 依赖c_utils库本身

## Assumptions
- 所有库模块都有对应的头文件
- 所有库模块都已经实现并可编译

## Acceptance Criteria

### AC-1: 识别缺少demo的库模块
- **Given**: 完整的c_utils库头文件列表和demo文件列表
- **When**: 比较两个列表
- **Then**: 生成缺少demo的库模块列表
- **Verification**: `programmatic`

### AC-2: 为缺少demo的模块创建实际的demo文件
- **Given**: 缺少demo的库模块列表
- **When**: 为每个模块创建demo文件
- **Then**: 每个demo文件都实际调用库函数，不使用模拟实现
- **Verification**: `human-judgment`

### AC-3: 所有demo文件都能编译通过
- **Given**: 所有demo文件
- **When**: 编译所有demo文件
- **Then**: 所有demo文件都编译成功
- **Verification**: `programmatic`

### AC-4: 所有demo文件都能正常运行
- **Given**: 编译成功的demo文件
- **When**: 运行所有demo文件
- **Then**: 所有demo文件都能正常运行，输出预期结果
- **Verification**: `programmatic`

## Open Questions
- [ ] 如何确定哪些模块是核心模块，优先实现
- [ ] 如何处理可能存在的复杂依赖关系
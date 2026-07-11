import re
import json

# Read generated JS
with open('cloze_tests.js', 'r', encoding='utf-8') as f:
    js_content = f.read()

# Read expected answers
with open('答案.txt', 'r', encoding='utf-8') as f:
    ans_text = f.read()

# Parse expected answers
expected = {}  # {test_id: {section_key: {q_num: answer}}}
current_test = None
current_section = None

for line in ans_text.split('\n'):
    m = re.match(r'## 真题(\d+) 答案', line)
    if m:
        current_test = int(m.group(1))
        current_section = None
        if current_test not in expected:
            expected[current_test] = {}
        continue
    m = re.match(r'### \((\d+)\)', line)
    if m:
        current_section = m.group(1)
        if current_section not in expected[current_test]:
            expected[current_test][current_section] = {}
        continue
    m = re.match(r'\|\s*\(?(\d+)\)?\s*\|\s*([A-D])\s*\|', line)
    if m and current_test:
        num = int(m.group(1))
        ans = m.group(2)
        if current_section:
            expected[current_test][current_section][num] = ans
        else:
            if None not in expected[current_test]:
                expected[current_test][None] = {}
            expected[current_test][None][num] = ans

# Flatten expected into (test_id, q_num, section) -> answer
# Since q_nums can repeat across tests (e.g., both test 11 and test 12 have q1),
# we need to track by position in the JS array

# Extract all answers from the JS by finding each test block
# Strategy: find id: N, then find all answer: 'X' within that block

# Get positions of each test start
test_starts = [(m.start(), int(m.group(1)))
               for m in re.finditer(r'^\s+id: (\d+),', js_content, re.MULTILINE)]

# Find question answers within each test block
mismatches = []
for idx, (start_pos, test_id) in enumerate(test_starts):
    # Find end of this test block (start of next, or end of array)
    if idx + 1 < len(test_starts):
        end_pos = test_starts[idx + 1][0]
    else:
        end_pos = len(js_content)

    test_block = js_content[start_pos:end_pos]

    # Find all answers in this block
    gen_answers = re.findall(r"number: (\d+),.*?answer: '([A-D])'", test_block, re.DOTALL)

    # Compare with expected
    exp = expected.get(test_id, {})
    exp_flat = {}
    for sec_key, qs in exp.items():
        for q_num, ans in qs.items():
            exp_flat[q_num] = ans

    # Check each generated answer
    for q_num_str, gen_ans in gen_answers:
        q_num = int(q_num_str)
        exp_ans = exp_flat.get(q_num)
        if exp_ans is None:
            print(f"MISSING expected: Test {test_id}, Q{q_num} - generated {gen_ans}")
            mismatches.append((test_id, q_num, gen_ans, 'MISSING'))
        elif exp_ans != gen_ans:
            print(f"MISMATCH: Test {test_id}, Q{q_num} - expected {exp_ans}, got {gen_ans}")
            mismatches.append((test_id, q_num, gen_ans, exp_ans))

    # Check if any expected answers missing from generated
    gen_nums = set(int(n) for n, _ in gen_answers)
    exp_nums = set(exp_flat.keys())
    missing = exp_nums - gen_nums
    if missing:
        for q_num in missing:
            print(f"NOT GENERATED: Test {test_id}, Q{q_num} - expected {exp_flat[q_num]}")
            mismatches.append((test_id, q_num, 'NOT_GENERATED', exp_flat[q_num]))

if not mismatches:
    print("ALL ANSWERS MATCH CORRECTLY!")
else:
    print(f"\nTotal mismatches: {len(mismatches)}")

# Count total questions
total_qs = sum(1 for _ in re.finditer(r"number: (\d+),", js_content))
total_exp = sum(
    len(qs) for test_answers in expected.values()
    for sec_answers in test_answers.values()
    for qs in [sec_answers]
    if isinstance(sec_answers, dict)
)
# Better count
total_exp = 0
for test_id, secs in expected.items():
    for sec_key, qs in secs.items():
        total_exp += len(qs)

print(f"Total questions in generated: {total_qs}")
print(f"Total questions in expected: {total_exp}")

# Count sections per test
for test_id in range(1, 38):
    if test_id in expected:
        secs = expected[test_id]
        has_subs = any(k is not None for k in secs.keys())
        print(f"Test {test_id}: {len(secs)} sections" + (" (HAS SUBS)" if has_subs else ""))

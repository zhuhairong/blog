import re
import json

# Read the passage file
with open("短文.md", "r", encoding="utf-8") as f:
    passage_text = f.read()

# Read the answer file
with open("答案.txt", "r", encoding="utf-8") as f:
    answer_text = f.read()

# ========================================
# Parse answers first
# ========================================
answers = {}

current_test = None
current_section = None

for line in answer_text.split('\n'):
    m = re.match(r'## 真题(\d+) 答案', line)
    if m:
        current_test = int(m.group(1))
        current_section = None
        if current_test not in answers:
            answers[current_test] = {}
        continue

    m = re.match(r'### \((\d+)\)', line)
    if m:
        current_section = m.group(1)
        if current_section not in answers[current_test]:
            answers[current_test][current_section] = {}
        continue

    m = re.match(r'\|\s*\(?(\d+)\)?\s*\|\s*([A-D])\s*\|', line)
    if m and current_test:
        num = int(m.group(1))
        ans = m.group(2)
        if current_section:
            answers[current_test][current_section][num] = ans
        else:
            if None not in answers[current_test]:
                answers[current_test][None] = {}
            answers[current_test][None][num] = ans

print("Parsed answers for tests:", sorted(answers.keys()))

# ========================================
# Parse passages
# ========================================
passage_blocks = re.split(r'\n## 真题(\d+)\s*\n', passage_text)

tests = []
for i in range(1, len(passage_blocks), 2):
    test_id = int(passage_blocks[i])
    content = passage_blocks[i+1]

    lines = content.split('\n')

    title = None
    sections = []

    current_section_name = None
    current_section_lines = []
    in_header = True

    for line in lines:
        stripped = line.strip()

        if not stripped:
            if not in_header and current_section_lines:
                current_section_lines.append("")
            continue

        # Sub-section: ### (1) or ### (2)
        m_sub = re.match(r'### \((\d+)\)', stripped)
        if m_sub:
            if current_section_lines:
                sections.append((current_section_name, current_section_lines))
            current_section_name = m_sub.group(1)
            current_section_lines = []
            in_header = False
            continue

        # Title: ### Text (not (N))
        m_title = re.match(r'### (.+)', stripped)
        if m_title:
            title_text = m_title.group(1)
            if not re.match(r'\(\d+\)', title_text):
                title = title_text
                in_header = False
                continue

        # Skip instruction lines
        if in_header:
            if re.match(r'阅读|先阅读|先通读|通读|根据', stripped):
                continue
            in_header = False

        current_section_lines.append(line)

    if current_section_lines:
        sections.append((current_section_name, current_section_lines))

    test_entry = {
        "id": test_id,
        "title": title,
        "sections": []
    }

    answer_data = answers.get(test_id, {})

    for sec_name, sec_lines in sections:
        # Separate paragraph lines from question lines
        para_lines = []
        q_lines = []

        for line in sec_lines:
            stripped = line.strip()
            if re.match(r'\*\*(?:\(?\d+\)?)\.?\*\*\s+[A-D]\.', stripped):
                q_lines.append(stripped)
            elif stripped:
                para_lines.append(stripped)
            elif para_lines:
                para_lines.append("")

        # Build paragraph text
        # Join into one continuous text, preserving paragraph breaks with spaces
        full_text_parts = []
        current_buf = []
        for pl in para_lines:
            if pl == "":
                if current_buf:
                    full_text_parts.append(' '.join(current_buf))
                    current_buf = []
            else:
                current_buf.append(pl)
        if current_buf:
            full_text_parts.append(' '.join(current_buf))

        full_text = ' '.join(full_text_parts)

        # Replace blank markers
        full_text = re.sub(r'\*\*(\d+)\*\*', r'__BLANK_\1__', full_text)
        full_text = re.sub(r'\*\*\((\d+)\)\*\*', r'__BLANK_\1__', full_text)

        test_entry["sections"].append({
            "type": "paragraph",
            "text": full_text.strip()
        })

        # Parse question lines
        items = []
        for ql in q_lines:
            # Extract question number
            m_num = re.match(r'\*\*(?:\(?(\d+)\)?)?\.?\*\*\s+', ql)
            if not m_num:
                continue
            q_num = int(m_num.group(1))
            options_str = ql[m_num.end():]

            # Parse options: split by spaces before A. B. C. D.
            # Strategy: split by whitespace followed by capital letter and dot
            parts = re.split(r'\s+(?=[A-D]\.)', options_str)
            opts = {}
            for part in parts:
                m_opt = re.match(r'([A-D])\.\s*(.+)', part)
                if m_opt:
                    opts[m_opt.group(1)] = m_opt.group(2).strip()

            # Get answer
            sec_answers = answer_data.get(sec_name, answer_data.get(None, {}))
            ans = sec_answers.get(q_num, '?')

            items.append({
                "number": q_num,
                "options": opts,
                "answer": ans
            })

        if items:
            test_entry["sections"].append({
                "type": "question_block",
                "items": items
            })

    tests.append(test_entry)

print(f"Parsed {len(tests)} tests")

# ========================================
# Write JavaScript output
# ========================================
def js_string(s):
    s = s.replace('\\', '\\\\')
    s = s.replace("'", "\\'")
    s = s.replace('\n', ' ')
    return s

def format_js(obj, indent=0):
    sp = '  ' * indent
    sp1 = '  ' * (indent + 1)
    sp2 = '  ' * (indent + 2)
    sp3 = '  ' * (indent + 3)

    if isinstance(obj, str):
        return "'" + js_string(obj) + "'"
    elif isinstance(obj, bool):
        return 'true' if obj else 'false'
    elif isinstance(obj, (int, float)):
        return str(obj)
    elif obj is None:
        return 'null'
    elif isinstance(obj, list):
        if not obj:
            return '[]'
        items = [sp1 + format_js(item, indent + 1) for item in obj]
        return '[\n' + ',\n'.join(items) + '\n' + sp + ']'
    elif isinstance(obj, dict):
        if not obj:
            return '{}'
        items = []
        for k, v in obj.items():
            key_str = "'" + js_string(str(k)) + "'" if not k.isidentifier() else k
            val_str = format_js(v, indent + 1)
            # Strip leading spaces from val_str on first line
            lines = val_str.split('\n')
            if len(lines) == 1:
                items.append(sp1 + key_str + ': ' + lines[0])
            else:
                items.append(sp1 + key_str + ': ' + '\n'.join(lines))
        return '{\n' + ',\n'.join(items) + '\n' + sp + '}'

js_lines = ['const CLOZE_TESTS = [']
for i, test in enumerate(tests):
    test_js = format_js(test, 1)
    if i < len(tests) - 1:
        js_lines.append(test_js + ',')
    else:
        js_lines.append(test_js)
js_lines.append('];')
js_lines.append('')
js_lines.append("if (typeof module !== 'undefined' && module.exports) {")
js_lines.append("  module.exports = { CLOZE_TESTS };")
js_lines.append("}")

output = '\n'.join(js_lines)

with open("cloze_tests.js", "w", encoding="utf-8") as f:
    f.write(output)

print(f"Written cloze_tests.js with {len(tests)} tests")
print(f"File size: {len(output)} bytes")

# Verify a few tests
for t in tests[:3]:
    title = t.get('title', 'NO TITLE')
    sec_count = len(t['sections'])
    total_q = sum(len(s.get('items', [])) for s in t['sections'] if s['type'] == 'question_block')
    print(f"  Test {t['id']}: '{title}' - {sec_count} sections, {total_q} questions")
    # Print first question of each test to verify options
    for s in t['sections']:
        if s['type'] == 'question_block' and s['items']:
            q = s['items'][0]
            print(f"    Q{q['number']}: {q['options']} -> {q['answer']}")
            break

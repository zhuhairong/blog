import json

for f in ['grade7a.json','grade7b.json','grade8a.json','grade8b.json','grade9.json']:
    d = json.load(open(f, encoding='utf-8'))
    total = sum(len(u['words']) for u in d['units'])
    print(f'{f}: {total} words in {len(d["units"])} units')

d = json.load(open('confusingWords.json', encoding='utf-8'))
print(f'confusingWords: {len(d)} headwords')

# Total
total_all = 0
for f in ['grade7a.json','grade7b.json','grade8a.json','grade8b.json','grade9.json']:
    d = json.load(open(f, encoding='utf-8'))
    total_all += sum(len(u['words']) for u in d['units'])
print(f'Total words: {total_all}')

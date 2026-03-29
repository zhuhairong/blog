/**
 * 英语语法学习网页 - 题库数据
 */

const QuestionBank = {
  // 现在完成时
  'present-perfect': [
    {
      type: 'choice',
      question: 'I _____ already _____ my homework.',
      options: ['have; finished', 'has; finished', 'had; finish', 'have; finish'],
      answer: 0,
      explanation: '现在完成时的构成是 have/has + 过去分词。主语 I 用 have，finish 的过去分词是 finished。'
    },
    {
      type: 'choice',
      question: 'She _____ to Beijing twice.',
      options: ['has gone', 'has been', 'have been', 'went'],
      answer: 1,
      explanation: 'has been to 表示"去过某地（已回来）"；has gone to 表示"去了某地（还没回来）"。这里说她去过两次，说明已回来，用 has been。'
    },
    {
      type: 'fill-blank',
      question: 'They _____ (live) here for ten years.',
      answer: 'have lived',
      explanation: 'for ten years 表示一段时间，用现在完成时。主语 They 用 have，live 的过去分词是 lived。'
    },
    {
      type: 'fill-blank',
      question: 'He _____ (not finish) his work yet.',
      answer: 'has not finished',
      explanation: 'yet 常用于否定句，表示"还没有"。主语 He 用 has，否定形式为 has not + 过去分词。'
    },
    {
      type: 'judge',
      question: '"I have bought this book yesterday." 这个句子是正确的。',
      answer: false,
      explanation: '现在完成时不能与具体过去时间连用。yesterday 是具体过去时间，应该用一般过去时："I bought this book yesterday."'
    },
    {
      type: 'choice',
      question: 'We have known each other _____ 2010.',
      options: ['since', 'for', 'in', 'at'],
      answer: 0,
      explanation: 'since + 时间点（具体时间）；for + 时间段（一段时间）。2010 是时间点，用 since。'
    },
    {
      type: 'word-order',
      question: 'I have already read this book',
      words: ['I', 'have', 'already', 'read', 'this', 'book'],
      answer: 'I have already read this book',
      explanation: 'already 放在 have 和过去分词之间。句子顺序：主语 + have/has + already + 过去分词 + 其他。'
    },
    {
      type: 'choice',
      question: '_____ you ever _____ to Japan?',
      options: ['Have; been', 'Has; been', 'Have; gone', 'Did; go'],
      answer: 0,
      explanation: 'ever 常用于现在完成时的疑问句。主语 you 用 Have，been to 表示去过。'
    },
    {
      type: 'fill-blank',
      question: 'The train _____ (leave). We have to wait for the next one.',
      answer: 'has left',
      explanation: '火车已经离开了，对现在造成影响（我们要等下一趟），用现在完成时。'
    },
    {
      type: 'judge',
      question: '"have gone to" 和 "have been to" 的意思相同。',
      answer: false,
      explanation: 'have gone to 表示"去了还没回来"；have been to 表示"去过已经回来"。两者意思不同。'
    },
    {
      type: 'choice',
      question: 'He _____ the army for three years.',
      options: ['has been in', 'has joined', 'joined', 'has joined in'],
      answer: 0,
      explanation: 'join 是瞬间动词，不能与一段时间连用。需转换为延续性动词 be in。'
    },
    {
      type: 'fill-blank',
      question: 'His grandmother _____ (die) for five years.',
      answer: 'has been dead',
      explanation: 'die 是瞬间动词，不能与一段时间连用。需转换为延续性状态 be dead。'
    },
    {
      type: 'choice',
      question: 'I have _____ this book for two weeks.',
      options: ['kept', 'borrowed', 'lend', 'keep'],
      answer: 0,
      explanation: 'borrow 是瞬间动词，不能与一段时间连用。需转换为延续性动词 keep（kept 是过去分词）。'
    },
    {
      type: 'judge',
      question: '"He has arrived here for two hours." 是正确的。',
      answer: false,
      explanation: 'arrive 是瞬间动词，不能与一段时间连用。应该说：He has been here for two hours.'
    },
    {
      type: 'choice',
      question: 'The film has _____ for ten minutes.',
      options: ['been on', 'started', 'begun', 'start'],
      answer: 0,
      explanation: 'start/begin 是瞬间动词，表示电影"开始"持续一段时间要用 be on。'
    }
  ],

  // 过去进行时
  'past-continuous': [
    {
      type: 'choice',
      question: 'I _____ TV when my mother came home.',
      options: ['was watching', 'were watching', 'watched', 'am watching'],
      answer: 0,
      explanation: '过去进行时表示过去某一时刻正在进行的动作。主语 I 用 was，watching 表示正在看电视。'
    },
    {
      type: 'choice',
      question: 'They _____ basketball at 5 o\'clock yesterday.',
      options: ['were playing', 'was playing', 'played', 'play'],
      answer: 0,
      explanation: '主语 They 是复数，用 were。at 5 o\'clock yesterday 表示过去某时刻，用过去进行时。'
    },
    {
      type: 'fill-blank',
      question: 'What _____ you _____ (do) at eight last night?',
      answer: 'were; doing',
      explanation: '主语 you，过去进行时疑问句结构：Were/Was + 主语 + doing?'
    },
    {
      type: 'fill-blank',
      question: 'She _____ (not sleep) when I called her.',
      answer: 'was not sleeping',
      explanation: '主语 She 用 was，否定形式 was not + doing。当我打电话时她没在睡觉。'
    },
    {
      type: 'judge',
      question: '过去进行时可以单独使用，不需要时间状语。',
      answer: false,
      explanation: '过去进行时通常需要时间状语或上下文来明确"什么时间正在做什么"。'
    },
    {
      type: 'word-order',
      question: 'He was reading a book at that time',
      words: ['He', 'was', 'reading', 'a', 'book', 'at', 'that', 'time'],
      answer: 'He was reading a book at that time',
      explanation: '过去进行时结构：主语 + was/were + doing + 时间状语。'
    },
    {
      type: 'choice',
      question: 'While I _____ dinner, the phone rang.',
      options: ['was cooking', 'cooked', 'were cooking', 'cook'],
      answer: 0,
      explanation: 'while 引导的时间状语从句常用过去进行时，表示"当...正在...的时候"。主语 I 用 was。'
    },
    {
      type: 'choice',
      question: 'The students _____ when the teacher entered the classroom.',
      options: ['were talking', 'was talking', 'talked', 'talk'],
      answer: 0,
      explanation: '主语 students 是复数，用 were talking，表示老师进来时他们正在说话。'
    },
    {
      type: 'fill-blank',
      question: 'It _____ (rain) heavily when we went out.',
      answer: 'was raining',
      explanation: '当我们出去时，雨正在下得很大。用过去进行时描述当时正在发生的事。'
    },
    {
      type: 'judge',
      question: '"I was doing my homework yesterday." 这个句子语法正确。',
      answer: true,
      explanation: '这个句子是正确的。过去进行时可以与 yesterday 连用，表示昨天某段时间正在做作业。'
    }
  ],

  // 情态动词
  'modal-verbs': [
    {
      type: 'choice',
      question: '_____ I use your phone?',
      options: ['Can', 'Must', 'Should', 'Would'],
      answer: 0,
      explanation: 'Can 用于请求许可，"Can I...?" 表示"我可以...吗？"是最常用的礼貌请求方式。'
    },
    {
      type: 'choice',
      question: 'You _____ finish your homework before watching TV.',
      options: ['must', 'can', 'may', 'could'],
      answer: 0,
      explanation: 'must 表示强制性的"必须"，这里表示规则要求必须先完成作业才能看电视。'
    },
    {
      type: 'fill-blank',
      question: 'He _____ (can) swim when he was five years old.',
      answer: 'could',
      explanation: 'can 的过去式是 could。当他五岁时他就能游泳了，用过去式。'
    },
    {
      type: 'choice',
      question: '_____ you help me with this box?',
      options: ['Could', 'Must', 'Should', 'Shall'],
      answer: 0,
      explanation: 'Could 用于礼貌地请求帮助，比 Can 更委婉礼貌。'
    },
    {
      type: 'judge',
      question: '"must" 和 "have to" 意思完全相同。',
      answer: false,
      explanation: 'must 强调主观意愿"必须"；have to 强调客观需要"不得不"。意思相近但有区别。'
    },
    {
      type: 'choice',
      question: 'You _____ take a break. You look tired.',
      options: ['should', 'must', 'can', 'may'],
      answer: 0,
      explanation: 'should 表示建议"应该"，这里建议对方休息一下。'
    },
    {
      type: 'word-order',
      question: 'You should not stay up late',
      words: ['You', 'should', 'not', 'stay', 'up', 'late'],
      answer: 'You should not stay up late',
      explanation: '情态动词否定形式：情态动词 + not + 动词原形。'
    },
    {
      type: 'fill-blank',
      question: 'It _____ (may) rain tomorrow. Take an umbrella.',
      answer: 'may',
      explanation: 'may 表示可能性"可能"，明天可能下雨，带把伞。'
    },
    {
      type: 'choice',
      question: 'She _____ be at home. I saw her at the supermarket just now.',
      options: ['can\'t', 'mustn\'t', 'shouldn\'t', 'may not'],
      answer: 0,
      explanation: 'can\'t 表示否定推测"不可能"，我刚才在超市看到她，所以她不可能在家。'
    },
    {
      type: 'judge',
      question: '情态动词后面接动词原形。',
      answer: true,
      explanation: '正确。情态动词（can, may, must, should 等）后面都接动词原形，不加 to。'
    },
    {
      type: 'choice',
      question: 'You _____ smoke here. It\'s forbidden.',
      options: ['mustn\'t', 'needn\'t', 'can\'t', 'may not'],
      answer: 0,
      explanation: 'mustn\'t 表示"禁止"，这里禁止吸烟。needn\'t 表示"不必"。'
    },
    {
      type: 'choice',
      question: 'You _____ come early. The meeting starts at 9.',
      options: ['needn\'t', 'mustn\'t', 'can\'t', 'shouldn\'t'],
      answer: 0,
      explanation: 'needn\'t 表示"不必"，会议9点开始，不必早来。'
    },
    {
      type: 'choice',
      question: '_____ we go out for dinner tonight?',
      options: ['Shall', 'Will', 'Must', 'Need'],
      answer: 0,
      explanation: 'Shall 用于第一人称表示建议，"Shall we...?" 表示"我们...好吗？"'
    },
    {
      type: 'choice',
      question: '_____ I open the window? It\'s hot in here.',
      options: ['Shall', 'Must', 'Need', 'Would'],
      answer: 0,
      explanation: 'Shall I...? 用于主动提供帮助或征求意见，"我...好吗？"'
    },
    {
      type: 'judge',
      question: '"You mustn\'t worry" 和 "You needn\'t worry" 意思相同。',
      answer: false,
      explanation: 'mustn\'t worry = 禁止担心；needn\'t worry = 不必担心。两者意思不同。'
    }
  ],

  // 被动语态
  'passive-voice': [
    {
      type: 'choice',
      question: 'The book _____ by many students.',
      options: ['is read', 'reads', 'is reading', 'read'],
      answer: 0,
      explanation: '被动语态结构：be + 过去分词。书被学生阅读，主语 book 用 is，read 的过去分词是 read。'
    },
    {
      type: 'choice',
      question: 'The bridge _____ last year.',
      options: ['was built', 'is built', 'built', 'has built'],
      answer: 0,
      explanation: 'last year 表示过去时间，用一般过去时的被动语态 was/were + 过去分词。'
    },
    {
      type: 'fill-blank',
      question: 'English _____ (speak) in many countries.',
      answer: 'is spoken',
      explanation: '英语在许多国家被使用。一般现在时被动语态：is/are + 过去分词。'
    },
    {
      type: 'fill-blank',
      question: 'The letter _____ (write) by him yesterday.',
      answer: 'was written',
      explanation: 'yesterday 表示过去，用一般过去时被动语态。write 的过去分词是 written。'
    },
    {
      type: 'judge',
      question: '被动语态可以省略动作的执行者。',
      answer: true,
      explanation: '正确。当动作执行者不重要或未知时，可以省略 by + 执行者。'
    },
    {
      type: 'choice',
      question: 'Many trees _____ in our school every year.',
      options: ['are planted', 'is planted', 'planted', 'were planted'],
      answer: 0,
      explanation: 'every year 表示经常性动作，用一般现在时被动语态。主语 trees 是复数，用 are。'
    },
    {
      type: 'word-order',
      question: 'The work was finished by them',
      words: ['The', 'work', 'was', 'finished', 'by', 'them'],
      answer: 'The work was finished by them',
      explanation: '被动语态结构：主语 + be + 过去分词 + by + 执行者。'
    },
    {
      type: 'choice',
      question: 'A new hospital _____ in our city next year.',
      options: ['will be built', 'is built', 'was built', 'builds'],
      answer: 0,
      explanation: 'next year 表示将来，用一般将来时被动语态：will be + 过去分词。'
    },
    {
      type: 'fill-blank',
      question: 'The window _____ (break) by the wind.',
      answer: 'was broken',
      explanation: '窗户被风打破了。一般过去时被动语态，break 的过去分词是 broken。'
    },
    {
      type: 'judge',
      question: '所有动词都可以变成被动语态。',
      answer: false,
      explanation: '错误。不及物动词（如 happen, die, arrive 等）没有被动语态，因为它们没有宾语。'
    },
    {
      type: 'choice',
      question: 'The room must _____ every day.',
      options: ['be cleaned', 'clean', 'is cleaned', 'cleaned'],
      answer: 0,
      explanation: '含情态动词的被动语态：情态动词 + be + 过去分词。'
    },
    {
      type: 'fill-blank',
      question: 'The homework should _____ (finish) before dinner.',
      answer: 'be finished',
      explanation: '含情态动词 should 的被动语态：should + be + 过去分词。'
    },
    {
      type: 'choice',
      question: 'The book can _____ in the library.',
      options: ['be found', 'find', 'is found', 'found'],
      answer: 0,
      explanation: '含情态动词 can 的被动语态：can + be + 过去分词。'
    },
    {
      type: 'judge',
      question: '短语动词的被动语态中，介词不能省略。',
      answer: true,
      explanation: '正确。短语动词如 look after, turn off 等在被动语态中是一个整体，介词不能省略。如：The children are looked after.'
    },
    {
      type: 'choice',
      question: 'I was _____ a new bike by my father.',
      options: ['given', 'gave', 'give', 'gives'],
      answer: 0,
      explanation: '双宾语动词 give 的被动语态：I was given a new bike（我收到了一辆新自行车）。'
    }
  ],

  // 比较级最高级
  'comparison': [
    {
      type: 'choice',
      question: 'Tom is _____ than Jack.',
      options: ['taller', 'tallest', 'more tall', 'most tall'],
      answer: 0,
      explanation: '形容词比较级：单音节词加 -er。tall 是单音节词，比较级是 taller。'
    },
    {
      type: 'choice',
      question: 'This is the _____ book I have ever read.',
      options: ['most interesting', 'more interesting', 'interesting', 'interested'],
      answer: 0,
      explanation: '最高级：多音节形容词用 most + 形容词。interesting 是多音节词。'
    },
    {
      type: 'fill-blank',
      question: 'This question is _____ (easy) than that one.',
      answer: 'easier',
      explanation: 'easy 以 y 结尾，变比较级时把 y 变成 i 加 -er：easier。'
    },
    {
      type: 'fill-blank',
      question: 'She is the _____ (beautiful) girl in our class.',
      answer: 'most beautiful',
      explanation: 'beautiful 是多音节词，最高级用 most + beautiful。'
    },
    {
      type: 'judge',
      question: '"good" 的比较级是 "gooder"。',
      answer: false,
      explanation: '错误。good 是不规则变化，比较级是 better，最高级是 best。'
    },
    {
      type: 'choice',
      question: 'He runs _____ than his brother.',
      options: ['faster', 'more fast', 'fastest', 'most fast'],
      answer: 0,
      explanation: '副词比较级：单音节词加 -er。fast 是单音节词，比较级是 faster。'
    },
    {
      type: 'word-order',
      question: 'She is taller than me',
      words: ['She', 'is', 'taller', 'than', 'me'],
      answer: 'She is taller than me',
      explanation: '比较句结构：主语 + be + 比较级 + than + 比较对象。'
    },
    {
      type: 'choice',
      question: 'China is one of the _____ countries in the world.',
      options: ['largest', 'larger', 'large', 'most large'],
      answer: 0,
      explanation: 'one of the + 最高级 + 名词复数，表示"最...之一"。large 最高级是 largest。'
    },
    {
      type: 'fill-blank',
      question: 'The weather is getting _____ and _____ (cold).',
      answer: 'colder; colder',
      explanation: '"越来越..." 用比较级 + and + 比较级。cold 的比较级是 colder。'
    },
    {
      type: 'judge',
      question: '"bad" 的最高级是 "worst"。',
      answer: true,
      explanation: '正确。bad 是不规则变化：比较级 worse，最高级 worst。'
    },
    {
      type: 'choice',
      question: 'This book is _____ that one.',
      options: ['as interesting as', 'as interesting so', 'so interesting as', 'interesting as'],
      answer: 0,
      explanation: 'as...as 结构表示"和...一样..."，中间用形容词原级。'
    },
    {
      type: 'judge',
      question: '"He is as taller as me." 这个句子是正确的。',
      answer: false,
      explanation: '错误。as...as 中间只能用原级，不能用比较级。正确：He is as tall as me.'
    },
    {
      type: 'choice',
      question: 'This problem is _____ more difficult than that one.',
      options: ['much', 'very', 'quite', 'too'],
      answer: 0,
      explanation: 'much 可以修饰比较级表示"...得多"。very, quite, too 不能修饰比较级。'
    },
    {
      type: 'fill-blank',
      question: 'He is the _____ (tall) of the two boys.',
      answer: 'taller',
      explanation: '"the + 比较级 + of the two" 表示"两者中较...的一个"。'
    },
    {
      type: 'choice',
      question: 'This film is not _____ that one.',
      options: ['as good as', 'so good than', 'as good so', 'good as'],
      answer: 0,
      explanation: 'not as/so...as 表示"不如..."。否定形式可用 not as...as 或 not so...as。'
    }
  ],

  // 不定式和动名词
  'infinitive-gerund': [
    {
      type: 'choice',
      question: 'I want _____ a doctor.',
      options: ['to be', 'being', 'be', 'to being'],
      answer: 0,
      explanation: 'want 后接不定式：want to do something。want to be a doctor 想成为医生。'
    },
    {
      type: 'choice',
      question: 'She enjoys _____ music.',
      options: ['listening to', 'to listen to', 'listen to', 'listened to'],
      answer: 0,
      explanation: 'enjoy 后接动名词：enjoy doing something。enjoy listening to music 喜欢听音乐。'
    },
    {
      type: 'fill-blank',
      question: 'It\'s time _____ (go) to school.',
      answer: 'to go',
      explanation: 'It\'s time to do something 是固定句型，表示"该做某事了"。'
    },
    {
      type: 'fill-blank',
      question: 'He is good at _____ (play) basketball.',
      answer: 'playing',
      explanation: 'be good at 后接动名词，at 是介词，介词后接动词要用动名词形式。'
    },
    {
      type: 'judge',
      question: '"stop to do" 和 "stop doing" 意思相同。',
      answer: false,
      explanation: '错误。stop to do 表示"停下来去做另一件事"；stop doing 表示"停止正在做的事"。'
    },
    {
      type: 'choice',
      question: 'Please remember _____ the door when you leave.',
      options: ['to lock', 'locking', 'lock', 'locked'],
      answer: 0,
      explanation: 'remember to do 表示"记得要去做"；remember doing 表示"记得做过"。离开时记得锁门，是要去做，用 to lock。'
    },
    {
      type: 'word-order',
      question: 'I like reading books',
      words: ['I', 'like', 'reading', 'books'],
      answer: 'I like reading books',
      explanation: 'like 后可接动名词或不定式。like reading books 表示喜欢读书这个习惯。'
    },
    {
      type: 'choice',
      question: 'They decided _____ to Beijing for vacation.',
      options: ['to go', 'going', 'go', 'to going'],
      answer: 0,
      explanation: 'decide 后接不定式：decide to do something。决定去北京度假。'
    },
    {
      type: 'fill-blank',
      question: 'Thank you for _____ (help) me.',
      answer: 'helping',
      explanation: 'for 是介词，介词后接动词用动名词形式。thank you for helping me 感谢你帮助我。'
    },
    {
      type: 'judge',
      question: '"see" 后面可以接不定式。',
      answer: true,
      explanation: '正确。see 后可接不定式（see sb do sth 看见某人做了某事）或动名词（see sb doing sth 看见某人正在做某事）。'
    },
    {
      type: 'choice',
      question: 'I don\'t know _____ next.',
      options: ['what to do', 'how to do', 'to do what', 'do what'],
      answer: 0,
      explanation: '疑问词 + 不定式结构。what to do 表示"做什么"。'
    },
    {
      type: 'fill-blank',
      question: 'Can you tell me how _____ (get) to the station?',
      answer: 'to get',
      explanation: '疑问词 + 不定式结构。how to get 表示"如何到达"。'
    },
    {
      type: 'choice',
      question: 'My mother made me _____ my homework.',
      options: ['do', 'to do', 'doing', 'did'],
      answer: 0,
      explanation: '使役动词 make 后接省略 to 的不定式：make sb do sth。'
    },
    {
      type: 'judge',
      question: '被动语态中，使役动词后要还原 to。',
      answer: true,
      explanation: '正确。主动：He made me do it. 被动：I was made to do it. 被动语态要还原 to。'
    },
    {
      type: 'choice',
      question: 'I saw him _____ the room.',
      options: ['enter', 'to enter', 'entered', 'enters'],
      answer: 0,
      explanation: '感官动词 see 后接省略 to 的不定式：see sb do sth（看见某人做了某事）。'
    }
  ],

  // 连词和状语从句
  'conjunctions': [
    {
      type: 'choice',
      question: 'I will call you _____ I arrive.',
      options: ['when', 'if', 'because', 'although'],
      answer: 0,
      explanation: 'when 引导时间状语从句，表示"当...的时候"。我到达时会给你打电话。'
    },
    {
      type: 'choice',
      question: 'He didn\'t go to school _____ he was ill.',
      options: ['because', 'if', 'when', 'so'],
      answer: 0,
      explanation: 'because 引导原因状语从句，表示"因为"。他因为生病没去上学。'
    },
    {
      type: 'fill-blank',
      question: '_____ it rains tomorrow, we won\'t go hiking.',
      answer: 'If',
      explanation: 'If 引导条件状语从句，表示"如果"。如果明天下雨，我们就不去徒步了。'
    },
    {
      type: 'fill-blank',
      question: 'He studied hard _____ (so that) he could pass the exam.',
      answer: 'so that',
      explanation: 'so that 引导目的状语从句，表示"为了"。他努力学习是为了通过考试。'
    },
    {
      type: 'judge',
      question: '"although" 和 "but" 可以在同一个句子中同时使用。',
      answer: false,
      explanation: '错误。although 和 but 不能同时使用，中文说"虽然...但是..."，英语只用 although 或 but 其中一个。'
    },
    {
      type: 'choice',
      question: 'She was late _____ the traffic was heavy.',
      options: ['because', 'so', 'although', 'if'],
      answer: 0,
      explanation: 'because 表示原因。她迟到是因为交通拥堵。'
    },
    {
      type: 'word-order',
      question: 'If you study hard you will pass the exam',
      words: ['If', 'you', 'study', 'hard', 'you', 'will', 'pass', 'the', 'exam'],
      answer: 'If you study hard you will pass the exam',
      explanation: '条件状语从句：If + 从句，主句。从句用一般现在时，主句用一般将来时。'
    },
    {
      type: 'choice',
      question: 'I have been learning English _____ I was ten.',
      options: ['since', 'for', 'when', 'while'],
      answer: 0,
      explanation: 'since 引导时间状语从句，表示"自从"。我从十岁开始学习英语。'
    },
    {
      type: 'fill-blank',
      question: 'He ran fast _____ (so that) he could catch the bus.',
      answer: 'so that',
      explanation: 'so that 引导目的状语从句。他跑得快是为了赶上公交车。'
    },
    {
      type: 'judge',
      question: '"while" 可以表示两个动作同时进行。',
      answer: true,
      explanation: '正确。while 表示"当...时"，常用于描述两个动作同时进行。如：While I was reading, he was watching TV.'
    },
    {
      type: 'choice',
      question: 'I will tell him the news _____ he comes back.',
      options: ['as soon as', 'until', 'while', 'before'],
      answer: 0,
      explanation: 'as soon as 表示"一...就..."，引导时间状语从句。'
    },
    {
      type: 'choice',
      question: 'The box is _____ heavy _____ I can\'t carry it.',
      options: ['so; that', 'such; that', 'too; to', 'very; that'],
      answer: 0,
      explanation: 'so + 形容词 + that 引导结果状语从句，表示"如此...以至于..."。'
    },
    {
      type: 'fill-blank',
      question: 'It was _____ (such) a good movie that I watched it twice.',
      answer: 'such',
      explanation: 'such + a/an + 形容词 + 名词 + that 表示"如此...以至于..."。'
    },
    {
      type: 'judge',
      question: '"because" 和 "so" 不能同时使用。',
      answer: true,
      explanation: '正确。英语中 because 和 so 不能同时使用，只能用其中一个。'
    },
    {
      type: 'choice',
      question: 'Don\'t get off the bus _____ it stops.',
      options: ['until', 'when', 'while', 'as'],
      answer: 0,
      explanation: 'not...until 表示"直到...才..."。不要在公交车停下之前下车。'
    }
  ]
};

// 获取指定模块的题目
function getQuestions(moduleId) {
  return QuestionBank[moduleId] || [];
}

// 获取随机题目
function getRandomQuestions(moduleId, count = 10) {
  const questions = getQuestions(moduleId);
  const shuffled = [...questions].sort(() => Math.random() - 0.5);
  return shuffled.slice(0, count);
}

// 导出题库函数
window.QuestionBank = QuestionBank;
window.getQuestions = getQuestions;
window.getRandomQuestions = getRandomQuestions;
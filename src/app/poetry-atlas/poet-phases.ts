/**
 * 诗人人生分期（客户端安全）。
 *
 * 从诗人页抽出来，因为「一生行迹」地图（客户端组件）也要按分期着色、
 * 显示阶段标签。数据源只有这一处，避免页面与地图各写一套而漂移。
 *
 * ⚠️ 分期**因诗人而异**：李白是「长安三年」，杜甫是「困居长安十年」，
 * 套用同一套分期会把两人的生命形状都磨平。所以每位诗人单独定义，
 * 且只在此处维护。
 */

export interface Phase {
  key: string;
  label: string;
  /** 起始年（含） */
  from: number;
  /** 结束年（含） */
  to: number;
}

/**
 * 逐诗人分期。边界依各自年谱的关键转折设定，不追求等长。
 * 未收录的诗人由 `phasesOf` 按其生卒年自动三等分兜底。
 */
export const PHASES_BY_POET: Record<string, Phase[]> = {
  libai: [
    { key: 'shu', label: '蜀中成长', from: -Infinity, to: 723 },
    { key: 'chu-you', label: '出蜀漫游', from: 724, to: 741 },
    { key: 'changan', label: '长安三年', from: 742, to: 744 },
    { key: 'donglu', label: '梁宋东鲁', from: 745, to: 754 },
    { key: 'luanli', label: '安史乱中', from: 755, to: 759 },
    { key: 'wan', label: '晚年漂泊', from: 760, to: Infinity },
  ],
  dufu: [
    { key: 'shu', label: '早慧与吴越', from: -Infinity, to: 734 },
    { key: 'qizhao', label: '齐赵壮游', from: 735, to: 745 },
    { key: 'changan', label: '困居长安十年', from: 746, to: 755 },
    { key: 'luanli', label: '安史乱中陷贼', from: 755, to: 759 },
    { key: 'shu-zhong', label: '客蜀草堂', from: 760, to: 765 },
    { key: 'kuizhou', label: '夔州巅峰', from: 766, to: 767 },
    { key: 'wan', label: '湖湘漂泊', from: 768, to: Infinity },
  ],
  wangwei: [
    { key: 'hezhong', label: '蒲州少年', from: -Infinity, to: 720 },
    { key: 'chu-shi', label: '初仕与贬谪', from: 721, to: 733 },
    { key: 'zhangjiuling', label: '张九龄时期', from: 734, to: 739 },
    { key: 'wangchuan', label: '辋川半隐', from: 740, to: 755 },
    { key: 'xianzei', label: '陷贼与责授', from: 756, to: 757 },
    { key: 'wan', label: '晚年尚书右丞', from: 758, to: Infinity },
  ],
  menghaoran: [
    { key: 'lumen', label: '鹿门隐居', from: -Infinity, to: 727 },
    { key: 'jing-shi', label: '游京师与吴越', from: 728, to: 731 },
    { key: 'huan-xiang', label: '还襄阳', from: 732, to: 736 },
    { key: 'jingzhou', label: '荆州幕府', from: 737, to: 738 },
    { key: 'wan', label: '归隐与终老', from: 739, to: Infinity },
  ],
  baijuyi: [
    { key: 'shaonian', label: '少年避乱', from: -Infinity, to: 799 },
    { key: 'dengdi', label: '登第与校书郎', from: 800, to: 805 },
    { key: 'jian-guan', label: '谏官与翰林', from: 806, to: 814 },
    { key: 'jiangzhou', label: '江州司马', from: 815, to: 817 },
    { key: 'zhongzhou', label: '忠州与回朝', from: 818, to: 821 },
    { key: 'hangsu', label: '杭苏刺史', from: 822, to: 826 },
    { key: 'wan', label: '洛阳闲居', from: 827, to: Infinity },
  ],
  censen: [
    { key: 'shu', label: '江陵与嵩阳', from: -Infinity, to: 743 },
    { key: 'chushi', label: '初仕长安', from: 744, to: 748 },
    { key: 'chusai', label: '两度出塞', from: 749, to: 756 },
    { key: 'changan', label: '长安为官', from: 757, to: 765 },
    { key: 'wan', label: '嘉州与罢官', from: 766, to: Infinity },
  ],
  sushi: [
    { key: 'meishan', label: '眉山与汴京', from: -Infinity, to: 1068 },
    { key: 'wairen', label: '熙宁外任', from: 1069, to: 1079 },
    { key: 'huangzhou', label: '黄州贬谪', from: 1080, to: 1084 },
    { key: 'yuanyou', label: '元祐回朝', from: 1085, to: 1093 },
    { key: 'huizhou', label: '惠州儋州', from: 1094, to: 1100 },
    { key: 'wan', label: '北归与卒', from: 1101, to: Infinity },
  ],
  lishangyin: [
    { key: 'guer', label: '幼孤力学', from: -Infinity, to: 828 },
    { key: 'linghu', label: '令狐知遇与登第', from: 829, to: 837 },
    { key: 'jingyuan', label: '入泾原幕·娶王氏', from: 838, to: 841 },
    { key: 'dingyou', label: '丁忧与党争', from: 842, to: 846 },
    { key: 'yuanmu', label: '远幕桂徐', from: 847, to: 850 },
    { key: 'dongchuan', label: '东川五年', from: 851, to: 855 },
    { key: 'wan', label: '还京与晚岁', from: 856, to: Infinity },
  ],
  dumu: [
    { key: 'mendi', label: '长安门第', from: -Infinity, to: 827 },
    { key: 'dengdi', label: '及第与幕游', from: 828, to: 832 },
    { key: 'yangzhou', label: '宣幕与扬州', from: 833, to: 835 },
    { key: 'dongdu', label: '东都分司', from: 836, to: 841 },
    { key: 'huangzhou', label: '刺黄州', from: 842, to: 844 },
    { key: 'chizhou', label: '刺池州', from: 845, to: 846 },
    { key: 'muzhou', label: '刺睦州', from: 847, to: 849 },
    { key: 'wan', label: '刺湖州与中书舍人', from: 850, to: Infinity },
  ],
  luyou: [
    { key: 'luanli', label: '乱离与力学', from: -Infinity, to: 1154 },
    { key: 'chushi', label: '初仕与罢黜', from: 1155, to: 1169 },
    { key: 'rushu', label: '入蜀与南郑从军', from: 1170, to: 1173 },
    { key: 'shuzhong', label: '宦游蜀中', from: 1174, to: 1178 },
    { key: 'donggui', label: '东归起落', from: 1179, to: 1188 },
    { key: 'yanzhou', label: '知严州与在朝', from: 1189, to: 1191 },
    { key: 'shanyin', label: '山阴闲居二十年', from: 1192, to: Infinity },
  ],
  liqingzhao: [
    { key: 'shaonv', label: '章丘少女', from: -Infinity, to: 1100 },
    { key: 'bianjing', label: '汴京新婚', from: 1101, to: 1106 },
    { key: 'qingzhou', label: '屏居青州', from: 1107, to: 1126 },
    { key: 'nandu', label: '靖康南渡', from: 1127, to: 1131 },
    { key: 'linan', label: '流寓临安', from: 1132, to: Infinity },
  ],
};

/** 未收录诗人的兜底分期：按其自身生卒年三等分，避免一律归入「早年」 */
export function genericPhases(birth: number | null, death: number | null): Phase[] {
  if (birth == null || death == null || death <= birth) {
    return [{ key: 'all', label: '一生', from: -Infinity, to: Infinity }];
  }
  const a = birth + Math.round((death - birth) / 3);
  const b = birth + Math.round(((death - birth) * 2) / 3);
  return [
    { key: 'early', label: '早年', from: -Infinity, to: a },
    { key: 'mid', label: '中年', from: a + 1, to: b },
    { key: 'late', label: '晚年', from: b + 1, to: Infinity },
  ];
}

/** 取某诗人的分期表 */
export function phasesOf(
  poetId: string,
  birth?: number | null,
  death?: number | null,
): Phase[] {
  return PHASES_BY_POET[poetId] ?? genericPhases(birth ?? null, death ?? null);
}

/** 取某年份所属分期 */
export function phaseLabelOf(
  poetId: string,
  year: number,
  birth?: number | null,
  death?: number | null,
): string {
  return (
    phasesOf(poetId, birth, death).find((p) => year >= p.from && year <= p.to)?.label ?? ''
  );
}
